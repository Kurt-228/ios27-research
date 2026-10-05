#!/usr/bin/env python3
"""length_audit.py — find kexts that bound a user-supplied length from above
but never from below.

The reasoning, because the naive version of this filter produces noise rather
than findings. Searching for "size" and "count" strings finds hundreds of
drivers and tells us nothing: almost every driver of any size checks an upper
bound, and that is correct behaviour. What actually predicts a bug is an
asymmetry. Apple's drivers validate a declared length twice — once against the
capacity of the buffer that will receive the copy, and once against a minimum
that makes the subsequent header subtraction well-defined. The second check is
the one that gets dropped, because a packet that is too short does not crash
anything immediately; it only makes a length field wrap or go negative, and the
copy that follows is what overflows.

So the filter does not look for size vocabulary, it looks for the *pair*:

  upper present, lower absent  -> candidate: subtraction of a header from an
                                  unchecked length

Strings are grouped by kext and then by which fields they name, using the field
names Apple prints in the format strings themselves ("in_args->data_size (%lu) >
in_args->data[] count (%lu)"). A driver that bounds three different lengths and
prints a "shorter than minimum" for one of them has shown it knows the pattern,
which makes its other unchecked length more interesting rather than less.

Weighting is deliberately coarse and is reported with the evidence attached.
This ranks candidates; it does not decide them. A rank with no strings behind it
is not auditable, and §143's lesson was that load-bearing claims need their
evidence visible rather than summarised.
"""
import os
import re
import sys

# ---- upper-bound evidence: declares a length that exceeds a capacity
UPPER = [
    (re.compile(rb'longer than', re.I), "declared length exceeds capacity"),
    (re.compile(rb'>\s*%?\w*[Bb]uffer', re.I), "compared against buffer"),
    (re.compile(rb'[Ss]ize\s*>\s*%?\w*[Cc]ount'), "size > count"),
    (re.compile(rb'[Cc]ount\s*>\s*%?\w*[Ss]ize'), "count > size"),
    (re.compile(rb'exceeds\s+max', re.I), "exceeds max"),
    (re.compile(rb'too\s+(big|large)', re.I), "too big"),
    (re.compile(rb'>\s*0x[0-9a-f]{4,}'), "against a hex constant bound"),
    (re.compile(rb'[Bb]ounds?\s+check', re.I), "explicit bounds check"),
    (re.compile(rb'out\s*of\s*range', re.I), "out of range"),
]

# ---- lower-bound evidence: declares a length too small to be valid
LOWER = [
    (re.compile(rb'shorter\s+than', re.I), "shorter than minimum"),
    (re.compile(rb'too\s+(small|short)', re.I), "too small"),
    (re.compile(rb'minimum\s+(size|length|packet)', re.I), "minimum size"),
    (re.compile(rb'below\s+minimum', re.I), "below minimum"),
    (re.compile(rb'underflow', re.I), "underflow named explicitly"),
    (re.compile(rb'malformed\s+(packet|header|length)', re.I), "malformed packet"),
    (re.compile(rb'less\s+than', re.I), "less than"),
]

# Copy primitives and descriptor vocabulary: what would actually overflow.
DANGER = [
    (re.compile(rb'\bmemcpy\b'), "memcpy"),
    (re.compile(rb'\bmemmove\b'), "memmove"),
    (re.compile(rb'\bbcopy\b'), "bcopy"),
    (re.compile(rb'\bmbcopy\b'), "mbcopy"),
    (re.compile(rb'IOMemoryDescriptor', re.I), "IOMemoryDescriptor"),
    (re.compile(rb'DMA|descriptor', re.I), "descriptor/DMA"),
]


def scan(path):
    d = open(path, "rb").read()
    strs = [m.group().decode(errors="replace")
            for m in re.finditer(rb"[ -~]{10,}", d)]
    up, lo, dg = [], [], []
    for s in strs:
        for rx, why in UPPER:
            if rx.search(s.encode()):
                up.append((why, s))
                break
        for rx, why in LOWER:
            if rx.search(s.encode()):
                lo.append((why, s))
                break
        for rx, why in DANGER:
            if rx.search(s.encode()):
                dg.append(why)
                break
    return up, lo, dg, len(strs)


def main():
    kdir = sys.argv[1]
    out = sys.argv[2] if len(sys.argv) > 2 else None
    rows = []
    for name in sorted(os.listdir(kdir)):
        if not name.endswith(".macho"):
            continue
        p = os.path.join(kdir, name)
        try:
            up, lo, dg, nstr = scan(p)
        except Exception as e:
            print("scan failed %s: %s" % (name, e), file=sys.stderr)
            continue
        if len(up) < 1 or not dg:
            continue
        # asymmetry is the signal: many upper bounds, few or no lower ones
        ratio = len(lo) / float(len(up)) if up else 1.0
        rows.append((len(up) * len(dg) / (1.0 + ratio), name,
                     len(up), len(lo), sorted(set(dg)), nstr))

    rows.sort(reverse=True)
    print("%-46s %5s %5s %4s %s" % ("kext", "upper", "lower", "str", "copy/desc"))
    print("-" * 100)
    for score, name, nu, nl, dg, nstr in rows[:30]:
        print("%-46s %5d %5d %5d %s" % (name[:46], nu, nl, nstr, ",".join(dg)[:28]))

    if out:
        with open(out, "w") as f:
            for name in sorted(os.listdir(kdir)):
                if not name.endswith(".macho"):
                    continue
                up, lo, dg, nstr = scan(os.path.join(kdir, name))
                if len(up) < 1 or not dg:
                    continue
                f.write("\n=== %s  (upper=%d lower=%d strings=%d)\n"
                        % (name, len(up), len(lo), nstr))
                for why, s in up:
                    f.write("  UP  %-28s %s\n" % (why, s[:150]))
                for why, s in lo:
                    f.write("  LO  %-28s %s\n" % (why, s[:150]))
        print("\nper-kext evidence -> %s" % out)


if __name__ == "__main__":
    main()