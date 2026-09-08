# AGX queue execution: статический разбор iOS 27b4 — почему сабмит проходит, а GPU не пишет

Документ отвечает на пять вопросов постановки:

1. Разбор блоба 0x410 селектора 6 (создание command queue).
2. Что Metal делает до/после queue create (macOS-эталон) и как это ложится на iOS-нумерацию.
3. Где решается «no-op vs реальное исполнение» и что означает completion-статус `{0,5}`.
4. Вердикт: минимальный исполняемый набор из App Sandbox и точное место гейта.
5. Чеклист для фаззера.

Ключевые поправки к журналу (part13/part16) — из-за ошибки декодирования указателей
таблиц в прошлых сессиях селекторы 40–45 были идентифицированы неверно
(см. «Исправление карты селекторов»). Все адреса ниже перепроверены.

---

## 0. Методология и источники

- **Цель:** iOS 27.0b4, A17 Pro (T8122), стрипнутый kernelcache `results/kc27/`
  (`com_apple_iokit_IOGPUFamily.macho`, TEXT_EXEC @ `0xfffffff009d525d0`;
  `com_apple_AGXG16P.macho`).
- **Символьный референс:** macOS 27.0 (T8132) BootKernelCollection
  `results/kc-extract/*.macho` (IOGPUFamily и AGXG16G с полной символикой).
  Нумерация селекторов macOS ≠ iOS; сверка только по сигнатурам
  (sin/strIn/sout/strOut) и семантике.
- **Декодер указателей таблиц.** Ячейки DATA — chained-fixup auth-указатели,
  декодируются `va = 0xfffffff000000000 | ((raw & 0xffffffff) ^ K)`.
  Раньше использовался K=0x0b000000, что давало правдоподобные, но **ложные**
  адреса (сдвиг ровно на 0x4000 попадал в соседние функции). Истинный ключ для
  iOS IOGPUFamily: **K = 0x0b00c000** (выведен по 19 функциям с известными
  адресами, см. ниже). После исправления все 56 записей таблицы указывают на
  валидные прологи (pacibsp/bti.c).
- **Ground truth для имён.** В iOS-кекст не вырезаны `__FUNCTION__`-строки вида
  `static IOReturn IOGPUDeviceUserClient::s_*(...)`; 19 таких строк xref'нуты
  на функции и сопоставлены с записями таблицы — это зафиксировало K и имена.

---

## 1. Разбор блоба 0x410 (sel6, new_command_queue)

`IOGPUCommandQueue::init(IOGPU*, IOGPUDevice*, IOGPUDeviceNewCommandQueueArgs*)`:
macOS @ `0xfffffe000abbb034`, **iOS-аналог @ `0xfffffff009d872c0`**
(построчно верифицирован: kalloc 0x2a8, идентичные ivar-офсеты).

Поля входной структуры (x22 = args, размер 0x410):

| Офсет | Тип | Куда | Смысл |
|---|---|---|---|
| +0x000 | 0x400 байт | `memcpy(q+0x10, args, 0x400)` | process path (первые ~64 байт — путь исполняемого процесса; заполняется userland'ом, в трейсе — полный path) |
| +0x400 | u32 | `[q+0x450]` | **version** (НЕ priority; журнальное «priority=2» = version=2). Гейт: **version < 5**, иначе init fail |
| +0x404 | byte | `[q+0x439]` | флаг A |
| +0x405 | byte | `[q+0x527]` | флаг B |
| — | const qword | `[q+0x448]` | константа из __TEXT; macOS-значение: u32[2,3,1,4] → `[q+0x44c]=3` (см. §4, формат v5) |
| — | — | `[q+0x444]=1` | флаг инициализации |
| — | obj | `[q+0x488]=device`, `[x21+0x60]→[q+0x490]`, `getGPUTask→[q+0x480]` | привязки |
| — | obj | `[[q+0x530]+0x158]→[q+0x538]` | scheduler link |
| — | str | `[q+0x550]` | traceTag; **выход селектора = {qid, traceTag}** |

Затем: `IOGPUEventMachine::initEvent(+0x498/+0x4d8)`, `bzero(+0x588, 0x40)`,
`IOGPUScheduler::registerCommandQueue([q+0x538], q) → [q+0x424]`.

Гейт размера — в обёртке `s_new_command_queue` (macOS
`0xfffffe000ab889a0`; **iOS @ `0xfffffff009d59280`**, проверено):
`structureInputSize >= 0x408` и `== [obj+0x2b8]` (== 0x410), иначе ошибка;
также лимит числа очередей (`[obj+0x30]` vs `[[uc+0xf8]+0x288]` → 0xe0078001).
Коды ошибок размера/аргументов: macOS 0xe00002c2, **iOS 0xe00002bd**
(литералы проверены на iOS @ 0x9d59380/0x9d593bc).

Цепочка создания очереди на iOS:
`externalMethod` (`0x9d5bd00`, cmp w1,#0x37; таблицы `0x814d8c0` / `0x814de00`
по флагу `[uc+0x103]&1`) → `s_new_command_queue` (гейты/лимиты/размер) →
виртуальная фабрика очереди → `AGXCommandQueue::init` (в AGXG16P:
PM-config, `AGXWorkQueue::init`) → `IOGPUCommandQueue::init` (блоб выше) →
`IOGPUScheduler::registerCommandQueue`.

---

## 2. Исправленная карта селекторов iOS (type 0x1 / 0x100001)

`IOGPUDeviceUserClient`, 56 селекторов (stride 0x18). Полные имена для
19 методов — из `__FUNCTION__`-строк; остальные — по сигнатурному
соответствию macOS-таблице (совпадают sin/strIn/sout/strOut и порядок).

| sel | функция iOS | имя | sin/strIn/sout/strOut |
|---|---|---|---|
| 0–5 | 0x9d58f0c… | get_config / get_name / get_event_machine / get_surface_info / get_current_trace_filter / get_device_info | 0/0/0/64,64,536,16,16,16* |
| 6 | 0x9d59280 | **s_new_command_queue** | 0/VAR/0/16 |
| 7 | 0x9d59580 | s_delete_command_queue | 1/0/0/0 |
| 8 | 0x9d59610 | **s_new_resource** | 0/VAR/0/VAR |
| 9 | 0x9d59794 | s_delete_resource | 1/0/0/0 |
| 10 | 0x9d597a8 | s_finish_object_event | 2/0/0/0 |
| 11 | 0x9d597c0 | s_set_resource_purgeable | 2/0/1/0 |
| 12 | 0x9d597e0 | **s_create_shmem** | 2/0/0/16 |
| 13 | 0x9d59808 | s_destroy_shmem | 1/0/0/0 |
| 14 | 0x9d5981c | **s_create_notificationqueue** | 2/0/0/16 |
| 15 | 0x9d5983c | s_destroy_notificationqueue | 1/0/0/0 |
| 16 | 0x9d59850 | s_get_shared_info | 0/0/0/8 |
| 17 | 0x9d59860 | s_create_mtlfence | 0/0/0/4 |
| 18 | 0x9d59870 | s_destroy_mtlfence | 1/0/0/0 |
| 19 | 0x9d59884 | s_create_mtlevent | 1/0/0/24 |
| 20 | 0x9d5989c | s_destroy_mtlevent | 1/0/0/0 |
| 21 | 0x9d598b0 | s_get_memory_data | 0/0/0/48 |
| 22 | 0x9d598c0 | s_unsupported | 0/0/0/VAR |
| 23 | 0x9d598d0 | s_get_allocated_size | 0/0/1/0 |
| 24 | 0x9d598e0 | **s_set_notification_queue** (bind notif→queue) | 2/0/0/0 |
| 25 | 0x9d59998 | **s_submit_command_buffers** | 4/VAR/1/0 |
| 26 | 0x9d59c24 | s_set_priority_and_background | 1/12/1/0 |
| 27 | 0x9d59cc8 | s_set_quality_of_service | 1/4/0/0 |
| 28 | 0x9d59d68 | s_create_mtllateevalevent | 0/0/2/0 |
| 29 | 0x9d59d9c | s_destroy_mtllateevalevent | 1/0/0/0 |
| 30 | 0x9d59dd4 | s_async_signal_mtlLateEvalevent | 2/0/0/0 |
| 31 | 0x9d59ea0 | s_query_mtlLateEvalevent | 1/0/2/0 |
| 32 | 0x9d59f74 | s_set_display_params_for_gpu | 2/0/0/0 |
| 33 | 0x9d59fb0 | s_set_app_gpu_role | 2/0/0/0 |
| 34 | 0x9d59fec | s_get_app_gpu_role | 1/0/1/0 |
| 35 | 0x9d5a050 | s_set_resource_owner_identity | 2/0/0/0 |
| 36 | 0x9d5a068 | s_create_resource_iosurface | 3/0/1/0 |
| 37 | 0x9d5a100 | s_resource_detach_backing | 1/0/0/0 |
| 38 | 0x9d5a114 | s_resource_replace_backing_bytes | 0/24/0/0 |
| 39 | 0x9d5a124 | s_resource_replace_backing_ranges | 0/24/1/0 |
| 40 | 0x9d5a170 | **s_create_vniodesc** (IOGPUVnioDesc::withFileDescriptor — **настоящий VM-attach fd→GPU**) | 1/0/2/0 |
| 41 | 0x9d5a2a0 | s_create_io_command_queue | 1/0/0/0 |
| 42 | 0x9d5a2c8 | **s_destroy_io_command_queue** | 2/0/2/0 |
| 43 | 0x9d5a4a8 | (iOS-специфичный, sin=1) | 1/0/0/0 |
| 44 | 0x9d5a528 | **s_set_io_notification_queue** | 2/0/0/0 |
| 45 | 0x9d5a674 | **s_submit_io_commands** | 1/VAR/0/0 |
| 46 | 0x9d5a8dc | s_create_io_command_buffer | 1/0/2/0 |
| 47 | 0x9d5a994 | s_destroy_io_command_buffer | 2/0/0/0 |
| 48 | 0x9d5aa4c | s_try_cancel_io_command_buffer | 2/0/0/0 |
| 49 | 0x9d5ab04 | s_perform_io | 1/0/0/0 |
| 50 | 0x9d5aba8 | s_io_command_buffer_complete | 1/0/0/0 |
| 51 | 0x9d5ac48 | s_io_command_buffer_barrier_complete | 3/0/0/0 |
| 52 | 0x9d5ad04 | s_group_add_resources | 2/VAR/0/0 |
| 53 | 0x9d5ae90 | s_group_remove_resources | 2/VAR/0/0 |
| 54 | 0x9d5b01c | s_create_device_assertion (по сигнатуре) | 2/0/1/0 |
| 55 | 0x9d5b06c | s_perform_mapping (по сигнатуре) | 0/0/0/0 |

\* strOut у get_surface_info/get_device_info на iOS меньше macOS (8/16 vs 16/32) —
iOS-варианты структур компактнее; порядок и смысл совпадают.

**Исправление журнальных идентификаций (part13 §77–78, part16 §80):**
- «sel42/44/45 vmid/attach/mapping» — **ложная трактовка** (артефакт старого K).
  Реально: sel42 = destroy_io_command_queue, sel44 = set_io_notification_queue,
  sel45 = submit_io_commands. Это **IO-command path** (отдельный firmware-путь
  подачи команд, используется не-графическими клиентами), а не VM-attach.
- Настоящий VM-attach — **sel40 s_create_vniodesc** (fd → IOGPUVnioDesc →
  namespace, out {id, token}); macOS-аналог `s_create_vniodesc`
  @ `0xfffffe000ab89920` разобран: `ldr w0,[structureInput]` = fd →
  `IOGPUVnioDesc::withFileDescriptor(fd)` → `IOGPUNamespace::addObject` →
  out = {nsid, [desc+0x20]}.
- «check_capabilities в 0x9d6a98c» (part13) — функция в IOGPUFamily
  (`0x9d6a870` обёртка с блокировкой, вызов через `0x9d6add0` с кодами
  команды 2/3) — внутренности IO-command подсистемы, не GPU-bind ресурсов.

**Userclient types.** macOS `IOGPU::newUserClient` (`0xfffffe000abb83c8`)
раскладывает type: low16 выбирает класс (5 → IOGPUDeviceUserClient),
**high16 = variant передаётся в `IOGPUDeviceUserClient::init(..., type>>16)`**
и переключает таблицу методов (обычная `sDeviceMethods` @ 0x854a5a8 /
restricted @ 0x854ab48 по флагу `[uc+0x103]&1`). macOS-Metal: 0x100005 =
device UC + variant 0x10. На iOS из sandbox открываются 0x1 и 0x100001
(обе — device UC с одной таблицей по part16 §80); 0x100005 → 0xe00002c7.

---

## 3. Что делает Metal до/после queue create (macOS-эталон, part16 §81)

| # | вызов | параметры | iOS-эквивалент |
|---|---|---|---|
| 1 | IOServiceOpen type **0x100005** | AGXAcceleratorG16G | закрыт из sandbox (0xe00002c7) |
| 2 | sel9 **×3** | stIn 0x68, формат B | **sel8 ×3 — внутренние ресурсы ДО очереди** |
| 3 | sel7 | stIn 0x410, path@0 | **sel6** |
| 4 | sel16 | {0x100, 0x28} | **sel14** create_notificationqueue |
| 5 | sel28 | {qid, nqid} | **sel24** bind |
| 6 | sel9 ×~20 | формат B, flags 0x470/0x430/0xc30 | **sel8** ресурсы |
| 7 | sel14 ×2 | {0x4000,0},{0x4000,1} | **sel12** shmem (seglist id1, kcmd id2) |
| 8 | Trap4 sel0 | {qid, 0x40, entryVA, outVA} | **trap0** submit |
| 9 | sel17{1}; sel8{1}; sel15{2}; sel15{1} | scalar | post-submit bookkeeping |
| 10 | Trap1 sel1 | per-rid | release/untrack |

**Чего в трейсе НЕТ:** SetNotificationPort, MapMemory, VM-attach вызовов —
Metal на этом пути их не делает. Значит, «недостающий init» сводится к:
(a) type 0x100005 (закрыт), (b) pre-queue внутренние ресурсы (sel8 формат B),
(c) post-submit bookkeeping. Гипотеза «VM-attach sel42/44/45» (part16 §80)
не подтверждается — этих вызовов у Metal нет, а селекторы иные (см. §2).

---

## 4. Путь submit и где решается «no-op vs исполнение»

Цепочка (macOS-символы; iOS-структура идентична по ivar-офсетам):

1. **sel25** `s_submit_command_buffers` → `IOGPUCommandQueue::submit_command_buffer`
   (macOS `0xfffffe000abbbba78`).
2. Виртуальный `AGXCommandQueue::submitCommandBuffer(args)`
   (macOS `0xfffffe0008b0bd14`):
   - резервирование/проверка ёмкости (`[vtable+0x218]`, w1=1); при неудаче —
     **`[q+0x520] = 8`**, выход (это журнальный «status 8»);
   - иначе → `processCommandBuffer` → `processSegmentKernelCommand`.
3. `processSegmentKernelCommand` (macOS `0xfffffe0008b0e438`):
   - вход: `ldr w8,[q+0x44c]; cmp w8,#5; b.eq …` — `[q+0x44c]` = формат
     kernel-command (из константы блоба, обычно 3; **5 = альтернативный формат
     «v5»**, разрешён только при флаге firmware `[[q+0x530]+0x1b0d0]&1`, иначе
     status 0x108). Для нас не гейт, справочно.
   - `AGXKernelCommand::parseAndValidate(AGXSharedStreamParser)`; при ошибке —
     **`[q+0x520] = код ошибки из объекта команды (поле +0xc)`** — отсюда
     журнальные per-entry коды 8/9/0xa и статус 5;
   - успех: сабмит в scheduler (`[[q+0x530]+0x570]` vtable+0x238, w1=0) +
     `kick_scheduler`.

**Вывод по no-op.** «Принятие» (kr 0, outw 0, status 0) означает только
прохождение парсеров KEXT и постановку в scheduler. Реальное исполнение
решается на GPU/firmware-стороне по **device command stream** — packed
AGX-пакетам в kcmd-shmem, которые ядро содержательно не парсит. Если stream
ссылается на чужие/немапнутые GPUVA (pool-окна 0x28000/0x2a000, шейдерные
структуры из захваченного AGFI-образа), GPU либо пишет мимо, либо молча
no-op'ит — kernel-ошибки не возникает, completion всё равно приходит со
статусом 0 (или кодом валидации). Это согласуется со всей эмпирикой part13
(§74–79: принято, статус 0/{0,5}, записи нет) и объясняет, почему «стена»
не в entitlement очереди, а в семантике AGFI/stream + GPUVA-принадлежности.

**Completion-запись.** Писатель — `IOGPUFenceMachine::sendCompletionNotification`
(macOS `0xfffffe000ab9357c`; вызывается из `submit_command_buffer` и
`IOGPUBlockFence::notifyClient`). Запись 0x28 байт в shared-data очередь
notification queue (shmem от sel14):

| rec+0x00 | rec+0x08 | rec+0x10 | rec+0x18 | rec+0x20 |
|---|---|---|---|---|
| u64 (arg) | startTime (конвертация t0) | endTime (конвертация t1) | **u32 status** | u64 (extra) |

status = **`[q+0x520]`** (u32 состояния command buffer). Известные значения
(литералы проверены в AGXG16G/IOGPUFamily):

| status | смысл |
|---|---|
| 0 | ok — parse/queueing прошли (не гарантирует наблюдаемое исполнение) |
| 1 | внутренняя ошибка сегмента (ветка 0x8b0ec2c) |
| **5** | **эхо данных, не литерал** (см. §8.4): полный скан TEXT_EXEC обоих iOS-кекстов не нашёл ни одного store литерала 5 в [x,+0x520] и ни одного `mov #5 → str [+0xc]`; статус копируется failure-ветками из +0xc парс-объектов / слот-таблицы ресурсов, т.е. 5 — содержимое данных команды или kernel-слота (в replay-прошивке — значение из захваченного Metal-буфера). Не «исполнено» и не «код валидации №5» |
| 8 | отказ резервирования submit (0x8b0be24) / ветка сегмента (0x8b0ecec) |
| 9, 0xa | ошибки parseAndValidate (propagate из +0xc) |
| 0x108 | формат v5 без firmware-флага (0x8b0ec4c) |
| 0x10a/0x10b | спец-ветки: 0x10a сбрасывается в 0 (0x8b0ed6c), 0x10b — обработка reset |

Журнальное `{0,5}` = запись с полем «0» (arg/stamp) и **status=5**: сабмит
сошёл с дистанции на failure-ветке (тип команды/парсер), а 5 — эхо данных
буфера/слота, не номер проверки (скан литералов: §8.4). `{0,0}` = дошёл
до scheduler-ветки; (см. выше) это не значит, что GPU что-то записал.

---

## 5. Вердикт: гейт и минимальный исполняемый набор из App Sandbox

**Гейт — один, и он на открытии клиента, не в пути очереди.**
0x100005 из App Sandbox → 0xe00002c7 (kIOReturnNotPermitted) на этапе
IOServiceOpen; в TEXT AGXG16P/IOGPUFamily литерала 0xe00002c7 нет — отказ
даёт sandbox/IOKit-слой (allow-list userclient types в sandbox-профиле),
а не код драйвера. Внутри пути sel6/sel8/sel12/sel14/sel24/trap0 статически
**никакого entitlement-чека нет** — только гейты размера блоба, лимиты
числа очередей/ресурсов и валидация аргументов.

**Минимальный набор для «исполняемой очереди» (уже достижим из sandbox):**
sel14 (notification queue) → sel6 (queue, блоб 0x410 version<5) →
sel24 (bind) → sel12 ×2 (shmem seglist+kcmd) → sel8 (ресурсы) → trap0
(submit) — всё это наш клиент уже делает, kr 0, статусы 0/5. Т.е.
«исполняемая очередь» в смысле «ядро принимает сабмиты» **получена**.

**Для наблюдаемого GPU-write недостаёт не гейта, а двух семантических
слоёв:**
1. валидный **device command stream** (packed AGX packets) с ссылками на
   наши ресурсы — последний недекодированный слой (part13 §79);
2. **GPUVA-принадлежность** наших буферов: stream пишет по GPUVA
   0x1_00018000-подобным адресам, которым мы не владеем (part13 §76–77).

Три пути закрытия слоя 2 без Metal-коннекта:
- **sel40 create_vniodesc** — VM-attach своего fd в GPU-пространство
  (настоящий «attach», которого искали в part13);
- **pre-queue внутренние ресурсы** (sel8 формат B ×3 до очереди) — то, что
  реально делает Metal до queue create;
- **IO-command path** sel41→sel44→sel45 — отдельный firmware-путь подачи
  команд (может принимать команды без AGFI-образа; sel45 возвращал 0x2c2 —
  теперь известно, что это submit_io_commands, и коды ошибок можно
  картировать точечными мутациями).

Гипотеза part16 §80 «Metal на iOS открывает тот же 0x100001, разница в
init» остаётся жизнеспособной и ложится на пункты (b)/(c): воспроизвести
полный init (pre-queue ресурсы + post-submit bookkeeping) на нашем коннекте.

---

## 6. Чеклист для фаззера

1. **sel40 (create_vniodesc)**: fd-варианты (file, shmem-open, vm-object),
   повторные attach, чтение out {nsid, token} → проброс своей памяти в GPUVA.
   Цель: дать stream'у адресуемые нами окна.
2. **Replay после полного init**: поднять pre-queue внутренние ресурсы
   (sel8 формат B, stIn 0x68, flags 0x470/0x430/0xc30) ДО sel6, затем
   replay захваченного AGFI-blit/compute и свип GPUVA по окнам vniodesc.
3. **IO-command path**: sel41 (create_io_command_queue) → sel44
   (set_io_notification_queue) → фазз strIn VAR sel45 (submit_io_commands):
   картировать 0x2c2-ветки на грамматику IO-command descriptor'ов
   (коды команд 2/3 вокруг 0x9d6a870/0x9d6ad10).
4. **Harvest статусов trap0**: на каждой мутации сегмент-листа снимать
   completion status@+0x18 и per-entry коды; целевые ветки:
   0x8b0ec2c (1), 0x8b0ec6c (parser code → 5/9/0xa), 0x8b0ec4c (0x108),
   0x8b0ecec (8). Мутации, переводящие статус 5→0 при неизменной записи,
   сузят «молчащий» слой stream-ссылок.
5. **Блоб sel6**: version 0..4 (гейт <5 — фаззить 5+, ожидается чистый
   init-fail), байты +0x404/+0x405 (ивар-флаги q+0x439/q+0x527 — влияют на
   ветки в submit), размеры strIn 0x407/0x408/0x411 (границы ccmp).
6. **Post-submit bookkeeping**: iOS-аналоги macOS sel17{1}/sel8{1}/sel15{2,1}
   — подобрать валидные формы (на iOS sel17→0x2c2, см. part16 §81) —
   release-цикл ресурсов может быть обязательным для реального исполнения.
7. **sel43 и sel54/55** — неидентифицированные хвосты таблицы; прогон
   sin-наборов {0,1,2,3} с записью kr/статусов (дешёвый enumeration).

## 7. Адресный справочник (для перепроверки)

- iOS IOGPUDeviceUserClient::externalMethod `0xfffffff009d5bd00`;
  таблицы `0x814d8c0` / `0x814de00` (K декодера = **0x0b00c000**).
- iOS s_new_command_queue `0x9d59280` (гейт 0x410 @ 0x9d59380).
- iOS IOGPUCommandQueue::init `0x9d872c0` (блоб 0x410).
- iOS submit-цепочка: sel25 `0x9d59998`; обёртки `0x9d598e0` (sel24),
  `0x9d597e0` (sel12), `0x9d5981c` (sel14).
- macOS: submit_command_buffer `0xfffffe000abbbba78`;
  AGXCommandQueue::submitCommandBuffer `0xfffffe0008b0bd14`;
  processSegmentKernelCommand `0xfffffe0008b0e438`;
  sendCompletionNotification `0xfffffe000ab9357c`;
  IOGPU::newUserClient `0xfffffe000abb83c8`;
  s_create_vniodesc `0xfffffe000ab89920`.

---

## 8. Семантика исполнения (userland AGXMetal vs наш pipeline)

Статика userland-драйвера `/tmp/ipsw_out/AGXMetalG16G_B0` (arm64e dylib,
с символами; VMA __text `0x214d2e040`) + перекрёстная верификация iOS-кекстов.
Ответ на вопрос постановки «что семантически отличается в команде/контексте
Metal» — в §8.5.

### 8.1 Userland-цепочка commit (что делает AGXMetal перед trap0/sel25)

Разобрано по символам (все адреса VMA dylib'а):

1. `-[AGXG16GFamilyCommandQueue_mtlnext commit:count:]`
   @ `0x21504a0d4`: `preCommit:count:options:` на себе → затем
   count≥2: **класс-планировщик** (classref `0x278c5e4e0`, имя класса живёт
   в dyld-кеше, недоступно) `_commit:count:commitFeedback:`; count<2:
   `noMergeCommit:count:options:commitFeedback:error:` @ `0x21504a040`.
2. `noMergeCommit`: сначала **`AGX::InternalSparseQueue::flushAndSubmitAllMappings`**
   @ `0x215568e7c` (слив deferred sparse mappings в очередь), затем dispatch
   на реальный commit (objc-stub в кеше). Самого IOConnectCall в этом dylib
   **нет** — kernel-вызовы (sel25/trap0) делаются из libIOGPU
   (`IOGPUMetalCommandQueue`), который в кеш-вырезке отсутствует; его стабы
   здесь неразрешимы.
3. `-[AGXG16GFamilyCommandBuffer fillCommandBufferArgs:commandQueue:]`
   @ `0x2150454ac`: заполняет `IOGPUCommandQueueCommandBufferArgs`
   (та самая структура, что kext-код копирует в parse-объекты):
   `[args+0x30] = w0` (label/stamp command buffer), `[args+0x38] = x0`
   (ptr), затем C++-хелпер (selref `0x1f5517488`, неразрешим) с замыканием
   {cb, глобал} дописывает остальное — идентификаторы shmem (seglist/kcmd),
   позиции, fence-связи. Т.е. агрегат сабмита собирается **на стороне
   userland** и является простым плоским дескриптором; содержательной
   семантики он не несёт.
4. `-[AGXG16GFamilyCommandQueue initWithDevice:descriptor:]`
   @ `0x2150481e8`: AGX-часть init очереди; блоб 0x410 + вызов sel6
   строит суперкласс (libIOGPU). ДО этого устройство делает
   `AGX::Device::setupHWResourcePools` @ `0x214f97644` (см. §8.2).

**Важно:** «3 внутренних ресурса ДО очереди» из macOS-трейса (§3) — это
**не** setupHWResourcePools. Это pre-queue ресурсы (sel8 формат B), которые
создаёт libIOGPU до init очереди; их конструктор в недоступном дайлибе.
Из AGX-драйвера видно только, что очередь при своём init активно использует
45 пулов (§8.2) и sparse-подочередь — т.е. «внутренние ресурсы» — это
носители этих пулов/подочередей.

### 8.2 setupHWResourcePools — 45 HW-пулов, а не 3

`AGX::Device<HAL2008Encoders>::setupHWResourcePools(device, pools)`:
цикл по 45 индексам (0..0x2c), каждая итерация — создание
`IOGPUMetalResourcePool` через вызов с дескриптором 0x68 байт
(поля: name-строка из таблицы ×45, size `0x400`/`0x8000`/`0`,
option-флаги по классам):

| Класс индексов | option qword | size | Примечание |
|---|---|---|---|
| default | `0x08000000` | 0x400 | большинство пулов |
| отдельные (jump-таблица `0x215839808`, индексы 0x24..0x2c) | `0x08000000`, `0x18000000`, `0x38000000`, `0x48000000`, `0x48_08000000` | 0x400, 0x8000 или 0 | экстра-флаги из второй таблицы |
| env `AGX_…` (findEnvVarNum @ `0x215574b0c`, строки `0x215855efa`) | перекрытие | — | отладочный оверрайд |

Имена пулов — rebased-указатели в кеш (таблица `0x2741b9ff0`), из вырезки
не восстанавливаются. По адресам в коде рядом лежат getDepthBiasBuffer /
getScissorBuffer — пулы покрывают fixed-function буферы, UAB-окна,
visibility и т.п. Пулы — это **userland-side аllocators поверх обычных
sel8-ресурсов**, не отдельный kernel-механизм.

### 8.3 Что именно валидирует parseAndValidate (kext, перепроверено)

Все `*KernelCommand::parseAndValidate` iOS (kc27 AGXG16P) и macOS
(референс с символами) разобраны построчно:

| Функция (macOS addr) | iOS addr | Коды в +0xc | Что проверяет |
|---|---|---|---|
| `AGXKernelCommand::parseAndValidate` `0x8b6acdc` | `0x8395e58` | **0xa** | underrun стрима; копия дескриптора (8B mac / 16B iOS) в +0x10 |
| `AGXHardwareKernelCommand::parseAndValidate(P,P)` `0x8b68138` | `0x8370d58` | **0x100, 0x102, 0x109** | копия 0xc0-байтного hw-дескриптора; границы стрима (0x100); OR-полей +0xb4..+0xcc **> 0x3ff** → 0x109 (лимит суммарного числа ресурсных ссылок 1023); под-списки 0x98/0x9c/0xa0/0xa4/0xa8 — пары {count,off} со своими границами (0x102 при переполнении второго парсера) |
| `AGXComputeHardwareKernelCommand` `0x8b19eec` | (аналог по сигнатуре) | **0x100** | только границы |
| `AGXRenderHardwareKernelCommand` `0x8b82ef8` / `AGXFastRender…` `0x8b21284` | » | **0x100** | только границы |
| `AGXSegmentResourceList` `0x8b8b948`, `AGXSegmentResourceDescGroup` `0x8b8b8c8`, shared-event семейство `0x8b8e584+` | `0x8395eac`, `0x8395f08` | **0xa** | underrun; группы по 0x18/0x20 байт |
| (iOS-only, рядом с `0x83ad058`) | `0x83acfc0`-family | **0xa / 0x106 / 0x3ec / 0x274 / 0xc** | iOS-специфичные ветки (значения из ivar-полей объекта) |

`processSegmentKernelCommand` (iOS `0x831c940`, macOS `0x8b0e438`):
- тип команды = low32 скопированного из стрима дескриптора (`[sp+0x48]`,
  клиентские данные); диспатч: 3/4/0xb — AGX hw-команды (render/compute/
  fastrender-подобные), 0xd/0xe/0x10002/0x10004 — служебные/generic пути,
  остальное — сразу в scheduler (success-ветка `0x831d240`);
- status-писатели [q+0x520]: литералы **8** (reserve-fail `0x83180bc`,
  fence-ветки), **0xa**, **2** (late-eval/флаги) + три **копии данных**:
  `0x831d21c` ← `[sp+0x44]` (+0xc base-объекта ∈ {0, 0xa}),
  `0x831d228`/`0x831d238` ← `[sp+0x8c]` (+0xc объекта для типов
  0xd/0xe/0x10004), `0x831d284` ← `[sp+0x74]` (+0xc объекта типа 0x10002,
  парсер `0x832b304` с кодами 0x100/0x103);
- тип **0x10002** (generic kernel command) — единственный путь, где status
  питается от **kernel-состояния**: слот-таблица `[channel+0x48]`, слот
  `0x60`×resource-id; в слот пишется `counter+0x100` (`0x831cb4c`) и тот же
  слот читается обратно в status-failure. Слот индексируется **клиентским
  resource id** из команды (с проверкой бита валидности `[x23+0x178]`).

IOGPUFamily (`submit_command_buffer` окрестности `0x9d87a40`–`0x9d8bc30`):
литералы **2, 4, 6, 8, 9, 0xa** (4 — при флаге `[q+0x43a]` = блоб +0x404).

### 8.4 Откуда взятся `{0,5}` — поправка к §4

Полный скан TEXT_EXEC обоих iOS-кекстов (капстон-перебор всех инструкций):
**ни одного** `mov w?,#5` со store в `[x,+0x520]` (окно 32 инструкции) и ни
одного `mov #5 → str [+0xc]` (окно 8). Следовательно, наблюдаемый status 5 —
**эхо клиентских/сло́товых данных** на failure-ветках §8.3 (точные сайты:
`0x831d21c` / `0x831d228` / `0x831d284`), а не семантический «код
валидации №5». Для нашей replay-прошивки захваченного Metal-буфера это
значение из самого буфера/состояния слота (малое число — по смыслу rid/
счётчик живого Metal-прохода). Практический вывод: статус 5 не диагностирует
«какой гейт сработал» — он лишь говорит, что сабмит дошёл до
failure-ветки соответствующего типа команды.

> **Уточнение (v126, см. §8.7):** гипотеза «эхо клиентских данных» для
> значения 5 **опровергнута** полным сканом — писателей `5` в `[+0xc]` и
> клиент-данных→`[+0xc]` не существует вовсе; 5 поставляет firmware через
> completion-блок (копия `0x831fba8`). §8.4 остаётся верным для общей
> структуры статус-путей, но не для происхождения пятёрки.

### 8.5 GPUVA-принадлежность — не гейт

- В TEXT_EXEC обоих iOS-кекстов **нет** литеральной базы арены
  `0x1_00000000` (скан всех `movk … lsl #32`): базы GPUVA-арен —
  data-driven (конфиг устройства/VM), клиентом не выбираются.
- VA ресурса назначается ядром (sel8 → слот-таблица `[channel+0x48]`,
  §8.3) и возвращается в userland; драйвер лишь встраивает его в пакеты.
- Наш replay использует VA, разданные нашему коннекту — для **своих**
  ресурсов они валидны по построению. Ранешняя гипотеза «stream пишет по
  чужим окнам 0x1_00018000» объяснялась отсутствием **своих** ресурсов
  в нужных ролях (shader structs, UAB), а не «чужим» диапазоном: диапазон
  тот же механизм, что у Metal, просто другая арена выделения.

### 8.6 Вердикт: почему наша очередь не исполняет и что с этим делать

Семантическая разница Metal-vs-наш pipeline — не в одном поле и не в
гейте, а в **трёх слоях, которые существуют только внутри userland-энкодера
AGXMetal**:

1. **Packed device command stream.** В kcmd-shmem лежат VDM/CDM-пакеты
   (kick-дескрипторы, program tables, tile-конфиг), которые строят
   классы `AGX::…Context`/`…Encoder` дайлиба (`performEnqueueKernel`,
   `getKickSlotVDMControlFlags`, `finalizeScsParameters` и т.д. — тысячи
   функций). Kext содержательно не парсит пакеты: parseAndValidate
   проверяет только **границы и счётчики** (§8.3). Значит, «валидная
   команда» = «такие байты, которые GPU-firmware примет» — грамматика
   firmware-протокола недоступна из сырых селекторов и не выводится из
   kernel-кода.
2. **Ресурсные роли.** Живая очередь ссылается на shader-структуры,
   UAB-окна пулов и пр., созданные через пулы §8.2 и слот-таблицу §8.3
   под конкретный encoder state. Наш replay воспроизводит RID-ссылки
   захваченного буфера, но роли (какой rid — шейдер, какой — UAB)
   восстанавливаются только вместе с userland-энкодером.
3. **Подпись процесса и версионность.** Блоб 0x410 (path@0, version<5,
   u32[2,3,1,4]) привязывает очередь к конкретному userland-клиенту;
  firmware/scheduler видят разницу между «очередь Metal-процесса» и
   «очередь фаззера» на уровне содержимого команд, а не entitlement.

**Итог (фиксируем как окончательный):** исполнение требует
userland-построения device command stream'а — этот слой живёт в AGXMetal
(энкодеры) + libIOGPU (submit), и из App Sandbox воспроизводится только
вместе с ними. Минимальный «исполняемый набор без Metal» сводится к
воспроизведению энкодерной грамматики, что эквивалентно встраиванию
AGXMetal. Практический путь не меняется: **патч живого Metal-процесса**
(текущий вектор v89) — единственный способ получить валидный stream под
нашим контролем; наша собственная очередь (connect 0x1) остаётся
инструментом фаззинга kernel-валидации и IO-command пути, а не исполнения.

### 8.7 Источник status 5 (v126) — первопричина {0,5} на replay v85

**Постановка.** Replay v85: verbatim capture-образы, kr 0 на submit,
completion-записи `{0,5}`, исполнения нет. Вопросы: (1) кто пишет ненулевое
в +0xc parse-объектов и что это поле; (2) какое условие валидатора даёт 5;
(3) какое свойство replay-контента триггерит отклонение. Все выводы ниже —
капстон-сканы TEXT_EXEC обоих iOS-кекстов (AGXG16P + IOGPUFamily, kc27),
сайты проверены дизасмом построчно.

#### (1) Семантика +0xc и его писатели

`+0xc` — error-code поле **стекового parse-result объекта** раскладки
{vptr@+0, ok-byte@+8, err u32@+0xc, скопированный дескриптор@+0x10}.
Диспатч — `processSegmentKernelCommand` (iOS `0x831c940`); base-парсер
`0x83737ac`. При fail его status копируется из `[sp+0x44]` (=obj+0xc) в
`[q+0x520]` сайтом `0x831d21c`; типы 3/4/0xb/0xd/0xe/0x10004 →
`[sp+0x8c]` (сайты `0x831d228`/`0x831d238`); тип 0x10002 → `[sp+0x74]`
(сайт `0x831d284`).

Полный перебор писателей `+0xc` — **только литералы**:

| Парсер | Адрес | Коды в +0xc |
|---|---|---|
| base (seglist) | `0x83737ac` | 0xa (underrun) |
| seglist-группы | `0x8395eac`, `0x8395f08` | 0xa |
| hw-команды | `0x8370d58` | 0x100; ветки 0x102/0x109 — тоже литералы |
| generic (тип 0x10002) | `0x832b304` | 0x100/0x103 |
| iOS-большой (`0x83aceb0`-family, дескриптор 0x9a8) | `0x83acfc0`-хвост | формально ivar-echo (`ldr w8,[x19,#0x3ec]` / `ldr w8,[x19,x8]`, x8∈{0x274,0x3ec,0x106}), **но**: `[x,#0x3ec]` пишется только ctor-zero (`0x831f100`), `[x,#0x274]` — ctor-zero (`0x831f0d0`), `[x,#0x106]` — вообще ни одного store ⇒ echo-пути всегда дают 0 |

Значение **5 никто не пишет ни в `+0xc`, ни как литерал в `[q+0x520]`.**

#### (2) Полный перебор писателей [q+0x520]

- **AGXG16P** (`/tmp/dsr/scan520b.py`): литералы {1,2,4,6,8,9,0xa,0xc,0x10,
  0x100,0x106,0x108,0x10d,0x114,0x10f,0x110,0x111} + echo `[x19/x23+0xc]`,
  `[sp+0x8c]` (все literal-bounded) + `0x831e050: ldr w8,[sp,#0x2c]` — но
  sp+0x2c зануляется в `0x831d8ec` (других записей нет) + `0x83242c8:
  str w20,[x19,#0x520]` — w20 = литерал 0x10d/0x114 (`0x83242a0`/`0x83242a8`)
  + **единственный неконстантный путь: `0x831fba8: ldr x10,[x9,#0x220];
  str x10,[x21,#0x520]`** — копия из completion-блока.
- **IOGPUFamily** (`/tmp/dsr/scan520.py`): литералы {2,4,6,8,9,0xa} +
  `str w12` где w12=0xa (инициализация `mov w12,#0xa` в `0x9d8a934`/`0x9d8a9f8`).

Допустимых kext-значений **нет 5** ⇒ 5 приходит не от валидатора.

#### (3) Цепочка происхождения 5

Completion-обработчик: `x26`=4-й аргумент (присвоение `0x831f544:
mov x26,x4`), `x9=[x26+0x18]` — указатель на расширенный статус-блок.
Раскладка блока: timestamps @+0x200..+0x238, **status u64 @+0x220**,
флаги-байты @+0x24d..+0x250 — классический GPU writeback. Копия полями
в queue-объект идёт `0x831fb00`–`0x831fc30`, включая `0x831fba8:
[x21+0x520]=[x9+0x220]`. Сторонних писателей status-поля +0x220 нет
(скан `/tmp/dsr/scan220.py`: только несвязанные `str w26` после
lock-вызовов и pointer-store `str x0` в `0x831a1ac`).

**⇒ 5 пишет firmware/GPU в completion-блок; kext лишь копирует его в
[q+0x520] → nq-запись.** Строитель nq-записи — `0x9d60abc` (0x28 байт,
status u32 @+0x18 = w20=[q+0x520]; layout совпал с fuzzer-комментарием
«completion records {u64, u64 startTime, u64 endTime, u32 status @+0x18}»).
Оба сайта вызова читают `[x19+0x520]` прямо перед `bl` (`0x9d87e10` →
`0x9d87e20`; `0x9d87f10` → `0x9d87f20`); дополнительный consumer —
`0x9d87e30: ldr w8,[x19,#0x520]; cbz w8; str w8,[x9,#0xc8]` (error-catch).

Значения семантики: 0 — не исполнялось/принято, 4 — completed (Metal,
part16), 5 — отклонение device stream на **исполнении** firmware'ем.

#### (4) Вердикт

- Гипотеза «v5-формат без firmware-флага 0x108» **опровергнута**: литерал
  0x108 существует (`0x831e034: str w8,[x20,#0x520]` при сброшенном бите
  `[x20+0x530]+0x1b054` bit3), но это kext-статус устройственного гейта,
  к 5 отношения не имеет.
- Наблюдаемые `{0,5}` = запись №1 «принято/распарсено» (status 0 — kext-
  валидация нашего kcmd **проходит**, incl. hw-парсер с лимитом 0x3ff
  ресурсных ссылок) + запись №2 «firmware отклонил stream при исполнении».
  Против `{0,0}` (firmware не нашёл что исполнять) это **прогресс**:
  firmware реально пытался исполнить capture-stream и отверг его
  содержимое. Отклоняется не формат капсулы, а **содержимое device
  stream** (команды/ссылки внутри капсул) — т.е. гипотеза §8.6 о
  недоступности энкодерной грамматики подтверждается с другой стороны:
  kernel-валидация проходится, firmware-грамматика — нет.

#### (5) Рекомендация для пайплайна

1. **Трактовать вторую nq-запись как firmware-статус исполнения**
   (0=не исполнялось, 4=успех, 5=reject), а не результат kernel-валидации.
   Первая запись (0) подтверждает лишь приём kcmd — она не сигнал
   исполнения.
2. **Фаззить содержимое device stream с сигналом «5→4»** (см.
   `docs/device_stream_builder.md`): kcmd-валидация пройдена, дальше
   работа над грамматикой stream-пакетов внутри капсул (VDM/CDM-команды,
   rid-ссылки, ордеринг kick-дескрипторов).
3. В fuzzer ориентироваться **не на «status==0» первой записи, а на
   вторую запись != 5**; status 5 как crash-signal бессмысленен — это
   штатный reject, не баг.

