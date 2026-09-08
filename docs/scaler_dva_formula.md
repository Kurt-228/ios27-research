# Формула write_start_DVA скейлера (AppleM2ScalerCSCDriver, iOS 27.0b4)

Кэш: `results/kc27/com_apple_driver_AppleM2ScalerCSCDriver.macho` (carve из
`results/kc27/kernelcache_iphone16.macho`, та же сборка, что на девайсе).
Эмпирика — `docs/SPTM_research_journal_part17.md`, секции 87–97.
Все адреса VA kext'а: `__text` 0xfffffff008f70870+, `__cstring` 0xfffffff0075ff9e0,
`__const2` 0xfffffff007f69f18, `__data` 0xfffffff00b41d438.

## 0. Сводка (вердикт)

- Стартовый DVA записи dst формируется железом как **base + offset**, где
  `base` — 64-битный DVA поверхности dst из DART-маппинга, `offset` —
  беззнаковая комбинация 17-битных полей X/Y дескриптора и pitch.
  Программирование дескриптора идёт через импортированный «writer»-синглтон
  (`setField(cmd=0x300, offset, value&0x1FFFF)`), весь путь от border/crop
  полей запроса до полей дескриптора — внутри kext'а и разобран ниже.
- **«Назад» от base напрямую нельзя**: координатные поля беззнаковые
  (маскирование `& 0x1FFFF`, знакового расширения нет). Отрицательные s32
  значения превращаются в большие беззнаковые 17-битные. Единственный
  наблюдаемый эффект «сдвига назад» — перенос произведения Y·pitch, который
  эмпирически возвращает старт записи к `dst_base` (wrap), а не за base.
- **Cross-surface write (запись в чужую поверхность через параметры) не
  подтверждён**. Выход за пределы mapped-окна поверхности даёт DART fault →
  `dartErrorHandlerCallback` → паника (bug 210: write DVA 0x1000003c000 /
  0x10000040000). Для прицельной записи «вперёд за extent» рабочий ресурс —
  span внутри/за краем dst-окна до ближайшей DART-границы.

## 1. Цепочка border → дескриптор (верифицировано дизасмом)

### 1.1 Поля запроса (request, x19)

| Смещение | Поле |
|---|---|
| +0xc48 | border enable (бит) |
| +0xc4c | border X |
| +0xc50 | border Y |
| +0xc54 | border W |
| +0xc58 | border H |
| +0xc78..+0xc84 | crop X, Y, W, H (см. §2) |
| +0xc70 | crop enable |
| +0xc88 | массив u32 для crop-списка (см. §2) |
| +0xca8 + id·4 | u32 на поверхность (индекс/флаги маппинга) |
| +0xcb0 + id | байт флагов на поверхность |
| +0x7c0 / +0x7c4 | делители координат (bpp/субсэмплинг формата) |

Геометрия: хелпер `0xfffffff0090691f4` возвращает объект x21=[x0+0x18],
у него dstW = [x21+0x20], dstH = [x21+0x24]; маппер — [x0+0x90].

### 1.2 validateBorderFill @ 0xfffffff008f8148c

- 4 wrap-сайта подтверждены; ошибка валидации `0xe00002c2`.
- Пройденные значения НЕ отбрасываются: дальше идут в дескриптор под маской
  `& 0x1FFFF` (нет клампа к dstW/dstH).

### 1.3 Строитель дескриптора @ 0xfffffff008f8c224 (второй экземпляр @ 0xf910xx)

- `W' = [req+0xc54] + geo-поле` (0xf8c2d4: `w26 = [x19+0xc54] + w26`,
  w26 ранее = ldp [x22+0x28] — строка/высота геометрии), аналогично `H'` на
  0xf8c2e0. Т.е. border W/H **складываются** с размером dst.
- Передача в writer — виртуальный вызов `vtable+0x90` (PAC-ключ 0xd7b<<48)
  объекта `[builder+0x10]`:
  `setField(this, request, cmd=0x300, offset, value = W' & 0x1FFFF)`.
- Карта полей дескриптора (offset → источник):
  - `+0x2dc` ← W'&0x1FFFF, `+0x2e0` ← H'&0x1FFFF (border);
  - `+0x234`, `+0x244` — пассы (цикл 0..3, мультиплановые дескрипторы);
  - `+0x308`, `+0x314` — вторичные поля.
- Путь по умолчанию при выключенном border (thunk @ 0xfffffff008f81bf4):
  `+0x1dc ← 0x40, +0x1e0 ← 0x40, +0x2dc ← 0x20, +0x2e0 ← 0x20`
  (дефолтные W/H = 32/64).
- Всего по kext'у 826 call-site'ов `cmd=0x300` — это основной способ записи
  32-битных полей дескриптора.

### 1.4 Crop-путь @ 0xfffffff008f93014 (ранее не разобран)

- `X=[req+0xc78], Y=[req+0xc7c], W=[req+0xc80], H=[req+0xc84]`.
- Делители: `X/divX`, `Y/divY` (`udiv` на [req+0x7c0], [req+0x7c4]).
- Упаковка: `bfi w6, yDiv, #16, #16` → `(Y/divY)<<16 | (X/divX)`.
- Прямой вызов `0xfffffff008f896a8` (7 аргументов:
  `writer, request, 0x300, 0, base=0x3000, offset, value`) — thunk:
  `[[__got 0x7f9b5b8] -> vtable+0x80]`, writer — **импортированный синглтон**
  (объект в DATA_CONST 0xfffffff007dd4510; реализация — в другом kext'е).
- Поля региона base 0x3000:
  `+0x3004 ← raw Y`, `+0x3008 ← packed (Ydiv<<16|Xdiv)`, `+0x300c ← H`,
  `+0x3010 ← W`, `+0x3054 ← H/div`, `+0x3058 ← W/div`.
- Если `[req+0xc74]==2`: цикл по массиву `[req+0xc88 + i·4]`, count =
  `[geo+0x158]`, значение `(val>>4)&0xFFF` → `+0x3014 + i·4` (пользовательский
  массив прямо в дескриптор, 12 бит на элемент).

## 2. Базовый DVA и маппинг

### 2.1 mapBufferOnDartGatedIfNeeded @ 0xfffffff00900c7c8

1. `x19 = helper(0x90691f4)(surface, …)` → ctx; mapper = `[ctx+0x90]`.
2. Out-параметры: `[fp-0x34] = index`, `[sp+0x40] = DVA`.
3. Маппинг: прямой вызов `0x900c6a4` (либо fast-path `vtable+0xe8`), внутри —
   импортированные стабы `0x90aca60`/`0x90aca90` + вирт. `mapper vtable+0xa0`.
4. После успеха: вирт. вызов **surface vtable+0x108** =
   `onBufferMapped(surface, request, index, DVA)` — DVA запоминается в объекте
   поверхности (cstring 'onBufferMapped' @ 0x760e3ca).
5. Лог (0x900c8f8): `'  Scaler[%d] Request %d, %s Surface %d DVA 0x%llx
   mapper[%d] %dx%d'` — печатает DVA из [sp+0x40], Surface id [req+0xd2c],
   размеры [ctx+0x28].
6. Получение DVA обратно: `mapper vtable+0x108` (this=mapper, index) → DVA.

### 2.2 DART-домен

- `initMappers`: `iommu-parent` из DT, мапперы `SID[0]`, `SID[1]`
  ('Mapper: SID[0]: %p, SID[1]: %p', 'Failed to create mapper sid[%u]!!').
- `activateDART_gatedContext` (активизация TTBR по контекстам), строки
  'Scaler[%d] activateDART_gatedContext activate=%d', флаг отказа
  'activateDART() failed: 0x%x'.
- Кэш маппингов: `mapShadowMapperCacheEntry_gated` /
  `unmapShadowMapperCacheEntry_gated` / `updateShadowMapperCacheTTL_gated`,
  тип `site.ShadowMapperCacheEntry` — маппинги кэшируются с TTL; персистентность
  shared-домена между клиентами после v95 — под вопросом.
- Обработчик ошибок: `dartErrorHandlerCallback` — печать
  `'[IOSA] dart-scaler%d error!! type %s, %s SID %u %s DVA %llx Msr IntrSts %x'`.
- Spill buffer тоже маппится: 'SpillBuffer: DVA: %#llx', 'Failed to map
  spillbuffer!'.
- Диапазон DVA на девайсе: база 0x10000000000 (1 ТБ), наблюдаемая
  гранулярность 0x4000 (паники bug 210: write 0x1000003c000/0x10000040000,
  READ 0x10000094000).

## 3. Формула старта записи

Из семантики дескриптора (поля §1.3–1.4) + эмпирики журнала:

```
Y' = Y & 0x1FFFF        // беззнаково, знакового расширения нет
X' = X & 0x1FFFF
row_addr = dst_base + Y' · pitch          // 64-бит, беззнаково
write_start_DVA = row_addr + X' · bytesPerPixel
write_end_DVA   ≈ write_start_DVA + span(W, H, pitch)
```

- `bytesPerPixel` = делитель [req+0x7c0] (зависит от dst-формата);
  `pitch` — байтовый шаг строки dst (для 4096-ширины ≈ 8192 при 2 байтах/пиксель).
- **Y-wrap (Y=0xFFFFFFF0, H=0x20)**: Y'=0x1FFF0=131056; Y'·pitch ≈ 1 ГБ —
  уходит далеко за окно. Наблюдено 3/3: запись 0x00 в `[dst_base, +0x4c000)`
  (span 0x4c000 = 315392 B). Т.е. старт переносится обратно к base; точный
  механизм внутри импортированного writer'а/железа, эмпирически — wrap-to-base.
- **Двойной wrap (X=Y=0xFFFFFFF0)**: kr=0, `N_qwords ≈ 2016 + 63·W`
  (span зависит от W линейно, от X — нет).
- Записываемый байт всегда 0x00 вне зависимости от цветовых полей
  (border/crop значения влияют только на адрес/размер, не на данные).

### Гейты валидации (kr != 0 при нарушении)

- `min(W,H) ≥ 64 ∧ max(W,H) ≥ 1024` (иначе 0xe00002c2-цепочка);
- на ширине 4096: `W ≤ 0x80`;
- wrap-значения координат проходят гейт (kr=0).

## 4. Параметры для прицельной записи

| Цель | X (border X / crop X) | Y | W | H | Ожидаемый эффект |
|---|---|---|---|---|---|
| Запись от base вперёд | 0 | 0 | 0x80 | 0x20 | [dst_base, +span) |
| Сдвиг старта на +k байт | (k mod pitch)/bpp | k/pitch | ≥0x40 | ≥0x40 | start = base + k |
| Сдвиг за правый край | large (≤0x1FFFF) | 0 | ≥0x40 | ≥0x40 | start внутри окна, за edge rows; дальше — DART fault по окну |
| «Назад» base−k | — | — | — | — | **недостижимо**: поля беззнаковые; единственный «back»-эффект — wrap-to-base (§3) |
| Максимальный span | 0xFFFFFFF0 | 0xFFFFFFF0 | 0x80 | 0x20 | kr=0, N_qwords ≈ 2016+63·W ≈ 7056 qword ≈ 56 КБ с base |

Все значения проходят в дескриптор под маской 0x1FFFF; промежуточной
проверки «start ≤ dst_base + size» в SW нет (проверено: ни в builder'e, ни в
crop-пути сравнения с размером поверхности отсутствуют — контроль только со
стороны DART).

## 5. Что осталось незакрытым

1. **Импортированный writer** (реализация `vtable+0x80/+0x90`): синглтон
   0xfffffff007dd4510 в DATA_CONST (сегмент числится за записью
   AppleARMPMU — вероятно, артефакт разметки KC; реально класс writer'а
   живёт в другом kext'е). Декод PAC'd указателей vtable в этом KC не
   подтверждён (формула `VA = 0xfffffff007004000 + (raw & 0xFFFFFFFF)`
   верна для `__got`/`__mod_init_func` и не-PAC записей; для PAC'd
   (`raw>>56 ≥ 0x80`) — требует отдельной проверки). Без этого не доказан
   точный порядок 64-битной записи base DVA в дескриптор (каким cmd/offset).
2. Точная железная математика span (2016+63·W — эмпирика; коэффициенты
   pitch/bpp внутри writer'а/HW).
3. Детали ShadowMapperCache (TTL, shared vs per-client) — только строки и
   точки входа; требуется дизасм соответствующих функций.
4. Stub-карта импортов сохранена в /tmp/stub_map.json (189 стабов → target
   VA); при перезагрузке машины пересобирается из parent KC (см. §приложение).

## Приложение. Инструменты разбора (для продолжателя)

- Полный дизасм: `objdump -d results/kc27/com_apple_driver_AppleM2ScalerCSCDriver.macho`
  (в сессии жил в /tmp/scaler_disasm.txt).
- Carve: `results/kc-extract/carve_fileset.py results/kc27/kernelcache_iphone16.macho com.apple.driver.AppleM2ScalerCSCDriver`.
- Декод указателей данных в этом KC: `VA = 0xfffffff007004000 + (raw & 0xFFFFFFFF)`
  (подтверждено на `__got`, `__mod_init_func`: 0x…→pacibsp).
- Стуб→target: 16-байтные записи `__auth_stubs` @ 0xfffffff0090ac2d0,
  слот = ADRP(page)+ADD(imm12·8); слоты в `__auth_got` 0xfffffff007f9af58.
- Ключевые адреса:
  validateBorderFill 0xfffffff008f8148c; builder 0xfffffff008f8c224
  (экз.2 0xf910xx); crop 0xfffffff008f93014; setField-thunk 0xfffffff008f896a8;
  mapBufferOnDartGatedIfNeeded 0xfffffff00900c7c8; helper 0xfffffff0090691f4;
  defaults-thunk 0xfffffff008f81bf4.
