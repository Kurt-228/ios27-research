#!/usr/bin/env python3
"""audit_kexts.py — rank never-studied kexts by how much they look like they
copy a user-supplied length without checking it.

Method, and why it is shaped this way. §145 closed the two reachable userclients
by asking a concrete question of each: does any live method read a
structureInput at all. That question needed the device. Here we cannot ask the
device about the 114 kexts the sandbox never reaches, and for a bounty report
that is not fatal — a memory-corruption bug in a kext is a finding whether or
not this particular build's sandbox can reach it, because it may be reachable
from a privileged daemon or a different entitlement set. What we need instead
is a static prior: which drivers even contain the code shape where the bug
lives.

That shape is a user-declared length checked against a count derived from a
structure. Apple's drivers name these fields in their own assert strings, which
is why string evidence works at all: the presence of "in_args->data_size (%lu) >
in_args->data[] count (%lu)" told us IOMobileGraphicsFamily had the construct
before we ran a single method. So this script scores each kext on those strings,
weighted, and prints the reasoning rather than just the number — a rank with no
explanation is not auditable, and §143's whole point was that load-bearing
claims need their evidence visible.

Three weights, chosen for what actually distinguishes a hit from noise:
  2x  an explicit comparison of a size field against a count/array bound, the
      literal shape of the bug we want
  1x  memcpy/memmove/mbcopy/bcopy present, the actual copy primitives
  1x  the caller-supplied-length vocabulary (size/len/count/offset/length/num)
The remainder is deliberately small context lines around each hit, because the
string itself is the evidence and the surrounding text is what makes it
checkable by hand later.
"""
import os
import re
import struct
import subprocess
import sys

LC_FILESET = 0x80000035

STRONG = [
    (rb'>\s*%?\w*[Ss]ize[^>]{0,40}>\s*[^%]{0,20}%?\w*[Cc]ount', 2, "size compared against count"),
    (rb'[Ss]ize\w*\s*\(\s*%\w+\s*\)\s*>\s*\w+\[\]\s*[Cc]ount', 2, "size > array count"),
    (rb'[Cc]ount\s*\(\s*%\w+\s*\)\s*<', 2, "count compared against limit"),
    (rb'\[\]\s*count', 2, "flexible-array count"),
    (rb'[Oo]ut\s*[_A-Za-z]*[Bb]ytes\s*>?\s*\w*size', 1, "out_bytes vs size"),
    (rb'[Bb]uffer\s*size', 1, "buffer size vocabulary"),
]
COPY = [
    (rb'\bmemcpy\b', 1, "memcpy"),
    (rb'\bmemmove\b', 1, "memmove"),
    (rb'\bmbcopy\b', 1, "mbcopy"),
    (rb'\bbcopy\b', 1, "bcopy"),
]
VOCAB = [
    (rb'[Dd]escriptor', 1, "descriptor (DMA/IOMemory)"),
    (rb'[Ee]lement\s*[Cc]ount', 1, "element count"),
    (rb'[Oo]ffset', 1, "offset field"),
    (rb'[Pp]ayload', 1, "payload vocabulary"),
]


def carve(kc, bundle, outdir):
    """Run the existing carver and take the filename back from its stdout.

    carvesit_fileset.py prints "wrote <name> (<n> bytes)", so splitting on
    whitespace and taking the last field grabbed "bytes)". Parse the parentheses
    instead, and resolve relative to cwd because the carver writes to its own
    directory rather than wherever the caller wanted it.
    """
    r = subprocess.run([sys.executable, "results/kc-extract/carve_fileset.py", kc, bundle],
                       capture_output=True, text=True, cwd=os.getcwd())
    for line in r.stdout.splitlines():
        if "wrote" in line:
            after = line.split("wrote", 1)[1].strip()
            name = after.split("(")[0].strip()
            if os.path.exists(name):
                return os.path.abspath(name)
            alt = os.path.join("results/kc-extract", name)
            if os.path.exists(alt):
                return os.path.abspath(alt)
            return name
    return None


def score(data):
    total = 0
    hits = []
    for pat, w, why in STRONG:
        for m in re.finditer(pat, data):
            s = max(0, m.start() - 60)
            ctx = data[s:m.end() + 60]
            ctx = re.sub(rb'[^\x20-\x7e]', b'|', ctx).decode()
            hits.append((2 * w, why, ctx[:170]))
            total += 2 * w
            break
    for pat, w, why in COPY:
        if re.search(pat, data):
            hits.append((w, why, ""))
            total += w
    for pat, w, why in VOCAB:
        if re.search(pat, data):
            hits.append((w, why, ""))
            total += w
    return total, hits


def main():
    kc = sys.argv[1]
    outdir = sys.argv[2]
    bundles = sys.argv[3:]
    results = []
    for b in bundles:
        path = carve(kc, b, outdir)
        if not path:
            print("carve failed: %s" % b)
            continue
        data = open(path, "rb").read()
        sc, hits = score(data)
        results.append((sc, b, path, len(data), hits))
    results.sort(reverse=True, key=lambda t: t[0])
    for sc, b, path, n, hits in results:
        print("=" * 78)
        print("score %-3d  %-52s  %d bytes" % (sc, b, n))
        for w, why, ctx in sorted(hits, reverse=True):
            print("   +%d  %s" % (w, why))
            if ctx:
                print("        %s" % ctx)


if __name__ == "__main__":
    main()
