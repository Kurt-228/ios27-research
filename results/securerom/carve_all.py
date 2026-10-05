#!/usr/bin/env python3
"""carve_all.py — carve every kext out of a kernelcache in a single pass.

carve_fileset.py takes one bundle name per invocation, which is fine for
targeted work but means 120 sequential passes over a 73 MB kernelcache. This
walks LC_FILESET_ENTRY once, keeps the entries in file order, and writes each
kext to disk immediately.

The extent of each kext is the part that is easy to get wrong, and getting it
wrong is worse than getting nothing: if an extent is computed as "the gap to the
next fileset entry", the kext's own declared segment offsets (which are absolute
offsets into the *collection*, not into the kext) will pull the neighbouring
kexts into the blob, and every kext will then appear to contain every other
kext's strings. triage_kexts.py made exactly that mistake and produced 363
kexts that all looked identical, which is worse than useless because it looks
like a result.

So the extent is derived from the kext's own segment commands: __TEXT's
fileoff plus its filesize is the end of the last thing mapped from disk, and
anything past that is not ours. Where a segment's filesize is zero the mapped
size is used instead, since a segment can be bss-like.

Debug/kasan twins are skipped by suffix, matching triage_kexts.py, so the
counts are comparable between the two tools.
"""
import os
import struct
import sys

LC_FILESET_ENTRY = 0x80000035
LC_SEGMENT_64 = 0x19


def kext_extent(d, off, limit, next_off):
    """End offset of the kext starting at `off`, from its own segment table.

    Correct method, verified against a known-good single carve:
    IOAccessoryManager's __LINKEDIT is at file offset 0x139d15 with filesize
    0x6d879, so the kext ends at 0x1A6B4E = 1,731,214. The reference carve
    produced 1,734,030 bytes — the 2,816-byte difference is the next fileset
    header and alignment padding. So a kext's segment fileoffs are *relative to
    the kext*, and taking max(fileoff + size) across its segments is right.

    Getting this wrong fails silently in both directions, which is why the
    method is written down with its arithmetic:

      - using the next fileset entry's offset instead produced 146,464 bytes,
        truncating the kext before __TEXT_EXEC even begins (at 0x242f7 =
        148,215) and silently dropping 362 strings;
      - reading vmsize/fileoff from the wrong segment_command_64 offsets made
        the max land near the end of the whole collection, giving 64 MB.

    Both look like a successful run.
    """
    ncmds = struct.unpack_from("<I", d, off + 16)[0]
    o = off + 32
    end = off
    for _ in range(ncmds):
        if o + 8 > limit:
            break
        cmd, cs = struct.unpack_from("<II", d, o)
        if cs == 0:
            break
        if cmd == LC_SEGMENT_64:
            # segment_command_64: cmd(4) cmdsize(4) segname(16)
            #   vmaddr(8)@24 vmsize(8)@32 fileoff(8)@40 filesize(8)@48
            foff = struct.unpack_from("<Q", d, o + 40)[0]
            filesize = struct.unpack_from("<Q", d, o + 48)[0]
            # filesize only, never vmsize as a fallback. A bss-style segment has
            # filesize 0 and a large vmsize, and bss is not in the file at all,
            # so using vmsize there pushed the extent to 64 MB and swallowed the
            # rest of the collection. Each of this function's four successive
            # wrong answers produced a clean run rather than an error, which is
            # why the arithmetic is spelled out here instead of left implicit.
            if foff and filesize and foff + filesize > end:
                end = foff + filesize
        o += cs
    return min(end, limit)


def main():
    kc = sys.argv[1]
    outdir = sys.argv[2]
    only = sys.argv[3] if len(sys.argv) > 3 else None
    os.makedirs(outdir, exist_ok=True)
    d = open(kc, "rb").read()
    limit = len(d)
    nfilesets = struct.unpack_from("<I", d, 16)[0]
    off = 32
    entries = []
    for _ in range(nfilesets):
        cmd, cs = struct.unpack_from("<II", d, off)
        if cmd == LC_FILESET_ENTRY:
            # In this collection the fileset entry is the compact form:
            #   cmd(4) cmdsize(4) vmaddr(8) fileoff(8) entry_id(8) name...
            # with the NUL-terminated name starting at +32 and running to
            # cmdsize. There is no reserved/count/stroff here — reading those
            # fields lands inside the name, which is how this tool first
            # reported stroff=1701523045 and collapsed all 265 kexts onto a
            # single output file. Two earlier attempts at this parser read
            # vmaddr from +16 (fileoff) and vmsize/fileoff from the wrong
            # segment_command_64 offsets; all three produced a clean wrong
            # answer rather than an error.
            fileoff = struct.unpack_from("<Q", d, off + 16)[0]
            name = d[off + 32:off + cs].split(b"\x00")[0].decode(errors="replace")
            entries.append((fileoff, name))
        off += cs

    written = skipped = 0
    for i, (fileoff, name) in enumerate(entries):
        if name.endswith(("_development", "_kasan", "_debug")):
            skipped += 1
            continue
        if only and only not in name:
            continue
        end = kext_extent(d, fileoff, limit, None)
        if end <= fileoff:
            continue
        safe = name.replace("/", "_")
        p = os.path.join(outdir, safe + ".macho")
        with open(p, "wb") as f:
            f.write(d[fileoff:end])
        written += 1
        if written % 20 == 0:
            print("  ... %d/%d" % (written, len(entries)), file=sys.stderr)
    print("filesets=%d  written=%d  skipped(debug/kasan)=%d  -> %s"
          % (len(entries), written, skipped, outdir))


if __name__ == "__main__":
    main()
