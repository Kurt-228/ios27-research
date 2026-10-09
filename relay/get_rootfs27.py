#!/usr/bin/env python3
"""Range-extract a filesystem dmg from an iOS IPSW (public Apple CDN).

Companion to get_kc27.py (kernelcache). Steps:
  1. HEAD -> Content-Length
  2. Range GET tail -> EOCD -> central directory offset/size (ZIP64 aware)
  3. Range GET central directory -> list *.dmg entries (name/size/method/offset)
  4. With --list: print the table and exit (costs a few MB).
  5. With --get NAME: Range GET the entry's local header + compressed bytes,
     stream-inflate (or copy if stored) into OUT/NAME.

Usage:
  python3 relay/get_rootfs27.py [--list] [--get NAME] [ipsw_url] [outdir]
"""
import os
import struct
import sys
import urllib.request
import zlib

URL = None
OUT = "results/rootfs27"
MODE = "list"
WANT = None

args = sys.argv[1:]
i = 0
while i < len(args):
    a = args[i]
    if a == "--list":
        MODE = "list"
    elif a == "--get":
        MODE = "get"
        WANT = args[i + 1]
        i += 1
    elif not a.startswith("-") and URL is None:
        URL = a
    elif not a.startswith("-"):
        OUT = a
    i += 1

if URL is None:
    URL = ("https://updates.cdn-apple.com/2026SpringSeed/fullrestores/140-58754/"
           "BE6FFC19-A20C-4D29-B0E0-EF9DF7CC0EBE/iPhone16,2_27.0_24A5390f_Restore.ipsw")

TAIL = 256 * 1024
bytes_downloaded = 0


def fetch(start, end, desc=""):
    global bytes_downloaded
    req = urllib.request.Request(URL, headers={"Range": f"bytes={start}-{end}"})
    with urllib.request.urlopen(req, timeout=180) as r:
        data = r.read()
    bytes_downloaded += len(data)
    print(f"  [{desc}] {start}-{end}: {len(data)} B "
          f"(total {bytes_downloaded / 1e6:.1f} MB)", flush=True)
    return data


def fetch_stream(start, end, fh, desc=""):
    """Range GET written straight to file handle; returns bytes written."""
    global bytes_downloaded
    req = urllib.request.Request(URL, headers={"Range": f"bytes={start}-{end}"})
    n = 0
    with urllib.request.urlopen(req, timeout=600) as r:
        while True:
            chunk = r.read(1 << 20)
            if not chunk:
                break
            fh.write(chunk)
            n += len(chunk)
            bytes_downloaded += len(chunk)
            if n % (128 << 20) < (1 << 20):
                print(f"  [{desc}] {n / 1e6:.0f} MB "
                      f"(total {bytes_downloaded / 1e6:.0f} MB)", flush=True)
    return n


req = urllib.request.Request(URL, method="HEAD")
with urllib.request.urlopen(req, timeout=60) as r:
    total = int(r.headers["Content-Length"])
print(f"IPSW: {total} bytes ({total / 1e9:.2f} GB)", flush=True)

tail = fetch(max(0, total - TAIL), total - 1, "tail")
eocd = tail.rfind(b"PK\x05\x06")
assert eocd >= 0, "EOCD not found"
n_entries = struct.unpack_from("<H", tail, eocd + 10)[0]
cd_size = struct.unpack_from("<I", tail, eocd + 12)[0]
cd_off = struct.unpack_from("<I", tail, eocd + 16)[0]
if cd_off == 0xFFFFFFFF or cd_size == 0xFFFFFFFF or n_entries == 0xFFFF:
    loc = tail.rfind(b"PK\x06\x07", 0, eocd)
    assert loc >= 0, "ZIP64 locator missing"
    z64_off = struct.unpack_from("<Q", tail, loc + 8)[0]
    z64 = fetch(z64_off, z64_off + 96, "zip64")
    assert z64[:4] == b"PK\x06\x06"
    n_entries = struct.unpack_from("<Q", z64, 32)[0]
    cd_size = struct.unpack_from("<Q", z64, 40)[0]
    cd_off = struct.unpack_from("<Q", z64, 48)[0]
print(f"entries={n_entries} cd_off=0x{cd_off:x} cd_size=0x{cd_size:x}", flush=True)

cd = fetch(cd_off, cd_off + cd_size - 1, "central dir")

entries = []
p = 0
while p + 46 <= len(cd) and cd[p:p + 4] == b"PK\x01\x02":
    (sig, made, need, flags, method, mtime, mdate, crc, csize, usize,
     nlen, elen, clen, disk, iattr, eattr, loff) = struct.unpack_from(
        "<IHHHHHHIIIHHHHHII", cd, p)
    name = cd[p + 46:p + 46 + nlen].decode("utf8", "replace")
    # ZIP64 extra (0x0001) overrides 0xFFFFFFFF fields
    extra = cd[p + 46 + nlen:p + 46 + nlen + elen]
    q = 0
    while q + 4 <= len(extra):
        eid, esz = struct.unpack_from("<HH", extra, q)
        body = extra[q + 4:q + 4 + esz]
        if eid == 0x0001:
            k = 0
            if usize == 0xFFFFFFFF:
                usize = struct.unpack_from("<Q", body, k)[0]; k += 8
            if csize == 0xFFFFFFFF:
                csize = struct.unpack_from("<Q", body, k)[0]; k += 8
            if loff == 0xFFFFFFFF:
                loff = struct.unpack_from("<Q", body, k)[0]; k += 8
        q += 4 + esz
    entries.append(dict(name=name, method=method, csize=csize, usize=usize,
                        loff=loff))
    p += 46 + nlen + elen + clen

print(f"parsed {len(entries)} entries", flush=True)
dmgs = [e for e in entries if e["name"].lower().endswith(".dmg")]
print("\n== dmg entries ==")
for e in sorted(dmgs, key=lambda x: -x["usize"]):
    print(f"  {e['name']}: uncompressed={e['usize'] / 1e9:.2f} GB "
          f"compressed={e['csize'] / 1e9:.2f} GB method={e['method']} "
          f"off=0x{e['loff']:x}")
print("\n== largest entries overall ==")
for e in sorted(entries, key=lambda x: -x["usize"])[:8]:
    print(f"  {e['name']}: {e['usize'] / 1e9:.2f} GB method={e['method']}")

if MODE != "get":
    sys.exit(0)

target = next((e for e in entries if e["name"] == WANT), None)
assert target, f"{WANT} not in archive"
os.makedirs(OUT, exist_ok=True)
dst = os.path.join(OUT, os.path.basename(WANT))

# local header -> data offset
lh = fetch(target["loff"], target["loff"] + 64 - 1, "local hdr")
assert lh[:4] == b"PK\x03\x04", "bad local header"
ln, le = struct.unpack_from("<HH", lh, 26)
data_off = target["loff"] + 30 + ln + le
print(f"entry {WANT}: data_off=0x{data_off:x} "
      f"csize={target['csize']} method={target['method']}", flush=True)

end = data_off + target["csize"] - 1
if target["method"] == 0:  # stored
    with open(dst, "wb") as fh:
        fetch_stream(data_off, end, fh, WANT)
else:
    assert target["method"] == 8, f"unexpected method {target['method']}"
    dec = zlib.decompressobj(-15)
    with open(dst, "wb") as fh:
        global_total = 0
        req = urllib.request.Request(
            URL, headers={"Range": f"bytes={data_off}-{end}"})
        with urllib.request.urlopen(req, timeout=600) as r:
            while True:
                chunk = r.read(1 << 20)
                if not chunk:
                    break
                out = dec.decompress(chunk)
                fh.write(out)
                global_total += len(out)
                bytes_downloaded += len(chunk)
                if global_total % (128 << 20) < (1 << 20):
                    print(f"  [inflate] {global_total / 1e6:.0f} MB out "
                          f"(total {bytes_downloaded / 1e6:.0f} MB)", flush=True)
        tail_out = dec.flush()
        fh.write(tail_out)
        global_total += len(tail_out)
print(f"WROTE {dst}: {os.path.getsize(dst)} bytes "
      f"(expected {target['usize']})", flush=True)
assert os.path.getsize(dst) == target["usize"], "size mismatch"
print("DONE", flush=True)
