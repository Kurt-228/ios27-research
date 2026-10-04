#!/usr/bin/env python3
"""scan_ipsw.py — fast sequential inventory of zip members, no central directory.

The archive we were handed is truncated: PK\x03\x04 is present at offset 0 but
the End-of-Central-Directory record is unreachable, so unzip refuses it. A zip
stream does not need that record to be read, and the boot-chain components we
want sit early in an IPSW, so a sequential walk recovers them.

This is the same walk as carve_ipsw.py but done in blocks. Seeking one byte at a
time and re-reading a 30-byte header per candidate costs thousands of syscalls
on a 12 GB file, which is why the first attempt was still running when it was
cut off. Here a large block is read once and searched in memory with
bytes.find, which is several orders of magnitude faster, and only the members
that are actually wanted get parsed and inflated.

Used with --find to answer "is it even in here" before spending time on
extraction, which matters when the archive may not be complete.
"""
import struct
import sys
import zlib

BLOCK = 32 << 20      # 32 MB reads
OVERLAP = 4096        # keeps headers that straddle a block boundary visible
LFH = b"PK\x03\x04"
CENTRAL = b"PK\x01\x02"


def u16(b, o):
    return struct.unpack_from("<H", b, o)[0]


def u32(b, o):
    return struct.unpack_from("<I", b, o)[0]


def main():
    path, outdir = sys.argv[1], sys.argv[2]
    want = sys.argv[3:]
    seen = set()
    hits = []
    with open(path, "rb") as f:
        size = f.seek(0, 2)
        f.seek(0)
        base = 0
        prev_tail = b""
        while base < size:
            buf = f.read(BLOCK)
            if not buf:
                break
            data = prev_tail + buf
            start = base - len(prev_tail)
            i = 0
            while True:
                p = data.find(LFH, i)
                if p < 0 or p + 30 > len(data):
                    break
                try:
                    flags, method = u16(data, p + 6), u16(data, p + 8)
                    csize, usize = u32(data, p + 18), u32(data, p + 22)
                    nlen, elen = u16(data, p + 26), u16(data, p + 28)
                except struct.error:
                    i = p + 1
                    continue
                if nlen == 0 or nlen > 1024:
                    i = p + 1
                    continue
                if p + 30 + nlen > len(data):
                    break                        # header spans the boundary
                name = data[p + 30:p + 30 + nlen].decode("utf-8", "replace")
                abs_off = start + p
                if name not in seen:
                    seen.add(name)
                    if not want or any(w in name for w in want):
                        mark = "WANT" if want and any(w in name for w in want) else "    "
                        note = ""
                        if csize == 0 and (flags & 0x08):
                            note = " [sizes in data descriptor]"
                        print("%s %-66s off=%-12d csize=%-10d method=%d%s"
                              % (mark, name[:66], abs_off, csize, method, note))
                        sys.stdout.flush()
                        if mark == "WANT" and csize:
                            data_off = abs_off + 30 + nlen + elen
                            f.seek(data_off)
                            cand0 = f.read(csize)
                            raw = cand0
                            if method == 8:
                                try:
                                    raw = zlib.decompress(raw, -15)
                                except Exception as e:
                                    print("     inflate failed: %s" % e)
                                    raw = None
                            elif method != 0:
                                print("     unsupported method %d" % method)
                                raw = None
                            if raw is None and method == 8:
                                # Resync on the deflate signature rather than
                                # trusting data_off. Some entries here carry a
                                # local header whose extra field the
                                # 30+nlen+elen arithmetic does not describe, so
                                # the payload starts a few bytes late and
                                # inflate reports a misleading "invalid block
                                # type". Sliding to the next 0x78 zlib-style
                                # header inside the first 64 bytes recovers the
                                # true start without needing the central
                                # directory we do not have.
                                p2, tries = cand0.find(b"\x78"), 0
                                while p2 >= 0 and p2 < 64 and tries < 64:
                                    if cand0[p2] in (0x01, 0x5E, 0x9C, 0xDA):
                                        try:
                                            raw = zlib.decompress(cand0[p2:], -15)
                                            print("     resynced inflate at +%d" % p2)
                                            break
                                        except Exception:
                                            pass
                                    tries += 1
                                    p2 = cand0.find(b"\x78", p2 + 1)
                            if raw is not None:
                                out = "%s/%s" % (outdir, name.rsplit("/", 1)[-1])
                                with open(out, "wb") as o:
                                    o.write(raw)
                                print("     wrote %s (%d bytes)" % (out, len(raw)))
                                hits.append(out)
                i = p + 4
            prev_tail = data[-OVERLAP:]
            base += len(buf)
    print("\n%d unique members, %d written" % (len(seen), len(hits)))


if __name__ == "__main__":
    main()
