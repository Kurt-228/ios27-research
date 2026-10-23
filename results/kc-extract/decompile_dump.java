// Ghidra headless postScript: apply symbol map (GHIDRA_SYMS csv "hexoff,name")
// and decompile every function to C (GHIDRA_OUT output file).
//@category Analysis
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.symbol.SourceType;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;

public class decompile_dump extends GhidraScript {

    @Override
    protected void run() throws Exception {
        String syms = System.getenv("GHIDRA_SYMS");
        String outPath = System.getenv("GHIDRA_OUT");
        if (outPath == null) {
            outPath = System.getProperty("user.home") + "/ghidra_decomp.c";
        }

        Address base = currentProgram.getImageBase();

        if (syms != null) {
            int n = 0;
            try (BufferedReader br = new BufferedReader(new FileReader(syms))) {
                String line;
                while ((line = br.readLine()) != null) {
                    line = line.trim();
                    if (line.isEmpty() || line.startsWith("#")) continue;
                    int c = line.indexOf(',');
                    if (c < 0) continue;
                    try {
                        Address a = base.add(Long.parseLong(line.substring(0, c), 16));
                        currentProgram.getSymbolTable()
                            .createLabel(a, line.substring(c + 1).trim(),
                                         SourceType.USER_DEFINED);
                        n++;
                    } catch (Exception e) {
                        // skip bad rows
                    }
                }
            }
            println("applied " + n + " symbols");
        }

        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);
        PrintWriter out = new PrintWriter(new FileWriter(outPath));
        int ok = 0;
        int fail = 0;
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        while (it.hasNext()) {
            Function f = it.next();
            try {
                DecompileResults res = di.decompileFunction(f, 180, monitor);
                if (res.decompileCompleted()) {
                    out.println("/* ===== " + f.getName() + " @ off 0x" +
                        Long.toHexString(f.getEntryPoint().subtract(base)) + " ===== */");
                    out.println(res.getDecompiledFunction().getC());
                    out.println();
                    ok++;
                } else {
                    fail++;
                    if (fail <= 3) {
                        println("decompile FAILED: " + f.getName() + " @ " +
                            f.getEntryPoint() + " : " + res.getErrorMessage());
                    }
                }
            } catch (Exception e) {
                fail++;
            }
        }
        out.close();
        println("decompiled " + ok + " functions (" + fail + " failed) -> " + outPath);
    }
}
