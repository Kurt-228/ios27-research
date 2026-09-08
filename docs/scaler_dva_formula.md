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
3. ~~Детали ShadowMapperCache (TTL, shared vs per-client)~~ — закрыто в §6.
4. Stub-карта импортов сохранена в /tmp/stub_map.json (189 стабов → target
   VA); при перезагрузке машины пересобирается из parent KC (см. §приложение).

## 6. DART-домен: чей он, кэш маппингов, DVA-аллокатор (статика kc27)

Разбор `results/kc27/com_apple_driver_AppleM2ScalerCSCDriver.macho` (дизасм
`/tmp/scaler_disasm.txt`, 323990 строк). Отвечает на открытые вопросы §5.3 и
формулирует условия cross-surface write. VA ниже сокращены до low-32.

### 6.1 Вопрос 1: IOMMU-контекст, синглтон vs per-client — ДОМЕН ОБЩИЙ

- **initMappers @ 0x8fdf19c** (cstring 'initMappers' @ 0x760f9a3): создаёт
  ровно два маппера, один раз при загрузке. Цикл w25=0..1: индекс SID
  выбирается по chip-id (ccmp по глобалям 0xb440b98 +0xe4/+0xa4, при
  условии добавляется 2 — 'Defer reset (MSR Dart Ganging)' 0x760f44c,
  'msr_multi_msr_dart_ganging' 0x7617895: два SID работают как ganged-пара),
  затем `bl 0x90ac740` (импорт, x0 = provider `[x20+0x78]` из DT-свойства
  'iommu-parent' 0x760f90b, x1 = sidIndex) → результат
  `str x0, [x22, x25, lsl #3]`, **x22 = x20+0x80**. Т.е. мапперы лежат в
  полях **драйвер-инстанса +0x80/+0x88**. Ошибка — '[IOSA][Boot ] Failed to
  create mapper sid[%u]!!' 0x760f944, лог 'Mapper: SID[0]: %p, SID[1]: %p'
  0x760f975. На каждый маппер вешается DART error handler (вирт. vtable+0x3a8
  с PAC'd колбэком 0x8fdf410 = dartErrorHandlerCallback).
- **Мап-ядро @ 0x8fed680** (вызывается из тела mapBufferOnDartGatedIfNeeded):
  маппер берётся как `[obj+0x80 + mapperIdx·8]` (0x8fed9fc) — индекс = SID,
  **никакой зависимости от клиента/процесса**. Второй массив `[obj+0x90 + idx·8]`
  — «shadow mapper»-объекты (создаются лениво в 0x8fed904 fallback'ом через
  импорт 0x90ac4f0 с page size 0x1000, оборачивая реальный маппер из +0x80).
- Клиентская различность есть только в **ShadowMapperCache** (§6.2) и она
  управляет временем жизни маппинга, а не адресным пространством.
- Подтверждение shared-домена эмпирикой: журнал §87–97 (E1/E2) — маппинги
  видны между двумя нашими коннектами.

**Вердикт: DART-домен — один на инстанс драйвера (два SID ganged). Все
userclients всех процессов (включая системные поверхности WindowServer/
backboardd через IOMobileFramebuffer, строка 0x7619b96) мапятся в одно и то же
DVA-aperture.** «Домен строго наш» — НЕ наш случай, задачу этим не закрыть.

### 6.2 Вопрос 2: ShadowMapperCache — ключ, TTL, клиенты

Функции (реальные, не стабы): map @ 0x8ff314c, updateTTL @ 0x8ff3424,
unmap @ 0x8ff3564; обёртки через command gate 0x900c594 (map) / 0x900c61c
(unmap) → action-заглушки 0x900c608 / 0x900c690.

- **Кэш**: массив бакетов `[driver+0x178 + mapperIdx·0x28]`: +0x00 голова
  LRU-списка, +0x10 count (лимит **0x41 = 65** на бакет), +0x18 суммарный
  wired size.
- **Entry** (kalloc 0x30, 'site.ShadowMapperCacheEntry' 0x7611faf):
  +0x00 IOSurface\* (retained), **+0x08 u32 clientID**, +0x0c flags
  (bit0 = активен/замаплен), +0x10 md/req ptr, +0x18 timestamp,
  +0x20/+0x28 линки.
- **Ключ = (surfaceID, clientID)**: surfaceID получается из md вирт. вызовом
  vtable+0xb0 (0x8ff31e0), сравнение указателя с [entry]. clientID — аргумент
  w3: при w3==0 lookup работает как wildcard (первый подходящий surface
  независимо от clientID, 0x8ff31fc–0x8ff3208).
- **Все внешние вызовы из кекста идут с clientID=0** (wildcard): 0x8fed600
  (map после успешного DART-map) и 0x8fedb1c (unmap) — оба w3=0. Поле clientID
  в энтри заложено, но per-client разделение кэша в наблюдаемых путях не
  используется.
- **TTL**: updateShadowMapperCacheTTL_gated: для энтри с flags bit0=0
  (неактивных) `now − entry[+0x18] ≥ 0x77359400` нс (**≈ 2 с**) → evict +
  release (0x90ac2f0), count--, вычитание wired size. now — clock_gettime
  (0x90acd20/0x90acc70), лог-обёртка делит на 0x3B9ACA00 — TTL в секундах.
  Sweep вызывается: на каждом map (0x8ff319c), из менеджера (0x8ff3394,
  0x8ff3670) и из fast-path 0x900a99c.
- **Продление жизни**: пока клиент активен, 0x900a99c (вызывается из тела
  mapBufferOnDartGatedIfNeeded, когда у клиента выставлен байт [client+0x209],
  т.е. клиент — «shadow-cache пользователь») обновляет TTL у всех энтри
  клиентов запроса (по битмапу [req+0x440], слоты [driver+0x150 + i·8],
  i = ffs([driver+0x1c0])) — маппинги не истекают, пока клиент шлёт запросы.
  Строки 'ActiveDartStartTime'/'ActiveDartEndTime' 0x7618bf8/0x7618c0c.
- **Клиенты драйвера**: 'clientOpened_gated %zu proc:%s' / 'clientClosed_gated
  %zu proc:%s' (0x7617f93/0x7617fc6) — драйвер регистрирует процессы;
  'IOMobileFramebuffer' 0x7619b96 — display pipeline идёт через этот драйвер,
  т.е. поверхности WindowServer/backboardd гарантированно проходят через тот
  же DART-домен; 'IOCoreSurfaceRoot'/'lookupSurfaces' 0x7619647.

### 6.3 Вопрос 3: DVA-аллокатор — его НЕТ в этом кексте

- В коде кекста **нет ни базы 0x10000000000** (ни одного `movk #0x1000, lsl
  #32`; все `lsl #32` — другие константы), **ни бамп-аллокатора/холл-поиска**.
- DVA назначает **IODARTMapper** (чужой кекст) при map: мап-вызов —
  вирт. `vtable+0x3a8` объекта из `[+0x80 + idx·8]` с опциями-строками
  'iomdEarlyReclaim'/'iomdEarlyPurge' (0x7611ca1/0x7611cb2, выбор по w2);
  драйвер затем только забирает готовый DVA из md (хелпер 0x8fed83c, вирт.
  vtable+0xb8, «getDmaCommandDva» 0x7611934) и кладёт в слот трекинг-структуры
  (0x98/0xa0 по признаку [req+0x428]==[md+0x68]).
- Константы 0x4000 в кексте — pitch/размерности в конфиге регистров
  поверхностей, к DVA-размещению отношения не имеют.
- Вывод: размещение в aperture — политика IODARTMapper (разрежённая карта).
  Это согласуется с эмпирикой v102g/v103 (DVA поверхностей НЕ смежны, дыры).
  База 0x10000000000/гранулярность 0x4000 — свойства DART-aperture этой
  платформы, не алгоритма этого драйвера. Предсказуемость чужих DVA из
  статики этого бинарня не следует — только эмпирика/груминг.

### 6.4 Вопрос 4: вердикт по wrap-записи в чужую поверхность

Wrap-путь (§3): start = dst_base + (Y&0x1FFFF)·pitch + (X&0x1FFFF)·bpp,
строго **вперёд** от base, span контролируется W. Из §6.1–6.3:

- Изоляции адресных пространств нет: чужие (WindowServer и др.) поверхности
  живут в том же aperture; известные нам DVA (0x1000003c000/0x10000040000/
  0x10000094000 — паники bug 210) попадают в один диапазон.
- Запись идёт вперёд ⇒ достижимы только чужие маппинги с **DVA ≥ нашей
  dst_base** и < dst_base + span.
- Чужой маппинг должен быть **жив** в момент записи: неактивные эвиктятся
  через ~2 с (§6.2); активный клиент продлевает TTL штатными запросами.
- Размещение разрежённое (§6.3, политика — в §7) ⇒ случайное попадание маловероятно; для
  прицельного нужен либо груминг (поднять нашу поверхность выше цели нельзя —
  запись только вперёд, значит цель должна оказаться выше нас и до неё должен
  доставать span), либо утечка DVA цели (лог 'Scaler[%d] Request %d, %s
  Surface %d DVA 0x%llx mapper[%d] %dx%d' 0x7618ab6 печатает DVA только в
  system log — напрямую из юзерспейса не читается).
- За пределами любого mapped-окна — DART fault → dartErrorHandlerCallback →
  паника (bug 210), т.е. «залповая» запись через span — самоубийственна.

**Вердикт: теоретически возможно, практически требует (а) знания/угадания
DVA чужой живой поверхности выше нашей базы и (б) span, её достающего, без
пересечения конца её окна — fault до края гасит запись (precise abort).
Закрыть линию как «домен строго наш» нельзя — домен общий; реальная защита —
разрежённость размещения IODARTMapper + 2-секундный TTL неактивных маппингов.
Дальнейшая эмпирика: серия v102g-style с фиксацией DVA соседних поверхностей
системных клиентов.**

### 6.5 Вопрос 5: что значит «Gated» в mapBufferOnDartGatedIfNeeded

- Тело: **mapBufferOnDartGatedIfNeeded @ 0x900c7c8** исполняет работу через
  command gate: `[driver+0xb8]->vtable+0xe8` (сигнатура runAction: action,
  arg0..arg3) с action = 0x900c6a4. Аналогично обёрнуты ShadowMapperCache
  map/unmap (0x900c594/0x900c61c, gate у объекта `[x0+0xb8]`). Те же gated-имена
  у всей обвязки ('activateDART_gatedContext', 'clientOpened_gated' и т.д.) —
  это сериализация на workloop драйвера ('cannot create a workloop'
  0x7623690), **не проверка безопасности**.
- **Ungated-путь существует**: 0x900c878 — прямой `bl 0x900c6a4` без
  runAction, когда у клиента байт [client+0x209]==0 (клиент не пользуется
  shadow-кэшем) или выставлен бит [driver+0x638] (фича-флаг). Т.е. gated/ungated
  — про взаимодействие с ShadowMapperCache, а не про доверие к аргументам.
- Реальные проверки внутри action: mapper существует (иначе lazy-create),
  power/трекинг-флаги ([md+0x1f8]==1 для fast-path TTL), retain/lock пары
  (0x90aca60/0x90aca90). Проверок «start ≤ dst_base + size» нет (§3) —
  контроль только со стороны DART.

### 6.6 Адреса для продолжателя

initMappers 0x8fdf19c; map-ядро 0x8fed680; getMapper 0x8fed904; DART-map
0x8fed9fc (vtable+0x3a8, iomdEarlyReclaim/Purge); getDva 0x8fed83c
(vtable+0xb8); wireAndMapBuffer-обёртка 0x8fed4e4; ShadowMapperCache:
map 0x8ff314c / TTL 0x8ff3424 / unmap 0x8ff3564, gate-обёртки 0x900c594/
0x900c61c, TTL-fastpath 0x900a99c; mapBufferOnDartGatedIfNeeded 0x900c7c8,
action 0x900c6a4, ungated-прямой вызов 0x900c878; unmap-путь 0x9006278;
трекинг-хелпер 0x90691f4 (md,idx)→ctx, mapper=[ctx+0x90]; клиентские поля
driver: +0x150 слоты[8], +0x1c0 битмап, +0xb8 gate; req: +0x428 клиент,
+0x440 битмап клиентов, +0xd2c surface id.



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


## 7. DVA-аллокатор IODARTMapper (iOS 27.0b4, статика kc27)

Разбор `results/kc27/com_apple_driver_IODARTFamily.macho` (carve:
`python3 results/kc-extract/carve_fileset.py results/kc27/kernelcache_iphone16.macho
com.apple.driver.IODARTFamily`; дизасм в сессии: `objdump -d` →
`/tmp/iokdart_disasm.txt`, 20518 строк). Закрывает §6.3: политика размещения
DVA найдена и верифицирована. VA сокращены до low-32; base text IODARTFamily =
0x9ce20a0, cstring base = 0x789e928, __const base = 0x8136240.

### 7.1 Где живёт класс и его alloc/free

- **Кекст `com.apple.driver.IODARTFamily`** (плюс девайс-половина
  `com.apple.driver.AppleT8110DART` — регистры DART для T8130). Скейлерский
  `iomdEarlyReclaim`/`iomdEarlyPurge` (§6.3) — имена опций вызовов вирт.
  методов IODARTMapper; сам DVA назначается здесь.
- Классы по метакласс-строкам (`site.<Name>`): `IODART` (/IODART.cpp),
  `IODARTMapperNub`, `IODARTClient` (/IODARTClient.cpp), **`IODARTMapper`**
  (/IODARTMapper.cpp), **`IODARTVMSpace`** (/IODARTVMSpace.cpp),
  **`IODARTVMAllocatorGeneric`** (/AllocGeneric.cpp),
  `IODARTPIOAllocatorGeneric`, `IODARTMapperClient` (/IODARTMapperClient.cpp).
- Цепочка выдачи DVA: `IODARTMapper::_iovmAlloc*` → `IODARTVMSpace` →
  `IODARTVMAllocatorGeneric::vmAlloc` → внешний (kernel) range-аллокатор.
  Строки методов IODARTMapper (all в page 0x789fxxx): `_iovmAlloc` 0x789f97f,
  `_iovmAllocDMACommand` 0x789f988, `_iovmFreeDMACommand` 0x789f9f6,
  `_findVMSpace` 0x789fadc, `_findVMSpaceReserved` 0x789fc60,
  `_iovmAllocPIO` 0x789fc12, `_iovmInsertBatch` 0x789fd6c, `_iovmFree`
  0x789fef6, `_iovmInsert` 0x789fe0b, `_registerMapper` 0x789f6fa
  («Failed to register mapper for SID %d» — **маппер на каждый (DART-unit,
  SID)**, что согласуется с §6.1: два ganged SID у скейлера).
- **Адреса функций аллокатора** (IODARTVMAllocatorGeneric, верифицировано
  дизасмом по сигнатурным строкам):
  - `init` @ **0x9cf18b4** — создаёт range-аллокатор: `bl` stub→kernel
    0xfffffff00af2ca40 (это экспорт `com.apple.kernel`), args (endOfRange=0,
    defaultAlignment=w1, capacity=8, options=0); page shift берётся из
    глобала ядра через GOT[0x8139360] (`1 << shift` = размер страницы
    aperture), defaultAlignment = [obj+0x120][0x4c] · (pagesize / ret(vtab+0x548)).
    Строка при ошибке: «Failed to DMA create range allocator» 0x78a03d8 —
    историческое имя класса IODMARangeAllocator.
  - `vmAlloc` @ **0x9cf1ad0** — gate, isActive-чек (vtab+0xe8), затем
    аллокация объектом `[this+0x48]` через vtab+0xb0 (out-параметр —
    адрес) / vtab+0xb8; ошибки: «vmAlloc» 0x78a0410, **«VM exhausted»**
    0x78a0418, «cannot make requested allocation at 0x%x/0x%x» 0x78a0433.
  - `vmAllocReserved` @ **0x9cf1d90** (0x78a04a3; «failed to get VM space
    for allocation at 0x%x» 0x78a04b3 — фиксированный адрес).
  - `vmReserve` @ **0x9cf21ec** (0x78a0523; «Insufficient buffer space»
    0x78a0538). Используется для persistent-диапазонов: «Unable to reserve
    DVA range %u @ [%#llx..%#llx) … possible overlapping ranges» 0x789f5fa,
    DT-ключи «dart-all» 0x789ec39, «range-base» 0x789f745, «range-size»
    0x789f750, «iommu-initial-translations» 0x789f59c.
  - `vmFree` @ **0x9cf2f80** (0x78a04ef).
- **Отложенное освобождение (IOMD-кэш маппера)**: DT/параметры «iomd-cache-size»
  0x789f3b4, **«iomd-cache-ttl»** 0x789f3c4, «iomd-early-reclaim» 0x789f3d3,
  «iomd-cache-flush-on-deactive» 0x789f14b, «cacheFlushInactive» 0x789f3f8,
  состояние реклейм-треда: «_iomdCacheReclaimIsRunning» 0x789f7e2 /
  «_iomdCacheReclaimStop» 0x789f7fd / «_iomdCacheReclaimImmediate() failed!»
  0x789fa0a. Т.е. free DVA возвращается в пул не в момент unmap, а после
  истечения TTL кэша/форс-реклейма (аналогично ShadowMapperCache §6.2).
- **Клиентский интерфейс**: имена методов IODARTClient externalMethod
  (блок @ 0x9ce69a0, vtab-патч-цикл): «retrieveVMLimits» 0x78a0406,
  «setActive», «setAllocator», «captureRegisters»,
  «getProtectionGranularity», «numAllocations», **«iomdEarlyPurge» 0x789f48f,
  «iomdEarlyReclaim» 0x789f49e** — форс-очистка IOMD-кэша из клиента.
  У `IODARTMapperClient` есть externalMethod **«GetAllocations»** (0x78a07a8,
  ошибка «GetAllocations: no structure or descriptor») — **дамп аллокаций
  aperture наружу**; стоит проверить, достижим ли он из скейлер-клиента.

### 7.2 Политика размещения — first-fit по отсортированному списку свободных

Аллокатор — **`IORangeAllocator`** из ядра (xnu-12377.1.9,
`iokit/Kernel/IORangeAllocator.cpp`, код не менялся с ~2000 г.; в kc27 живёт
в `com.apple.kernel`, экспортируется наружу — стабы IODARTFamily резолвятся
в сегмент kernel 0xa76c000+). Верифицировано по исходнику:

- Структура: плоский массив элементов {start, end} **свободных** диапазонов,
  **отсортирован по адресу**; общий глобальный мьютекс `range_allocator_grp`
  (флаг kLocking). init(endOfRange) засевает один большой свободный
  диапазон aperture; в IODART он создаётся пустым (endOfRange=0) и
  засевается резервами из DT («dart-all» и пр.).
- `allocate(size, &out, align)`: **first-fit** — линейный скан от младших
  адресов, первый свободный элемент, в который влезает выровненный кусок,
  сплитится на [до][занято][после]. **НЕ бамп, НЕ LIFO-стек, НЕ битмап,
  без хинтов.** Размер округляется до defaultAlignment (для aperture =
  страница DART; эмпирика §6: гранула 0x4000).
- `deallocate(data, size)`: вставка обратно с **коалесценцией** соседних
  свободных элементов (headContig/tailContig).
- **Персистентность**: объект аллокатора принадлежит VM-space маппера и
  живёт всю жизнь маппера; дыры между map/unmap сохраняются и сливаются.
  Домен общий (§6.1) ⇒ один глобальный first-fit «ландшафт» на SID.

**Почему эмпирика v102g/v103 давала дыры:** (а) first-fit снизу-вверх —
после бутовых резервов низ aperture занят, наши и чужие маппинги ложатся
в разные младшие дырки, аджасенси не гарантирована; (б) unmap не возвращает
DVA в пул мгновенно — IOMD-кэш (TTL) + ShadowMapperCache (~2 с, §6.2), пока
запись идёт, дырки ещё нет; (в) любая чужая аллокация между нашими map/unmap
вклинивается в младшую дырку.

### 7.3 Сценарии

**(a) map X → unmap X → система мапит Y — тот же DVA?**
Да, **если** дырка от X (после коалесценции) — младший свободный диапазон,
вмещающий Y: first-fit её выберет детерминированно. Это не «LIFO reuse», а
свойство младшей дырки: если подходящих дырок ниже X нет, Y получит ровно
DVA(X). Предусловия: (1) оба кэша (ShadowMapper + IOMD) отпустили запись —
иначе дырки физически нет; (2) между unmap X и map Y в разрыв не вклинилась
чужая аллокация размера ≤ дырки (гонка, ничем не блокируется — gate сериализует
только внутри драйвера). Если младше X есть коалесцированная дырка больше
size(Y) — Y ляжет туда, а не в X.

**(b) Держим много маппингов — куда ляжет чужая Y?**
В младшую свободную дырку ≥ size(Y). Зная раскладку, можно **вытеснить** Y в
нужную дырку: заполнить все младшие дырки своими маппингами размера ≥ size(Y)
(tail-padding 0x789f3e6 подталкивает округление — дырку «под размер» шить
надо с учётом выравнивания). Чужая Y большого размера провалится глубже, в
более старшую дырку.

### 7.4 Вердикт: груминг чужой поверхности + паттерн map/unmap

**Груминг реален и детерминирован.** First-fit делает раскладку управляемой
точнее, чем LIFO: цель занимает не «последнее освобождённое», а «младшее
подходящее» — это можно подготовить. С учётом wrap только вперёд (§3) цель
должна лежать **выше нашей dst_base**:

1. **Спрей**: замапить N своих поверхностей → поднять фронт аллокаций и
   сформировать «ландшафт» дыр (заполнить всё младшее).
2. **Наша dst**: замапить → first-fit даст ей младшую дырку (низкая база —
   хорошо: у цели будет запас выше).
3. **Точная дырка**: размапить выбранный placeholder, лежащий прямо над dst
   (unmap обоих кэшей → ждать ~2 с TTL или форсировать реклейм; у клиента
   IODART есть «iomdEarlyReclaim» — если скейлер его выставляет наружу,
   ожидание сокращается).
4. **Подсаживание цели**: дёрнуть системного клиента (display pipeline идёт
   через этот же драйвер, §6.2) так, чтобы его поверхность Y замапилась,
   пока дырка — единственная младшая подходящая ⇒ Y ляжет в неё.
5. **Wrap-запись** с dst_base достаёт до Y вперёд (§3-§4).

Риски/ограничения: гонка на шаге 3–4 (чужой маппер той же DART может
перехватить дырку — повторять цикл); size(Y) должен влезать в дырку;
пока кэши держат запись, дырки нет; «залповый» span через незамапленное —
DART fault → паника (§6.4). Открытым осталось: доступность
GetAllocations/retrieveVMLimits из юзерспейса — это дало бы прямое чтение
раскладки aperture и превратило груминг из «надежды» в «точную науку».

### 7.5 Адреса для продолжателя

IODARTFamily: text 0x9ce20a0; init 0x9cf18b4, vmAlloc 0x9cf1ad0,
vmAllocReserved 0x9cf1d90, vmReserve 0x9cf21ec, vmFree 0x9cf2f80;
метод-имена клиента @ 0x9ce69a0 (блок adrp 0x78a0000 + add'и: 0x406
retrieveVMLimits, 0x417 setActive, 0x421 setAllocator, 0x42e
setIomdCacheAttribute, 0x444 captureRegisters, 0x455 getProtectionGranularity,
0x46e numAllocations, 0x48f iomdEarlyPurge, 0x49e iomdEarlyReclaim).
Строки IODARTMapper (page 0x789f000): см. §7.1. Стуб→kernel: __auth_stubs
0x9cf5ca0 + idx·0x10, слот __auth_got 0x8139128 + idx·8, target = kernel
__TEXT_EXEC 0xa76c000+. Девайс-половина: com.apple.driver.AppleT8110DART
(carve аналогично, дизасм /tmp/t8110dart_disasm.txt — не разбирался,
регистровые пути DART). Исходник политики: xnu-12377.1.9
iokit/Kernel/IORangeAllocator.cpp (allocate/deallocate, first-fit,
коалесценция).
