# Ghidra headless postScript: apply symbol map + decompile every function to C.
# Works under PyGhidra (Ghidra 12) and Jython (Ghidra <=11): py2/py3-safe syntax.
# Env: GHIDRA_SYMS = csv "hexoffset,NAME"; GHIDRA_OUT = output .c file.
import os

from ghidra.app.decompiler import DecompInterface
from ghidra.program.model.symbol import SourceType
from ghidra.util.task import ConsoleTaskMonitor

SYMS = os.environ.get('GHIDRA_SYMS', '')
OUT = os.environ.get('GHIDRA_OUT', os.path.expanduser('~/ghidra_decomp.c'))

prog = currentProgram
base = prog.getImageBase()
st = prog.getSymbolTable()

if SYMS and os.path.exists(SYMS):
    n_sym = 0
    fh = open(SYMS)
    for line in fh:
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        off_s, nm = line.split(',', 1)
        try:
            addr = base.add(int(off_s, 16))
            st.createLabel(addr, nm.strip(), SourceType.USER_DEFINED, True)
            n_sym += 1
        except Exception:
            pass
    fh.close()
    print('applied %d symbols' % n_sym)

di = DecompInterface()
di.openProgram(prog)
mon = ConsoleTaskMonitor()
fm = prog.getFunctionManager()
out = open(OUT, 'w')
n_ok = 0
n_fail = 0
for f in fm.getFunctions(True):
    try:
        res = di.decompileFunction(f, 180, mon)
        if res.decompileCompleted():
            out.write('/* ===== %s @ off 0x%x ===== */\n'
                      % (f.getName(), int(f.getEntryPoint().subtract(base))))
            out.write(res.getDecompiledFunction().getC())
            out.write('\n\n')
            n_ok += 1
        else:
            n_fail += 1
    except Exception:
        n_fail += 1
out.close()
print('decompiled %d functions (%d failed) -> %s' % (n_ok, n_fail, OUT))
