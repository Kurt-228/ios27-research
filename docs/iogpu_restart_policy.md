# IOGPUFamily: механика GPURestart / fault escalation (macOS 27.0 host, разбор BootKC)

Дата: 2026-09-08. Хост: macOS 27.0 (26A5388g), arm64e. Девайс-цель: iPhone 15 Pro Max (A17 Pro, G16P) —
семейство AGX G16 общее с этим Mac (AGXG16G = M3/A17-класс), код IOGPUFamily общий по всем платформам.

## 1. Как извлечены бинари

`/System/Library/KernelCollections/` на macOS 27 нет; boot kernel collection лежит в Preboot:

```
/System/Volumes/Preboot/<vol>/boot/<hash>/System/Library/Caches/com.apple.kernelcaches/kernelcache
```

Извлечение:

```sh
kmutil emit-macho --no-authorization -B <путь к kernelcache>
# -> /tmp/KMUtilProducts/BootKernelCollection.kc  (125 MB, Mach-O arm64e, LC_FILESET_ENTRY на каждый кекст)
```

KC — единый Mach-O, сегменты кекстов перемешаны (все `__TEXT` вместе, все `__TEXT_EXEC` вместе и т.д.),
поэтому kmutil extract не подходит — нужен carve с rebasing file offsets. Скрипт:
`results/kc-extract/carve_fileset.py` (парсит LC_FILESET_ENTRY, переписывает fileoff LC_SEGMENT_64 /
секций / LC_SYMTAB / LC_DYSYMTAB / LC_FUNCTION_STARTS). Извлечены:

| kext | версия | файл |
|---|---|---|
| com.apple.iokit.IOGPUFamily | 162.10 | `results/kc-extract/com_apple_iokit_IOGPUFamily.macho` |
| com.apple.AGXG16G | 360.32.1 | `results/kc-extract/com_apple_AGXG16G.macho` |
| com.apple.AGXFirmwareKextG16GRTBuddy | 1 | `results/kc-extract/com_apple_AGXFirmwareKextG16GRTBuddy.macho` |

Полные символы сохранены (`nm` работает). Дизассемблер — `llvm-objdump -d` из Xcode-beta toolchain
(обычный `otool -tV` не видит `__TEXT_EXEC`). Полные листинги:
`results/kc-extract/{iogpu_full_disasm,agx_full_disasm,rtbuddy_disasm}.txt` (~80k / ~215k строк).
Строку по vmaddr читать: `results/kc-extract/rstr.py` (по KC) / `rstr2.py` (по carved mach-o).

## 2. Политика рестартов: `IOGPUCommandQueue::retireCommandBuffer`

`IOGPUCommandQueue::retireCommandBuffer(IOGPUEventFence*)` @ `0xfffffe000abbebf0`
(IOGPUFamily 162.10, адреса KTRR-сдвинуты в рантайме, смотреть по смещениям от базы кекста).

Разобран полностью (`results/kc-extract/retire_cb_disasm.txt`). Логика:

```c
// поля IOGPUCommandQueue (смещения):
// +0x43a u8  fRestartDenied          (флаг "deny submissions/ignore")
// +0x43c u32 fSubmissionCount        (счётчик сабмитов)
// +0x440 u32 fGPURestartCount        (счётчик рестартов)
// +0x488     IOGPUDevice*
// +0x490 u32 pid владельца очереди

void IOGPUCommandQueue::retireCommandBuffer(IOGPUEventFence *fence) {
    u32 type = *(u32*)((char*)fence + 0xc8);        // тип события на фенсе
    if (type == 2 || type == 3 || type == 0xb) {     // фенс, отмеченный рестартом
        u32 n = ++this->fGPURestartCount;            // [x19,#0x440]
        if (n == 2) {                                // ровно на ВТОРОМ рестарте — решение
            proc_pidinfo(this->pid, &pbi, 0x80);     // имя процесса
            if (this->device && this->device->noIgnoreOnGPURestart()) {
                verdict = "Immunity for";              // НЕ ставим deny-флаг
            } else {
                this->fRestartDenied = 1;              // [x19,#0x43a] = 1
                verdict = "Deny submissions/ignore";
            }
            IOLog("%s: %s app[%s] with %d GPURestarts in %d submissions.\n",
                  __func__, verdict, pbi.pbi_name, n, this->fSubmissionCount);
        }
    } else if (type == 0) {                          // обычное завершение сабмита
        u32 s = ++this->fSubmissionCount;            // [x19,#0x43c]
        if (s >= 0x3e9 /*1001*/) {                   // раз в ~1000 сабмитов
            if (!this->fRestartDenied) {
                this->fSubmissionCount = 0;
                if (this->fGPURestartCount) {
                    IOLog("%s: Clean slate for app[%s] with %d GPURestarts in %d submissions.\n",
                          __func__, pbi_name, n, s);
                    this->fGPURestartCount = 0;      // счётчики сброшены
                }
            }
            // если fRestartDenied == 1 — clean slate НЕ происходит никогда:
            // очередь остаётся в deny-состоянии перманентно
        }
    }
}
```

Строки (подтверждено по байтам в KC):
- fmt: `"%s: %s app[%s] with %d GPURestarts in %d submissions.\n"` @ 0x7a9285a
- fmt: `"%s: Clean slate for app[%s] with %d GPURestarts in %d submissions.\n"` @ 0x7a92816
- verdicts: `Immunity for` @ 0x7a8cdb4, `Deny submissions/ignore` @ 0x7a8cdc1

### Пороги (итог)

| параметр | значение | где |
|---|---|---|
| порог решения | **2 GPURestarta** на одну command queue (срабатывает ровно при n==2) | `retireCommandBuffer` |
| окно «clean slate» | каждые **1001 submissions** (тип события 0) | `retireCommandBuffer` |
| deny | флаг `+0x43a`; после него сабмиты всегда падают, clean slate отключён | `retireCommandBuffer` / `submit_command_buffers` |
| учёт | **строго per-command-queue**, идентификация по `pid` (имя через proc_pidinfo) | queue+0x490 |

### Кто получает Immunity

`IOGPUDevice::noIgnoreOnGPURestart()` @ `0xfffffe000aba9dc8`:

```c
return task_has_entitlement("com.apple.private.graphics-restart-no-kill")   // @ 0x7a8c154
    || (device->flags & 0x40000000);                                        // [dev+0x48]+0x23c
```

То есть immune: процессы с entitlement (`backboardd`, `SpringBoard`, `WindowServer` и т.п.) либо
device-flag (critical system device). Для них — "Immunity for app[backboardd]...", deny-флаг не
ставится, счётчики периодически сбрасываются clean slate. Для всех остальных — deny.

### Последствие deny: `IOGPUCommandQueue::submit_command_buffers`

@ `0xfffffe000abbb744`. Перед сабмитом:

```c
if (!IOGPU::thread_can_submit(this))  -> error 0xe00002d7 (0xe00002bc + 0x1b), flag +0x526 = 1
if (this->fRestartDenied /*+0x43a*/)  -> [this+0x520] = 4; return;  // submission denied
```

Код ошибки **4** пишется в поле `+0x520` очереди (внутренний enum ошибки command buffer;
пользовательский клиент получает failed submission → Metal device invalid → процесс обычно
сам падает/перезапускается). Также после рестарта: `IOGPUScheduler::getGPURestartSeed()` —
если seed изменился с прошлого сабмита, очередь получает vendor error code через vtable+0x1a8
(`setGPURestartClientErrorCode`, `guiltyForHardwareReset` — см. п.4).

**В IOGPUFamily 162.10 нет счётчика "N рестартов → kernel panic/terminate".** Эскалация
для не-привилегированного процесса = deny сабмитов (процесс умирает в юзерспейсе), для
привилегированного = ничего (immunity + чистый счётчик). Паник-пути — только отдельные
watchdog/boot-arg ветки (п.6).

## 3. Как фолт превращается в рестарт очереди владельца

Цепочка (вызывающая сторона — AGX, через vtable IOGPU; прямые вызовы отсутствуют поэтому
в дизасме IOGPUFamily caller'ов не видно — это by design):

```
BIF0 page fault (GPU MMU, аппаратно, асинхронно)
  → AGX: прерывание / firmware event (restart_reason "MMU interrupt")
  → AGXAccelerator::restartWorkQueue(AGXWorkQueue*)  @ 0x8ae6820  (AGXG16G, ~12k инструкций —
      "restart analysis": чтение fault-регистров, TA/CL/DM состояний, поиск виновного контекста;
      getMMUFaultInfo — см. п.5, getGuiltyChannel — ниже)
  → IOGPUScheduler::signalHardwareError(eRestartRequest, err) @ 0xaba6e24
      (битовая таблица активных запросов [sched+0x100] по 1<<request; логи
       "GPURestartSignaled/Enqueued/Dequeued/Begin/End", "Redundant hardware error";
       вызов нотификатора vtable+0x1f8, kdebug 0x85090088/0x8c)
  → IOGPU::restart() @ 0xabba5ac → vtable+0x168 на каждом workqueue
  → IOGPUWorkQueue::restart() @ 0xaba1b00  (разобрано, wq_restart_disasm.txt):
      - vtable+0x150: "ring is empty and all finished. Nothing to do." -> skip (или forced)
      - выбор stamp-записи из кольца [wq+0x48]/[wq+0x54]; берётся IOGPUEventFence x22=[ent+0x100]
        и владелец x20=[ent+0x60] (IOGPUCommandQueue)
      - vtable+0x180 (AGX: причина рестарта, строка; "GPU hang: %s" при типе hang)
      - в фенс пишется тип события: [fence+0xc8] = (state in {2,3,4}) ? 3 : 2
      - IOGPU::reportGPURestart(device, queue, cause_string, type, fence) @ 0xabb7cb0
      - device flag [+0x23c] bit1 (panic_on_gpu_hang) -> PANIC (п.6)
      - if ([+0x23c] bit1? нет — иначе) если есть task: имя процесса ->
        IOGPUFenceMachine::guiltyForHardwareReset(queue, type) @ 0xab954c4
        (иначе setGPURestartClientErrorCode)
      - проход по кольцу: все записи, чей [ent+0x60] == эта очередь, получают vtable+0x148
  → когда GPU фактически перезапущен, фенсы доезжают → retireCommandBuffer видит
    тип 2/3/0xb → ++fGPURestartCount владельца очереди (п.2)
```

`IOGPU::reportGPURestart` @ 0xabb7cb0 — чисто репортинг: собирает словарь
(`Application`, `AppPath`, `Graphics`+`Report` = "GraphicsReport", `Signature`,
`GPUSubmissionTraceID`, имена из proc_pidinfo), пишет в реестр / лог, без kill/panic.

### Почему рестартится очередь backboardd, а не наша

1. Рестарт атрибутируется **контексту (channel), чьи таблицы страниц использовались при
   фолте**, а не процессу, сгенерировавшему адрес. Определение виновного:
   - состояние акселератора `[AGXAccelerator + 0x18e34]` (анализ прошивки/регистров,
     заполненный `restartWorkQueue`);
   - `AGX3DWorkQueue::getGuiltyChannel()` @ `0x8ba6e44` (`guilty.txt`):
     если `[accel+0x18e34] == 0x80` — канал берётся по индексу из слота очереди
     (`[wq+0x220]`=544 или `[wq+0x218]`=536, выбор по сравнению свойства реестра
     `x20+0xd08`), иначе канал = результат vtable-запроса по state (объект сравнивается
     с OSSymbol из `[0xca79dd0]`); если виновный не найден — `panic()` (строка
     @ 0x7151aa8, "…no guilty channel…", IOGPUWorkQueue.cpp-аналог, line 1047 — отдельный
     паник-путь);
   - далее channel → его `IOGPUCommandQueue` → `queue->pid` (queue+0x490) → это и есть
     "app[backboardd]" в логе.
2. Если наш процесс уже мёртв, его channel/context уничтожены вместе с unmap'ом страниц.
   Доступ, вызвавший фолт (в т.ч. DMA/чтение из in-flight команд или общей IOSurface,
   которая после смерти процесса осталась замаплена только в системных контекстах),
   фолтится уже в контексте того, кто реально трогает память, — т.е. backboardd.
3. backboardd держит `com.apple.private.graphics-restart-no-kill` → immunity → очередь
   перезапускается сколько угодно раз, deny не ставится, kernel ничего не эскалирует.

## 4. Перечень причин рестарта и requestor ID

### restart_reason_desc (AGXG16G, порядок строк в бинаре, `strings` offset ~0x1057a)

```
0  timestamp timeout
1  firmware-detected lockup
2  firmware assert
3  CDM Kill timeout
4  FRG Kill timeout
5  MMU interrupt            <- BIF0/BIF1 page fault попадает сюда
6  unknown vendor lockup
7  progress timeout
8  timestamp timeout (unlocked)
9  timestamp timeout (locked)
10 blocked by IOFence (speculative)
11 blocked by IOFence
```

Оговорка: на iOS в gpuEvent (bug_type 284) встречается restart_reason=3 с описанием
"BIF0 page fault" — т.е. на iOS-сборке порядок/значения enum отличаются от macOS-списка
выше (либо iOS использует другой enum eRestartReason). Макет вывода в отчёте:
`"BIF%d page fault"` + поля `requestor`, `sideband`, `level`, `is_read`, `pm_protect`,
`address`, ключи `bif0_fault`/`bif1_fault` (AGXRestartReport, `com.apple.agx.restartreport`).

### Декодер фолта: `AGXAcceleratorG16::getMMUFaultInfo(u32, MMUFaultInfo&, bool)`

@ `0x8b5d5d4` (только bif0; w1!=0 → return 0). Читает 64-битный fault-регистр через HAL
vtable+0x11b8. Раскладка регистра:

```
bit 0      valid
bits 3:1   level (3 бита, кламп к 5)          -> индекс второй таблицы имён
bit 4      pm_protect                          -> MMUFaultInfo+0x21
bits 8:7   is_read (2 бита)                    -> MMUFaultInfo+0x20
bits 16:9  sideband/requestor index (8 бит)     -> MMUFaultInfo+0x14, индекс kG16BifRequestorInfo
bits 22:17 requestor id (6 бит)                -> MMUFaultInfo+0x10
bits 29:23 поле "unit#" (7 бит)                -> MMUFaultInfo+0x18
```

Второй регистр (адрес): `addr = raw << 6` → MMUFaultInfo+0x28.

### Таблица `kG16BifRequestorInfo` @ vmaddr 0x7f48700 (KC fileoff 0xf44700)

16-байтные записи `{u32 name_off_in_KC; u32 tag 0x200000; u64 id}`. Индекс — биты 16:9
fault-регистра. id ∈ {0,1,2,4} — групповая маска юнита (0 = INVALID). Полная таблица (G16):

| idx | id | имя | | idx | id | имя |
|---|---|---|---|---|---|---|
| 0 | 1 | DCMP0 | | 32 | 1 | DCMP2 |
| 1 | 1 | UL1C0 | | 33 | 1 | UL1C2 |
| 2 | 4 | CMP0 | | 34 | 4 | CMP2 |
| 3 | 1 | GSL1_0 | | 35 | 1 | GSL1_2 |
| 4 | 0 | INVALID04 | | 36 | 1 | GL2CC_META2 |
| 5 | 2 | VCE0 | | 37 | 2 | VCE2 |
| 6 | 2 | TE0 | | 38 | 2 | TE2 |
| 7 | 2 | RAS0 | | 39 | 2 | RAS2 |
| 8 | 2 | VDM0 | | 40 | 2 | VDM2 |
| 9 | 2 | PPP0 | | 41 | 2 | PPP2 |
| 10 | 4 | IPF0 | | 42 | 4 | IPF2 |
| 11 | 4 | IPF_CPF0 | | 43 | 4 | IPF_CPF2 |
| 12 | 4 | VF0 | | 44 | 4 | VF2 |
| 13 | 4 | VF_CPF0 | | 45 | 4 | VF_CPF2 |
| 14 | 4 | ZLS0 | | 46 | 4 | ZLS2 |
| 15 | 0 | INVALID0F | | 47 | 1 | GL2CC_META3 |
| 16 | 1 | DCMP1 | | 48 | 1 | DCMP3 |
| 17 | 1 | UL1C1 | | 49 | 1 | UL1C3 |
| 18 | 4 | CMP1 | | 50 | 4 | CMP3 |
| 19 | 1 | GSL1_1 | | 51 | 1 | GSL1_3 |
| 20 | 1 | GL2CC_META0 | | 52 | 0 | INVALID34 |
| 21 | 2 | VCE1 | | 53 | 2 | VCE3 |
| 22 | 2 | TE1 | | 54 | 2 | TE3 |
| 23 | 2 | RAS1 | | 55 | 2 | RAS3 |
| 24 | 2 | VDM1 | | 56 | 2 | VDM3 |
| 25 | 2 | PPP1 | | 57 | 2 | PPP3 |
| 26 | 4 | IPF1 | | 58 | 4 | IPF3 |
| 27 | 4 | IPF_CPF1 | | 59 | 4 | IPF_CPF3 |
| 28 | 4 | VF1 | | 60 | 4 | VF3 |
| 29 | 4 | VF_CPF1 | | 61 | 4 | VF_CPF3 |
| 30 | 4 | ZLS1 | | 62 | 4 | ZLS3 |
| 31 | 1 | GL2CC_META1 | | 63 | 0 | INVALID3F |

Индексы 24/25 (VDM1/PPP1 на GPC0) — совпадают с "sideband 24/25" из iOS-логов; индексы
208/209 в 8-битное поле не влезают — на iOS либо другой расклад регистра, либо requestor
там публикуется из другого источника (не различимо без iOS-кекста).

Вторая таблица @ 0x7f49700 (level, stride 8, имена: DCMP9/UL1C9/CMP9/GSL1_9) — декодер
instance/GPC-суффикса; `printBIFFaultSideband` в macOS-сборке — заглушка (return 1).

Прочее по фолт-путям:
- `AGXSecureGart::isPageFaultExpected(u64, u32)` @ 0x8b60044 — soft fault (UAT demand
  paging): делегирует в `AGXUAT::isPageFaultExpected`; неожиданный фолт здесь НЕ паникует,
  идёт в общий restart-путь.
- `AGXFirmware::processFirmwareInitiatedRecovery` / `processFirmwareTACommandsRecovery`
  @ 0x8ba9270/0x8ba9238 — на G16 это panic-заглушки (unimplemented → panic via
  `panic()` thunk 0x8bab470). Firmware-initiated recovery на этом поколении не используется.

## 5. Паник-пути (что реально доступно)

| путь | условие | где |
|---|---|---|
| **panic_on_gpu_hang=1** | boot-arg → IOGPU flags `+0x23c` bit1 (парсинг в `IOGPU::start` @ 0xabb6264) | `IOGPUWorkQueue::restart` @ 0xaba1d18: `tbnz [io+0x23c],#1` → cold.1 → `panic("\"GPU hang (boot-args contains \"panic_on_gpu_hang=1\")\" @%s:%d", "IOGPUWorkQueue.cpp", 767)`. **Подтверждено по байтам.** На стоковом iOS boot-arg не выставлен |
| wq_stall_panic_seconds | boot-arg → `IOGPU+0x2ac` | watchdog ожидания завершения рестарта; связан с паниками вида `Timeout SharedEvent: Cmd queue %p sleep port_name %d value: %08llx timed out waiting for prior signals!` (cold-паники в IOGPUWorkQueue/событиях). Потребитель — код ожидания фенсов, точную ветку не вычитывал |
| iogpu_panic_on_event_timeout | boot-arg → `+0x23c` bit 0x8000000 | паники по таймауту событий (shared event wait) |
| no guilty channel | объект состояния отсутствует при рестарте | `AGX3DWorkQueue::getGuiltyChannel` → `panic()` @ 0x8ba6f44 (строка @ 0x7151aa8, line 1047) — **внутренний assert, reachable при рассогласовании состояния анализа** |
| restart analysis asserts | рассогласование stamp/состояний | крупный `AGXAccelerator::restartWorkQueue` содержит ~287 вызовов panic-thunk 0x8bab470 (многие — OOM/assert-ветки) |
| SharedEvent timeout | `iogpu_panic_on_event_timeout` или stall-секунды | cold.1-style паники с "Timeout SharedEvent" |

Строка `"GPU hang: %s"` печатается в `IOGPUWorkQueue::restart` при получении строки-причины
от AGX (vtable+0x180) — сама по себе не паникует.

## 6. Выводы для дизайна спрея

Вопрос: сколько и каких фолтов доставить backboardd и есть ли путь от повторных рестартов
к panic / исполнению наших данных.

1. **Счётчик рестартов в kernel не эскалируется.** Для immune-процесса (backboardd) 2, 20
   или 2000 рестартов — одно и то же: "Immunity for app[...]", clean slate каждые ~1000
   сабмитов, никакого deny/panic. Лимита «N рестартов → kernel panic» в IOGPUFamily 162.10
   **нет**. Значит «достаточно доставить K фолтов backboardd» — тупиковая стратегия для
   panic; это чистый DoS дисплея (SpringBoard/backboardd рестартуют GPU, возможны фризы),
   kernel остаётся стабилен by design.
2. **Panic только через нарушение инвариантов рестарта**, не через счётчики:
   - заставить restart-analysis не найти виновный канал (`getGuiltyChannel` panic) —
     например фолт в момент, когда подозреваемый channel уже уничтожен (гонка смерти
     нашего процесса и обработки фолта — ровно та ситуация, что уже наблюдается, но
     channel при этом подменяется на backboardd'ов; нужна гонка, при которой candidate
     объект состояния == NULL);
   - panic-thunk'и внутри `AGXAccelerator::restartWorkQueue` (0x8bab470 x287) — assert/OOM
     ветки, reachable при искажённом состоянии фенсов/стемпов (кандидат на fuzzing,
     т.к. restart analysis читает состояние, записанное до рестарта);
   - watchdog-пути (Timeout SharedEvent) — если фолт повесить так, что рестарт не завершится
     (зависание completion фенса), сработает wq_stall_panic_seconds (дефолтное значение
     на устройстве не извлечено — требуется iOS-кекст/бут-аргументы).
   - `panic_on_gpu_hang=1` на стоковом iOS недоступен (boot-arg).
3. **Контролируемая порча / исполнение через этот путь — нет.** Весь fault→restart путь
   только читает fault-регистры и пересчитывает состояние; нашими данными там управляют
   разве что (а) address в fault-регистре (публикуется в репорт, 42 бита) и (b) имена/
   pid в логах. Это инфо-лик, не контроль.
4. Практический вывод: серия «GPURestart у backboardd» — индикатор, что фолт дошёл до
   kernel-обработки; как оружие — DoS уровня процесса (для не-entitled жертв deny работает:
   2 фолта на одну очередь → deny сабмитов → процесс без GPU). Для panic целиться нужно в
   гонки уничтожения channel/context с in-flight fault (getGuiltyChannel panic и assert'ы
   restartWorkQueue), а не в накопление счётчиков.

## 7. Что не разобрано / оговорки

- iOS-специфичные значения enum restart_reason (см. оговорку в п.4) — нужен iOS KC
  (например из ipsw/kcache) для точного маппинга restart_reason=3 → "BIF0 page fault".
- `getMMUFaultInfo` вызывается через HAL vtable (+0x11b8 внутри неё — чтение регистра);
  прямых caller'ов в BootKC нет (таблица HAL-указателей лежит в __LINKEDIT KC в формате
  {code,data}-пар — вероятно runtime-патчится; на iOS расклад может отличаться).
- Дефолты `wq_stall_panic_seconds`/`wq_timeout_sec` (поле IOGPU+0x2ac) на macOS не заданы
  (только boot-arg парсинг); iOS-значения не извлечены.
- Имя enum-значения ошибки сабмита «4» (queue+0x520) по публичным заголовкам на хосте
  не найдено; по контексту = permanent deny после "Deny submissions/ignore".
- Адреса привязаны к BootKC macOS 27.0 (build 26A5388g); на iOS 27 смещения будут другие,
  но структуры/строки/пороги — идентичны по коду семейства.
