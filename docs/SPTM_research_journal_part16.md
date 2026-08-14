# Журнал, часть 16 (секции 77–79, 14.08.2026) — Metal self-reference: дискриминация «байты vs очередь»

## 77. Эталон Metal внутри нашего процесса (v86, фаза p_mtlself, env FUZZ_MTLSELF=1)

Metal на iOS 27 (A17 Pro) доступен из фаззера напрямую (-framework Metal).
Классы: `AGXG16PFamilyCommandQueue` / `AGXG16PFamilyCommandBuffer` (device — AGXG16Device).
Introspection (objc/runtime, свой процесс):
- `MTLIOAccelCommandBuffer` → ivar `_commandBufferStorage` → storage:
  **+0x28 = kernel cmd shmem VA, +0x68 = segment list shmem VA, +0x20/+0x60 =
  IOGPUMetalDeviceShmem объекты** (ivars `_shmemID`, `_shmemSize`, `_virtualAddress`) —
  карта из macOS-реверса подтверждена на iOS побайтово. id: seglist=1, kcmd=2.
- Эталонные hexdump'ы (pre-commit и post-commit) в results/run-v86*.log
  (REF-kcmd/REF-seg/POST-kcmd/POST-seg).
- **Commit финализирует seglist**: pre-commit +0x20/+0x24/+0x34/+0x40/+0x44 = 0;
  post-commit: segCount=1, totalSize=0x80000130, cmdEnd=0x404,
  numResources=0x14 (20), numResourceGroups=4. Наши hand-crafted значения
  (numResources/numGroups в сегменте) — правильная конвенция.
- Реальный segment list Metal: 4 шестипак-группы, 20 ресурсов (rids 0xb..0x22).
  usage значения: 1 (read), 3 (rw), 4, 0x80. Группа буферов: rids 0x15/0x16,
  sizeKB 0x40/0x40, usage {1, 3} (src read / dst rw).
- REF kernel cmd (blit copy, len 0x330) vs наш capture (fill, len 0x358):
  59/0x400 байт отличаются — структура идентична.
- Metal sanity: commit → waitUntilCompleted → bufB все 0x41 (cb status 4) —
  GPU запись работает, эталон исполняется.

### Селекторная карта Metal (macOS trace + iOS проверка)

macOS: Metal открывает **AGXAcceleratorG16G, type 0x100005**; sel 7 = new_command_queue
(stIn 0x410, в блобе @+0 — **полный path процесса**), sel 9 = new_resource (формат B),
sel 14 = device shmem {size,type}, sel 16 = notification queue {0x100, 0x28},
sel 28 = bind {qid, nqid}, trap0 = submit, post-submit: sel 17 {1}, sel 8 {1}, sel 15 {2}/{1}.
iOS: прямой open AGXAcceleratorG16P type 0x100005 НЕ работает (sandbox; открывается только
type 1, и это не тот клиент). НО: command-коннекшн Metal в нашем процессе —
**IOGPU type-1-совместимый userclient** (sel8 resource/sel12 shmem работают с iOS-нумерацией),
его порт найден по адресу `AGXG16Device._deviceRef + 0x14` (ivars дамплены:
`_acceleratorPort` @+664 — НЕ он, там sel8/12 unsupported).
Ресурсы, созданные на коннекшне Metal, получают rid 35/36 и GPUVA 0x1_000130000+
(VM Metal'а, тот же 0x1-space).

## 78. Дискриминационная матрица (raw replay эталонных байт через НАШУ очередь)

| Вариант | Что подано | outU32 | completion | запись |
|---|---|---|---|---|
| R1 | эталонные shmem побайтово (post-commit) | 9 | {9, 9} | нет |
| R2 | R1 + rids буферов и GPUVA bufA/bufB → наши | 9 | {9, 9} | нет |
| R3 | R2 + ВСЕ 20 ресурсов пересозданы (sizeKB из групп) | 0 | {0, 5} | нет |
| R5 | наш fill + 2 ресурса, очередь с блоб-вариантами (zero/name/path/prio0/prio2/fastEvent) | 0 | **{0, 0}** | нет |
| R4 | наш fill через **очередь Metal** (conn из _deviceRef, qid 1) | 0 | н/д | **процесс убит <5 мс после trap** |

Выводы:
1. **Байты shmem — не блокер**: эталонные байты через нашу очередь ведут себя так же,
   как hand-crafted (после полного набора ресурсов — {0,5}).
2. **Completion status зависит от ресурсной конфигурации/содержимого, не от очереди**:
   fill + 2 буфера → {0,0} («исполнение успешно»), но записи нет — т.е. status 0 на
   execution-стадии ≠ реальная GPU-запись. Copy + 20 нулевых ресурсов → {0,5}
   (контент внутренних pool/bplist-ресурсов нулевой — execution это замечает).
3. **Очередь Metal качественно отличается**: тот же сабмит на ней убивает процесс за
   миллисекунды (kr 0/outU32 0, затем смерть — консистентно с реальным заходом на GPU
   и фолтом на мусорных aux-refs, либо с ассертом completion-машинерии Metal на чужой
   command buffer). Устройство не страдает.
4. Глитчи экрана в v84b, вероятно, связаны с replay-нагрузкой (единственное окно, где
   GPU touch'ит наши структуры) — прямого подтверждения записи нет.

## 79. Открытый фронт

- status 0 без записи → GPU исполняет «пустую» программу. В capture был третий регион:
  **device command stream** (reg_10d460000, 0x130 байт packed AGX packets) и
  **data segment** (reg_104f70000, паттерн заливки). Похоже, kext при трансляции
  строит device stream из kernel cmd + ресурсов, и без корректных внутренних
  структур (bplist shader cache, pool-таблицы) получается no-op.
- Следующий шаг: разобрать, что Metal делает между endEncoding и trap0 на СВОЕМ
  коннекшне (post-submit sel 17/8/15 — macOS-нумерация; на iOS type-1 коннекшне
  подобрать аналоги), либо добить формат device command stream по
  reg_10d460000 + fn_0x831e1ec.
- Побочное: [cb error] в varargs fprintf дважды дал немедленную смерть процесса
  (v86/v86b), после выноса в переменную — работает; причина не до конца ясна (ARC/BGP?).

## 80. Коннекты и userclient types (v87, фаза p_connprobe, env FUZZ_CONNPROBE=1)

- В registry ровно ОДИН GPU-сервис: `AGXAcceleratorG16P`
  (`IOService:/AppleARMPE/arm-io@10F00000/AppleH16IO/sgx@80000000/AGXAcceleratorG16P`);
  матчинги "IOGPU"/"AGXAccelerator"/"AGXAcceleratorG16" резолвятся в него же.
- IOObjectGetClass/IORegistryEntryGetPath на io_connect_t на iOS 27 → 0xe00002c2
  (introspection коннектов закрыта); Metal-коннект идентифицирован поведенчески (v86).
- Перебор IOServiceOpen type (0..0x20, 0x100, 0x1000, 0x10000, 0x100000..0x100005)
  на IOGPU и AGXAcceleratorG16P — открываются из нашего sandbox'а ТОЛЬКО:
  - type 0x1 — наш рабочий клиент (sel6/8/12/14/24/trap0);
  - type 0x100001 — открывается, та же селекторная таблица, тот же результат сабмита;
  - всё остальное, включая Metal'овский macOS-type **0x100005 → 0xe00002c7** (denied,
    entitlement/sandbox-gate).
- Submit-тест (verbatim fill, 2 ресурса) на каждом открытом type: kr 0, outU32 0,
  completion {0,0}, записи в B нет — идентично для 0x1 и 0x100001, оба сервиса
  (один объект) ведут себя одинаково.
- Крашей/паник при переборе не было, FUZZ_CONNPROBE_SKIP не понадобился.

Вывод: «настоящий» GPU-контекст привязан к привилегированному userclient type
(0x100005 у Metal на macOS; на iOS открытие из App-Sandbox запрещено). Доступные
нам типы (0x1, 0x100001) дают функционально полный, но «беззубый» конвейер:
трансляция проходит (completion 0), реальной записи GPU не производит.
Остаётся гипотеза, что Metal на iOS открывает тот же type 0x100001, а разница —
в дополнительной инициализации (VM attach sel42/44/45, event setup), которую
Metal делает на своём коннекте и которую можно попробовать воспроизвести на нашем
type-0x100001 (см. v74: sel42/44/45 vmid/attach давали kr 0 — стоит прогнать
replay ПОСЛЕ полного VM-attach).

## 81. Попытка in-process трейса Metal на iOS (v88) — почему нельзя, и эталонная карта

Инфраструктура: `relay/iotrace.m` — interpose-dylib (`__DATA,__interpose`, 16 туплов:
IOServiceOpen/Close, IOConnectCall{Method,Struct,Scalar,Async}Method,
SetNotificationPort, MapMemory64, Trap0–6, mach_msg с фильтром msgh_id 2800–3199),
линкуется в app через Frameworks/libiotrace.dylib (build.sh), ring-buffer 2048 записей,
дамп по маркам. Фаза `p_mtltrace` (env FUZZ_MTLTRACE=1).

Результат (run-v88..v88c): interpose РАБОТАЕТ для вызовов main executable → IOKit
(self-test: наш IOServiceOpen попал в трейс), но Metal-сеанс даёт НОЛЬ записей.
Причина: Metal.framework и IOKit оба в dyld shared cache, а **cache-internal binds
не interposable** (и страницы cache не перезаписываемы — fishhook бессилен).
Вывод: in-process symbol tracing вызовов Metal на iOS-девайсе без jailbreak
невозможен. Эталоном остаётся macOS-трейс (/tmp/iogpu_trace.log, тот же стек).

### Эталонная последовательность Metal (macOS 27, AGXAcceleratorG16G type 0x100005)

| # | вызов | параметры | смысл |
|---|---|---|---|
| 1 | IOServiceOpen | AGXAcceleratorG16G, **type 0x100005** | Metal-коннект |
| 2 | sel 9 ×3 | stIn 0x68 (формат B) | внутренние ресурсы ДО очереди |
| 3 | **sel 7** | stIn 0x410, @+0 полный path процесса | new_command_queue → qid |
| 4 | **sel 16** | {0x100, **0x28**} | notification queue → nqid, VA |
| 5 | **sel 28** | {qid, nqid} | bind |
| 6 | sel 9 ×~20 | формат B (flags 0x470/0x430/0xc30) | все ресурсы (внутренние + буферы) |
| 7 | sel 14 ×2 | {0x4000, 0}, {0x4000, 1} | shmem: seglist (id 1), kcmd (id 2) |
| 8 | **Trap4 sel 0** | {qid, 0x40, entryVA, outVA} | submit |
| 9 | sel 17 {1}; sel 8 {1}; sel 15 {2}; sel 15 {1} | scalar | post-submit bookkeeping |
| 10 | Trap1 sel 1 {0x16..0x21, 3, ...} | per-rid | release/untrack ресурсов |

Чего НЕТ в трейсе: SetNotificationPort, MapMemory, VM-attach — Metal их на этом
пути не делает (либо делал при более раннем init, не попавшем в окно).
Соответствие нумерации iOS type-1 (проверено на устройстве и на коннекте Metal в v86):
queue **6**, resource **8** (формат B тот же), shmem **12**, notif **14**, bind **24**,
submit trap0 — идентичен. Не замаплены/не проверены: post-submit 17/8/15 (iOS sel17 →
0x2c2) и release-Trap1. Кандидатные «недостающие init» по сути сводятся к:
(a) type 0x100005 (закрыт), (b) pre-queue внутренние ресурсы, (c) post-submit
bookkeeping — всё тестируемо на нашем type-1 коннекте без трейса.
