#!/usr/bin/env python3
"""AVE2 kext functional diff: b4 (24A5390f) vs release (24A435 RC == 24A437 FCS).

Kexts are fully stripped (no symtab in KC). We recover structure:
  - function boundaries  : LC_FUNCTION_STARTS (ULEB128 deltas)
  - function names       : AVE_* cstring literals passed to the log macro
                           (format "AVE %s: ..."), found via adrp+add xrefs
  - xrefs                : decoded from raw instruction bytes (no objdump text
                           parsing for immediates -> exact)
  - function-level diff  : name-anchored match, mnemonic-sequence comparison

Usage: ave2_funcdiff.py <b4.macho> <rel.macho> <b4.asm> <rel.asm> <outdir>
"""
import bisect
import os
import re
import struct
import sys

INSN_RE = re.compile(r'^([0-9a-f]{16}):\s+((?:[0-9a-f]{2} )*[0-9a-f]{2})\s*(.*)$')
NAME_RE = re.compile(r'^AVE_[A-Za-z0-9_]+$')


# ---------------------------------------------------------------- macho parse
def decode_fstarts(blob):
    out, p, cur = [], 0, 0
    while p < len(blob):
        shift = delta = 0
        while True:
            b = blob[p]
            p += 1
            delta |= (b & 0x7F) << shift
            shift += 7
            if not (b & 0x80):
                break
        cur += delta
        if cur == 0:
            break
        out.append(cur)
    return out


def parse_macho(path):
    data = open(path, 'rb').read()
    ncmds = struct.unpack_from('<I', data, 16)[0]
    sizeofcmds = struct.unpack_from('<I', data, 20)[0]
    off = 32
    segs, sects, fstarts = {}, [], None
    while off < 32 + sizeofcmds:
        cmd, cmdsize = struct.unpack_from('<II', data, off)
        if cmd == 0x19:
            segname = data[off + 8:off + 24].rstrip(b'\0').decode()
            vmaddr, vmsize, fileoff, filesize = struct.unpack_from('<QQQQ', data, off + 24)
            segs[segname] = (vmaddr, vmsize, fileoff, filesize)
            nsects = struct.unpack_from('<I', data, off + 64)[0]
            for s in range(nsects):
                so = off + 72 + s * 80
                sects.append(dict(
                    seg=data[so + 16:so + 32].rstrip(b'\0').decode(),
                    name=data[so:so + 16].rstrip(b'\0').decode(),
                    addr=struct.unpack_from('<Q', data, so + 32)[0],
                    size=struct.unpack_from('<Q', data, so + 40)[0],
                    off=struct.unpack_from('<I', data, so + 48)[0]))
        elif cmd == 0x26:  # LC_FUNCTION_STARTS
            dataoff, datasize = struct.unpack_from('<II', data, off + 8)
            fstarts = decode_fstarts(data[dataoff:dataoff + datasize])
        off += cmdsize
    base = min(v[0] for v in segs.values())
    exec_seg = segs.get('__TEXT_EXEC', segs['__TEXT'])
    fs = [base + d for d in (fstarts or [])]
    fs = [a for a in fs if exec_seg[0] <= a < exec_seg[0] + exec_seg[1]]
    return dict(data=data, segs=segs, sects=sects, fstarts=fs, base=base,
                exec_seg=exec_seg)


def build_strings(m):
    """addr -> text for cstring-like sections (null-terminated, printable)."""
    out = {}
    for s in m['sects']:
        if s['name'] not in ('__cstring', '__oslog', '__const', '__ustring'):
            continue
        blob = m['data'][s['off']:s['off'] + s['size']]
        a = s['addr']
        for chunk in blob.split(b'\0'):
            if 4 <= len(chunk) <= 200 and all(32 <= b < 127 for b in chunk):
                out[a] = chunk.decode('latin1')
                a += len(chunk) + 1
            else:
                a += len(chunk) + 1
    return out


# ---------------------------------------------------------------- arm64 decode
def is_adrp(w):
    return (w & 0x9F000000) == 0x90000000


def adrp_target(pc, w):
    immlo = (w >> 29) & 3
    immhi = (w >> 5) & 0x7FFFF
    imm = (immhi << 2) | immlo
    if imm & (1 << 20):
        imm -= 1 << 21
    return (pc & ~0xFFF) + (imm << 12), w & 0x1F


def add_imm(w):
    top = (w >> 24) & 0xFF
    if top == 0x91 or top == 0x11:  # ADD imm 64/32-bit (op=0, S=0)
        sh = (w >> 22) & 1
        imm12 = (w >> 10) & 0xFFF
        return (w >> 5) & 0x1F, w & 0x1F, imm12 << (12 if sh else 0)
    return None


# ---------------------------------------------------------------- asm parse
def parse_asm(path):
    """-> list of (addr, word, text). objdump: 'ADDR:\tbytes\tmnemonic\tops'"""
    insns = []
    with open(path, errors='replace') as f:
        for line in f:
            m = INSN_RE.match(line)
            if not m:
                continue
            addr = int(m.group(1), 16)
            bs = bytes.fromhex(m.group(2).replace(' ', ''))
            w = struct.unpack('<I', bs)[0]
            insns.append((addr, w, m.group(3).strip()))
    return insns


def xrefs(insns):
    """adrp(+add) pairs -> list of (insn_addr_of_add, resolved_target)."""
    out = []
    for i, (pc, w, text) in enumerate(insns):
        if not is_adrp(w):
            continue
        page, reg = adrp_target(pc, w)
        for j in range(i + 1, min(i + 8, len(insns))):
            pc2, w2, t2 = insns[j]
            if is_adrp(w2) and (w2 & 0x1F) == reg:
                break
            ai = add_imm(w2)
            if ai:
                rn, rd, imm = ai
                if rn == reg:
                    out.append((pc2, page + imm))
                    break
    return out


# ---------------------------------------------------------------- functions
class Func:
    __slots__ = ('start', 'end', 'insns', 'name', 'refs', 'strings', 'tags')

    def __init__(self, start):
        self.start = start
        self.end = start
        self.insns = []
        self.name = None
        self.refs = []
        self.strings = []
        self.tags = set()

    @property
    def size(self):
        return self.end - self.start


def build_funcs(m, insns):
    fs = sorted(set(m['fstarts']))
    funcs = [Func(a) for a in fs]
    idx = [f.start for f in funcs]
    strings = build_strings(m)
    straddrs = strings
    for ins in insns:
        i = bisect.bisect_right(idx, ins[0]) - 1
        if i < 0:
            continue
        funcs[i].insns.append(ins)
    for f in funcs:
        f.end = f.insns[-1][0] + 4 if f.insns else f.start
    for (pc, tgt) in xrefs(insns):
        i = bisect.bisect_right(idx, pc) - 1
        if i < 0:
            continue
        f = funcs[i]
        f.refs.append((pc, tgt))
        if tgt in straddrs:
            s = straddrs[tgt]
            f.strings.append((tgt, s))
            if NAME_RE.match(s):
                if f.name is None:
                    f.name = s
            if 'MCTF' in s:
                f.tags.add('MCTF')
            if 'SVE' in s:
                f.tags.add('SVE')
    for f in funcs:
        f.strings = list({t: s for t, s in f.strings}.items())
    return funcs, strings


def mnemonics(f):
    return [t.split()[0] if t else '?' for _, _, t in f.insns]


# ---------------------------------------------------------------- main
def main():
    b4_m, rel_m, b4_asm, rel_asm, outdir = sys.argv[1:6]
    os.makedirs(outdir, exist_ok=True)

    mb = parse_macho(b4_m)
    mr = parse_macho(rel_m)
    print('b4 : base=%#x funcs=%d' % (mb['base'], len(mb['fstarts'])))
    print('rel: base=%#x funcs=%d' % (mr['base'], len(mr['fstarts'])))

    fb, sb = build_funcs(mb, parse_asm(b4_asm))
    fr, sr = build_funcs(mr, parse_asm(rel_asm))
    print('b4 : named=%d/%d  strings=%d' % (sum(1 for f in fb if f.name), len(fb), len(sb)))
    print('rel: named=%d/%d  strings=%d' % (sum(1 for f in fr if f.name), len(fr), len(sr)))

    named_b = {f.name: f for f in fb if f.name}
    named_r = {f.name: f for f in fr if f.name}
    common = sorted(set(named_b) & set(named_r))
    only_b = sorted(set(named_b) - set(named_r))
    only_r = sorted(set(named_r) - set(named_b))
    print('common names=%d  only-b4=%d  only-rel=%d' % (len(common), len(only_b), len(only_r)))

    rep = open(os.path.join(outdir, 'ave2_funcdiff.txt'), 'w')
    rep.write('AVE2 functional diff: b4 24A5390f -> release 24A435(==24A437)\n')
    rep.write('funcs: b4=%d rel=%d; named: b4=%d rel=%d; common=%d\n\n'
              % (len(fb), len(fr), len(named_b), len(named_r), len(common)))

    rep.write('=== NEW named functions in release ===\n')
    for n in only_r:
        f = named_r[n]
        rep.write('  + %s  @%#x size=%d insns=%d tags=%s\n'
                  % (n, f.start - mr['base'], f.size, len(f.insns), sorted(f.tags)))
    rep.write('\n=== REMOVED named functions (b4 only) ===\n')
    for n in only_b:
        f = named_b[n]
        rep.write('  - %s  @%#x size=%d insns=%d tags=%s\n'
                  % (n, f.start - mb['base'], f.size, len(f.insns), sorted(f.tags)))

    rep.write('\n=== CHANGED common functions (mnemonic seq differs) ===\n')
    n_changed = 0
    n_same = 0
    changed_list = []
    for n in common:
        a, b = named_b[n], named_r[n]
        ma, mrr = mnemonics(a), mnemonics(b)
        if ma == mrr and len(a.insns) == len(b.insns):
            n_same += 1
        else:
            n_changed += 1
            changed_list.append((n, a, b))
            rep.write('  * %s: insns %d -> %d (bytes %d -> %d) tags=%s\n'
                      % (n, len(a.insns), len(b.insns), a.size, b.size,
                         sorted(b.tags | a.tags)))
    rep.write('\nsummary: identical=%d changed=%d (of %d common named)\n'
              % (n_same, n_changed, len(common)))
    print('common named: identical=%d changed=%d' % (n_same, n_changed))

    # MCTF / SVE cluster detail
    rep.write('\n=== MCTF/SVE-tagged functions (both versions) ===\n')
    for tag_funcs, ver, m in ((fb, 'b4', mb), (fr, 'rel', mr)):
        for f in tag_funcs:
            if f.tags & {'MCTF', 'SVE'}:
                rep.write('  [%s] %s @%#x size=%d insns=%d tags=%s\n'
                          % (ver, f.name or 'sub_%x' % (f.start - m['base']),
                             f.start - m['base'], f.size, len(f.insns),
                             sorted(f.tags)))
                for t, s in sorted(f.strings):
                    rep.write('        str %#x: %s\n' % (t - m['base'], s[:120]))

    # strings present only in one version (semantic filter)
    rep.write('\n=== semantic strings: NEW in rel / GONE from b4 ===\n')
    def semantic(s):
        return bool(re.search(r'[A-Za-z]{4}', s)) and not re.fullmatch(r'[^\w]+', s)
    new_s = sorted(s for s in set(sr.values()) - set(sb.values()) if semantic(s))
    gone_s = sorted(s for s in set(sb.values()) - set(sr.values()) if semantic(s))
    for s in new_s:
        rep.write('  + %s\n' % s[:160])
    for s in gone_s:
        rep.write('  - %s\n' % s[:160])

    # who references the gone/new semantic strings
    rep.write('\n=== xref owners of NEW/GONE semantic strings ===\n')
    def owners(strings, funcs, m, ss):
        want = set(ss)
        for f in funcs:
            for t, s in f.strings:
                if s in want:
                    rep.write('  %s <- %s @%#x\n' % (s[:80],
                              f.name or 'sub_%x' % (f.start - m['base']),
                              f.start - m['base']))
    owners(sr, fr, mr, new_s)
    rep.write('  -- gone --\n')
    owners(sb, fb, mb, gone_s)
    rep.close()
    print('-> %s' % os.path.join(outdir, 'ave2_funcdiff.txt'))

    # dump cluster disassembly for manual diff
    cluster = set()
    for f in fr:
        if f.tags & {'MCTF', 'SVE'}:
            cluster.add(f.name or f.start)
    for n, a, b in changed_list:
        if 'MCTF' in n or 'SVE' in n or 'Sched' in n:
            cluster.add(n)
    rep2 = open(os.path.join(outdir, 'ave2_mctf_cluster.asm.txt'), 'w')
    for c in sorted(cluster, key=str):
        rep2.write('\n######## %s ########\n' % c)
        pairs = []
        if c in named_r:
            pairs.append(('rel', named_r[c]))
        if c in named_b:
            pairs.append(('b4', named_b[c]))
        if isinstance(c, int):
            for f in fr:
                if f.start == c:
                    pairs.append(('rel', f))
            for f in fb:
                if f.start == c:
                    pairs.append(('b4', f))
        for ver, f in pairs:
            m = mr if ver == 'rel' else mb
            rep2.write('\n--- %s %s @%#x (%d insns) ---\n'
                       % (ver, f.name or 'sub', f.start - m['base'], len(f.insns)))
            for addr, w, text in f.insns:
                rep2.write('%#x\t%s\n' % (addr - m['base'], text))
    rep2.close()
    print('-> %s' % os.path.join(outdir, 'ave2_mctf_cluster.asm.txt'))


if __name__ == '__main__':
    main()
