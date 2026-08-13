# Журнал, часть 4 (секции 40–43, 13.08.2026)

## 40. Девайс-фаза II: v9 — слепые структурные пробы

- outsz-карта (sel × outsz 0..0x1fff): **sel 11 — variable-length getter**: принимает любой outsz, пишет min(outsz, 0x298); остальные методы struct-output не принимают (0x2c2). Гипотеза v8 «у каждого метода фиксированный ожидаемый outsz» опровергнута.
- **Async-порог: outsz > 0x1000** → 0xe00002bf (0x1000 ещё sync, 0x1800 уже async).
- sel 5 — **scalar**-геттер: IOConnectCallScalarMethod in0/out1 → kr 0, значение 1 (= clientID, подтверждено реверсом).
- sel 10 enum 0..3 → kr 0 (все четыре), ≥4 → 0x2c2. Выходных данных нет.
- sel 7 oracle: surface id / малые int → kr 1 (KERN_FAILURE), qword'ы из sel 11-блоба → kr 2 (KERN_INVALID_ARGUMENT). Двухстадийная валидация (позже объяснено: input = VA, kr 1 = copyin failed).
- sel 9 pair-sweep 600 комбо из пула id — всё 0x2bd/0x2c2. sel 1 path A (+8≠0) со всеми кандидатами — без изменений.
- 20s race (7vs8, 9, 10/11, open/close storm 35k opens) — чисто. Steady fuzz 217k раундов — паник нет.

## 41. Локальный реверс IOSurfaceAccelerator.framework (перелом)

Mac: Mac16,10 (t8132), **macOS 27.0 (26A5388g)** — то же поколение, что и цель; в IORegistry живой AppleM2ScalerCSCDriver (scaler0, t8101-compatible). Извлечение dyld cache: `/System/Volumes/Preboot/Cryptexes/OS/.../dyld_shared_cache_arm64e` через dsc_extractor.bundle (iPhoneOS.platform) + самописный фронтенд (/tmp/dsc_extract.m) → 4084 образа в ~/.kimi-work/sptm-analysis/macoscache/.

Дизассемблинг `IOSurfaceAccelerator.framework` (36 КБ, полные символы) — **полная карта протокола userclient**:

| sel | in | семантика (framework-функция) |
|---|---|---|
| 0 | 0x40 | CaptureSurface (+0x20 surfID, +0x24/+0x28 dstW/H, +0x2c/+0x30 capW/H, +0x34/+0x38 offX/Y, +0x18 Transform<<1) |
| 1 | 0x1b0 | TransformSurface/TransferSurface/BlitSurface — request descriptor |
| 2/3 | — | AbortTransfers/AbortCaptures (scalar, без ввода) |
| 4 | 0x20 | SetCustomFilter {a1..a6 != 0, VA coeffs} |
| 5 | scalar→u32 | GetID (clientID) |
| 6 | 0xfa8 | **KernelTests: сырой passthrough, [0]=count ≤ 0x3e8** |
| 7 | 8 | GetHistogram: input = **userspace VA** буфера бинов |
| 8 | 8 | GetDiag: input = **userspace VA** diag-структуры, magic 0x6944506b ('kPDi') |
| 9 | 0x10 | GetTransformEstimation: {VA request 0x1b0, VA out 0x18} |
| 10 | 0x18 | SetProperty {prioBand<4, pad, dutyCycle<0xf4241, histDur<0xf4241}; Create шлёт {2, 50000, 500000} |
| 11 | out | caps getter (ParavirtProcessGuestData type 1) |

Request struct 0x1b0 (sel 1): +0x00 u32 srcID, +0x04 u32 dstID (**не +0x50/+0x58, как в v8/v9!**), +0x08..+0x1f 3×qword request-object/action (0 = scheduler path), +0x20 flags (59 бит опций, бит 12 FixUpscaling auto, без options выставляется бит 13), +0x28/+0x30 crop x/y fixed16, +0x38/+0x40 crop w/h fixed16, +0x48/+0x4c src w/h, +0x60 dst rect raw, +0x70/+0x74 dst w/h, +0x78/+0x90 mach-порты shared events, +0xa8..+0xd2 border/alpha/writeonly, +0xdc..+0x10f histogram params, +0x110..+0x1af 4×0x28 CommApi (проверки count ≤ 4 в framework НЕТ — userspace-переполнение при >4 элементах CFArray).

Create: IOServiceMatching("AppleM2ScalerCSCDriver"), type 0; ключ `Sharpener` в properties → жёсткий 0x2c7. Completion: IONotificationPort + IOConnectSetNotificationPort(type 0) + RunLoopSource.

## 42. v10: легитимные вызовы — пайплайн ожил

- **sel 1 TransformSurface 64×64 BGRA→BGRA → kr 0** (первый реальный трансформ; fw поднята).
- sel 9 GetTransformEstimation → 0, out: float-похожие оценки (0x213, 0x973, 0x36f).
- sel 8 GetDiag → 0, дамп состояния драйвера: счётчики, mach-таймстампы (0x0ac795b3…), поле +0x98 `01 00 26 03`.
- sel 7 GetHistogram → 0 (нули), sel 10 → 0, sel 5 → id 1.
- Отличия iOS 27 от macOS 27: sel 0 (Capture) → 0x2c7, sel 2/3 (Abort) → 0x2c7 — отсутствуют на iOS.
- sel 4 с константами SetFilterCoefficients type 1 → 0x2c2 (валидация на iOS строже/иная).
- **sel 6 KernelTests → 0x2e2 (not ready) даже после успешного трансформа** — гейт не fw-power.
- Async (outsz > 0x1000): после async-submit на notification port пошёл **шторм сообщений msgh_id 0x35, size 112** (тысячи; тело: 0x0c, 0x96, 0, 0, 16 случайных байт). Природа шторма не выяснена (в v11 64 async-submit'а → 0 сообщений за 3s — триггер не «просто async»).
- 20k раундов мутаций (flags/dims/rects/ids/req-qwords) — паник нет. Unmap-race → SIGSEGV userspace (ожидаемо), устройство живо.

## 43. v11: async-токены, KernelTests, гонки

- p1: 64/64 async принято, completions за 3s = 0 → сообщения v10 не являются мгновенными per-request completions.
- p2: token feedback пропущен (нет caps) — request-object путь (+0x08≠0) с настоящими токенами НЕ проверен.
- p3: sel 6: count 0x3e8 → 0x2e2, **0x3e9 → 0x2c2** — граничная проверка подтверждена на iOS; test-id в [0] 0..15 — все 0x2e2; async → 0x2bf.
- p4: diag принимает мутации полей; поле +0x10 ответа = 0x01000040_xxxxxxxx (var low dword).
- p5: 60k смешанных раундов sync+async — чисто.
- p6: гонки 60s (MADV_FREE под copyin sel 7/8; submit vs estimation) — без аномалий, устройство живо.

### Открыто / дальше
1. Триггер message-шторма (id 0x35): v10 шторм vs v11 тишина — найти условие; полный дамп 112 байт, корреляция с submit.
2. Request-object путь sel 1 (+0x08) с реальными токенами из сообщений.
3. Гейт sel 6 (0x2e2): кандидаты — entitlement, test-mode property, paravirt-флаг.
4. Декодирование GetDiag-полей (карта счётчиков/таймстампов).
5. CommApi (>4 записей в CFArray → выход за 0x1b0 в framework; kernel-сторона?).
6. План Б остаётся: VCPDRM-локи, Image4, IOGPU.
