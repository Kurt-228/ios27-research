# Журнал, часть 18 (секции 104–106, 04.09.2026) — инфра iOS27b4, xpleak (CPU-путь закрыт), mtlmut

## 104. Инфраструктурные сломы iOS 27b4 и их обход (пререквизит прогонов)

Симптомы при возврате к девайсу после 14.08: любой `devicectl device process launch`
с `--console` и/или env/argv → CoreDevice 10002 EINVAL; FrontBoard периодически
отклонял открытие ЛЮБОГО bundle id (FBSOpenApplicationServiceErrorDomain 1) —
лечится ребутом устройства (системные сервисы деградируют после фазз-сессий).

Найденные причины и фиксы:
1. **SIGTRAP при запуске**: новый Xcode/SDK убивает приложения без scene lifecycle
   (`__UIApplicationEvaluateRuntimeIssueForNoSceneLifecycleAdoption`, scene create).
   Фикс: `application:configurationForConnectingSceneSession:options:` в AppDelegate
   (fuzzer/main.m) — legacy UIWindow-путь продолжает работать.
2. **10002 EINVAL при `--console` + `--environment-variables`/`DEVICECTL_CHILD_`/argv**
   на этом билде: конфиг фаз теперь передаётся KEY=VAL аргументами командной строки,
   парсинг в main() через setenv (fuzzer/main.m). Запуск БЕЗ `--console`, вывод —
   через FUZZ_LOGFILE=1 (Documents/fuzz.log) + `devicectl device copy from
   --domain-type appDataContainer`.
3. **10002 EINVAL также возникает, если предыдущий инстанс приложения ещё жив** —
   обязателен terminate перед запуском (встроено в раннер).
4. Раннер `relay/run_phase.sh <log> <wait_secs> KEY=VAL...`: kill leftover →
   launch (argv, no console) → wait с детектом самостоятельной смерти процесса
   (краш/SIGKILL) → terminate → pull fuzz.log. Использовать вместо relay.py для
   разовых фаз.

## 105. Вектор «cross-process GPU memory leak» — CPU-путь закрыт (v111, фаза p_xpleak)

Гипотеза v90/v91: страницы GPU убитого процесса переходят новым владельцам без
очистки (тогда наблюдалось GPU-зондами по служебным страницам). Чистый
двухролевой эксперимент (fuzzer/t_xpleak.m, FUZZ_XPLEAK=victim|obs|loop):

- **Контроль (без жертвы)**: 12336 аллокаций (MTLBuffer storageModeShared
  16KB–1MB + IOSurface BGRA/420v 64²–1080p), 6.6 GB readback — все нули
  (results/run-xpleak-obs0-control.log).
- **victim → obs**: жертва заполняет 384 буфера / 256 MB маркерами
  (сигнатура 0xCAFEB0BA + ptr-like 0x09_xxxxxxxx + ASCII), kill(getpid(),SIGKILL)
  без освобождения (results/run-xpleak-victim.log); наблюдатель сразу после —
  23472+ буферов, 12.6 GB: **0 ненулевых байт, 0 сигнатур, 0 ptr-like**
  (results/run-xpleak-obs1-after-victim.log).
- **loop (same-process free→realloc)**: 64 MB маркеров освобождены штатно,
  немедленный реаллок-скан 9264 буфера / 5 GB — тоже все нули
  (results/run-xpleak-loop.log).

Вывод: CPU-видимые userland-аллокации (MTLBuffer shared, IOSurface) ВСЕГДА
занулены при выдаче — даже при same-process reuse. Страницы скрабятся до
попадания в пользовательское VA-окно; «грязь» v90/v91 существует только в
GPU-служебном слое (driver-internal страницы GPUVM) и видна исключительно
GPU read-примитивом (source-patch, v90), не CPU readback'ом. Вектор в постановке
«второй чистый infoleak-баг через CPU» закрыт. Остаётся узкий follow-up:
GPU-зонд скан свежих service-страниц после смерти ДРУГОГО приложения
(не своего) — отложен, ценность ниже mtlmut.

## 106. Вектор «in-place мутационный фазз живого Metal command buffer» (v112, фаза p_mtlmut)

Реализация: fuzzer/t_iosurface_scaler.m, фаза p_mtlmut (env FUZZ_MTLMUT,
SKIP/MAX/ONLY). База — p_mtpatch (v89) + защиты v91 (xor-masked gpuA/gpuB,
self-patch первым зондом, restore после кейса). Детерминированная нумерация
кейсов (drift baseline ~6 dword — сверять resume по хвосту лога).
План на A17 Pro: kclen 0x2d8, ph1 (dword-оффсеты × словарь) 2002,
ph2 (bit-flips qword) 1152, ph3 (pool-slot GPUVA подмены) 8,
ph4 (rid подмены seglist) 10, ph5 (size-поля OOB) 4 — всего ~3176 кейсов.

Дымовой прогон MAX=50 (results/run-mtlmut-smoke.log): все ph1-кейсы заголовка →
status 5 (kIOGPUCommandBufferCallbackErrorInvalidInput) — чистый reject драйвером,
без крашей/паник, фаза завершилась живой.

### Результаты полного плана (3176 кейсов, run-mtlmut-full1/2/3 + ph345 + ph45)

- 2138 кейсов — чистые reject'ы драйвера (status 5: Invalid Input / Internal /
  Invalid Resource); 57 — status 4 с игнором мутации (неиспользуемые поля).
- **off 0x174 (=0x190000, оффсет-поле)**: зануление/уменьшение →
  kIOGPUCommandBufferCallbackErrorPageFault (GPU address fault, пойман драйвером
  чисто). 2+ фолта → очередь латчится в SubmissionsIgnored (все последующие
  submit'ы игнорятся до нового процесса) — методологический урок: фазы с
  реальным исполнением (ph3+) прогонять в чистом процессе отдельно.
- **Три детерминированные kill-точки процесса** (без crash report = GPU-fault
  kill класса v78 R4, устройство не страдает):
  1. kcmd+0x150 dword (baseline 0x268 — поле длины команды) = 0xffffffff
     (case #929, repro 2/2 в run-mtlmut-only150.log);
  2. kcmd+0x150 qword (0x3_00000268 = {count 3, len 0x268}?) — bit-flip старших
     бит (ph2, case #2277+);
  3. ph3 dst pool-slot → GPUVA 0x1_00000000 (запись 64KB) — kill в чистой
     очереди (case #3161); dst → 0x1deadbeef0000 / 0xffffffff0000 — молчаливый
     drop (status 4), src → те же адреса — drop / 8 ненулевых байт readback.
- ph4 (rid-подмены в seglist): все 10 → Invalid Resource (residency-валидация
  цела). ph5 (size-поля 0xac): OOB-размеры → Invalid Input, 0 → no-op.
- Паник ядра, нестабильности устройства, записи вне своих буферов и утечек в
  readback не обнаружено. Известный дефект реализации ph3: gscan_patch
  перезаписывает собственные глобалы gpuA/gpuB (v91-ловушка в новом виде) —
  логи old/new в ph3 недостоверны, семантика кейсов восстановлена по порядку
  pv[]; при доработке хранить адреса только в g_srcx/g_dstx.

Вывод: однополевая мутация живой copy-команды ядро не ломает; валидация
драйвера робастна. Kill-точки — self-DoS (app-kill через GPU fault).

### Доразведка kcmd+0x150 (run-mtlmut-150rest{,2}.log)

Карта значений поля длины (baseline 0x268): 0x267/0x269 (off-by-one),
0x2680, 0x26800 → чистый Internal Error (драйвер валидирует длину против
shmem, OOB-read транслятором не достигнут); 0x7fffffff/0x80000000/0xffffffff
→ app-kill при commit. Т.е. порог «принято, но фолтит» существует только в
области гигантских значений — кernel-side интереса не представляет.
