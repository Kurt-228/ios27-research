# AGXAccelerator::restartWorkQueue + getGuiltyChannel: разбор структур и целей для phys-spray

Дата: 2026-09-08. Тот же источник, что и `iogpu_restart_policy.md`: BootKernelCollection.kc
macOS 27.0 (26A5388g), кекст AGXG16G (360.32.1), M3/A17-класс. Девайс-цель: iPhone 15 Pro Max
(A17 Pro, G16P) — код общий, но все смещения ниже проверены **только** на macOS-бинаре; на iOS
G16P они могут отличаться (отмечено [iOS?] где риск выше всего).

Контекст задачи: у нас есть phys-spray с контролем контента в freed ядерные страницы на девайсе.
Системные GPU-клиенты (WindowServer/бэкенды/др.) забирают эти страницы и фолтятся → кекст AGX
читает структуры, лежащие в переработанных страницах. Нужно понять, какие структуры читает
restart-анализ AGX-кекста, чтобы: (a) вызвать panic-assert, (b) увести чтение pid/имени процесса
по контролируемому адресу (infoleak в лог), (c) спровоцировать «no guilty channel» panic.

Инструменты: дизасм `results/kc-extract/restart_wq_full.txt` (3029 строк, функция
`AGXAccelerator::restartWorkQueue(AGXWorkQueue*)` @ 0xfffffe0008ae6820),
`results/kc-extract/guilty.txt` (`AGX3DWorkQueue::getGuiltyChannel() const` @ 0x8ba6e44).
Внешние вызовы идут через interposable-стабы (`adrp x17,0x7f5c000; add; ldr x16,[x17]; braa`) —
символьные имена стабов статически не резолвятся (jumptable хранит закодированные значения),
ниже они обозначены `stub@0x8bab000` и т.п. с семантикой по использованию.

## 1. Карта restartWorkQueue

Регистры на входе: x20 = this (AGXAccelerator*), x28 = wq (AGXWorkQueue*).
Все `add xN, xM, #k, lsl #12` — встроенные подобъекты внутри огромного AGXAccelerator.

| Блок | Адреса (func+off) | Что делает |
|---|---|---|
| Gate | +0x00–0x1ac | `ldrb [accel+0x18e38]`; вирт-вызов `[accel+0x570]+0x1a8(wq)` → при 0 ранний выход (+0x2f08). |
| Reason dispatch | +0x1c8–0xa80 | `w0 = [[accel+0x158]+0xf0]` (тип устройства, сравнение с 2..5); байт `[wq+0x8]` (причина 0..6) → enum w21 + строка описания. Прогресс-чек: `[x0+0x60]`=очередь, `[queue+0x0]` vs `[queue+0x30]`, `[x0+0x70]` vs `[queue+0x30]`, `[queue+0x30]` vs `[queue+0x40]`. |
| Stamp-ring stuck check | +0x68c–0x934 | x21 = `[wq+0xd8]` (stamp-ring объект); count=`[x21+0x288]`; массив `[x21+0x88 + i*16]` → каждый элемент в `stub@0x8bab250`, тест бита 63 результата → флаг «stuck» (w27) → reason 7/8. |
| iofence_list collection | +0x808–0x934 | по тому же кольцу: `stub@0x8baac70(count)` (аллок массива), для каждой записи `stub@0x8baae50` → объект (retain через вирт +0x20), `stub@0x8baac80(array, obj)` → позже уходит в debug-словарь под ключом `iofence_list` / `iofence_num_iosurfaces` / `iofence_iosurfaces`. |
| Телеметрия/GPU Hang log | +0x22f4–0x268c | getGuiltyChannel (`stub@0x8bab000`), `[ret+0x60]`=IOGPUCommandQueue, `[queue+0x490]`=pid → `stub@0x8bab4b0(pid, buf, 36)` (proc info → имя); уровень рестарта из `[[accel+0x158]+0xf0]-2` → `stub@0x8baa220(queue, level)`; итерация sideband-списка (sp+0x90) — ещё каналы/пиды. Строки: `'GPU Hang: '`, `' (pid=%u)'`, `'guilty_dm'`. |
| restart_reason_desc | +0x2030–0x2154 | вирт `[vtable+0x238]`('restart_reason_desc') → имя; snprintf_upd собирает сообщение. |
| USC/firmware status | +0x22f4 | для reason-битов 0xa10 (4,9,11): `u32 = [[[accel+0x570]+0xd18] + 0x50a8]` → `'%d'` в лог; вирт `[vtable+0x208]` с указателем `x19+0x4298` (блок +0xd18) — ключ `signature`. |
| Учёт | +0x2750 | `[accel+0x640]++`; `stub@0x8bab3e0` → timestamp в `[accel+0x650]`; `[[accel+0x148]+0x28](3,1)` и `(5)`. |
| RestartReport | +0x226c, +0x2dd8 | `AGXRestartReport::finalizeAndSendReports(accel)` над объектом из `[gMetaClass+0x710]`; затем release и очистка `[x27]`. |
| Fence walk | +0x2808–0x29e0 | `[x27+0x4d8]` (x27=accel+0x116e8) — список AGXIOFenceData: `reset()` по цепочке `[+0x38]`. `[x27+0x448]` — lock (пары `stub@0x8ba9bf0`/`stub@0x8ba9c30`). `[x27+0x3f0]` — fence-tracker: `[+0x1d0]` OSArray, count `[+0x28]`; для каждого fence: `w27=[fence+0x1e8]` (stamp idx, пропуск если <1), индексация bitmap-массивов `[A+0x418]/[A+0x420]/[A+0x428]/[A+0x430]` (A=[sp+0x48], границы через маску из `[A+0x400]`), вирт-вызов vtable+0x1e8(w1=-1), retain/release `[fence+0x1d0]`. |
| Ring cleanup | +0x2ca8–0x2dcc | bar-индексы `[accel+0x11c20+0x10000 +0x5b8 / +0x5bc]`; записи кольца `accel+0x11c48 + idx*0x60`: clear `+0x0`, `+0x50`; если запись жива: ptr=`[+0x50]`, i16=`[+0x58]`, `w = [[ptr+0x48] + 0x15bc]`; если НЕ (i<=0xff && w==0): `u16 0 → [[ptr+0x178] + 2*i]`; счётчик `[sp+0x28][0] += 0x100` → entry+0x48 и `[A+0x550+4i]`; вирт `[A vtable+0xfb0](i&0xff)`; если `[accel+0x1b09c].bit3`: лог через `[accel+0x1ba0]` (`'TA/3D/CL: stamp_idx=%d '`). |
| Wrap-up | +0x2dcc–0x2f68 | `[accel+0x540]->+0x60` вирт +0x138; `stub@0x8bab2b0([accel+0x548])`; `[accel+0x570]` вирт +0x450; `[accel+0x648]++`; очистка `accel+0x18f20..0x18fa0` и state `accel+0x18e30`; release wq. |

## 2. Таблица структур

Колонка «тип аллокации»: K = kext-объект (kalloc_type, не перерабатывается нашим
page-granular спреем напрямую), S = shared page / AGFI (карта ядром, пишется GPU/firmware;
перерабатываема только если страница шла из общего пула физстраниц), G = GPUVM/IOGPU shmem
(страницы из пула, кандидат №1 на спрей), HW = bar-регистры (MMIO, не спрей).

| Указатель | Смещение | Что читается | Валидация | Тип | Достижимость через spray |
|---|---|---|---|---|---|
| accel+0x18e38 | +0x0 (byte) | причина рестарта (sub 2, cmp 5) | нет | K (поле AGXAccelerator, копия из firmware event ring) | низкая; но значение пишется `AGXFirmware::drainFirmwareEventRing` из event ring → см. §4 |
| accel+0x18e34 | +0x0 (u32) | **guilty_stamp_index** (0x80 = none) | нет | K, значение из firmware event ring (S-источник) | **цель (a)/(c), см. §4** |
| accel+0x18fb8 | +0x0 (u64) | event payload | нет | K←S | как выше |
| [accel+0x570]+0xd18 | +0x50a8 (u32) | firmware/USC status → лог `'%d'` | нет | S (status block firmware) | **средняя-высокая** — если +0xd18 указывает на AGFI shared block; цель (b)-вариант «число в лог» |
| [accel+0x570]+0xd18 | +0x4298 (ptr) | строка `signature` → вирт+0x208 | нет | S | чтение как C-строки из контролируемого блока → infoleak/довычитка в пределах mapped-региона |
| bar [accel+0x11c20]+0x10000 | +0x5b8/+0x5bc | ring read/write idx | нет | HW/S | значения пишет GPU; гонка индексов → произвольный выбор записи кольца |
| ring entry accel+0x11c48+i*0x60 | +0x50 (ptr) | ptr → `[ptr+0x48]+0x15bc` (u32 read), `[ptr+0x178]+2*i` (u16 write 0) | нет (только liveness-битмапы accel+0x17c48/0x17c68) | K (записи встроены в accel; ptr — per-DM kext-объект) | низкая для записей; **средняя для ptr**, если ptr-объекты аллоцируются из shmem/GPUVM |
| ring entry | +0x58 (u16) | stamp idx (<=0xff gate) | сравнение с 0xff | K/HW | — |
| [wq+0xd8] → +0x88+i*16 | entry {IOSurface* @+0, u32 @+8} | элемент в `stub@0x8bab250` = IOSurface::getDetachModeCode, бит63 → stuck-detect; затем createFenceDebugDictionary/retain/массив | **нет** | **K — AGXIOFenceData, kalloc_type 664 B (§4-(d), §5.1 решён)** | **phys-spray мимо**; только UAF + kalloc-zone spray |
| [wq+0xd8] → +0x288 | count | граница циклов | **нет (vs 0x20)** | как выше | контроль count → OOB-итерация за пределы 664 B |
| [fence+0x1e8] (из [x27+0x3f0]+0x1d0) | stamp idx | индексация `[A+0x418/0x420/0x428/0x430]` | только >=1 и маска из `[A+0x400]`; **верхней границы по типу нет** | K (IOFence-объекты), значение шлёт пользователь в command buffer | **средняя-высокая**: значения fence/stamp приходят из пользовательских командных буферов; перекос маски → OOB в kalloc-массивы трекера |
| [channel+0x60] → queue | +0x490 (u32) | **pid** → proc info → имя в `'GPU Hang: '` | cbz на queue; **pid не валидируется** | K | pid не контролируется спреем, но неконтролируемый pid → чтение чужого proc — не наша цель |
| [channel+0x60] → queue | +0x0/+0x30/+0x40 | progress counters | нет | K | значения счётчиков влияют на reason-код (7/8/9/10/11) |

## 3. getGuiltyChannel (AGX3DWorkQueue, 0x8ba6e44) — точное условие panic

```
kc    = [this + 0xc0]                  // AGXAccelerator*
state = *(u32*)(kc + 0x18e34)          // guilty_stamp_index из firmware event ring

if (state == 0x80) {                   // "firmware виновного канала не назвал"
    a = stub@0x8bab000(this)           // host-side guilty detect
    d = stub@0x8baa460(a, key@0xca79eb0)
    if (d && stub@0x8bab150(stub@0x8baaff0(this), d+0xd08)) → return [this+0x218]
    else { x0 = [this+0x220]; if (x0) return x0; }        // w8=0x220 / 0x218 ветки
    // иначе fallback:
} else {                               // firmware дал guilty_stamp_index = state
    x20 = stub@0x8bab110(stub@0x8baaff0(this), state)     // lookup канала по индексу
    d   = stub@0x8baa460(x20, key@0xca79dd0)
    if (x20 == 0 || d == 0) → PANIC
    return d
}
return *(kc + 0x116f0)                 // default channel акселератора
```

Panic-строка (проверено чтением KC):

```
AGXk: %s:%d:%s: !!! getGuiltyChannel: Type confusion - invalid AGXChannel
      for firmware guilty_stamp_index %d
      (file: agxk_workqueue.cpp, line 1047,
       func: "virtual IOGPUChannel *AGX3DWorkQueue::getGuiltyChannel() const")
```

**Это и есть «no guilty channel» panic — цели (a) и (c) сходятся в одну точку.**
Условие: `state != 0x80` (firmware сообщил виновный stamp index) И lookup канала по этому
индексу вернул NULL (или у найденного объекта нет ожидаемого свойства).
В state==0x80-ветке panic нет вообще — там чистые fallback'и на [this+0x218/0x220] и
[kc+0x116f0]. То есть «no guilty channel» panic — только firmware-driven путь.

Базовый класс `AGXWorkQueue::getGuiltyChannel` @ 0x8ba6660 (не разбирался построчно;
AGXCLWorkQueue::getGuiltyChannel @ 0x8ba7bac — третий вариант, логика аналогична [iOS?]).

Кто пишет accel+0x18e34: `AGXFirmware::drainFirmwareEventRing` (0x8b224b4+):

```
x10 = [fw + 0x278]; x12 = x10 + 0x18000
x9  = *(u64*)(event_ring_ptr + 0x515c)   // payload события из firmware event ring
[x12+0xfb8] = x9                          // accel+0x18fb8
[x12+0xe38] = (0x060504_202 >> (dm*3)) & 7 // subtype -> accel+0x18e38
[x12+0xe30] = w8                          // -> accel+0x18e30
w2 = (w8_sp5c == -1) ? 0x80 : w8_sp5c
[x12+0xe34] = w2                          // guilty_stamp_index -> accel+0x18e34
```

(схематично; точные смещения source-полей и условия — `docs/fw_event_ring.md` §6:
записи происходят только если вирт-вызов fw vtable+0x1c8 вернул 0, значения — из
status block [fw+0xd18]+0x5000, индекс — из entry+0xc)

Event ring — AGFI shared memory (пишется firmware/GPU). Индекс **валидируется** в
drainFirmwareEventRing: `idx == -1 ∨ 0 ≤ idx < count(stamp-слотов)` иначе failure-паника
валидатора до записи в accel+0x18e34 (подробно — `docs/fw_event_ring.md` §5/§6).

## 4. Цели spray

### (c)+(a) panic «Type confusion / no guilty channel» — приоритет №1 (при подтверждении §5)

Цепочка: наш спрей → страницы firmware event ring / timestamp-колец → `drainFirmwareEventRing`
парсит событие → `accel+0x18e34 = guilty_stamp_index` (контролируемое значение != 0x80,
например 0xdead) → GPU hang того же контекста → restartWorkQueue → getGuiltyChannel →
lookup по индексу падает → **panic с нашим числом в строке**.

Что нужно для срабатывания:
1. Спрей-страница реально уходит в AGFI/event-ring пул (проверка — §5).
2. Значение по смещению события, из которого парсится индекс: `u32 @ entry+0xc`
   (см. `docs/fw_event_ring.md`). **Уточнение:** значение `idx ≥ count` отсекается
   валидатором event ring ещё до записи в accel+0x18e34 → panic случается, но другой
   («Ring entry contains bad data», без печати индекса). Для panic «Type confusion»
   с печатью нашего числа нужно `0 ≤ idx < count` при пустом stamp-слоте (канал
   уничтожен к моменту restartWorkQueue → lookup NULL).
3. Hang с firmware-инициированным recovery (reason из accel+0x18e38 != «host detected»),
   т.е. ждём реального GPU lockup, а не только host timeout.

Байт-карта (по записи события): type = 4 (u32 @ +0, в bitmap 0xa000ffd3), u32 индекса
@ +0xc в диапазоне [0, count) (не 0x80 — это маппинг -1); соседние поля события —
корректные reason/subtype, чтобы дойти именно до ветки guilty.
Оговорка: restartWorkQueue рано выходит, если `[accel+0x18e38]`/гейт не пройдены — событие
долно выставить и их (subtype byte accel+0x18e38 ∈ [2..7]).
Достижимость спрея — главный гейт: память event ring — dedicated AGXInternalResource
shared memory (boot-time, не kalloc-пул), напрямую спреем не перерабатывается; см.
`docs/fw_event_ring.md` §8 и §5 (любой мусор в кольце panic'ит валидатор — для DoS
достаточно менее точного попадания, чем для «Type confusion»).

### (b) infoleak — два варианта

Вариант B1 (слабый, без спрея): pid в `' (pid=%u)'` берётся из `[queue+0x490]` kext-объекта —
спреем не контролируется. Не тратим на него спрей.

Вариант B2 (основной): `[[accel+0x570]+0xd18]` — firmware status block. Для reason 4/9/11
кекст читает `u32 @ block+0x50a8` и печатает в `'GPU Hang: '` сообщение, а вирт-вызов
`[vtable+0x208]` получает указатель `block+0x4298` как строку (`signature`-контекст).
Если block в shared-странице из спрея: число из нашей страницы уходит в system log
(os_log телеметрия AGXKTelemetry) — грубый infoleak содержимого; строковый путь читает
до NUL в пределах mapped-страницы (довычитка соседних страниц в лог — ограниченная).
Приоритет средний: подтвердить, что +0xd18 — shared и что путь 4/9/11 достижим нашим hang-сценарием.

### (a-вариант) OOB в context-ID cleanup — полный разбор (приоритет ② пересмотрен)

> Важно: прежняя гипотеза «[x19+0x1e8] = stamp idx из пользовательского command buffer»
> **опровергнута** дизасмом. Это не fence и не user-поле. Ниже — установленная механика.

#### A. Что за объект x19 и откуда индекс

- `x19` = элемент OSArray, лежащего по `[[accel+0x11ad8] + 0x1d0]`. Элементы — объекты
  семейства **AGXGart / AGXSecureGart** (не IOSurface-fence, не sIOGPUIOFence).
  Идентификация: `AGXSecureGart::registerContextIDE(int)` @ 0x8b895a8 делает ровно
  `str w20, [x19, #0x1e8]` (w20 = w1 = аргумент-ID); `AGXGart::init` @ 0x8b6f7c4 инициализирует
  `[gart+0x1e8] = -1`. У gart'а: `+0x10` = AGXAccelerator, `+0x1d0` = retained ptr
  (tracker), вирт-метод слот vtable+0x1e8 = `registerContextIDE(int)`.
- `idx = [gart+0x1e8]` — это **context ID из AGXContextIDManager** (embedded-структура
  по адресу `accel+0x11ad8`, НЕ полиморфный объект: `+0x0` = ptr на владельца с OSArray,
  `+0x8` = lock (retained, = accel+0x11ae0), `+0x10` = capacity u32 (= accel+0x11ae8),
  `+0x1c` = flag byte, далее указатели на массивы). ID выдаёт
  `AGXContextIDManager::alloc(gart, hint, &out)` @ 0x8b1a14c: free-stack pop либо bitmap-scan
  с жёсткой границей `idx < capacity` (0x8b1a3b0 `cmp w9,w23; b.le`). Дескрипторы команд
  (3D/CL/IOSurfaceSharedEvent) вызывают alloc и кладут ID в `desc+0x154`.
- Помечать [iOS?]: смещения 0x11ad8/0x1e8 проверены только на macOS-27 AGXG16G.

#### B. Точная механика OOB (restartWorkQueue, блок 0x8ae8ff0–0x8ae9200)

Все «массивы A+0x418/0x420/0x428/0x430» — это поля менеджера, A+0x3f0.. = accel+0x11ad8:

| доступ | адрес | операция |
|---|---|---|
| `[A+0x3f8]` = manager+8 | lock retain/release вокруг цикла | — |
| цикл по `[[manager+0]+0x1d0]` OSArray (count>=2), элемент = gart | — | — |
| `ldr w27,[gart+0x1e8]` | idx | **проверка только `cmp w27,#1; b.lt`** |
| `ldr w8,[A+0x428 + idx*4]` = refcount[manager+0x38] | u32 READ, `cbnz → skip` | ветвление от OOB-данных |
| `ldr x11,[A+0x418+idx*8]`; RMW `((old&mask)+1)&mask \| (idx<<shift)` | u64 write, значение зависит от старого слова и idx | counter[manager+0x28] |
| `str xzr,[A+0x420+idx*8]` | **u64 zero-write** | gart-ptr array[manager+0x30] |
| вирт-вызов `registerContextIDE(-1)` (vtable+0x1e8) | сброс gart ID | — |
| `str w27,[A+0x430 + count++*4]`, count=[A+0x438] | u32 write самого idx | free-stack[manager+0x40/0x48] |

Верхней границы idx vs capacity НЕТ (только sxtw-ловушка movk #0x2bad → panics pal при
idx*8/idx*4 ≥ 2^31, т.е. idx до ~2^28..2^30 адресуется молча). Массивы — kalloc (heap
pointers в менеджере), размер = f(capacity); capacity пишется только на инициализации
(динамического роста не найдено, но и место записи в дизасме не локализовано — вероятно
регистровая адресация; [проверить динамически]).

Те же три примитива повторяются в **`AGXGart::free`** @ 0x8b70998 (fast path без проверки
idx < capacity: RMW counter+idx*8, zero gart-array+idx*8, registerContextIDE(-1), затем
поиск ID в массиве [manager+0x20] и push ID в free-stack) — то же отсутствие верхней
проверки. И в **fast path `AGXContextIDManager::alloc`** @ 0x8b1a180 (idx = старый
[gart+0x1e8] без перепроверки: refcount++, RMW, `str x22(gart ptr),[manager+0x30+idx*8]` —
OOB-запись УКАЗАТЕЛЯ НА GART, это самый сильный из трёх синков).

#### C. Вердикт по достижимости из userland — НЕ напрямую

Инвариант «`[gart+0x1e8] ∈ {-1} ∪ [0, capacity)`» держится чисто ядерной логикой:
- единственный writer поля — виртуальный `registerContextIDE(int)`; вызывается с
  bounded idx из alloc и с -1 из cleanup-путей;
- capacity не меняется в рантайме (записей не найдено);
- free-stack push/pop сбалансированы и подпитываются только валидными ID.
Все остальные потребители idx (complete 3D/CL @ 0x8ae0b2c, freeSourceContextId,
releaseTaskAndContextIDE @ 0x8b1a4fc) проверяют `idx < capacity` — а три cleanup-синка
(restartWorkQueue, AGXGart::free, alloc fast path) её сознательно не делают.

Следствие: это **defense-in-depth провал / второй хоп**, а не самостоятельный вектор из
userland. Прямого контроля idx (через command buffer, hint dword desc+0x89c/0x568/0x3d0
или спрей) нет. Для активации нужен отдельный примитив, ломающий инвариант:
1. **UAF/подмена gart-объекта**: объект в OSArray заменён/переиспользован так, что
   +0x1e8 содержит мусор ≥ capacity → restartWorkQueue (или AGXGart::free) даёт OOB
   u64-RMW + u64-zero + push мусорного ID в free-stack; далее alloc fast path пишет
   OOB указатель на gart по `manager+0x30 + idx*8` — удобная точка эскалации.
2. **Перезапись capacity вниз** (accel+0x11ae8) любым другим OOB-write.
3. **Подкласс AGXGart с переопределённым registerContextIDE**, получающим ID извне
   (статически не исключено; требует перебора vtable-наследников).

#### D. Практический вывод для фаззинга

- Не тратить спрей на «skew индексов fence» — вектора как самостоятельной цели нет.
- Держать как **усилитель**: gart-объекты (kalloc, ~0x300 байт, поля +0x1e8/+0x1d0)
  — приоритетная мишень для любого будущего kalloc-OOB; один испорченный dword +0x1e8
  превращается через cleanup/alloc в OOB-write указателя (case 1→alloc: `str x22`).
- Ранее пойманные panics `pal` (movk #0x2bad) при больших idx — это и есть срабатывание
  sxtw-ловушки на одном из трёх синков; если в логах panics-v* есть "invalid address
  (fault addr: ...2bad...)" в AGXGart::free/restartWorkQueue — инвариант уже ломался
  фаззингом, искать первопричину (коррупция gart) по логам.
- Фазз-кейс для девайса: штатный p_mtlmut по blit/copy командам (зацепляет prepare/
  complete дескрипторов → alloc/free gart ID), плюс churn создания/уничтожения command
  queue + secure contexts (AGXSecureGart create/destroy) для переиспользования kalloc-чанков.

### Цели «не трогать» (kalloc-only, низкий приоритет)

- Записи кольца `accel+0x11c48+i*0x60` и ptr `entry+0x50` — если ptr-объекты per-DM kalloc
  (AGXKernelContext), спрей page-granular мимо. Цепочка `[ptr+0x48]+0x15bc` (read u32) и
  `[ptr+0x178]+2*i` (write u16 0) — мощный примитив, но только при подтверждении, что
  ptr аллоцируется из shmem/GPUVM, а не kalloc.

### (d) Stamp-ring `[wq+0xd8]` = **AGXIOFenceData** (fence-data master) — полный разбор, §5.1 решён

Объект по `[wq+0xd8]` идентифицирован: это **AGXIOFenceData** (vtable `__ZTV14AGXIOFenceData`
@ 0x7f4eb80 ставится в allocIOFenceData @ 0x8b6abb8). Ответы на пять вопросов — по дизасму
(все адреса macOS-27 AGXG16G).

#### Q1. Кто / когда / каким аллокатором

- **Аллокатор**: `AGXIOFenceManager::allocIOFenceData` @ 0x8b6ab64 →
  `OSObject_typed_operator_new(AGXIOFenceData_ktv, 0x298)` (stub@0x8ba9db0, резолвится в
  `_OSObject_typed_operator_new`). Это **kalloc_type** с выделенным view `site.AGXIOFenceData`
  (ktv @ 0x7f59010; поле размера в ktv = 0x298 = **664 байта**, совпадает с непосредственным
  аргументом 0x298 в вызове). Обычная kalloc-зона: **не shmem, не boot-time, не GPUVM**.
  Сигнатура типа в ktv: '121122111222221111212121212121212121212121212121' (48 символов,
  укорочена — кап kalloc_type-sig).
- **Когда**: менеджер встроен в AGXAccelerator @ +0x11b28 и создаётся в `AGXAccelerator::start`
  (вызов `createIOFenceDataStructures` @ 0x8b6ac48 от 0x8af2280), которая **преаллоцирует 16
  мастеров** в LIFO-стек (manager+0x10, count [manager+0x88], lock [manager+0x8] =
  lck_mtx_lock/unlock stub@0x8ba9bf0/0x8ba9c30). Дальше `AGXAccelerator::
  createIOFenceWithTransaction` @ 0x8aeccb8: под lock берёт мастер из стека; при пустом стеке
  — `allocIOFenceData`; результат пишет в `desc+0x140` (0x8aecd8c). Освобождённые мастера
  возвращаются в стек `handleIOFenceCallback` @ 0x8afa158 / `handleIOFenceDataStash`
  @ 0x8afa544; при переполнении стека (count > 0xf, ветки 0x8afa5dc/0x8afa328) — release()
  через vtable+0x28 → `AGXIOFenceData::free`.
- **Связь с workqueue**: [wq+0xd8] = master-slot; [wq+0xe0] = &wq[0xd8] (self-pointer,
  ставится в init); обратный указатель **[master+0x70] = AGXWorkQueue\*** (доказано в
  handleIOFenceCallback: `x21=[master+0x70]; ldr x24,[x21+0xd8]; cmp x19,x24`).
  Init-пути (AGXWorkQueue::init 0x8ba65fc, AGX3DWorkQueue::init 0x8ba6de0, AGXCLWorkQueue::
  init 0x8ba7b48) пишут в +0xd8 только xzr. Прямая инструкция «[wq+0xd8] = master» в статике
  не найдена — запись, по всей видимости, идёт через &slot из [wq+0xe0] (регистровая
  адресация, grep'ом не ловится); [открыто] локализовать динамически (KTRAP/патч).

#### Q2. Layout объекта и entry

| Смещение | Поле |
|---|---|
| +0x00 | vtable (AGXIOFenceData) |
| +0x10 | AGXAccelerator* (back-ptr; [+0x10]+0x1b09c — бит ослога, +0x1b0a0 — os_log) |
| +0x18 | lck_mtx (IOLockAlloc; lock/unlock = stub@0x8ba9bf0/0x8ba9c30) |
| +0x20 | u64 (reset → 0) |
| +0x24 | 2×u32 stats (createFence инкрементирует векторно add.2s; печатаются в диагностике) |
| +0x28 | u32 fNumPendingFences (active count; free паникует при !=0: «Verification failed: fNumPendingFences == 0», agxk_iofence.cpp:76) |
| +0x30 | u64 generation (reset: ++) |
| +0x38/+0x40 | list-узел (stash-list accel+0x11bc8, flag byte [+0xc]) |
| +0x48..+0x6c | подструктура: биты +0x48/49/4a, qword-битмапы +0x4c/+0x5c, +0x6c = -1 |
| +0x70 | AGXWorkQueue* back-ptr |
| +0x78/+0x80 | второй list-узел |
| +0x88 | **inline array 32×16B** `{ IOSurface* @+0, u32 @+8 }`; init-значение {0, 15} (reset) |
| +0x288 | u32 count (≤ 0x20; createFence: `cmp w8,#0x20; b.hi` → «fSavedIOSurfaces <= AGXK_IOFENCE_DEBUG_MAX_SURFACE_COUNT», agxk_iofence.cpp:366) |
| +0x28c | u32 total (монотонный счётчик saveIOSurfaceInfo) |

Entry = сохранённый IOSurface + u32 (массив именно «saved IOSurfaces», имя из строки
паники). **IOFence* в entry не хранится** — `IOSurface::createFenceWithTransaction`
(stub@0x8baae60) возвращает IOFence* наружу ([sp+0x38] → caller); дедуп в createFence
(0x8b6aa14) сравнивает пару {IOSurface*, u32}. «Queue ptr» в entry тоже нет — очередь
достижима только через master+0x70.

#### Q3. Валидация перед разыменованием (restartWorkQueue)

- Единственная проверка перед циклами: `cbz x21` ([wq+0xd8] != NULL), 0x8ae6eac.
- `count = [master+0x288]` — граница обоих циклов, **проверки count ≤ 0x20 нет**
  (загружается по 0x8ae6eb8/0x8ae6ed8/0x8ae703c...). count > 32 → OOB-чтение за пределами
  664-байтного объекта.
- Валидации указателей entry нет: `entry+0` (IOSurface*) напрямую идёт в
  `IOSurface::getDetachModeCode()` (stub@0x8bab250; bit63 возврата = stuck-флаг), затем в
  `IOSurface::createFenceDebugDictionary()` (stub@0x8baae50) → retain → OSArray
  'iofence_list' (OSArray::withCapacity / OSNumber::withNumber([+0x28c],32); строки
  'Fences\n' 0x714c791, '%d %d, %d %d\n' 0x714c799).
- Итог: type-confusion чтения sprayed-указателя как IOSurface* — краш/логика внутри
  getDetachModeCode (deref [x0+…]).

#### Q4. Доступен ли ring для GPU-записи (GPUVM)?

**Нет.** Объект — kalloc_type (Q1); ни в allocIOFenceData, ни в createFence/reset/free
нет обращений к FW/GPU-маппингам; адрес мастера как контекст уходит только в
`IOSurface::createFenceWithTransaction` (x6 = master, arg5 = PAC-колбэк
`AGXIOFenceData::agxIOFenceCallback` @ 0x8b6a7bc). GPU доводит событие косвенно:
firmware → IOSurface-fence completion → kernel-колбэк. Память кольца GPU недоступна.
**Сценарий (б) «GPUVM-запись» закрыт.**

#### Q5. Вердикт «спреябельно ли»

**(а) phys-spray страницы — НЕТ.** 664 B → kalloc-зона (type zone по kt-view), объект не
page-backed (kalloc_large — от ~16 KB), страницы из phys-spray не перерабатывают зонные
чанки. Дополнительные гейты: kalloc_type группирует зоны по (size, signature), lifetime
мастеров длинный (16 преаллоцированы при старте + кеш-стек до 16, переиспользуются между
дескрипторами), churn — только создание/уничтожение command queue/каналов; и главное —
alloc-путь (allocIOFenceData) зовёт `reset()`, который **переинициализирует** count/array/
stats, т.е. «спрей содержимым при аллокации» до чтения restartWorkQueue не доживает.

Реалистичный вектор — только **UAF**: free-путь (handleIOFenceDataStash → release при
полном стеке) **не чистит [wq+0xd8]** (stores в free/stash не найдено; проверка
fNumPendingFences==0 гарантирует лишь отсутствие активных фенсов, не отсутствие ссылки из
wq). Если wq переживает мастера, restartWorkQueue читает освобождённый 664B-чанк:
count/entries = переработанное спреем содержимое (kalloc-zone spray: массовая аллокация
объектов того же size-class/сигнатуры, см. §4-A-D про churn очередей), а не phys-spray.
[открыто] подтвердить динамически, что slot реально данглит (возможно, wq всегда умирает
раньше мастера — тогда вектор только через heap-corruption из другого примитива).

**(б) GPUVM-запись — закрыт** (Q4).

План эксперимента на девайсе (если UAF подтвердится):
1. Churn создания/уничтожения command queue + secure contexts (гарантировать проход
   «stash full → release» — нужно >16 живых мастеров одновременно).
2. Сразу после уничтожения — kalloc-zone спрей 664B-чанков с шаблоном
   `{[ +0x288 ] = 0x40, entries = маркер}` (маркер либо 0x4141… для пойманного краша в
   getDetachModeCode, либо указатель на фейк-IOSurface из второго спрея).
3. Триггер GPU restart (штатный hang), ловить: panic-строки 'Fences\n'/'%d %d, %d %d\n'
   с нашим count, fault-адрес = маркер в `IOSurface::getDetachModeCode`, 'iofence_list'
   в диагностике. Признак успеха спрея — наши маркеры в panic-строке.

Пометка про «[ent+0x100]/[ent+0x60]»: в кольце AGXIOFenceData таких смещений нет (entry —
16 B). Чтения `[x0+0x60]` в restartWorkQueue (0x8ae6994/0x8ae6e78) идут из объекта,
возвращённого вирт-вызовом wq vtable+0x190 (progress counters: сравнение [obj+0] vs
[obj+0x30]); `[x0+0x100]` @ 0x8ae6c48 — поле объекта из lookup'а getGuiltyChannel
(stub@0x8bab000, default -1). К stamp-ring отношения не имеют. Вопрос снят.

## 5. Что подтвердить следующим (проверки)

1. ~~**Аллокация `[wq+0xd8]`**~~ — **РЕШЕНО**: объект = AGXIOFenceData (664 B, kalloc_type
   `site.AGXIOFenceData`), см. §4-(d). Вердикт: kalloc-only, phys-spray мимо; единственный
   живой вектор — UAF через неочищенный [wq+0xd8] (динамическая проверка в §4-(d)).
2. **Тип `[accel+0x570]+0xd18`**: найти владельца поля +0xd18 объекта [accel+0x570] и его
   инициализацию (shmem vs kalloc).
3. ~~Смещение guilty-индекса внутри firmware event entry~~ — **решено**: см.
   `docs/fw_event_ring.md` §3/§6: индекс = `u32 @ entry+0xc` (72-байт запись,
   `AGXFirmwareRingValidator::fetchNextEntry`), bounds-check `idx == -1 ∨ idx < count`
   в type-4 case до записи в accel+0x18e34; failure-пути валидатора — panic-стиль
   (`stub@0x8bab470`, «Ring entry contains bad data»), не silent-drop.
4. **iOS-смещения**: все «0x18e34/0x11c48/0xd18/0x490/0x1e8» сверить с iOS-G16P kernelcache
   (доступен на девайсе; этот Mac-бинарь — единственный источник сейчас).
5. **Context-ID менеджер** (§4-A): место записи capacity (accel+0x11ae8) и аллокации
   массивов accel+0x11af8..0x11b20 (в дизасме не локализовано — вероятно регистровая
   адресация в AGXAccelerator::init; точный capacity → размер kalloc-чанков под OOB);
   кто добавляет gart'ы в OSArray владельца [manager+0]+0x1d0; есть ли подклассы AGXGart
   с переопределённым registerContextIDE (п.3 §4-C).

## 6. Приоритеты (по достижимости)

1. **(c)+(a) panic через firmware guilty_stamp_index** — один достоверный путь, не требует
   контроля указателей, только значение; блокируется только вопросом §5.3 (спрей в event
   ring; §5.1 снят — см. §4-(d): kalloc-only, на эту цель не влияет).
2. **(a-вариант) OOB context-ID cleanup** — НЕ самостоятельный userland-вектор (см. §4-C):
   инвариант idx < capacity держится ядерной логикой. Ценность — как второй хоп/усилитель
   (один испорченный gart+0x1e8 → OOB-write указателя на gart через alloc fast path).
3. **(b) infoleak через status block [[accel+0x570]+0xd18+0x50a8 / +0x4298]** — зависит от §5.2;
   ценность — утечка содержимого нашей страницы в лог (подтверждение спрея + адресная инфа).
4. kalloc-only структуры: stamp-ring [wq+0xd8] (AGXIOFenceData) — **§5.1 решён: phys-spray
   мимо** (§4-(d); единственный живой вектор — UAF + kalloc-zone spray, gated). ring-entry
   ptr-цепочка accel+0x11c48 — только после подтверждения shmem/GPUVM-аллокаций ptr-объектов.

## 7. Побочные замечания

- Все `brk #0xc472` в функции — PAC-fail трапы (autda/xpacd verify), без обхода PAC не цель.
- `[accel+0x648]`, `[accel+0x640]` — счётчики рестартов; политика «2 рестарта → deny»
  (см. iogpu_restart_policy.md, queue+0x43a/0x43c/0x440) — в restartWorkQueue напрямую не
  читается, deny-логика живёт в вызывающем (processAllChannelCommands, блок за 0x8ae9768).
- Lock-пары `stub@0x8ba9bf0`/`stub@0x8ba9c30` — резолвятся точно: `_lck_mtx_lock` /
  `_lck_mtx_unlock` (IOLock == lck_mtx_t; в AGXIOFenceData мьютекс — поле +0x18). Гонки
  в ring cleanup возможны, но символьно не подтверждены.
