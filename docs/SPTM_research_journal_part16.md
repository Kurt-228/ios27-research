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
