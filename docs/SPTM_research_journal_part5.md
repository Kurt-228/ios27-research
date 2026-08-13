# Журнал, часть 5 (секции 44–47, 13.08.2026)

## 44. Kernel-сторона AppleM2ScalerCSCDriver (macOS 27.0, t8132) — полный реверс

Извлечение: BootKC `/System/Volumes/Preboot/<uuid>/boot/<hash>/System/Library/Caches/com.apple.kernelcaches/kernelcache` (IMG4) → `kmutil emit-macho -B` → `/tmp/KMUtilProducts/BootKernelCollection.kc` → вырезание fileset `com.apple.driver.AppleM2ScalerCSCDriver` (python, LC_FILESET_ENTRY; fileoff сегментов абсолютные в KC — требуют патча −base, включая LC_SYMTAB/LC_DYSYMTAB/linkedit-data) → `~/.kimi-work/sptm-analysis/mackc/AppleM2ScalerCSCDriver.macho` (14506 символов, полная символика!).

### Диспетчер
- `IOSurfaceAcceleratorClient::getTargetAndMethodForIndex` @ 0xfffffe00098c4954, selector ≤ 0xb, таблица @ 0xfffffe00081dcbd8, stride 0x30, формат {0, func, 0, flags, inSize, outSize}.
- sel 0/2/3 — стабы 0x2c7 (и на macOS тоже!). sel 1: только size==0x1b0 + type-check (kernel-client → panic "must only be called from user space"). sel 5: 0 скаляров in, 1 out (clientID). sel 11: variable out ≤0x298 + хвост зануляется.
- `newUserClient`: единственный гейт byte[driver+0x88]. **Entitlement-проверок в кексте нет вообще.**

### sel 6 KernelTests — гейт разгадан
- `user_kernel_tests` → `kernelTests`: `if (runtimeProperties.EnableKernelTests == 0) return 0xe00002e2`; `args[0xfa4]&1` обязателен; count ≤ 0x3e8 (только в user-обёртке!).
- EnableKernelTests — runtime-property №6, `setProperties`/`setRuntimeProperty` **без проверок прав**. На iOS `IORegistryEntrySetCFProperty` → 0x2e2 (символ экспортирован, но MIG-вызов gated sandbox'ом). Гейт снять с устройства не удалось.
- Формат args: +0 u32 count, +4 u32 surfaceIDs[count] (ровно 0xfa4 байта на 1000 id), +0xfa4 flag.
- k2kTests: цепочка из 5 обработчиков: testStress (count==0x3e8 обязателен, 1000 surfaces, 5 async scheduler'ов), testSynchronous (count≥2, 100 итераций k2k transform), testRetain, testCallback, testHistogram. lookupSurfaces → IOSurfaceRoot::lookupSurface(id, current_task()) — namespace per-task.
- Побочно: k2k-вызов kernelTests без лимита count → testStress пишет count·8 в стек 0x1f40 (stack overflow, только для kernel-callers).

### Async-механика (разгадка +0x08 и шторма)
- request +0x08..+0x18 = **asyncRef** (ставит ядро при async-вызове); +0x08 != 0 → `transformSurface_asynchronous`: аллок req, `setAsyncReference64(req, port=[client+0x120], ...)`, `prepareThreadWithAction(IOAsynchronousScheduler, asynchronousUserClientCompletionCallback, &tid, ...)`, tid → обратно в data+0x10. Бит 10 flags (+0x20) = SkipAsyncCallback подавляет уведомление.
- Completion: HW irq → sendCallback(client, tid, result) → notifyThread → action → `sendAsyncResult64` если порт есть и не suppress.
- **Формат 112-байтного сообщения**: msgh_id 0x35 = kOSNotificationMessageID; OSNotificationHeader64 {size 16, type 150 = kIOAsyncCompletionNotificationType, reference[8]} + IOAsyncCompletionContent {result} + 1 u64 arg. Шторм v10 = по сообщению на каждый завершившийся async-трансформ; «тишина» v11 — порт/очередь/подавление (send без CanDrop при переполнении очереди молча теряет).
- `registerNotificationPort` override: без super, без retain ipc_port_t, повторная регистрация → 0x2bc; release'а в clientClose нет.

### Валидация sel 1
- prepareTransform: alloc request, memmove 0x1b0, gatherOptions (~30 бит flags → копирование опциональных полей), crop fixed16 → **fcvtzu → u32 без clamping на уровне userclient** (лимиты — в HAL MSR-вариантах), surface IDs → IOSurfaceRoot::lookupSurface(id, current_task()), shared events → retainFromTask.
- sel 7/8/9: IOMemoryDescriptor::withAddressRange(va, len, opts, task) + prepare/complete; len — kernel-side. GetDiag: len из глобала (count лог-буферов), magic 0x6944506b ядром НЕ проверяется (юзерленд-контракт); отдаёт сырые log_activity записи по 0x7c. sel 9: readBytes 0x1b0 → стек → getEstimation_gated → writeBytes 0x18; double-fetch нет.

## 45. v12: проверки на устройстве

- EnableKernelTests через set_cf_property (asm-label на _IORegistryEntrySetCFProperty): **0x2e2, readback absent** — sandbox не пускает. Гейт стоит.
- GetDiag 64KB: ~0x9b18 байт лог-записей (таймстампы mach, счётчики, activity-коды), **kernel-указателей нет** — infoleak низкой ценности.
- Notify-port UAF v1: 20 попыток — но порт НЕ тот (повторная регистрация 0x2bc, тест невалиден).

## 46. v13–v15: boundary, форматы, UAF v2, верификация

- **Boundary sweep**: declared src/dst dims 63..65536 на реальных 64×64 surface'ах — ВСЕ kr 0 (драйвер принимает). Эмпирическая проверка (fill/readback): трансформ реально исполняется (контент меняется), но sync-вызов возвращается ДО hw-completion — «oversize исполнился, normal нет» был артефакт латентности. Видимой порчи нет: dims, по-видимому, редеривятся из IOSurface-объекта.
- **«Аномалия same-format 0x2c2» разрешилась**: это не формат, а **src==dst surface id** — in-place трансформы отвергаются 0x2c2 (обычная валидация, не баг). Доказано state-probe'ами: свежие пары всегда kr 0, диагональ матрицы (ids[a]→ids[a]) всегда 0x2c2.
- Форматная матрица: BGRA/RGBA/420v/420f кросс-пары — все kr 0; фейковый формат → 0x2c7.
- **Notify-port UAF v2 (destroy-first, свежие коннекты, спрей 64 портов, 30 попыток × 32 async на 2048²)** — устройство живо. Кандидат закрыт (видимо, ссылку держит IOKit-слой, либо completion не доходит/теряется без паники).
- State-probe после каждого sub-loop граничного свипа: состояние драйвера не портится (same-conn и fresh-conn kr 0).

## 47. Статус и оценка направления

Закрыто динамикой: границы dims/crop (fcvtzu-путь), форматная путаница, in-place, notify-port UAF (2 варианта), SetCFProperty-обход гейта KernelTests, diag-infoleak (пусто), double-fetch в sel 9 (статически нет). Суммарно ~500k+ вызовов sel 1 за все версии + мутации flags/rects/ids — **ни одной паники, ни одного неожиданного kr за пределами известного набора**.

Вывод: userclient-слой скейлера вылизан. Оставшиеся идеи по нему:
1. HAL MSR-варианты (tiling-математика, frameDescriptor::SingleDimension @ 0x98c1088+) — нужен прицельный статический разбор лимитов, фаззить вслепую неэффективно.
2. KernelTests остаётся недосягаемым (гейт EnableKernelTests; обход = отдельный примитив).
3. GetHistogram (sel 7) gated byte[client+0x158] — не изучено, что его ставит.

План Б (приоритет теперь выше): VCPDRM (sandbox), IOGPU, новые kext'ы 27.0 (Image4, AFKHIDTBDevice), либо фазз macOS-варианта драйвера локально на Mac (быстрая итерация, lldb) с переносом находок на iOS — риск паники хоста, требует согласия пользователя.
