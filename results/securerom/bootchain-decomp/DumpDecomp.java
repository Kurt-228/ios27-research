// DumpDecomp.java — headless decompiler dump for boot-chain RE.
// Args: [0] = output file. Dumps every function's pseudocode with entry
// point and size; failures are printed as markers, never skipped silently.
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;

import java.io.FileWriter;
import java.io.PrintWriter;

public class DumpDecomp extends GhidraScript {
    @Override
    public void run() throws Exception {
        String outPath = getScriptArgs()[0];
        DecompInterface di = new DecompInterface();
        di.toggleCCode(true);
        di.openProgram(currentProgram);
        PrintWriter w = new PrintWriter(new FileWriter(outPath));
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        int n = 0, fail = 0;
        while (it.hasNext() && !monitor.isCancelled()) {
            Function f = it.next();
            DecompileResults r = di.decompileFunction(f, 60, monitor);
            w.println("=== FUNC " + f.getEntryPoint() + " sz=" + f.getBody().getNumAddresses()
                    + " :: " + f.getName());
            if (r.decompileCompleted() && r.getDecompiledFunction() != null) {
                w.println(r.getDecompiledFunction().getC());
            } else {
                fail++;
                w.println("// DECOMPILE-FAILED: " + r.getErrorMessage());
            }
            n++;
        }
        w.close();
        println("DumpDecomp: " + n + " functions, " + fail + " failures -> " + outPath);
    }
}
