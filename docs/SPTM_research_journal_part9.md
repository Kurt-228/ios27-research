# Журнал, часть 9 (секции 55–58, 13.08.2026) — поиск write-примитива

## 55. Оружие border-fill: адресная арифметика (что узнали)

- kext-сторона (точно): frame dims регистры 0x3dc/0x3e0 = `(selW+bfW) & 0x1ffff` / `(selH+bfH) & 0x1ffff` (наш пейлоад → 0); border X/Y → 0xf044/0xf048 (+дубли 0xf04c/0xf050), 17 бит; base = сырой DVA surface; stride = surface stride. **Валидация bfX снимается при bfW=0** (cbz), bfY — при bfH=0.
- MSRCPU firmware (9 блобов Thumb-2, база IMEM 0x100000/DMEM 0x10000000, Cortex-M) — border-fill логики НЕ содержит: чистый оркестратор (frame descriptor ring, magic 0xfdbefdbe). Запись делает аппаратура (RTL), точная формула старта из бинарей не восстанавливается. Гипотеза по поведению: start ≈ base + (selH+bfY)*stride, далее построчно вверх до незамапленной страницы.
- Динамика v29–v33: при wrap — 0 записей в dst/src, фолт выше (старт за пределами обоих). Без wrap (frame ≤ buffer) — полная/частичная заливка dst с kr 0 (валидно, без OOB). bfX при bfW=0 НЕ двигает запись (oracle-свип 0..0x1f000 — идентично). bfY-свип: строгая проверка bfY>selY.
- v30/v28: corruption NC/tile-буферов ненаблюдаема (post-shot трансформы pixel-perfect); v34 cross-request race — безрезультатно (kill-shot сериализует драйвер).

**Итог по скейлеру**: write-примитив из border-fill wraparound не выстроен — запись уходит в незамапленное (DoS) либо в собственный dst (валидно). Смежности с чужими страницами в DART-домене добиться не удалось (маппинги per-request, unmap на completion; persistent-объекты внизу окна). Находка остаётся сильным 0-day DoS (panic bug 210).

## 56. План Б: карта атакуемой поверхности (v37/v38, динамика с устройства)

Открыто из sandboxed-приложения (без entitlements):
- **AppleJPEGDriver** (type 0–3), **IOMobileFramebufferAP** (type 0–3), **IOGPU** (type 1), **AppleKeyStore** (type 0–3), IOSurfaceRoot, AppleM2ScalerCSCDriver.
- Gated: VCPDRM (0x2e2), AppleImage4/Image4 (0x2c2), AppleCredentialManager, AppleSMC, NVMe, baseband, AuthCP, AVD (0x2c1), IOAudio2Device.

## 57. Триаж userclient'ов (статика по iOS BootKC)

### AppleJPEGDriver (10 селекторов, 2 стаба, 2 entitlement-gated)
- startDecoder/Encoder + Ext/2024 варианты. Код высокого качества (bounds-safety traps, проверки размеров). RST-таблица в DecoderExt/2024: n≤0x1000, n%3, capacity-clamped memcpy + post-check `brk`. Прямого write нет; единственный класс — DMA: рассинхрон размера из JPEG-заголовка vs dst surface (валидация в decodeRequestValidation — не добрана). Кандидаты: sel7 SOF 0xffff×0xffff на маленьком dst; sel4 encoder overflow (детект постфактум).
- Entitlements: com.apple.applejpegdriver.poweron (sel 8/9), ajpegtestapp.

### IOMobileFramebufferAP (100 записей, ~35–40 реализовано в Legacy)
- **sel 5 swap_submit: 32-битный wraparound в crop-валидации** (`x+w ≤ surfW` без overflow check @ 0xfffffff009ebce18) — тот же класс, что скейлер! НО: это OOB **read** в отображаемый кадр (DCP читает за пределами surface → infoleak в кадр), write-путь — в fileset'е IOMobileGraphicsFamily-DCP (не добран). Осложнения: нужен существующий swap id (begin-swap селектор не найден статически), frame rect должен быть fullscreen.
- sel 48 hdcp_get_reply: kernel пишет reply по user VA (self-write, слабо).
- Entitlement-гейты: set-block (sel 78/79), gain-map-access (sel 68 enum 0x11, sel 5 байт +0x3f2).

### IOGPUFamily (56 селекторов + 5 трапов, type 1 = полная таблица)
- **sel38/39 replace_backing — race закрыт статически**: ranges копируются одним атомарным LDR Q на запись, валидация и использование по kernel-копии (vm_region обход, executable-страницы отвергаются, overflow addr+len отвергается).
- **Рабочий layout sel8 s_new_resource восстановлен и проверен динамически**: insz 0x68/outsz 0x58, type 0x80, in+0x38=va1, +0x40=va, +0x48=size → kr 0, id в out+0x24.
- **Подтверждённая слабость: res->len = va1 − base НЕ валидируется против wired-размера** (in+0x48). Inflated resource (len ~1MB при wired 16KB) **принят** (rid 2, kr 0). Потребители res->len (GPU-VA mapping в submit/mapping-ops) — следующий фронт: GPU-запись за пределы wired-страниц.
- sel38/39 на обычном ресурсе → 0x2c2 (гейт replaceable [backing+0x30]&0x10; флаги +0x15/+0x16 его не снимают — источник флага не найден).
- sel42 (new command queue) работает: kr 0, out = id + token.
- Код высокого качества: umulh overflow-checks, metaCast, retain/release корректны.

## 58. Текущие активные направления (по приоритету)

1. **IOGPU inflated-len resource → потребители res->len**: построить submit-цепочку (command queue + command buffer через shared memory, sel6) с inflated ресурсом → GPU write за пределы wired-страниц. Самый конкретный путь к write-примитиву.
2. **IOMFB crop wrap**: найти begin-swap селектор динамически (перебор), проверить writeback-режимы в -DCP fileset'е (вырезан, не проанализирован).
3. **JPEG DMA**: крафт JPEG с SOF 0xffff×0xffff на маленьком dst — проверить 64-битность валидации output size.
4. GetHistogram client+0x148 — перезапись только из request type 0xc (внутренний), с userland не достаётся; len=1536 фиксирован. Закрыто.
5. SetCustomFilter (iOS): a2=a5=9, a3=a6=0x20, a1=a4≠0 → чтение ровно 0x1200 байт coeffs, OOB нет. Закрыто.

### Артефакты сессии
- Kext'ы: /tmp/AppleJPEGDriver.macho, /tmp/IOMobileGraphicsFamily.macho, /tmp/IOGPUFamily.macho (+ /tmp/*/disasm), скрипт /Users/kurt228/.kimi-work/sptm-analysis/extract_fileset.py
- MSR firmware blobs: /tmp/msrfw/blob0..8.bin + дизасмы
- Прогоны: results/run-v29..v39*.log
