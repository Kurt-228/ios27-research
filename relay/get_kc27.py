#!/usr/bin/env python3
"""Extract kernelcache from an iOS IPSW over HTTP range requests only.

Steps:
  1. HEAD -> Content-Length
  2. Range GET of tail -> locate EOCD (PK\x05\x06) -> central dir offset/size
  3. Range GET central directory -> parse PK\x01\x02 entries, find kernelcache.*
  4. Range GET local header + compressed data -> inflate (deflate wbits=-15)
  5. Strip IM4P (ASN.1) wrapper -> find Mach-O magic cf fa ed fe
  6. Parse LC_FILESET_ENTRY names, carve requested kexts (standalone Mach-O)

Usage: python3 relay/get_kc27.py [ipsw_url] [outdir]
"""
import os
import struct
import sys
import urllib.request
import zlib

URL = sys.argv[1] if len(sys.argv) > 1 else (
    "https://updates.cdn-apple.com/2026SpringSeed/fullrestores/140-58754/"
    "BE6FFC19-A20C-4D29-B0E0-EF9DF7CC0EBE/iPhone16,2_27.0_24A5390f_Restore.ipsw")
OUT = sys.argv[2] if len(sys.argv) > 2 else "results/kc27"
TAIL = 256 * 1024

os.makedirs(OUT, exist_ok=True)
bytes_downloaded = 0


def fetch(start, end, desc=""):
    """Inclusive byte range GET."""
    global bytes_downloaded
    req = urllib.request.Request(URL, headers={"Range": f"bytes={start}-{end}"})
    with urllib.request.urlopen(req, timeout=120) as r:
        data = r.read()
    bytes_downloaded += len(data)
    print(f"  [{desc}] bytes {start}-{end}: got {len(data)} "
          f"(total downloaded: {bytes_downloaded / 1e6:.1f} MB)", flush=True)
    return data


# 1. size
req = urllib.request.Request(URL, method="HEAD")
with urllib.request.urlopen(req, timeout=60) as r:
    total = int(r.headers["Content-Length"])
print(f"IPSW size: {total} bytes ({total / 1e9:.2f} GB)")

# 2. EOCD
tail = fetch(max(0, total - TAIL), total - 1, "tail")
eocd = tail.rfind(b"PK\x05\x06")
assert eocd >= 0, "EOCD not found in tail"
cd_size = struct.unpack_from("<I", tail, eocd + 12)[0]
cd_off = struct.unpack_from("<I", tail, eocd + 16)[0]
n_entries = struct.unpack_from("<H", tail, eocd + 10)[0]
if cd_off == 0xFFFFFFFF or cd_size == 0xFFFFFFFF or n_entries == 0xFFFF:
    loc = tail.rfind(b"PK\x06\x07", 0, eocd)
    assert loc >= 0, "ZIP64 EOCD locator not found"
    z64_off = struct.unpack_from("<Q", tail, loc + 8)[0]
    z64 = fetch(z64_off, z64_off + 96, "zip64 eocd")
    assert z64[:4] == b"PK\x06\x06", "bad zip64 EOCD"
    n_entries = struct.unpack_from("<Q", z64, 32)[0]
    cd_size = struct.unpack_from("<Q", z64, 40)[0]
    cd_off = struct.unpack_from("<Q", z64, 48)[0]
    print(f"ZIP64 EOCD at {z64_off}: entries={n_entries} cd_off={cd_off} "
          f"cd_size={cd_size}")
print(f"EOCD: central dir at {cd_off}, size {cd_size}, {n_entries} entries")

# 3. central directory
cd = fetch(cd_off, cd_off + cd_size - 1, "central dir")
entries = []
pos = 0
while pos + 46 <= len(cd) and cd[pos:pos + 4] == b"PK\x01\x02":
    method = struct.unpack_from("<H", cd, pos + 10)[0]
    csize = struct.unpack_from("<I", cd, pos + 20)[0]
    usize = struct.unpack_from("<I", cd, pos + 24)[0]
    nlen, xlen, clen = struct.unpack_from("<HHH", cd, pos + 28)
    lho = struct.unpack_from("<I", cd, pos + 42)[0]
    name = cd[pos + 46:pos + 46 + nlen].decode("utf-8", "replace")
    extra = cd[pos + 46 + nlen:pos + 46 + nlen + xlen]
    if csize == 0xFFFFFFFF or usize == 0xFFFFFFFF or lho == 0xFFFFFFFF:
        # ZIP64 extra: tag 0x0001, then 8-byte values in field order
        ep = 0
        while ep + 4 <= len(extra):
            tag, esz = struct.unpack_from("<HH", extra, ep)
            if tag == 0x0001:
                vals = []
                vp = ep + 4
                for f, v in (("usize", usize), ("csize", csize), ("lho", lho)):
                    if v == 0xFFFFFFFF:
                        vals.append((f, struct.unpack_from("<Q", extra, vp)[0]))
                        vp += 8
                for f, v in vals:
                    if f == "usize":
                        usize = v
                    elif f == "csize":
                        csize = v
                    else:
                        lho = v
                break
            ep += 4 + esz
    entries.append((name, method, csize, usize, lho))
    pos += 46 + nlen + xlen + clen
kc_entries = [e for e in entries if "kernelcache" in e[0].lower()]
print(f"central dir: {len(entries)} entries; kernelcache candidates:")
for name, method, csize, usize, lho in kc_entries:
    print(f"  {name}: method={method} csize={csize} usize={usize} lho={lho}")
assert kc_entries, "no kernelcache entries found"

def im4p_payload(blob):
    """Walk minimal DER: SEQUENCE { IA5 IM4P, IA5 type, IA5 id, OCTET STRING }.
    Returns the OCTET STRING contents."""
    assert blob[0] == 0x30, "not a DER SEQUENCE"
    p = 1
    ll = blob[p]
    p += 1
    if ll & 0x80:
        p += ll & 0x7F
    for _ in range(3):  # skip 3 IA5Strings
        assert blob[p] == 0x16, f"expected IA5STRING at {p}"
        p += 1
        l = blob[p]
        p += 1
        if l & 0x80:
            n = l & 0x7F
            l = int.from_bytes(blob[p:p + n], "big")
            p += n
        p += l
    assert blob[p] == 0x04, f"expected OCTET STRING at {p}"
    p += 1
    l = blob[p]
    p += 1
    if l & 0x80:
        n = l & 0x7F
        l = int.from_bytes(blob[p:p + n], "big")
        p += n
    return blob[p:p + l]


def lzfse_decompress(data):
    """LZFSE (bvx2) via system libcompression through ctypes."""
    import ctypes
    import ctypes.util
    lib = ctypes.CDLL(ctypes.util.find_library("compression"))
    COMPRESSION_LZFSE = 0x801
    lib.compression_decode_buffer.restype = ctypes.c_size_t
    lib.compression_decode_buffer.argtypes = \
        [ctypes.c_void_p, ctypes.c_size_t, ctypes.c_void_p, ctypes.c_size_t,
         ctypes.c_void_p, ctypes.c_int]
    for cap in (256 << 20, 512 << 20, 1024 << 20):
        dst = ctypes.create_string_buffer(cap)
        src = ctypes.create_string_buffer(bytes(data), len(data))
        n = lib.compression_decode_buffer(dst, cap, src, len(data), None,
                                          COMPRESSION_LZFSE)
        if n != 0 or cap >= (1024 << 20):
            if n == 0:
                raise SystemExit("lzfse decode failed")
            return dst.raw[:n]
    raise SystemExit("unreachable")


def strip_to_macho(blob, tag):
    """IM4P -> payload -> (lzfse) -> Mach-O; return (macho, notes)."""
    notes = []
    if blob[:1] == b"\x30":
        blob = im4p_payload(blob)
        notes.append("stripped IM4P ASN.1")
    if blob[:4] in (b"bvx1", b"bvx2", b"bvll"):
        notes.append(f"lzfse payload {blob[:4].decode()}")
        blob = lzfse_decompress(blob)
    magic_off = blob.find(b"\xcf\xfa\xed\xfe")
    if magic_off < 0:
        notes.append("WARNING: Mach-O magic not found; "
                     f"first 32 bytes: {blob[:32].hex()}")
        return None, notes
    notes.append(f"Mach-O at +{magic_off}")
    return blob[magic_off:], notes


# 4+5. download + extract + strip IM4P
machos = []
for name, method, csize, usize, lho in kc_entries:
    tag = os.path.basename(name).replace("kernelcache.release.", "") \
        .replace("kernelcache.", "").replace(".", "_")
    lhdr = fetch(lho, lho + 29, f"{tag} local header")
    assert lhdr[:4] == b"PK\x03\x04", "bad local header"
    nl, xl = struct.unpack_from("<HH", lhdr, 26)
    data_start = lho + 30 + nl + xl
    raw = fetch(data_start, data_start + csize - 1, f"{tag} payload")
    if method == 8:
        blob = zlib.decompress(raw, -15)
    elif method == 0:
        blob = raw
    else:
        raise SystemExit(f"{name}: unsupported method {method}")
    assert len(blob) == usize, f"size mismatch {len(blob)} != {usize}"
    raw_path = os.path.join(OUT, f"kernelcache_{tag}.raw")
    open(raw_path, "wb").write(blob)
    print(f"  wrote {raw_path} ({len(blob)} bytes)")

    macho, notes = strip_to_macho(blob, tag)
    print("  " + "; ".join(notes))
    if macho is None:
        continue
    mpath = os.path.join(OUT, f"kernelcache_{tag}.macho")
    open(mpath, "wb").write(macho)
    hdr = struct.unpack_from("<8I", macho, 0)
    print(f"  wrote {mpath} (magic=0x{hdr[0]:08x} filetype={hdr[3]} "
          f"ncmds={hdr[4]})")
    machos.append((tag, mpath, macho))

# 6. fileset entries
WANT_PATTERNS = ("AppleM2ScalerCSCDriver", "IOGPUFamily", "AGX", "IOSurface",
                 "AppleKeyStore")
for tag, mpath, macho in machos:
    magic, cputype, cpusubtype, filetype, ncmds, sizeofcmds, flags, res = \
        struct.unpack_from("<8I", macho, 0)
    if magic != 0xFEEDFACF:
        print(f"{tag}: not a 64-bit Mach-O, skipping fileset parse")
        continue
    off = 32
    names = []
    for _ in range(ncmds):
        cmd, cmdsize = struct.unpack_from("<II", macho, off)
        if cmd == 0x80000035:  # LC_FILESET_ENTRY
            stroff = struct.unpack_from("<I", macho, off + 24)[0]
            nm = macho[off + stroff:off + cmdsize].split(b"\x00")[0].decode()
            names.append(nm)
        off += cmdsize
    print(f"{tag}: {len(names)} fileset entries")
    for nm in names:
        if any(p in nm for p in WANT_PATTERNS):
            print(f"  MATCH: {nm}")

print(f"DONE. total downloaded: {bytes_downloaded / 1e6:.1f} MB")
