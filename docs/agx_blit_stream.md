# AGX blit copy — device stream: цепочка построения, байтовая карта, план payload-фазза

Дата: 2026-09-08. Источники: userland-драйвер **AGXMetalG16G_B0** (macOS 27.0 26A5388g,
dyld_shared_cache_arm64e, извлечён через `ipsw dyld extract --objc`; кэш-формат
arm64e: указатели в DATA-секциях закодированы как `{lo32 | (VA[32]<<31); tag}`,
тег 0x00100000), capture-эмпирика из `docs/SPTM_research_journal_part16.md`
§77–79/82–84, эталон `docs/agx_cmd_template.h`.

## 0. Главный вывод

**Device stream blit copy на G16 — это не «blit-пакет», а CDM-compute диспатч.**
Оба пути (uber `BlitContext::copyBufferToBufferImpl` и `LegacyBlitContext::
copyBufferToBuffer`) сходятся на `BlitDispatchContext::blitCDMBuffer`, который:

1. пишет CDM control stream (cursor `encoderObj+0x308`) — токены вида
   **0x60000160**, compute pass records;
2. резервирует per-resource токены (0x80 байт, тип из таблицы
   `[ctx+0x948 + kind*8]`, битмап ресурсов `[ctx+0x880]`);
3. формирует **BlitComputeArgumentTable** и **BlitComputeDriverTable** на стеке
   и кладёт их в DataBufferAllocator (pool window) через
   `BlitUSCStateLoader::emitComputeProgramVariantArguments`;
4. исполняет встроенный MSL-kerнел **`copy_buffer`** (pipeline `blit-compute-sl`).

Т.е. «0x130-байтный packed AGX packets» регион из capture (reg_10d460000) —
это **CDM control stream, лежащий в pool window** (та же 0x1_000138000-область,
что и pool-слоты v89), а src/dst GPUVA едут в **driver table** (пул-блоб),
на которую control stream ссылается 32-битными pool-relative offset'ами
(тег 0x100 в high dword — см. part13 §71). Это сходится со ВСЕЙ эмпирикой:
kcmd не содержит GPUVA (§82), buffer VA в pool-таблицах, маленькие копии (<0x4000)
идут другим путём (§84).

## 1. Цепочка построения (все адреса — AGXMetalG16G_B0, VA в кэше)

| Уровень | Функция | Адрес |
|---|---|---|
| ObjC entry | `-[AGXG16GFamilyBlitContext copyFromBuffer:sourceOffset:toBuffer:destinationOffset:size:]` | 0x215001630 |
| C++ disp. | `switchContextIfNeededImpl` (выбор uber/legacy по глобалам `UseLegacyBlitVariants`/`disable_uber_blit_variants`/`DisableMSLBlit`, байт `[devState+0x4326]^1`, глобал-байты 0x2741ba7d8/0x2741ba7e0) | 0x215012068 |
| uber | `AGX::BlitContext<HAL200,CommandEncoding>::copyBufferToBufferImpl(src,srcOff,dst,dstOff,size,bool)` | 0x215013b30 |
| legacy | `AGX::LegacyBlitContext<HAL200>::copyBufferToBuffer(...)` | 0x215016838 |
| общий | `AGX::BlitDispatchContext<HAL200>::blitCDMBuffer(src,srcOff,dst,dstOff,size,aux,auxSize,legacy)` | 0x215028fd8 |
| MSL-вариант | `AGX::MSLBlitDispatchContext<HAL200,CommandEncoding>::blitCDMBuffer` (другой вход, тот же смысл) | 0x2150316a4 |
| ресурсы | `BlitDispatchContext::bindComputeResources(&src,&dst,flag)` | 0x215028678 |
| pass | `endPreviousBlitCommand` / `beginComputePass` | 0x2150264bc / 0x21502ba60 |
| аргументы | `BlitUSCStateLoader::emitComputeProgramVariantArguments(pool, variant, argTable, driverTable, …)` | 0x21503d164 |

Аргументы ObjC-метода складываются в 0x30-байтные стековые блоки
`{buf, offset}` / `{buf, offset}` / `{size}` и уходят в C++ по указателю —
т.е. на границе ObjC→C++ никакой маршаллинг байтов команды не происходит,
вся кодировка — внутри blitCDMBuffer.

## 2. Что пишется в stream (восстановлено из blitCDMBuffer, 0x215028fd8)

### 2.1 CDM control stream (cursor `encoder+0x308`, буфер из DataBufferAllocator)

- `agxaReserveCDMTokenSpace(pool=&enc+0x18, type=0x16(22), …)` → cursor+4;
- **dword 0x60000160** — первый известный токен стрима (CDM control, opcode 0x6
  в верхних битах). Записи дальше идут через тот же механизм резерва;
- `beginComputePass` пишет pass-заголовок (функция 0x21502ba60, не разобрана
  построчно — следующий срез для реверса);
- завершение — `endPreviousBlitCommand`.

### 2.2 Параметры диспатча (kDQuadParamTable @ 0x2158398a8)

Индекс = min(clz(|srcOff−dstOff|), 4):

| idx | значение |
|---|---|
| 0 | 0x100 |
| 1 | 0x200 |
| 2 | 0x300 |
| 3 | 0x800 |
| 4 | 0xf00 |

w28 = table[idx] — элемент разбиения размера (ширина CDM-диспатча/размер чанка
в линиях копирования). Значение 0 → ветка без диспатча.

### 2.3 BlitComputeDriverTable (стек-структура sp+0x108, ~0x80 байт → pool)

Разметка локальной копии (офсеты от базы таблицы):

| Оффсет | Поле | Источник |
|---|---|---|
| +0x0c | u32 threadgroups = **min(size, 0x400)** | 0x2150295a4-0x2150295bc |
| +0x10 | {1,1} (threadsPerTG) | movi.2s #1 |
| +0x18 | u32 **size** (общий размер копии) | str w21 |
| +0x24 | {1,1} | stur d0 — **уточнение N4 (v127): {1,1} на самом деле @ +0x1c, zeros(8) @ +0x44** (`device_stream_builder.md` §2.2.3) |
| +0x38 | u32 **size** (повтор) | str w21 |
| +0x3c | u32 0 | |
| +0x4c | zeros(8) | — **N4 (v127): фактически @ +0x44** |
| +0x60 | **{srcBase, dstBase}** — базовые адреса (pool VA/alloc-cursor) | ldp [sp,#0x40] → stp [sp,#0x168] |
| +0x74 | u32 `variant->0xca8 << 2` (поле программы) | 0x2150295e8 |

ArgumentTable (sp+0x50, 0x30 байт + PAC'd block-ptr @ +0x00 с дивером 0xdce9)
обнуляется и заполняется рядом; точную раскладку не закрыл (см. §6 динамика).

Эмпирический факт v89/v91 подтверждает модель: GPUVA пары (src,dst) в pool-слотах
по ~0x10-byte шагу — это поле +0x60 driver table (и его копии), rid-patч резиденции
менял именно их отражение.

### 2.4 Правила разбиения (копипаста из кода)

- Выравнивание: 16 байт (and …,#0xf / 0x10);
- Чанк: до **0x8000**, округление `& ~0xf`;
- threadgroups копии: min(size, 0x400) — один диспатч покрывает ≤0x400 линий,
  бОльшие размеры — цикл по чанкам (overlap-ветка copyBufferToBufferImpl,
  0x215013bac+);
- overlap (диапазоны [srcOff,srcOff+size) ∩ [dstOff,dstOff+size) ≠ ∅):
  ветка chunk-loop; non-overlap: прямой blitCDMBuffer.

## 3. Сопоставление с capture-эталоном

- `agx_cmd_template.h` (0x358 байт, magic 0x10000 @ +0, subtype 3) — это
  **compute-dispatch inner command общего вида**, НЕ blit: blit в kcmd таких
  команд не пишет (part13 §70.2: magic 0x10000 при blit-submit в памяти нет).
  Blit живёт в CDM control stream + pool tables, kcmd только ссылается.
- 0x130-байтный регион (reg_10d460000) — сегмент CDM control stream: начинается
  с токенов вида 0x60000160, дальше pass/dispatch записи с pool-offset ссылками.
  Подтверждение: паттерн «адреса через 32-бит offsets, tag 0x100 в high dword»
  (§71) — ровно то, как control stream ссылается на driver/argument tables.
- pool-слоты 0x1_000139480..498 (v89) — записи {GPUVA} из driver table +0x60.
- data segment reg_104f70000 (паттерн заливки) — окно pool с argument/driver
  tables и/или USC constants.

## 4. Достижимость для in-place payload-патча

| Регион | GPUVA | Запись из процесса |
|---|---|---|
| kcmd shmem | 0x1_000… (id 2) | RW ✓ (патчится с v89) |
| seglist shmem (id 1) | 0x1_000… | RW ✓ |
| pool windows (driver tables, слоты) | 0x1_000138000+ | **RW ✓ — v89 WRITE CONFIRMED** |
| **CDM control stream (наш 0x130-регион)** | 0x10_d460000 (GPU-view) / pool window | pool-страницы принимают запись (та же shmem, что и слоты v89); прямая запись по GPUVA 0x10_* — DROP (v91) |
| чужие/служебные страницы | 0x1_0000c000+ и т.п. | DROP (v91 write-матрица) |

Вывод: **патчить payload надо в pool window через её CPU-мэппинг** (как v89
патчил слоты), а не по GPUVA и не в kcmd. Control stream достижим, ЕСЛИ его
страница в том же shmem-pool окне (весьма вероятно: резерв через тот же
DataBufferAllocator); динамически проверить — см. §6.

## 5. Перспективные мутации (payload-фазз)

Через in-place патч pool window между endEncoding и commit:

1. **GPUVA пары (driver table +0x60)** — уже работает (v89 write-зонд, v90
   read-зонд). Дальше: OOB read чужих регионов, адреса вне residency (промах
   бесплатен — v89b).
2. **size (+0x18/+0x38)** — размер копии: значения > буфера → OOB read/write
   за пределами ресурса на GPU. Кандидат №1 для следующего прогона.
3. **threadgroups (+0x0c)** — min(size,0x400) нормализуется на CPU, но патч
   ПОСЛЕ encode обходит нормализацию → dispatch с бОльшим числом threadgroups.
4. **таблица копипасты kDQuadParamTable** (idx 0..4 → 0x100/0x200/0x300/0x800/0xf00)
   — патч через pool непрямой; проще влиять выбором srcOff/dstOff (разница
   задаёт idx) — известная техника калибровки.
5. **токен control stream 0x60000160** — если страница stream'а доступна:
   мутации opcode (верхний нибл 0x6) → неизвестные CDM-опкоды прямо на GPU.
   Это и есть «фазз пейлоада, не envelope»; осторожно — читается firmware'м.
6. auxBuffer (x6/x21 аргументы, "FillValue") — заливка паттерном: значение
   байта-заполнителя и расширенные размеры.

## 6. Что НЕ закрыто статически + минимальный динамический план

Не закрыто:
- построчная разметка 0x130-региона (порядок токенов после 0x60000160;
  beginComputePass 0x21502ba60, reserve-helpers 0x21500c624 не дизассемблированы
  до конца);
- точная раскладка ArgumentTable (sp+0x50) и финальный формат записи в pool
  (endian/tag, кто из CPU VA превращается в GPUVA +0x100-tag offsets);
- формат USC state records для copy_buffer.

Динамика (по возрастанию ценности):
1. **Diff-capture одного blit copy**: endEncoding → dump pool window (CPU VA из
   `_commandBufferStorage` + seglist) → commit → dump. Разница даст точную
   байтовую карту 0x130 control stream + driver table для copy 0x10000
   (калиброванный случай v89). Искать якорь 0x60000160.
2. Patч size (+0x18/+0x38) на size+N → наблюдать запись за пределами буфера B
   (можно класть маркер-страж на следующей странице pool).
3. Проверить доступность страницы control stream: записать маркер в pool VA,
   соответствующий 0x10_d460000-view, readback через второй blit.
4. Мутация opcode-токена одним битом за прогон (σ-модель: crash/no-crash),
   список CDM-опкодов из kext-дизасма (agx_full_disasm.txt, CDM-парсеры).

## 7. Артефакты

- `/tmp/ipsw` (go-ipsw v3.1.712), извлечение:
  `ipsw dyld extract <DSC> <dylib> --objc -o out` — корректно разворачивает
  arm64e PAC-указатели и даёт 18.5k символов (методы ObjC + C++ mangled).
- Извлечённый драйвер: `/tmp/ipsw_out/AGXMetalG16G_B0` (символы + слайд).
- Формат указателей DATA-секций кэша (самостоятельно): cell = 8 байт,
  `VA = (lo32 & 0x7fffffff) | 0x200000000` при bit31=1 (иначе — 32-бит форма,
  семантика не закрыта), hi32 = тег (0x00100000 classlist/got/selrefs,
  0x00200000 cfstring.charPtr).
- Ключевые строки: `agxa_blit_legacy_template.hpp` @ 0x215857a46,
  `blit-compute-sl` @ 0x21585a275, `copy_buffer` @ 0x2158528df,
  `kDQuadParamTable` @ 0x2158398a8.
