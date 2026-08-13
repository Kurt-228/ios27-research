# Журнал, часть 7 (секция 51, 13.08.2026) — ПЕРВАЯ ПАНИКА

## 51. Border-fill wraparound: от статики до падения устройства

### Цепочка
1. macOS HAL-статика нашла кандидат A: `validateBorderFill` (MSR23) — 32-битные сложения в проверках границ border-fill rect.
2. iOS 27.0b4 kernelcache скачан range-запросами из официального ipsw (24A5390f, Apple CDN), развёрнут `kmutil emit-macho`, fileset скейлера вырезан: `~/.kimi-work/sptm-analysis/mackc/AppleM2ScalerCSCDriver-iOS27.macho` (67 МБ, stripped).
3. iOS-вариант валидации восстановлен (структурный матчинг с macOS-символами; xref'ы на строки — через суффиксы из-за string merging):
   - `validateBorderFill` A (близнец MSR23) @ `0xfffffff008f8148c`; wraparound-сложения: X+W @ `0x8f81504`, dstW+W @ `0x8f81528`, Y+H @ `0x8f8154c`, dstH+H @ `0x8f81570` — **32-бит, unsigned, wraparound присутствует в iOS-билде**.
   - Вариант B (base-класс) @ `0xfffffff00907601c` — без HW/align гейтов.
4. Полные проверки iOS: bfillEnable (req+0xc48); HW-gate hal+0x1f3 → 0x2c7; (X+W)≤bufW; X>selX строго; (dstW+W)≤bufW; то же по Y; цвета = 2^n−1 в битности компонента; align (W|H)&0x1f==0 при hal+0x1d2 → 0x2c7.

### Динамика (v18/v19)
- Первые baseline'ы 0x2c2: причина — `dstW + W > bufW` (fullscreen dest + любой border не влезает) и невалидные цвета (0x11223344 не 2^n−1).
- 32-выравненный baseline (X=Y=2, W=H=32, dstrect 32×32 в буфере 64×64, цвета 0xff) → **kr 0, border fill реально исполнился** (readback: заливка 0xff в dst). Путь жив на t8130.
- **KILL-SHOT пейлоад: X=Y=32, W=H=0xFFFFFFE0, dstrect 32×32, буфер 64×64 BGRA.**
  Все проверки проходят через wraparound: X+W = 2^32 → (u32)0 ≤ 64; X>0; dstW+W = 16 ≤ 64; W 32-выравнен; цвета валидны.
- Результат: sel 1 → **0xe00002d6 (kIOReturnDeviceError — аппаратный/DART fault), устройство ушло в ребут** (не вернулось за 7 минут — вероятно panic/boot-loop, нужен hard reset).

### Оценка
- Это **подтверждённый обход валидации границ в AppleM2ScalerCSCDriver на iOS 27.0b4**: userland без entitlements программирует DMA-запись border-fill с шириной ~4G пикселей → выход за пределы IOSurface-буфера → аппаратный фолт и потеря устройства.
- Следующие шаги: (1) снять panic log (после ребута: idevicecrashreport), классифицировать фолт (DART fault vs полноценная kernel panic); (2) варьировать W/H и форматы для контролируемой записи (направленный OOB write за пределы surface — примитив); (3) проверить вариант B (base-класс без гейтов) — какой класс активен на t8130 показал align-гейт, значит MSR23-подобный; (4) оценить, покрывает ли DART-маппинг соседние страницы (возможна запись в чужие IOSurface/смежные IOVA).
- Пейлоад-детерминизм: воспроизводится одним вызовом из свежего состояния (после baseline).

### Файлы
- iOS BootKC: `~/.kimi-work/sptm-analysis/ios27-BootKC.kc` (73 МБ, 265 filesets)
- iOS kext: `~/.kimi-work/sptm-analysis/mackc/AppleM2ScalerCSCDriver-iOS27.macho`
- macOS kext (символы): `~/.kimi-work/sptm-analysis/mackc/AppleM2ScalerCSCDriver.macho`
- Логи прогонов: `results/run-v18*.log`, `results/run-v19.log`
- Фаззер: `fuzzer/t_iosurface_scaler.m` (фаза p_borderfill)

### Классификация фолта (run-v19b, idevicesyslog)
- Воспроизводится детерминистично (2/2). Kernel-лог в момент атаки:
  `MSR Debug status (MSR_CTRL_DBGSTS) error = 0x1628`,
  `MSR Interrupt status (MSR_GLBL_IRQSTS) error = 0x2`,
  `MSR Error Occurred = 0xe00002d6` (AppleM2ScalerCSC, HAL).
- Железо скейлера поймало фолт само (MSR irq), драйвер вернул 0x2d6, система прожила ещё ~50 с нормальной активности, затем умерла — картина соответствует **аппаратному watchdog'у** (зависший scaler/DART конвейер тянет за собой display/backboardd). panic-full на диске после первой смерти не нашёлся (wdt-паники пишутся не всегда).
- Оценка: W=0xFFFFFFE0 при записи регистров усечётся до 17 бит (~0x1FFE0 px ширина заливки) — железо ушло за границу IOVA-региона surface'а → DART/MSR fault. Если пространство за dst занято другими замапленными surface'ами, фолта может не быть → **контролируемая запись 0xff-каналами через ~131k px × 32 строки по соседним IOSurface** = потенциальный OOB-write примитив. Эксперимент v20 (спрей 300 surface'ов + скан на порчу) подготовлен.

### Дальше
1. v20: spray-OOB эксперимент (готов, ждёт устройство).
2. Если запись доходит до соседей — мерить дальность, варьировать форматы/цвета (2^n−1 per channel), искать чувствительные соседние объекты (не IOSurface, а что маппится в тот же DART-домен).
3. Вариант B (base-класс без align/HW гейтов) — проверить, какой класс активен на t8130 (эмпирически: MSR23-подобный, align-гейт есть).
