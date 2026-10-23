#!/usr/bin/env python3
"""Generic function-level diff between two carved kext versions:
matches functions by byte-block similarity after normalizing absolute
addresses, ranks by similarity delta. Works without symbols (unlike
ave2_funcdiff.py which needs log-anchor names).

Usage: funcdiff_generic.py <old.macho> <new.macho> [topN]
"""
import struct
import sys


def parse_macho(path):
    data = open(path, 'rb').read()
    ncmds = struct.unpack_from('<I', data, 16)[0]
    sizeofcmds = struct.unpack_from('<I', data, 20)[0]
    off = 32
    segs = {}
    fstarts = []
    while off < 32 + sizeofcmds:
        cmd, cmdsize = struct.unpack_from('<II', data, off)
        if cmd == 0x19:
            segname = data[off+8:off+24].rstrip(b'\x00').decode()
            vmaddr, vmsize, fileoff, filesize = struct.unpack_from('<QQQQ', data, off+24)
            segs[segname] = (vmaddr, vmsize, fileoff, filesize)
        elif cmd == 0x26:
            dataoff, datasize = struct.unpack_from('<II', data, off+8)
            fstarts = decode_fstarts(data[dataoff:dataoff+datasize])
        off += cmdsize
    return data, segs, fstarts


def decode_fstarts(blob):
    out, p, cur = [], 0, 0
    while p < len(blob):
        shift = delta = 0
        while True:
            b = blob[p]; p += 1
            delta |= (b & 0x7F) << shift
            shift += 7
            if not (b & 0x80):
                break
        cur += delta
        if cur == 0:
            break
        out.append(cur)
    return out


def functions(data, segs, fstarts):
    """Return [(offset_in_exec, size, bytes)] for exec functions."""
    seg = segs.get('__TEXT_EXEC') or segs['__TEXT']
    vmaddr, vmsize, fileoff, filesize = seg
    base = min(v for (v, _, _, _) in segs.values())
    # fstarts are relative to image base (like ave2_funcdiff.py)
    fs_abs = [base + d for d in fstarts]
    fs = sorted(set(a for a in fs_abs if vmaddr <= a < vmaddr + vmsize))
    out = []
    for i, a in enumerate(fs):
        off = fileoff + (a - vmaddr)
        end = fileoff + (fs[i+1] - vmaddr) if i+1 < len(fs) else fileoff + filesize
        blob = data[off:end]
        if len(blob) < 8:
            continue
        out.append((a - vmaddr, end - off, blob))
    return out


def normalize(blob):
    """Blank out likely-immediate words (page addresses, branch targets) —
    keep opcode-class fingerprint: keep top byte masked 0x9F."""
    w = []
    for i in range(0, len(blob) - 3, 4):
        x = struct.unpack_from('<I', blob, i)[0]
        w.append(x & 0x9FFFFFFF)  # drop top cond/pac bits
    return tuple(w)


def sim(a, b):
    """O(ND) cheap: ratio of common normalized 4-gram bag."""
    from collections import Counter
    A = Counter(normalize(a[i:i+16]) for i in range(0, max(1, len(a)-15), 4))
    B = Counter(normalize(b[i:i+16]) for i in range(0, max(1, len(b)-15), 4))
    inter = sum((A & B).values())
    tot = max(1, sum(A.values()) + sum(B.values()))
    return 2.0 * inter / tot


def main():
    old_p, new_p = sys.argv[1], sys.argv[2]
    topn = int(sys.argv[3]) if len(sys.argv) > 3 else 40
    od, osegs, ofs = parse_macho(old_p)
    nd, nsegs, nfs = parse_macho(new_p)
    OF = functions(od, osegs, ofs)
    NF = functions(nd, nsegs, nfs)
    print('old funcs: %d, new funcs: %d' % (len(OF), len(NF)))

    # Match new→old by sliding anchor: for each new function, find best old by
    # local offset ± window using similarity.
    results = []
    used_old = set()
    nbase = {a: (s, b) for a, s, b in NF}
    for a_new, s_new, b_new in NF:
        best = (0.0, None)
        for a_old, s_old, b_old in OF:
            if a_old in used_old:
                continue
            if abs(a_old - a_new) > 0x40000:
                continue
            if abs(s_old - s_new) > max(64, s_new // 2):
                continue
            sc = sim(b_old, b_new)
            if sc > best[0]:
                best = (sc, a_old)
        results.append((a_new, s_new, best[0], best[1]))
        if best[1] is not None and best[0] > 0.6:
            used_old.add(best[1])
    results.sort(key=lambda r: (r[2], -r[1]))
    print('%-12s %-10s %-8s %-10s' % ('new_off', 'new_size', 'sim', 'old_off'))
    shown = 0
    for a_new, s_new, sc, a_old in results:
        if shown >= topn:
            break
        shown += 1
        print('%#-12x %-10d %-8.2f %#-12x %s' % (a_new, s_new, sc, a_old or 0,
              '<<< CHANGED' if sc < 0.85 else ''))


if __name__ == '__main__':
    main()