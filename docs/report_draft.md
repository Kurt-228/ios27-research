# Draft: Apple security report — iOS 27.0 beta 4 (24A5390f), iPhone 15 Pro Max (A17 Pro)

Статус: ЧЕРНОВИК для внутренней доработки. Все факты воспроизводимы из
App-Sandbox приложением без entitlements (fuzzer/ в этом репо, фазы и логи
указаны по именам). Журналы: docs/SPTM_research_journal_part*.md.

## Finding 1: AppleM2ScalerCSCDriver — 32-bit wraparound → DART write fault → kernel panic (bug_type 210)

- **Класс**: недостаточная валидация границ (integer wraparound) в
  validateBorderFill (4 сайта, `add wN, wM, wK` без overflow-проверки;
  iOS kext оффсеты 0xfffffff008f81504/528/54c/570).
- **Триггер**: один вызов selector 1 IOSurfaceAccelerator userclient
  (dst rect 32×32 в 64×64 BGRA, border X=Y=32, W=H=0xFFFFFFE0, цвета 0xff).
  Репродюсер: fuzzer/t_iosurface_scaler.m, фаза p_borderfill.
- **Эффект**: DMA-запись за пределы IOSurface → DART write fault →
  kernel panic bug_type 210 (AppleT8110DART.cpp:2265). Rate 30–50% за выстрел,
  иначе recoverable 0xe00002d6. Логи: results/run-v19*.log, results/panics-*.
- **Дополнительно**: Y-wrap вариант (Y=0xFFFFFFF0, H=0x20, dst 4096×4096)
  даёт подтверждённую запись нулей ~300KB ВНУТРИ пользовательской поверхности
  (v100); двойной wrap (X=Y=0xFFFFFFF0) принимается с kr 0 — молчаливая порча
  собственной поверхности контролируемой длины (span ≈ 8064+252·W байт, v101);
  hang-точки драйвера (off 0x084/0x114, v101) — live-lock DoS.
- **Границы**: запись не выходит за DART-окно (выход = fault = паника);
  контент записи всегда 0x00; DART-домен скейлера — системно-общий
  (docs/scaler_dva_formula.md §6: поверхности IOMobileFramebuffer/WindowServer
  в том же домене, TTL ~2с) — cross-surface запись геометрически возможна,
  но DVA-груминг чужих поверхностей не достигнут.
- **Оценка**: kernel DoS из sandbox (reliable-ish). Безопасностная ценность —
  отказ в обслуживании ядра; эскалация до write-примитива не показана.

## Finding 2: IOGPU/AGX — GPU write after resource destroy (UAF, stale GMMU TLB)

- **Класс**: рассинхрон lifetime — teardown GPU-ресурса (IOConnectTrap1 sel1)
  не синхронизирован с уже отправленными в очередь GPU-записями; запись
  исполняется в освобождённые страницы через stale GMMU TLB (окно ≥ сотни мс,
  async invalidate последний в AGXUAT::process).
- **Триггер**: in-place патч живого Metal command buffer (pool-slot GPUVA →
  raw-ресурс), конгестия очереди, commit → destroy → форс AGXUAT::process
  (33+ unmap). Репродюсер: фазы p_gpuuaf (v105) / p_uatrec S2/S3.
- **Эффект**: запись с ПОЛНЫМ контролем контента (байты src-буфера) в
  физические страницы, уже возвращённые в общий ядерный аллокатор.
  Подтверждено: v105 (write-after-destroy 65536×0x41), S3 (3/3 раунда,
  запись в старые страницы при переиспользованном GPUVA новым владельцем).
- **Системное проявление** (воспроизводимо при пользовательской активности):
  чужие GPU-клиенты (backboardd) получают наши freed-страницы → их очередь
  фолтится → серии GPURestart (kernel log), пользовательски — фризы/глитчи
  Dynamic Island, остановка аудио, деградация LTE (results/run-uatrec-s4*,
  syslog-корреляция по времени).
- **Границы**: наблюдение/прицеливание записи не достигнуто (страницы
  скрабятся при выдаче userland; kernel-структуры в пути — kalloc_type/
  dedicated shmem, спреем не достижимы); reclaim в наши аллокации не
  работает (v106–v110). Эскалация до контролируемой kernel-порчи не показана.
- **Оценка**: memory-safety дефект в ядре (write-after-free по физическим
  страницам) с системными последствиями; прямой эксплойтации без
  дополнительного примитива нет.

## Finding 3: GPU VM — чтение неочищенных страниц убитых процессов (infoleak)

- **Класс**: отсутствие scrub'инга GPU-служебных страниц при
  перераспределении между процессами.
- **Триггер**: in-place патч source pool-slot живого Metal blit (GPU read
  любых замапленных GPUVA своего контекста) после краша соседнего процесса.
- **Эффект**: чтение остатков GPU-памяти убитого процесса, включая таблицы
  с CPU-указателями ядерных/драйверных объектов (0x09_xxxxxxxx heap VA) —
  де-ASLR материал из App-Sandbox (v90/v91, results/gpuvm-dump/).
- **Границы**: CPU-видимые аллокации скрабятся (v111/xpleak — 19GB сканов,
  все нули); утечка только в GPU-служебном слое и только после краша соседа.
- **Оценка**: infoleak/KASLR-relevant material; требует краша соседнего
  GPU-процесса.

## Finding 4: IOGPUCommandQueue::init — NULL-deref panic из App-Sandbox (09.09)

- **Класс**: чтение неинициализированного члена в error-path (CWE-476).
- **Триггер**: sel6 s_new_command_queue, поле version (structureInput+0x400)
  ≥ 5 → gate «version < 5» → ранний error-exit → диагностический блок
  безусловно читает this+0x488 (device ptr, пишется только на success-пути)
  → ldr [NULL+0x38] → kernel data abort, panic bug_type 210 (far 0x38).
  Требует ненулевого глобального log-флага (на 27.0b4 установлен).
- **Воспроизводимость**: 3/3 детерминированных паники 09.09 (12:44/13:59/14:06),
  triage: docs/panic_triage_0909.md (символизация по kc27, UUID совпал).
- **Оценка**: kernel DoS из sandbox одним вызовом; corruption нет.

## Инфраструктура воспроизведения

- fuzzer/ — приложение (clang build через relay/build.sh), фазы по env
  (см. диспетчер в конце t_iosurface_scaler.m); запуск relay/run_phase.sh.
- Все запуски — на исследовательском устройстве, iOS 27.0 beta 4.
