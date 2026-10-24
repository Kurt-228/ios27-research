#!/usr/bin/env python3
"""mk_corrupt_db.py v2 — structured corruption variants of StoreSystem.db.
ВСЕ ПОЛЯ SQLITE — BIG-ENDIAN. Оффсеты header-страницы:
  0..15 magic; 16..17 pagesize(BE u16, 1=65536); 18 write ver; 19 read ver;
  20 reserved; 21..23 max/min/payload frac; 24..27 change counter;
  28..31 in-header db size (pages); 32..35 freelist trunk page;
  36..39 freelist count; 40..43 schema cookie; ...
b-tree страница (после 100-байт заголовка на page1):
  +0 type; +1..2 first freeblock; +3..4 ncells; +5..6 cell content start;
  +7 frag; +8.. page1: rightmost child (interior); далее cell pointer array.
Usage: mk_corrupt_db.py <valid.db> <outdir>"""
import sqlite3, struct, sys, os

src, outdir = sys.argv[1], sys.argv[2]
os.makedirs(outdir, exist_ok=True)

base = bytearray(open(src, 'rb').read())
pagesize = struct.unpack_from('>H', base, 16)[0]
if pagesize == 1: pagesize = 65536
print(f"page size: {pagesize:#x}, размер: {len(base)}")

def save(d, name):
    p = os.path.join(outdir, name)
    open(p, 'wb').write(bytes(d))
    verdict = "?"
    try:
        con = sqlite3.connect(p)
        rows = con.execute("PRAGMA integrity_check").fetchall()
        con.close()
        verdict = f"integrity: {rows[0][0] if rows else '?'}"
    except Exception as e:
        verdict = f"sqlite err: {e}"
    print(f"{name}: {verdict}")
    return p

# v1: cell count на 2-й странице = 0xFFFF (OOB walk по cell pointer array)
d = bytearray(base)
if len(d) >= pagesize + 5:
    struct.pack_into('>H', d, pagesize + 3, 0xFFFF)
save(d, "v1-cellcount2.db")

# v3: freelist trunk = страница 2, count = huge
d = bytearray(base)
struct.pack_into('>I', d, 32, 2)
struct.pack_into('>I', d, 36, 0x7FFFFFFF)
save(d, "v3-freelist-loop.db")

# v4: page-size = 0x0101
d = bytearray(base)
struct.pack_into('>H', d, 16, 0x0101)
save(d, "v4-pagesize.db")

# v5: in-header db size = huge
d = bytearray(base)
struct.pack_into('>I', d, 28, 0xFFFFFF)
save(d, "v5-dbsize-huge.db")

# v6: cell content start на 2-й странице = 1 (низ → OOB при записи)
d = bytearray(base)
if len(d) >= pagesize + 7:
    struct.pack_into('>H', d, pagesize + 5, 0x0001)
save(d, "v6-cellcontent-low.db")

# v7: payload-fragment поля заголовка = 0 (невалидно: min payload 0)
d = bytearray(base)
d[21] = 0; d[22] = 0; d[23] = 0
save(d, "v7-payloadfrac-zero.db")

# v8: schema cookie = huge + text encoding = 0xFFFFFF
d = bytearray(base)
struct.pack_into('>I', d, 40, 0xFFFFFF)
struct.pack_into('>I', d, 56, 0xFFFFFF)
save(d, "v8-encoding-invalid.db")
print("готово")
