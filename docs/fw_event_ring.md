# Firmware event ring (AGFI) — формат записей, валидация, caller-цепочка, вердикт по спрею

Дата: 2026-09-08. Источник: `results/kc-extract/agx_full_disasm.txt` (полный дизасм кекста
AGXG16G 360.32.1) из BootKernelCollection.kc **macOS 27.0 (26A5388g)**.
**iOS-сверка выполнена** (2026-09-08): `results/kc27/com_apple_AGXG16P.macho` +
`kernelcache_iphone16` (iOS 27.0b4, A17/G16P). Всё, кроме явно помеченного «macOS-only»,
подтверждено на iOS; расхождения — в колонках «iOS G16P» (смещения записи валидатора,
accel-оффсеты, fw-поля). Родительский документ: `docs/restart_analysis_structs.md`
(panic «Type confusion» через `guilty_stamp_index` — приоритет ①, настоящий док закрывает
его пункт «дочитать drainFirmwareEventRing»).

Центральная функция: `AGXFirmware::drainFirmwareEventRing()` @ `0xfffffe0008b223fc`.
Вспомогательные: `AGXFirmwareRingValidator::fetchNextEntry` @ `0x8b3591c`,
`AGXFirmwareRingValidator::snapshotState` @ `0x8b35818`,
`AGXFirmwareRingValidator::ringHasOutstandingEntries` @ `0x8b358c8`.

## 1. Хранилище: validator-record внутри AGXFirmware

> iOS G16P: функция drainFirmwareEventRing @ `0xfffffff00832f8b8` (весь путь живёт
> в G16P, не в RTBuddy); validator-record по **fw+0x7a0** (macOS fw+0xb68), accel ptr
> **[fw+0x270]** (macOS [fw+0x278]), status block **[fw+0x900]+0x5000** (macOS
> [fw+0xd18]+0x5000). Внутренняя раскладка записи — та же.

Кольцо описывается встроенной 0x30-байтной записью по `fw + 0xb68` (x19 = AGXFirmware,
`accel = [fw+0x278]`). Идентичные записи (таблица колец) инициализируются подряд в
`AGXFirmware::init` @ `0x8b2b5c0` (0x8b2b6d4–0x8b2b854); event-ring запись — последняя:

| Смещение | Поле | Источник (init @ 0x8b2b83c–0x8b2b854) |
|---|---|---|
| fw+0xb68 (+0x00) | accel ptr | `x9 = [fw+0x278]` |
| fw+0xb70 (+0x08) | header ptr (shared) | `x10 = [fw+0xc78]` |
| fw+0xb78 (+0x10) | entries base ptr (shared) | `x11 = [fw+0xc98]` |
| fw+0xb80 (+0x18) | кэш host read idx (runtime) | пишется drain'ом |
| fw+0xb84 (+0x1c) | кэш GPU write idx (runtime) | пишется drain'ом |
| fw+0xb88 (+0x20) | bitmap допустимых типов (u64) | константа **0xa000ffd3** |
| fw+0xb90 (+0x28) | capacity (u64) | константа **0x100 = 256** |

Соседние записи той же таблицы (другие кольца/каналы): validator'ы @ fw+0x958…0xb38
(trace ring @ fw+0xb38: header [fw+0xb40], entries [fw+0xb48], bitmap [fw+0xb58]
= 0x15fff0000000, capacity [fw+0xb60] = 0x20) — не путать с event ring.

Указатели [fw+0xc78]/[fw+0xc98] (и соседние пары 0xc80/0xca0, 0xc88/0xca8, 0xc90/0xcb0)
заполняются до init извне AGX-текста (шаринг с RTBuddy/mapper, статически не
прослежено); обнуляются в fail-пути @ 0x8b290a8. Тип памяти — shared, пишется GPU/
firmware (см. §7).

## 2. Header кольца и цикл drain

Header (shared, по [fw+0xb70]):
- `u32 @ +0x00` — host read idx (кэшируется в fw+0xb80 на старте drain);
- `u32 @ +0x20` — GPU write idx (кэшируется в fw+0xb84).

Цикл (0x8b224b4): пока read != write — `fetchNextEntry(fw+0xb68, &entry[sp+0x50])`,
switch по `u32 @ entry+0`, case-хвосты сходятся на `b 0x8b224b4`. Gate'и на входе:
- `[fw+0xb78] == NULL` → репорт «Using uninitialized validator class» (line 147);
- `capacity <= read_idx` или `capacity <= write_idx` (b.ls) → репорт
  «!!! read_index out of bounds» (0x7147f7a, строки 47/54/58, файл
  agxk_firmware_ring_validator.cpp).

Продвижение read idx происходит **внутри fetchNextEntry до валидации полей**
(0x8b35a04–0x8b35a24: idx+1 mod capacity, `dmb ish`, запись в [header+0]) — то есть
прочитанная запись из кольца выталкивается всегда (кроме bitmap-miss, см. §5).

## 3. Формат записи — AGFIFirmwareEventRingEntry, 72 байта (0x48)

fetchNextEntry (0x8b3591c): смещение записи = `idx*9<<3` (= idx*72), копия 18 dword'ов
в стековый буфер. Тип события = `u32 @ entry+0`. Дальнейшие обращения case-обработчиков
идут по копии на стеке (sp+0x50).

> iOS ✓ (G16P): формат подтверждён — fetchNextEntry @ `0x83428a0`, смещение записи
> `idx*9<<3` (= idx·72), копия 18 dword — идентичны; bitmap-miss → строка 179,
> «Using uninitialized validator class» line 147, «read_index out of bounds»
> lines 54/58 — все номера строк совпадают с macOS.

```
+0x00  u32  type (kAGFIFirmwareEventType*)
+0x04  u32  arg0     \ в type 1 читаются как ДВА u64: q0 = [+0x04..+0x0b], q1 = [+0x0c..+0x13]
+0x08  u32  arg1    /
+0x0c  u32  stamp/guilty index   (type 4: 0xffffffff = «none»; общий bounds-check vs count)
+0x10  u32  arg3    \ type 6: < 0x40; type 9: qword != 0 (вместе с +0x14)
+0x14  u16  arg4     — type 1: < 6; type 9: < count
+0x1c  u32  arg6     — type 9: != 0
+0x20…+0x44          копируются, case-специфичное использование не встречалось
```

## 4. Таблица типов (jump-таблица @ 0x8b236f0, dispatch от 0x8b22504)

Bitmap 0xa000ffd3: биты 0,1,4,6,7,8,9,10,11,12,13,14,15,29,31 — типы 2,3,5 отклоняются
ещё в fetch. Типы 15/29/31 проходят bitmap, но switch покрывает 0..0xe → default-репорт.
Каждый case сначала перепроверяет `entry[0] == type`, иначе mismatch-репорт с
per-case дескриптором validateType (строки 0x8b23584–0x8b235f8 — это сигнатуры вида
`AGXFirmwareRingValidator::validateType(...) [RET=..., FWET1=kAGFIFirmwareEventType...]`).

| Тип | Адрес case | Валидация полей | Действие |
|---|---|---|---|
| 0 GPURestart | 0x8b22490 | `accel+0x18e38 != 0` (иначе continue) | вирт-вызов accel vtable+0x890 (host recovery handler) |
| 1 StampsUpdated | 0x8b22804 | `u16 @ +0x14 < 6` | маски q0=[+4], q1=[+0xc] → побитовый цикл clearIndex в event machine ([accel+0x140]); `clock_gettime` → accel+0x1b088 |
| 2, 3, 5 | — | bitmap-miss в fetch | репорт (§5) |
| 4 guilty | 0x8b2267c | **+0x0c == 0xffffffff ИЛИ 0 ≤ idx < count**, count = stub@0x8baa650([accel+0x140]) | см. §6 |
| 6 | 0x8b22d44 | +0x04 < count; +0x08 < 0x7f; +0x10 < 0x40 | запись в кольцо accel+0x17fc8 (слоты 32B, head/tail [accel+0x187c8/+0x187cc], lock [accel+0x187d0]) → wake [accel+0x458] vtable+0x1f8(0,0,0) |
| 7 | 0x8b22578 | +0x04 < 5; +0x08 < 3; +0x0c bounds vs count | telemetry-объект [fw+0x280]; sub-switch +0x04 (0..4) → перезапуск AGXWorkQueue (vtable+0x148, wake [fw+0xc00]+0x30) — триггер restartWorkQueue из события |
| 8 | 0x8b2261c | — | handler [fw+0xff0] vtable+0x140(2, entry+4…) |
| 9 | 0x8b2291c | +0x08(q) != 0; +0x04 < 0x100; +0x14 < count; +0x1c != 0 | то же кольцо accel+0x17fc8 (64 слота, маска 0x3f, head [accel+0x187cc], lock [accel+0x187d8]) → wake [accel+0x460] |
| 0xa | 0x8b229e0 | — | минимальный (continue) |
| 0xb | — | bitmap-hit, в таблице → continue-путь | — |
| 0xc | 0x8b22b00 | — | os_log с таблицей [accel+0x1b108]; вирт +0x18/+0x28 на объекте |
| 0xd | 0x8b22a30 | +0x08 < 0x100; +0x0c(q) != 0; +0x14(q) != 0; +0x04 < count | channel = [accel+0x11aa8 + (+0x08)*8] при +0x08 < [accel+0x11aa0] → restartWorkQueue(type 13) |
| 0xe | 0x8b22550 | — | release [accel+0x150](qword @ +4) |
| 0xf/0x1d/0x1f | default | bitmap-hit | default-репорт 0x7146ff8 |

> iOS ✓ (G16P): bitmap **0xa000ffd3** и capacity **0x100 = 256** — без изменений;
> статическая таблица колец в __TEXT @ ~0x7117ab0: запись {…0x15fff0000000, 0x100}
> (trace-ring) и {0xa000ffd3, 0x100} (event-ring) — та же пара констант. iOS-адреса
> case'ов: type 4 @ `0x832fb30` (валидация 0x832fb3c–0x832fb64, запись 0x832fc90–
> 0x832fca0); type 7 sub-switch и валидации type 9 (+0x04 < 0x100, +0x08(q) != 0,
> +0x0c bounds, +0x14 < count) идентичны macOS.

### 4.1 Requestor/sideband на iOS (Q4)

Таблица `kG16BifRequestorInfo` существует на iOS **идентично** macOS: 64 записи
× 16 байт `{u32 name_off (KC-relative); u32 0x200000; u64 id}`. В G16P macho —
fileoff **0xdda94** (rec0 = 0x11ef90/0x200000/1 = DCMP0, проверено чтением
kernelcache.macho по KC-fileoff). idx 24 = VDM1 (id 2), idx 25 = PPP1 (id 2) —
sideband 24/25 из iOS-логов = VDM1/PPP1 (GPC0) ✓.

Нюанс: таблица **не референсится кодом** ни одного извлечённого кекста (adrp-скан
всего kernelcache, исправленный xref-сканер) → на iOS 'requestor'/'sideband'
публикуются сырыми значениями из MMUFaultInfo-структуры, а не через декодер таблицы:
заполняет вирт-метод accel **vtable+0x7b0** (вызов в restartWorkQueue @ 0x82f8bf4 с
(accel, w1=1, sp-0xf0, w3=0); поля локальной копии: requestor @ -0xdc, sideband @
-0xd8, level @ -0xd0, is_read @ -0xcf; свойства 'requestor'/'sideband'/'level'/
'is_read' — строки 0x7126a35/a3f/a48/a4e, ключи bif0_fault/bif1_fault @ 0x7126b22/b17).

Requestor 208/209 из iOS-паник > 6-битного macOS-поля (bits 22:17) → iOS-расклад
fault-регистра иной; точный бит-расклад скрыт за PAC'd vtable (слот +0x7b0) —
**[частично открыто: декодер за vtable, уточнить динамически]**.

## 5. Семантика отказов валидации — КОРРЕКТИРОВКА прежних выводов

Прежняя рабочая гипотеза «невалидная запись логируется и пропускается» **неверна**.

Все failure-пути сходятся на reporter-стаб `stub@0x8bab470` (имя статически не
резолвится; сигнатура строкой 0x7148131: `void _expectInner(bool, const char *,
const int, const char *, ...)` — assert-стиль `_expectInner(false, fmt/expr, line, file, …)`).
Признаки noreturn-поведения: в ~128 из ~150 call-site'ов кекста следующая инструкция —
пролог следующей функции (`pacibsp`/`bti c`), т.е. компилятор кладёт вызов в
tail-position блока и не возобновляет поток; failure-блоки drain выстроены цепочкой
fall-through друг за другом (0x8b23534 → 0x8b2355c → 0x8b2367c → 0x8b2369c → 0x8b236c4),
что возможно только если reporter не возвращает.

> iOS ✓ (G16P): failure-пути сходятся на тот же assert-стаб `0x83b1c54`; репорт
> «Ring entry contains bad data» line 256 печатает **256** (константа 0x100) — как macOS.

Вывод (с оговоркой [уточнить динамически на девайсе — один запуск с крафтовой записью
снимет вопрос окончательно]): **любая запись с некорректным type/полями → panic в
контексте drain (interrupt/workloop)**, а не skip. Перечень failure-триггеров из
пользовательски-контролируемого содержимого кольца:
- type ∉ bitmap (2,3,5 и любые прочие нули битов) → fetch bitmap-miss, строка 179;
- type 15/29/31 и прочие «вне switch» → default-репорт;
- idx полей вне bounds (vs capacity 256 / count стампов) → «!!! Ring entry contains
  bad data» (строка 256, печатает count=256);
- read/write idx в header вне capacity → «!!! read_index out of bounds»;
- mismatch entry[0] ↔ case → per-case дескриптор validateType + строка 237.

Следствие для фаззинга: event ring — это panic-на-первой-же-мусорной-записи поверхность;
не нужно точно попадать в семантику type 4 — достаточно, чтобы спреенная страница
оказалась под entries и firmware продвинул write idx. (Побочный риск: livelock-подобный
повтор не возникает — read idx продвинут до валидации, panic случается сразу.)

## 6. Type 4 (guilty) — полный разбор, 0x8b2267c–0x8b22800

1. `w22 = u32 @ entry+0xc` (guilty_stamp_index из события).
2. Bounds: `w22 == -1` → ок; иначе `0 ≤ w22 < count`, где
   `count = stub@0x8baa650([accel+0x140])` (IOGPUEventMachine; вероятно число stamp-
   слотов — sentinel 0x80 у «none» намекает на ёмкость 0x80, [проверить динамически]).
   Нарушение → failure-репорт §5 (запись в accel+0x18e34 **не происходит**).
3. Вирт-вызов `fw vtable+0x1c8`(fw, &entry+4). Если вернул 0:
   - читает status block `[fw+0xd18] + 0x5000` (B2-цель infoleak из родительского дока):
     `u32 @ +0x509c` → флаг (бит3 в accel+0x18fc0), dwords `+0x50a8` → accel+0x18e30 и
     subtype-decode в accel+0x18e38 (магия 0x6504020206 >> (d1*8), min 2),
     q `@ +0x50b0` → accel+0x18f08, q `@ +0x50bc` → accel+0x18f10,
     u32 `@ +0x50b8` → accel+0x18f18, 0x98 байт с `+0x50c4` → accel+0x18f20,
     q `@ +0x515c` → accel+0x18fb8;
   - **accel+0x18e34 = (entry+0xc == -1) ? 0x80 : entry+0xc** — guilty_stamp_index;
   - `[[accel+0x158]]` set flag 1.
4. Если вирт-вызов вернул != 0 → обход без записи accel+0x18e34 (0x8b235fc-ветка,
   os_log 0x71471da, строка 3233).

> iOS ✓ (G16P, case @ 0x832fb30–0x832fd28): idx = `[sp+0x5c]` = **entry+0xc** ✓;
> count-стаб **0x83b0e34([[accel+0x140]])**; валидация `idx == -1 ∨ 0 ≤ idx < count`
> (cmn/cset/csel @ 0x832fb4c–0x832fb64) → failure 0x330930; вирт-вызов **fw vtable+0x108**
> (macOS +0x1c8); запись accel+0x18dec = (idx==-1)?0x80:idx; subtype magic **0x0605040202**
> (macOS 0x6504020206), min 2 → accel+0x18df0; status-копии: +0x50a8→0x18de8,
> +0x50b0→0x18ec0, +0x50b8→0x18ed0, +0x50c4 (0x98 B)→0x18ec8, +0x515c→0x18f70;
> flag byte bit3 → accel+0x18f78 (macOS 0x18fc0); [[accel+0x158]] set flag 1.

Связка с getGuiltyChannel (`AGX3DWorkQueue::getGuiltyChannel` @ 0x8ba6e44, panic-строка
agxk_workqueue.cpp:1047 «Type confusion - invalid AGXChannel for firmware
guilty_stamp_index %d»): `idx = [accel+0x18e34]`; `idx == 0x80` → host-side fallback'и
(panic нет); `idx != 0x80` → `stub@0x8bab110(stub@0x8baaff0(wq), idx)` (lookup канала
по stamp-индексу) → NULL или несовпадение имени → panic с нашим числом в строке.

**Уточнение условия для приоритета ① родительского дока:** «кладём индекс != 0x80 и !=
любого валидного» — ошибочно по верхней границе: `idx ≥ count` отсекается валидатором
event ring ещё до записи в accel+0x18e34 (failure-пути §5, до getGuiltyChannel дело не
доходит — но panic всё равно случается, только другой, «Ring entry contains bad data»,
без нашего числа и в другом контексте). Реальный путь к «Type confusion» с печатью
индекса: `0 ≤ idx < count` при **пустом/освобождённом stamp-слоте** (канал teardown'нут
к моменту restartWorkQueue) → lookup NULL → panic. Т.е. значение берётся из диапазона
stamp-слотов, а не «большое крафтовое». iOS ✓: диапазон idx `[0, count)` тот же —
валидация (cmn/cset/csel) и sentinel 0x80 идентичны macOS.

## 7. Кто и когда вызывает drain

Статический поиск xref'ов: ни одного `bl`/adrp-add на 0x8b223fc во всём KC-text
(AGXG16G, RTBuddy, IOGPUFamily); единственное «вхождение» адреса в данных — __LINKEDIT
(отладочная карта). Причина: указатель PAC-подписан в данных/heap (регистрация как
action прерывания), либо вызов через vtable.

Установлено по vtable: `__ZTV11AGXFirmware` (0x7f454f0), слот **+0x288 = drainFirmwareEventRing**,
слот +0x290 = drainFirmwareLogRing (stub `bti c; ret`). Декод записей vtable на диске
кастомный (PAC-диверсификация); идентификация слота сделана по совпадению low-битов с
границами функций (0x8b223fc/0x8b223f4) и подтверждена косвенно структурой соседства.

Видимая interrupt-цепочка: firmware IRQ → `AGXFirmware::handleEvent(AGXInterruptIndex)`
@ 0x8b2530c (зарегистрирован как action; диспатч по индексу: 0/4/6/7 + vtable-
переходы на [fw+0x210]+0x20 для 1–3):
- idx 6: `accel vtable+0xc48`(out u32 @ sp+0xc) → таблица per-type handler'ов
  `accel+0x11c48 + idx*32` (handler @ +8), биты в accel+0x17c68;
- idx 7: `accel vtable+0xc50` (tail call), idx 4: `accel vtable+0x898` → далее
  drainFirmwareRings(1);
- idx 0: `accel vtable+0x360`/`+0xc40` под флагами.

Косвенные родственники (другие кольца, не event ring!): `drainFirmwareRings` @ 0x8b255dc
(trace ring, validator fw+0xb38) ← handleEvent idx7-ветка и
`AGXArmFirmware::stopFirmwareForRecoveryInspection` @ 0x8b4a60c (путь restart-инспекции);
`drainFirmwareTraceRing` @ 0x8b257c0 ← `ensureTracesUpdated` @ 0x8afb2e8.

[Открыто] Точный call-site вирт-вызова vtable+0x288 (чей action/кто зовёт) статически
не локализован — кандидаты: interrupt event source (heap PAC-указатель), подкласс
AGXArmFirmware, код IOGPUFamily. На достижимость разбора это не влияет: кольцо дрейнится
по прерыванию от firmware всякий раз, когда read != write.

## 8. Происхождение памяти кольца и вердикт по спрею

- Backing создаётся в `AGXAccelerator::start` @ 0x8af1f3c–0x8af21d4 через
  `AGXInternalResource::initInner` (0x8b69e78) → `makeFWMapping` (0x8b69ffc) →
  `makeCPUMapping` (0x8b6a16c) → `prepareMappings`; CPU VA через vtable+0x138
  (0x8af219c–0x8af21c4). Это dedicated IOGPU-family shared resource: аллокация один раз
  на старте, не из общего kalloc-пула страниц.
- Header/entries (fw+0xb70/b78 ← fw+0xc78/c98) — та же shared-память, пишется GPU/
  firmware (write idx в header @ +0x20 крутит firmware).

**Вердикт:** page-granular спрей freed-страниц с девайса перерабатывает страницы
общих пулов (IOSurface/backing и т.п.). Память event ring — dedicated shmem, созданная
на boot и не освобождаемая → **напрямую спреем не достижима**. Цепочка «спрей → event
ring → guilty index» (приоритет ① родительского дока) требует либо (a) непрямого
пути: спрей → страницы какого-то пользовательского GPU-аллокатора, чьи страницы
firmware использует для событийного кольца (подтверждений такого reuse не найдено,
[проверить динамически: смотреть, из какого пула физстраниц AGXInternalResource берёт
память — IOBufferMemoryDescriptor vs dedicated carveout]), либо (b) иного примитива,
портящего уже размещённое кольцо (GPU-side OOB write из command buffer — вне scope
текущего phys-spray подхода).

Побочная находка: если спрей всё же попадёт в entries (вариант (a)), достаточно
**любого** мусора — panic случится по failure-путям §5 без точной наводки на type 4;
точный type-4 с idx в [0,count) нужен только для panic «Type confusion» с печатью
индекса (§6) и телеметрии B2.

## 9. Открытые вопросы

1. [Динамика] noreturn-природа stub@0x8bab470: подтвердить panic (ожидается) vs
   error-log одним прогоном с подсунутой мусорной записью.
2. [Динамика] count из stub@0x8baa650: ёмкость слотов (0x80?) vs число живых стампов —
   определяет, насколько широк диапазон idx для «Type confusion».
3. [Статика→динамика] пул физстраниц AGXInternalResource (IOGPU shmem) — шанс reuse
   нашими спреями.
4. [Статика] кто зовёт vtable+0x288 (action регистрации прерывания).
5. ✓ сверено 2026-09-08 (iOS G16P): fw+0x7a0…0x7d0, accel+0x18de8…0x18f78 (сдвиг
   −0x48 от macOS-диапазона 0x18e30–0x18fb8), типы/bitmap 0xa000ffd3/capacity 256 —
   идентичны; fw+0x900 status block; fw vtable+0x108 (virt-call type 4).
