# Журнал, часть 6 (секции 48–50, 13.08.2026)

## 48. HAL-слой (MSR) — статика потока transform → DMA

Дизасм через llvm-objdump (otool -tv молчит на __TEXT_EXEC). Цепочка:
sel 1 → transform_surface → prepareTransform (memcpy user→request+0x58, gatherOptions ~35 бит flags) → transformGeneral → Hal::initPipeForTransform → validateDimensions (vtable+0x200) → validateXYOffsets (vtable+0x220) → checkAberrantSizes → setDimensionsProper → validate/executePipe → wireIOSurface/mapIOSurface (DART) → configureSizes/Offsets/Strides (MSR23) → DDA-solvers → executeTransform_gatedContext → per-unit prepare → transformKick.

Ключевые факты:
- **Declared dims (user +0x48/+0x4c/+0x70/+0x74) в геометрии НЕ участвуют** — единственный читатель getRequestLoadSize (статистика). HAL берёт реальные размеры из IOSurface-объекта. «Oversize принимается» из v13 — ожидаемо и безвредно.
- crop x/y — чистая дробь [0,1) (только low16); crop w/h — до 2^47 px, но дальше fcvtzu + валидации.
- Реальные лимиты: validateDimensions (min/max из caps, luma/chroma консистентность; planeCount<2 → chroma-проверки скипаются, udiv by 0 → 0 → молчаливый pass); validateXYOffsets (x+w ≤ realW для обоих DS — закрыто); checkAberrantSizes.
- DMA: IOVA из mapper'а, strides из surface; dims в регистрах усечены 17/18 бит, coords 14 бит, tile-coords strh (16 бит) — расхождения возможны только при dims > 0x20000 px (недостижимо для IOSurface практически).
- in-place src==dst → 0x2c2 подтверждено статически (accelerator-id equality check).

## 49. Кандидаты и их проверка на устройстве

### A. validateBorderFill 32-bit wraparound (MSR23 0x9880584)
`selX + selW` (32-бит) ≤ realW; selX=0xFFFFFFFE, selW=2 → сумма 0 → проходит. Пейлоад: flags bit 28, +0xac/+0xb0 = X/Y, +0xb4/+0xb8 = W/H, crop сужен до real−2.
- v16 (raw struct): даже baseline (0,0 2×2) → 0x2c2.
- v17 (НАСТОЯЩИЙ IOSurfaceAccelerator.framework на устройстве, dlopen + TransformSurface, все 7 ключей): baseline → 0x2c2. Геометрия-изоляция показала: rects = сырые u32-пиксели (fixed16 → 0x2c2), с zero-rects и NULL-opts → kr 0. Т.е. border-fill запрос формально валиден, но iOS 27 его отвергает — путь, видимо, вырезан/изменён в iOS-билде (как sel 0/2/3). **Без настоящего iOS-kext'а не верифицируемо → припарковано.**
- Побочный результат: рабочий вызов приватного фреймворка с устройства (Create/TransformSurface kr 0) — инструмент для будущих проб.

### B. dimensionAdjustmentsWithExtendedPixels (+1 px при full-frame 420)
420f 64×64 full-frame crop → kr 0, исполняется, видимой порчи нет (stride с padding). Benign.

### Закрыто статикой
- crop/dst x+w — закрыто validateXYOffsets + checkAberrantSizes; fcvtzu-saturation → reject.
- zeroFill — bounds-checks перед каждой записью.
- Тайлинг (makeActiveWindows/segmentTileSingleDimen) — min-клампы, чисто; sdiv by 0 → 0 без трапа.

## 50. Итоги направления скейлера (финальные)

- Протокол userclient восстановлен полностью (userland framework + kernel kext), легитимные вызовы работают, ~1M+ вызовов за v9–v17, **0 паник**.
- Все статические кандидаты проверены динамикой: UAF порта (2 варианта), border-fill wraparound, ext-pixels, boundary, format confusion, in-place, diag-infoleak, sel 9 double-fetch. **Багов в userclient/HAL пути не найдено.**
- Недосягаемое: KernelTests (гейт EnableKernelTests, sandbox не даёт SetCFProperty), GetHistogram byte-гейт.
- Для дальнейшей работы по скейлеру нужен настоящий iOS 27.0b4 kext (драйвер в 27 переписан; macOS-вариант — прокси): скачать kernelcache d84ap из ipsw range-запросами и продиффить border-fill/validate-цепочки.
- План Б остаётся: новые kext'ы 27.0 (VCPDRM gated, Image4, AFKHIDTBDevice), IOGPU.
