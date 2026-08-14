# Журнал, часть 17 (секция 86, 14.08.2026) — JPEG-трек: разведка боем

## 86. AppleJPEGDriver из App-Sandbox (v93, фаза p_jpeg, env FUZZ_JPEG=1)

Цель: аппаратный JPEG-декодер (DMA-класс: рассинхрон размеров из JPEG SOF vs dst
surface — кандидат из part9 §57: sel7 SOF 0xffff×0xffff).

### Разведка
- Сервис в registry один: `AppleJPEGDriver` (матчинги SJPEGDriver/AppleH16JPEG
  не найдены; в KC-строках есть ещё `SJPEGDriverUserClient` — отдельного сервиса
  на устройстве нет).
- **IOServiceOpen принимает ЛЮБОЙ type** (0..8, 0x100, 0x1000, 0x10000,
  0x100000..0x100005) с kr 0 — type-blind newUserClient, что само по себе
  признак деградированного/стабового клиента (настоящие per-type клиенты type
  валидируют).
- Класс коннекта прочитать нельзя (IOObjectGetClass → 0xe00002c2, как в v87).

### Карта селекторов (полная, все формы вызова)
sel 0..9 × {CallStructMethod, CallAsyncMethod(wake+ref), CallScalarMethod,
CallMethod(scalar+struct), CallMethod stIn{0x1d0,0x2c8,0x3c0}×stOut{0x100,0x1d0,
0x1000}, IOConnectTrap0/1} × types {0..8, 0x100..0x100005}:
**ВСЁ 0xe00002c2 (kIOReturnUnsupported)**. Ни один селектор ни в одной форме не
дошёл до dispatch. (run-v93{,b,c,d,e}.log)

### Вывод
С нашими entitlements userclient не имеет ни одного доступного метода — клиент
деградирован (вероятно, полный клиент создаётся только для entitled-процессов;
в KC-строках видны гейты com.apple.applejpegdriver.poweron (sel 8/9) и
com.apple.applejpegdriver.ajpegtestapp). Sanity decode и SOF-фазз через прямой
userclient из App-Sandbox **невозможны без entitlement'а** — трек закрыт на
этом этапе. Альтернативный путь к декодеру — через mediaserverd/ImageIO
(косвенно, без контроля над struct'ами) — для DMA-фаззинга бесполезен.

Статика для будущего: /tmp/AppleJPEGDriver.macho (extract_fileset.py), но
fileset-extract теряет chained fixups → таблица externalMethod не восстанавливается
наивным поиском text-ptr'ов; при возврате к треку — декодировать
LC_DYLD_CHAINED_FIXUPS или анализировать newUserClient на предмет entitlement-веток
(строки гейтов известны).

## 87. DART-домен скейлера: PERSISTENT + SHARED, cross-request write подтверждён (v94)

Фаза `p_dartmap` (env FUZZ_DARTMAP=1, FUZZ_DART_STEP=0..3, run-v94{,b,c,d}.log).
База: killshot-пейлоад (sel1, dst rect 32×32 в 64×64 BGRA, flags bit28,
border X=Y=32 W=H=0xFFFFFFE0, цвета 0xff) — baseline: panic (bug 210) в 30–50%
выстрелов, иначе 0xe00002d6.

### Эксперименты

- **E1 (step 0)**: легитимный scale 2048×2048 (2×16MB поверхности) → kr 0;
  затем 10 killshot'ов на ТОМ ЖЕ коннекте: **10/10 recoverable (0xe00002d6),
  ни одной паники** (baseline 30–50%/shot → p≈0.001 за чистую удачу).
  Маппинги переживают запрос — домен НЕ per-request.
- **E2 (step 1)**: то же, но killshot'ы на СВЕЖЕМ втором коннекте (после big-map
  на первом): **10/10 recoverable** — маппинги первого коннекта видны второму:
  домен **shared между коннектами** (общий DART context скейлера).
- **E4 (step 3)**: большая dst (16MB) заполнена page-маркерами (байт = номер
  страницы), легально замаплена нормальным scale; затем 6 killshot'ов с
  маленьким dst. CPU-readback: **4080/4096 страниц повреждены** (первая — уже
  страница 1, оффсет 0x1000) — *** CROSS-REQUEST WRITE CONFIRMED ***.
  Паттерн записи: страницы **зануляются** (ff 0 / zero 4096 / other 0) — цвета
  0xff из пейлоада НЕ проходят (видимо, premultiply/format path), пишется 0x00.
  Охват — практически весь extent легального маппинга (~16MB).

### Вердикт

DART-домен AppleM2ScalerCSCDriver — **persistent + shared**: маппинги живут
после завершения запроса и общие между userclient-коннектами. wraparound-запись
попадает в страницы, замапленные ДРУГИМИ запросами (и, потенциально, другими
клиентами скейлера — WindowServer!, у него постоянные scale-операции). Примитив:
**массовое зануление (~16MB за вызов) страниц чужого маппинга** — классический
ингредиент эскалации (zeroing страницы, которую ядро переиспользует; дальше —
угадывание/поджим страницы под kernel object). Значение записи пока 0x00 —
подбор color-полей для controlled value — следующий шаг (color pipeline).

Не сделано: E3 (extent vs dst size серия с замером fault-DVA из panic-логов) —
код есть (FUZZ_DART_STEP=2, FUZZ_DART_SIZE=W), паники не понадобились для
вердикта; оставлено на потом (extent-слэк за пределами поверхности не измерен).


