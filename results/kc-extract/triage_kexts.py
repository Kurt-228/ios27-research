#!/usr/bin/env python3
"""Triage every kext in a kernel collection for App-Sandbox-reachable input
surface.

Rationale (docs/SPTM_research_journal_part19.md §142): the project has closed
every write primitive reachable from the App-Sandbox on the surfaces it
studied (11 kexts). The remaining population is ~700 kexts, and the one class
that still yields a jailbreak-grade primitive is a user-controlled length or
offset used without validation — a kernel heap overflow, or a UAF whose freed
content we control. Finding that class needs a reachability-prioritised
inventory, not blind fuzzing.

For each fileset entry we score the kext by signals that a handler exists and
accepts user input, and by the presence of validation-failure strings (which
mark the exact places where a length check may be missing or wrong). Nothing
here executes kernel code; it is a static filter to decide what deserves a
real disassembly pass.
"""
import struct, re, sys, json

KC = sys.argv[1] if len(sys.argv) > 1 else 'results/kc-extract/BootKernelCollection.kc'
data = open(KC, 'rb').read()
magic = struct.unpack_from('<I', data, 0)[0]
assert magic == 0xFEEDFACF, hex(magic)

# ---- enumerate fileset entries
off, entries = 32, []
for _ in range(struct.unpack_from('<I', data, 16)[0]):
    cmd, cmdsize = struct.unpack_from('<II', data, off)
    if cmd == 0x80000035:
        # LC_FILESET_ENTRY: entry.vmaddr first, entry.fileoff second.
        fileoff = struct.unpack_from('<Q', data, off + 16)[0]
        stroff = struct.unpack_from('<I', data, off + 24)[0]
        name = data[off + stroff:off + cmdsize].split(b'\x00')[0].decode(errors='replace')
        entries.append((fileoff, name))
    off += cmdsize
entries.sort()
entries = [(f, n) for f, n in entries if not n.endswith(('.development', '_kasan', '_debug'))]
print(f"kexts after dropping debug/kasan variants: {len(entries)}")

# ---- per-kext signals
RE_USERCLIENT = re.compile(rb'[A-Za-z_]{2,40}UserClient')
RE_DISPATCH   = re.compile(rb'externalMethod|dispatchExternalMethod|sMethodDesc')
RE_VALIDATE   = re.compile(
    rb'(invalid |out of range|too (big|large|small)|must be |exceeds |overflow|'
    rb'underflow|bad index|out of bounds|truncat)', re.I)
RE_MEMCPY     = re.compile(rb'memcpy|memmove|bcopy|OSCompareAndSwap|OSWrite')
COPY_HINT     = RE_MEMCPY
# surfaces the project already closed — deprioritised, not skipped
DONE = ('AppleM2ScalerCSC', 'IOSurface', 'IOGPU', 'AGXG16', 'AGXFamily',
        'IODART', 'T8110DART', 'SEPKeyStore', 'IOMobileFramebuffer', 'CLCD')

rows = []
# Filesets are laid out contiguously by fileoff; using "gap to the next entry"
# keeps each kext's string view inside its own image. (Using the segment max
# instead pulls in the collection-wide __PRELINK_TEXT pool, so every kext sees
# every UserClient name and the triage is meaningless.)
for i, (foff, name) in enumerate(entries):
    end = entries[i + 1][0] if i + 1 < len(entries) else min(len(data), foff + 16_000_000)
    end = min(end, foff + 16_000_000)
    if end <= foff + 2048:
        continue
    if foff + 32 > len(data) or struct.unpack_from('<I', data, foff)[0] != 0xFEEDFACF:
        continue
    blob = data[foff:end]
    if len(blob) < 4096:
        continue
    ucs = set(m.group().decode() for m in RE_USERCLIENT.finditer(blob))
    has_dispatch = bool(RE_DISPATCH.search(blob))
    val = len(RE_VALIDATE.findall(blob))
    copy = len(COPY_HINT.findall(blob))
    if not ucs and not has_dispatch:
        continue
    done = any(d in name for d in DONE)
    score = len(ucs) * 3 + (5 if has_dispatch else 0) + min(val, 40) / 10.0 + min(copy, 40) / 20.0
    if done:
        score *= 0.15
    rows.append((score, name, sorted(ucs)[:6], has_dispatch, val, copy, done))

rows.sort(reverse=True)
print(f"kexts with a userclient/dispatch surface: {len(rows)}\n")
print(f"{'score':>6}  {'done':>4}  {'val':>4} {'cp':>3}  kext")
for score, name, ucs, disp, val, copy, done in rows[:30]:
    print(f"{score:6.1f}  {'yes' if done else '-':>4}  {val:4d} {copy:3d}  {name}")
    if ucs:
        print(f"          classes: {', '.join(ucs)}")

json.dump([{'name': n, 'score': s, 'classes': u, 'validate': v, 'copy': c, 'done': d}
           for s, n, u, _, v, c, d in rows], open('results/kc-triage.json', 'w'), indent=1)
print("\nwrote results/kc-triage.json")