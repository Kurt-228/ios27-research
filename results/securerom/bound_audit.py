#!/usr/bin/env python3
"""bound_audit.py — count one-sided vs two-sided bounds checks, per kext.

The first version of this filter compared "longer than"/"too big" against
"shorter than"/"too small" and reported AppleAVE2 as 54:2, which read like the
best candidate in the collection. It was wrong about the interesting part. Among
those 54 strings:

    DPE register address is out of range 0x%x [0x%x, 0x%x]
    address is out of range 0x%llx [0x%llx, 0x%llx)

the lower bound *is* checked, in the same message, printed as a [min, max] pair.
A filter that only looks for "shorter than" cannot see that, so it systematically
overstates asymmetry — and a lead that is wrong in the direction that looks most
promising is worse than no lead.

So this version classifies each message into bound *shape* rather than into
vocabulary:

    TWO_SIDED   a range is printed as [min, max] or (min, max] — both ends
    UPPER_ONLY  "exceeds max", "too large", "N > cap", "out of range" with one end
    LOWER_ONLY  "too small", "shorter than", "below minimum"
    NONE        irrelevant

The shape test is deliberately conservative: a message is only called TWO_SIDED
when it visibly carries two range endpoints next to range language, because the
cost of a false TWO_SIDED is a missed candidate while the cost of a false
UPPER_ONLY is chasing a phantom.

The output is per kext: how many upper-only checks, how many lower-only, and
then the ranked list of upper-only checks that have no lower partner *for the
same field name*. The field name is what makes this less arbitrary than the
previous version — "Number of interrupts exceeds max size" and "Number of
interrupts too small" are a pair even if they sit in different functions, and
that is the level at which a real omission shows up.
"""
import re
import sys
import collections

RANGE_RE = re.compile(
    r"out\s*of\s*range|in\s+range|between|out\s+of\s+bounds|not\s+in\s+\[?\s*%",
    re.I)
# two range endpoints in one message: [a, b]  (a, b]  [0x%x, 0x%x)
PAIR_RE = re.compile(r"[\[(]\s*%[-+ #0-9.*lh]*[a-z]\s*,\s*%[-+ #0-9.*lh]*[a-z]\s*[\])]")

UPPER_RE = re.compile(
    r"exceeds\s+max|exceed\s+max|too\s+large|too\s+big|too\s+high|"
    r"larger\s+than\s+(max|limit|cap)|greater\s+than\s+(max|limit)|"
    r"over\s+max|overflow|out\s*of\s*range|out\s*of\s+bounds|"
    r"cannot\s+exceed|limited\s+to", re.I)
LOWER_RE = re.compile(
    r"too\s+small|too\s+short|smaller\s+than\s+(the\s+)?min|"
    r"less\s+than\s+(the\s+)?min|below\s+min|shorter\s+than|"
    r"underflow|at\s+least|must\s+be\s+(greater|nonzero)|"
    r"is\s+zero|cannot\s+be\s+zero", re.I)

# strip the leading log-site decoration so the same message counted twice by
# different %s::%s prefixes collapses to one
SITE_RE = re.compile(r"^[^A-Za-z]*%[-+ #0-9.*lh]*[a-z]\s*")
FIELD_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]{3,}")


def shape(s):
    """(shape, field) for one message."""
    if RANGE_RE.search(s) and PAIR_RE.search(s):
        return "TWO_SIDED", None
    if UPPER_RE.search(s):
        m = FIELD_RE.search(SITE_RE.sub("", s))
        return "UPPER_ONLY", (m.group().lower() if m else None)
    if LOWER_RE.search(s):
        m = FIELD_RE.search(SITE_RE.sub("", s))
        return "LOWER_ONLY", (m.group().lower() if m else None)
    return "NONE", None


def main():
    tsv = sys.argv[1]
    top = int(sys.argv[2]) if len(sys.argv) > 2 else 25
    per = collections.defaultdict(lambda: {"TWO": 0, "UP": 0, "LO": 0,
                                           "upfields": collections.defaultdict(list),
                                           "lofields": set()})
    for line in open(tsv, errors="replace"):
        p = line.rstrip("\n").split("\t", 3)
        if len(p) < 4:
            continue
        k, s = p[0], p[3]
        sh, fld = shape(s)
        e = per[k]
        if sh == "TWO_SIDED":
            e["TWO"] += 1
        elif sh == "UPPER_ONLY":
            e["UP"] += 1
            if fld:
                e["upfields"][fld].append(s)
        elif sh == "LOWER_ONLY":
            e["LO"] += 1
            if fld:
                e["lofields"].add(fld)

    rows = []
    for k, e in per.items():
        if not e["UP"]:
            continue
        # upper-only checks whose field never appears in any lower-bound message
        orphan = {f: v for f, v in e["upfields"].items() if f not in e["lofields"]}
        n_orph = sum(len(v) for v in orphan.values())
        if not n_orph:
            continue
        rows.append((n_orph, e["UP"], e["LO"], e["TWO"], k, orphan))

    rows.sort(reverse=True)
    print("%-44s %5s %5s %6s %6s" % ("kext", "up", "lo", "two", "orphan"))
    print("-" * 74)
    for n_orph, nu, nl, nt, k, orphan in rows[:top]:
        print("%-44s %5d %5d %6d %6d" % (k[:44], nu, nl, nt, n_orph))

    print("\n=== orphan upper-bound checks with no lower partner (top 3 kexts) ===")
    for n_orph, nu, nl, nt, k, orphan in rows[:3]:
        print("\n--- %s  (up=%d lo=%d two-sided=%d orphan=%d)"
              % (k, nu, nl, nt, n_orph))
        shown = 0
        for f, msgs in orphan.items():
            for m in msgs:
                print("   [%s] %s" % (f, m[:118]))
                shown += 1
                break
            if shown >= 8:
                break


if __name__ == "__main__":
    main()