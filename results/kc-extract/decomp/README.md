# Ghidra-декомпилы кекстов (v249)

Gzip-архивы `decompile_dump.java`-дампов (Ghidra 12.1.4 headless, brew-путь
`/opt/homebrew/opt/ghidra/libexec`). Формат: `FUN_<vmaddr> @ off <offset>` от
базы образа; `thunk_EXT_FUN_*` — внешние (XNU) символы по chained-fixup GOT.

| файл | кекст | сборка |
|---|---|---|
| gpu_b4.c.gz | IOGPUFamily | 24A5390f (27.0b4 — устройство) |
| gpu_435.c.gz | IOGPUFamily | 24A435 (27.0 GM) |
| gpu_B115.c.gz | IOGPUFamily | 24B115 (27.2) |
| agx_435.c.gz / agx_B115.c.gz | AGXG16P | 27.0 / 27.2 |
| disp_435.c.gz / disp_B115.c.gz | AppleDisplayCrossbar | 27.0 / 27.2 |
| iomg_435.c.gz / iomg_B115.c.gz | IOMobileGraphicsFamily-DCP | 27.0 / 27.2 |
| kernel_435.c.gz / kernel_B115.c.gz | XNU kernel | 27.0 / 27.2 |
| ave2_b4.c.gz / ave2_rel.c.gz / ave2_b115.c.gz | AppleAVE2Driver | b4 / 27.0 / 27.2 (§231 CVE-2026-84607) |
| ave_venc.c.gz | ave.videoencoder | 24A5390f |

Карвы для повторной загрузки: `results/kc27-{b4,435,B115}/kexts/*.macho`
(в `.gitignore`). Ключевые находки по этим декомпилам: §231 (AVE2 MCTF),
§240 (IOGPU parent-suballoc bounds), §241 (гейты FUN_24b11e4).