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


