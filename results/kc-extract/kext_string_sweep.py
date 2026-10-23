#!/usr/bin/env python3
"""Semantic string sweep across all carved kexts: b4 (24A5390f) vs release
(24A435 RC == 24A437 FCS, byte-identical). Writes per-kext NEW/GONE string
markers to results/diffs/strings_b4_vs_435.txt — patch markers for all CVEs."""
import os

B4 = 'results/kc27-b4/kexts'
REL = 'results/kc27-435/kexts'
OUT = 'results/diffs/strings_b4_vs_435.txt'


def strings_alpha(data, minlen=10):
    out = set()
    cur = bytearray()
    for b in data:
        if 32 <= b < 127:
            cur.append(b)
        else:
            if len(cur) >= minlen:
                s = cur.decode('latin1')
                good = sum(1 for c in s if c.isalpha() or c in '_.:%-')
                if good >= len(s) * 0.7 and any(c.isalpha() for c in s):
                    out.add(s)
            cur = bytearray()
    return out


def main():
    os.makedirs('results/diffs', exist_ok=True)
    names = sorted(set(os.listdir(B4)) & set(os.listdir(REL)))
    out = open(OUT, 'w')
    n_changed = 0
    for n in names:
        if not n.endswith('.macho'):
            continue
        s1 = strings_alpha(open(os.path.join(B4, n), 'rb').read())
        s2 = strings_alpha(open(os.path.join(REL, n), 'rb').read())
        new = s2 - s1
        gone = s1 - s2
        if new or gone:
            n_changed += 1
            out.write('=== %s ===\n' % n)
            for s in sorted(new):
                out.write('  + %s\n' % s[:160])
            for s in sorted(gone):
                out.write('  - %s\n' % s[:160])
            out.write('\n')
    out.close()
    print('kexts with semantic string changes: %d / %d' % (n_changed, len(names)))
    print('-> %s' % OUT)


if __name__ == '__main__':
    main()
