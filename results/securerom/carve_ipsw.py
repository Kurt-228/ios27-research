#!/usr/bin/env python3
"""carve_ipsw.py — pull entries out of an IPSW by streaming local file headers.

Why this exists rather than `unzip`: the archive we were given reports
"End-of-centdir-64 not found" even though the local header signature PK\x03\x04
is present at offset 0 and the file is 12.3 GB. That is a truncated or
partially-copied archive — its central directory, which lives in the last few
kilobytes, is missing or unreachable.

A zip stream does not need the central directory to read entries, though. Every
member is preceded by its own local file header carrying the name and (usually)
the sizes. Walking the file sequentially and parsing those headers recovers
members that live before whatever truncation happened, which covers exactly the
part we want: BuildManifest.plist and the Firmware/24A5390f/* boot-chain
components sit near the start of an IPSW, nowhere near the end.

Two details that bite here:

  - Data descriptors. When the general-purpose bit 3 is set, the local header
    carries zeros for compressed/uncompressed size and the real values follow
    the data in a trailing descriptor. Apple sets it inconsistently across
    component files, so a header that claims size 0 is reported rather than
    trusted, and the next header position is resynchronised from the following
    PK\x03\x04 signature if the walk goes wrong.

  - Seeking. Sizes are trusted only after the entry's bytes have been seen, so
    a truncated tail produces a short read that gets logged, not a silently
    misparsed entry.
"""
import struct
import sys
import zlib

LFH = b"PK\x03\x04"


def u16(b, o):
    return struct.unpack_from("<H", b, o)[0]


def u32(b, o):
    return struct.unpack_from("<I", b, o)[0]


def carve(path, wanted, outdir, max_hits=40):
    hits = []
    with open(path, "rb") as f:
        pos = 0
        size = f.seek(0, 2)
        f.seek(0)
        # A 30-byte local header plus name plus extra. Cap the variable part so a
        # corrupt length cannot make us read the whole remainder as a filename.
        while pos < size and len(hits) < max_hits:
            f.seek(pos)
            hdr = f.read(30)
            if len(hdr) < 30:
                break
            if hdr[:4] != LFH:
                # Resynchronise on the next signature. Streaming forward in
                # bounded steps keeps a damaged entry from costing the run.
                pos += 1
                continue
            ver, flags, method = u16(hdr, 4), u16(hdr, 6), u16(hdr, 8)
            csize, usize = u32(hdr, 18), u32(hdr, 22)
            nlen, elen = u16(hdr, 26), u16(hdr, 28)
            if nlen == 0 or nlen > 4096:
                pos += 1
                continue
            name = f.read(nlen).decode("utf-8", "replace")
            f.seek(elen, 1)          # skip extra field
            data_off = pos + 30 + nlen + elen

            if any(w in name for w in wanted):
                descriptor = ""
                if csize == 0 and usize == 0 and (flags & 0x08):
                    descriptor = " (data descriptor: sizes unknown in header)"
                print("FOUND %-70s method=%d flags=0x%04x csize=%d usize=%d off=%d%s"
                      % (name, method, flags, csize, usize, data_off, descriptor))
                out = None
                for w in wanted:
                    if w in name:
                        base = name.rsplit("/", 1)[-1]
                        out = "%s/%s" % (outdir, base)
                        break
                if csize:
                    f.seek(data_off)
                    raw = f.read(csize)
                    if method == 8:
                        try:
                            raw = zlib.decompress(raw, -15)
                        except Exception as e:
                            print("       inflate failed: %s" % e)
                            raw = None
                    elif method != 0:
                        print("       unsupported compression method %d" % method)
                        raw = None
                    if raw is not None:
                        with open(out, "wb") as o:
                            o.write(raw)
                        print("       wrote %s (%d bytes)" % (out, len(raw)))
                        hits.append((name, out, len(raw)))

            # Advance past this entry's payload.
            if csize:
                nxt = data_off + csize
                if flags & 0x08:
                    nxt += 16          # descriptor, worst case
            else:
                nxt = data_off        # cannot skip; resync by scanning on
            if nxt <= pos:
                nxt = pos + 1
            pos = nxt
    print("\n%d entries written to %s" % (len(hits), outdir))
    return hits


if __name__ == "__main__":
    ip = sys.argv[1]
    outdir = sys.argv[2]
    want = sys.argv[3:]
    carve(ip, want, outdir)
