# Журнал исследования, часть 2 (секции 21–37)

> Продолжение docs/SPTM_research_journal_part1.md

---

## 21. Batch-путь 0xa8cb8a0 — kernel-only, ветка закрыта

### 21.1 Идентификация глобалов 0xb3855c0/0xb3856c0

По ~180 референсам в pmap-коде и коду инициализации (0xa924400): это **две статические pmap-структуры** (0x100 байт каждая, индекс pmap = (VA + 0x2400000000)>>6 — тот же трюк, что в аллокаторе). Т.е. kernel pmap + вторая служебная pmap.

### 21.2 Семантика 0xa8cb8a0 и 0xaf226b0

- x0 — **vm_map** (не vm_object): [x0+0x38] = pmap-компаньон, сравнивается с двумя kernel pmap; [x0+0x18] flags & 0x1650 == 0x200.
- Значит 0xa8cb8a0 помечает nibble 0xe только страницы **карты, привязанной к kernel pmap** — kernel/boot страницы (вероятно, разметка исполняемых диапазонов kc при загрузке).
- 0xaf226b0 (обёртка, x0 = родительская структура со списком 0x28-записей, options &0x600, error 0xe00002d9) — **ноль вызовов во всех exec-сегментах** (kTEXT_EXEC, PRELINK_TEXT, BOOT_EXEC) и ноль указателей в файле → вызывается через runtime-таблицы (MIG/routine) или из BootKC-структур; не user-reachable.

**Ветка закрыта:** batch-путь не досягаем из userland и работает только с kernel pmap.

### 21.3 User-reachable путь к типу 14 — только pmap_enter с EXECUTE

Остаётся единственный: vm_map_enter/vm_protect с VM_PROT_EXECUTE → pmap_enter (0xa8c89bc) → nibble 0xe → donation tag 0xe → SPTM 11→14. W^X-контроль происходит **до** pmap_enter — в vm-слое (cs-проверки при выдаче exec-прав). SPTM здесь лишь исполнитель.

### 21.4 Уточнение по perm-классам (RWX-анализ)

RWX-класс = writable (bit3=0) ∧ user-exec (bit0=0) → классы {0,2,4,6}.
- type 14 mask 0xba80 → {7,9,11,12,13,15} — **ни одного RWX-класса** ✗
- type 15 mask 0xba88 → {3,7,9,11,12,13,15} — класс 3 writable, но UXN=1 ✗
- **Единственный тип без маски — 33 (XNU_DEFAULT)**: пропускает и mask-проверку, и guard'ы (секция 14). Страница типа 33 может получить PTE класса {0,2,4,6} = **настоящий RWX**. Но assign в 33: kinds {3,4,5}, VMID=1, FTE-условия (секция 14.3) + XNU никогда не заказывает тег 0x21 статически.

### 21.5 Финальная сводка (iOS 27.0b, t8130)

| Вектор | Вердикт |
|---|---|
| USER_DEBUG W^X flip | ✗ (13) |
| USER_DEBUG из XNU | ✗ (20.2) |
| Batch-путь 0xa8cb8a0 | ✗ kernel pmap only (21.2) |
| Mailbox double-fetch | ✗ (18.2) |
| Retype-гейты SPTM | ✗ adjacency+domain+hooks (18-19) |
| **Type 33 RWX** | теоретически даёт RWX, но assign недостижим статически — **цель №1** |
| **cs-гейт в vm-слое** (кто разрешает EXECUTE до pmap_enter) | не разобран — **цель №2** |
| **VMID-confusion** (0x12/0x13/0x21) | не разобран — **цель №3** |
| **Runtime-таблицы** (rec-хуки, Table A/B, fast-path op-id) | требует дампа — **цель №4** |

### 21.6 Следующий шаг

Разбор **cs-гейта в vm-слое**: цепочка vm_map_enter(VM_PROT_EXECUTE) → где проверяется codesign/entitlement до вызова pmap_enter (кандидаты: vm_map_enter → cs_invalid_page / vm_map_cs*, или проверка [map+0x30] бит 0xf из секции 17.0). Найти все места, где EXECUTE-прот выдаётся без cs-проверки (JIT-пути, shared cache, nested-pmap для VM).

---

## 22. jitbox: аппаратное JIT-окно A17 — реальный механизм W^X в iOS 27

### 22.1 Аппаратные регистры (jitbox.c, кластер 0xa965xxx-0xa966xxx)

| Регистр | Роль | Запись |
|---|---|---|
| **s3_4_c15_c15_1** | дескриптор JIT-окна: PA + код размера (clz) + nibble флагов | 0xa966594 (единственный msr в kc) |
| **s3_4_c15_c15_4** | управление/магия: 0x61e0c5b (mode ≤ 2), 0xe1e0c5b (mode > 2), 2 (x0==0 → off) | 0xa9664f8 |
| **s3_4_c15_c15_6** | per-thread состояние окна; сохраняется/восстанавливается при dispatch ([ctx+0x1a8]) | 0xa946ca8/0xa946cb4 |

Программирование окна: **0xa9664a8**(x0=enable, x1=mode, x2=PA, x3=size): валидация PA-диапазона (окно 0xffff_ffdc00000000, размер ≤ 0x10000000000, граничные проверки битов 0x37), паника jitbox.c:0x5c при нарушении.

### 22.2 Per-task дескриптор окна и context-switch

- **proc+0x360/0x368/0x370/0x378** = (mode, PA, size, count/enable) — дескриптор JIT-окна задачи.
- **0xa946c78** (вызывается из dispatch: 0xa946c00, 0xa94861c): сравнивает proc-дескриптор с per-CPU кэшем [cpuctx+0x210..0x228]; при расхождении перепрограммирует окно через 0xa9664a8 и ставит флаг [thread+0x6b]=1.
- Т.е. **привилегированная запись в JIT-страницы возможна только когда текущий тред задачи с зарегистрированным окном выполняется на CPU** — аппаратное окно следует за тредом.

### 22.3 SPRR/perm-group assign: 0xa964d64

x0 = запрос: [x0]=pmap (0x2200-байтная структура, refcount [pmap+0x2128], lock [pmap+0x2198]), [x0+0xb0]=vm_map. По значению [x20+8] (perm-дескриптор) ∈ {**0xc6000019**, **0xc600001a**, ...} выбирает путь (два разных lock-режима 0xa7d58b8/0xa7d5970) и назначает perm-группы. 0x19/0x1a — вероятно exec / non-exec группы SPRR.

### 22.4 MIG-поверхность

0xa964d64 вызывается из **MIG-диспетчера 0xa960ce0** (гигантский switch по msgh_id: подсистема с id вида 0x30xx_xxxx–0x32xx_xxxx; точные: 0x3000300f, 0x31003c09, 0x320003ff — нестандартная, ApplePrivate-подсистема). Гейт вызова: [req+0xd8]==0. Диспетчер не имеет прямых bl-вызовов и raw-указателей — регистрация через runtime-таблицу routine'ов.

### 22.5 Связь с pmap

**0xa966888** (рядом в кластере): pmap_enter с фиксированными флагами w6=0x1440, w7=0x26 и **[pmap+0x68]=2** — создание специальной pmap (type 2 = jitbox/VM pmap?), далее обход списка шагом 0xe с вызовом 0xa8ccb88. Кандидат на «создать JIT-карту».

### 22.6 Почему это главный вектор (а не SPTM retype)

1. SPTM-анализ (секции 13-21) показал: ни USER_DEBUG, ни RWX perm-класс из XNU не получить — система закрыта с этой стороны.
2. **jitbox — легальный W^X bypass by design**: окно делает JIT-страницы записываемыми для тредов задачи-владельца. Весь контроль — в том, **кто может зарегистрировать окно в proc+0x360** и **кто назначает exec perm-группу** (0xc6000019 через MIG 0xa960ce0).
3. Поверхность атаки:
   - регистрация дескриптора proc+0x360..0x378 (найти запись — syscall/MIG; гейты: entitlement/cs?);
   - MIG-подсистема 0xa960ce0: фаззинг msgh_id, type-confusion в req-структурах ([req+0xd8] гейт);
   - угон чужого окна: подмена proc+0x368/0x370 (PA/size) другой задачи через kernel-read/write примитив — тогда dispatch сам запрограммирует окно на чужие PA;
   - гонки dispatch vs изменение дескриптора (окно программируется до флага [thread+0x6b]).

### 22.7 Next steps

- [ ] Найти syscall/MIG регистрации proc+0x360..0x378: скан записей +0x368/+0x370 с PA/size (кандидаты из 21-го скана: сузить по соседним entitlement-проверкам).
- [ ] Идентифицировать подсистему 0xa960ce0: найти mig_routine-таблицу (поиск в __DATA по msgh_id константам 0x3000300f и т.п.).
- [ ] jitbox fault handler («jitbox fault in the kernel, state=%p, esr=%#llx»): где ловится, что делает с нарушителем — понять, что окно запрещает (запись вне allowlist → fault?).
- [ ] Семантика 0xc6000019/0x1a: какая группа — exec; где ещё используются эти константы.

---

## 23. Гейт MAP_JIT: _proc_check_map_anon в AMFI (iOS 27.0b)

### 23.1 Локализация

- Строки MAP_JIT живут в **AppleMobileFileIntegrity.kext** (macho @ file 0x6569c0; __TEXT vm 0xfffffff00765a9c0; __TEXT_EXEC vm 0xfffffff009179450, file 0x2175450, size 0x2ab80).
- Гейт: **_proc_check_map_anon** = **0xfffffff00918a48c** (имя-строка @ 0xfffffff007662e3a).

### 23.2 Логика (полная)

Вход: (proc x20, cred x19, addr x2, size x3, w4, flags w5). Срабатывает только для анонимного MAP_JIT: `w5==0x800 && (x2|x3)==0 && w4==0`.

Цепочка проверок по порядку:
1. **Лог-раз в сессию**: [0xb446758]==0 → OSReport один раз (0x918a4cc-0x918a4e4).
2. **0x91816b8()** — «jit разрешён конфигурацией системы»; если ==0 → лог «MAP_JIT but jit is disallowed by system configuration» → **deny** (w0=1).
3. **0x9180d58()** — «developer mode resolved»; если ==0 → лог «developer mode status has not been resolved!» → **return 0x10**.
4. **[0xb446758]** — «device unlocked»; если ==0 → лог «MAP_JIT, but device still locked!» → **return 0x10**.
5. **0x91a3730(proc)** — доп. проверка процесса; если !=0:
6. Entitlement-цепочка (0x9182cf8(cred) → entitlements, 0x918e6d8 = has-entitlement):
   - **`dynamic-codesigning`** → **ALLOW сразу** (0x918a550 tbnz → 0x918a58c);
   - иначе нужны ОБА: **`com.apple.developer.web-browser-engine.webcontent`** + **`com.apple.developer.cs.allow-jit`** → ALLOW.
7. Иначе deny (w0=1).

### 23.3 Выводы для джейлбрейка

- **JIT на iOS 27 требует разблокированного устройства (пароль) + developer mode + entitlement**. Для Dopamine-style jb: таргет-процесс с `dynamic-codesigning` (JIT-наследуемый?) или WebContent (`web-browser-engine.webcontent` + `cs.allow-jit`) — WebContent имеет оба by design.
- Гейт «device still locked» ([0xb446758]) — состояние first-unlock; записывается AMFI при разблокировке. Точка для патча при наличии kernel R/W: флипнуть [0xb446758] и глобал конфигурации (результат 0x91816b8/0x9180d58 кэшируется? проверить).
- Соседняя функция (0x918a614+) проверяет `system-task-ports.*` + `get-task-allow` — похоже, гейт task_for_pid/отладки (релевантно для tfp0-подобных примитивов).

### 23.4 Открытая часть цепочки: от MAP_JIT к jitbox-окну

_proc_check_map_anon лишь разрешает флаг; дальше vm-слой должен:
1. создать vm_entry с JIT-меткой,
2. при бэкинге страниц — зарегистрировать окно в **proc+0x360..0x378** (mode, PA, size, count) — точка записи НЕ найдена статически в этом заходе (см. 22.7),
3. pmap_enter разметит страницы (тип 14 + SPRR группа 0xc6000019?).

**Ключевая неизвестная:** где PA/size окна попадают в proc+0x368/0x370 и валидируется ли, что они принадлежат JIT-региону задачи (иначе — угон окна на чужие PA = аппаратный RWX-allowlist для произвольных страниц).

### 23.5 Next steps

- [ ] Кто вызывает 0x918a48c (mac hook registration → mmap path): подтвердить, что после ALLOW флаг MAP_JIT проходит в vm_map_enter и где там метка JIT.
- [ ] Найти запись proc+0x360..0x378: кандидат — pmap/vm код при fault'е JIT-страниц или при первом enter'е; проверить валидацию PA против vm_entry диапазона.
- [ ] Глобалы [0xb446758] (device-unlocked) и код 0x91816b8/0x9180d58: кэшированные флаги → пригодность для патча при jailbreak.
- [ ] 0xc6000019/0x1a SPRR-группы: подтвердить exec-семантику (секция 22.3).

---

## 24. Регистрация jitbox-окна: полная цепочка от MAP_JIT до аппаратного RWX

### 24.1 Функция регистрации: 0xa9665d0 (jitbox.c)

Аргументы: (x0 = map, x1 = out_va, x2 = req_size, x3 = flags, x4, x5).

Алгоритм:
1. **Размер окна**: req_size → округление вверх до степени двух, **минимум 0x2000000 (32 МБ)**, максимум 1<<0x28.
2. **Единственность**: swpb на байте **proc+0x359** — второе окно для процесса = error 6.
3. **Аллокация**: 0xa8921fc (аллокатор map'а) запрашивает регион размером 0x10000000 + 2×window, флаги (user_flags & ~0x4000000000) | 0x80 → из него вырезается **pow2-выравненное** окно (излишки по краям освобождаются через 0xa88b0b0).
4. **Перемаркировка**: повторный 0xa8921fc по фиксированному адресу окна с флагами **0x4081 | 0x240<<32** (JIT-метка региона).
5. **Запись дескриптора** (0xa966774-0xa966780): **proc+0x368 = physmap-VA окна** (НЕ user-VA!), **proc+0x370 = размер**, **proc+0x378 = 1**.
6. **Cleanup**: на стеке собирается структура с PAC'd-колбэком **0xa96681c** (дискриминатор 0x2abe) и регистрируется через 0xa9386f0 — снос окна при смерти процесса.

### 24.2 Гейты вызывающего (0xaccce88, vm_map_enter-путь, MIG-reached)

Два входа в 0xa9665d0:
- **прямой** (0xaccd70c): после проверок PAC'd map (дискриминатор 0xc75) и флагов;
- **ограниченный** (0xaccd7c8): `proc == [0x7d6f048]` (proc, встроенный в boot-структуру 0xe20 байт +0x768 = **kernel_task**) ИЛИ entitlement-хук AMFI (blraa дискриминатор 0xe4fe через [0x7d6be28]+0x1c8).

Т.е. тройной гейт суммарно: **_proc_check_map_anon (AMFI, секция 23) → gate kernel_task/entitlement (0xaccce88) → единственность окна (swpb)**.

### 24.3 КЛЮЧЕВОЙ ВЫВОД: окно аллоцируется ядром — PA не пользовательский

proc+0x368 получает physmap-VA, **возвращённый ядерным аллокатором** — пользователь не контролирует ни PA, ни размещение. Валидация «PA принадлежит JIT-региону» не нужна по построению: окно — всегда свежая выделенная память.

### 24.4 Вектор угона окна (при наличии kernel R/W)

При dispatch (0xa946c78 → 0xa9664a8) единственная валидация дескриптора:
- x2 ∈ [0xffff_ffdc00000000, 0xffffffbfffffffff] (physmap);
- x3 ≤ 0x10000000000; pow2-выравнивание (из clz-логики).

**Атака**: перезаписать proc+0x368 (physmap-VA) и proc+0x370 на произвольный физический диапазон — например, physmap-VA **текста ядра/SPTM**. При следующем dispatch треда этой задачи железо (s3_4_c15_c15_1/4, магия 0x61e0c5b) откроет **аппаратную запись** в этот диапазон из EL0 тредов задачи — патчинг kernel text напрямую из userland, минуя SPTM (окно работает на уровне MMU/SPRR, а не frame types).

Проверить: распознаёт ли 0xa9664a8/dispatch, что proc+0x368 изменился легально (пересоздание окна) vs подделан — видимо нет, т.к. сравнение идёт с per-CPU кэшем [cpuctx+0x210..0x228], который тоже можно сбросить (сменить count proc+0x378 → перепрограммирование).

### 24.5 Cleanup-колбэк

**0xa96681c** (дискриминатор 0x2abe, зарегистрирован через 0xa9386f0 с w1=1) — вызывается при смерти процесса; сносит окно (вероятно, через тот же 0xa9664a8 с x0=0 → magic 2 = off). Вторая PAC'd-функция в структуре: 0xa938ce4.

### 24.6 Итоговая карта исследования (iOS 27.0b, t8130)

```
MAP_JIT (mmap, флаг 0x800)
  → _proc_check_map_anon [AMFI]      devmode + unlocked + dynamic-codesigning/(webcontent+allow-jit)
  → 0xaccce88 (vm enter, MIG)        kernel_task || entitlement-hook
  → 0xa9665d0 (jitbox.c)             аллокация pow2-окна ≥32МБ, proc+0x368/0x370/0x378
  → dispatch 0xa946c78               программирует s3_4_c15_c15_1/4/6 из proc-дескриптора
  → АППАРАТНОЕ ОКНО: треды задачи пишут в окно из EL0
```

- **SPTM/retype/USER_DEBUG — закрыто полностью** (секции 13-21): Apple вынесла W^X в jitbox+SPRR.
- **Легальный путь к JIT**: entitlements (WebContent имеет оба; dynamic-codesigning — debug-подпись через amfid).
- **Векторы**: (1) подмена proc+0x368/0x370 через kernel R/W → аппаратный RWX на произвольный физдиапазон (24.4); (2) подмена дескриптора + race с dispatch; (3) MIG-подсистема 0xa960ce0 (SPRR perm-group assign, секция 22.3) — не изученный фаззинг-таргет; (4) патч глобалов AMFI ([0xb446758] device-unlocked).

### 24.7 Next steps

- [ ] Найти MIG-stub → 0xaccce88 (msgh_id входа в JIT-регистрацию): скан mig routine-таблиц vm_map-подсистемы.
- [ ] 0xa8921fc: идентифицировать аллокатор (physmap?) и флаги 0x4081/0x240<<32 — семантика JIT-метки vm_entry.
- [ ] 0xa96681c (cleanup): подтвердить снятие окна; проверить use-after-free окна при race exit vs dispatch.
- [ ] Семантика s3_4_c15_c15_1 битов (nibble из clz(size), 0x4081): точный формат дескриптора окна — нужно для эксплоита 24.4.

---

## 25. jitbox: точки программирования окна и callout-механика

### 25.1 Вход в 0xaccce88

Прямых bl/b-вызовов нет ни в одном exec-сегменте (bias kernelcache uniform = 0xfffffff007004000, проверен по AMFI — сканирование корректно); указателя в таблицах нет → вызывается через runtime-установленные таблицы (bincompat op-blocks, секция 15.6) — как и весь pmap-кластер. Для эксплоита вход = mach_vm API с JIT-флагом (через mmap MAP_JIT).

### 25.2 ТРИ точки программирования аппаратного окна

| Точка | Триггер | Источник дескриптора |
|---|---|---|
| **0xa946c78** | dispatch треда (context switch) | proc+0x360..0x378 через per-CPU кэш [cpuctx+0x210..0x228] |
| **0xa96681c** | callout (регистрируется при создании окна через **0xa9386f0**, список 0xb335398) | proc+0x360..0x378 текущего proc, если == владельцу |
| **0xa9665d0** | создание окна | — |

Все три читают дескриптор из proc **без дополнительной валидации** (только range-check в 0xa9664a8). 0xa938ce4 — callout cancel/free (blraa disc 0x2abe → колбэк, refcount, освобождение через хэш-таблицу 0x7ccf168).

### 25.3 Race-поверхность (уточнение 24.4)

1. Callout 0xa96681c срабатывает по событию и перепрограммирует окно, если **текущий proc == владелец**: при смерти владельца и reuse proc-структуры (zone reuse) возможен сбой сравнения? — нет, сравнение по указателю, но **окно и дескриптор живут независимо**: cleanup должен и деаллоцировать окно (0xa88b0b0), и обнулить дескриптор — если между деаллокацией физстраниц окна и снятием per-CPU кэша есть окно, железо держит allowlist на уже свободные PA → **UAF на физических страницах с аппаратным RWX** (запись из EL0 в переиспользованные страницы — включая page tables нового владельца).
2. Cleanup-порядок в 0xa9665d0-родственном деструкторе — не разобран статически (где вызов 0xa88b0b0 при смерти; вероятно в ветке 0xaccce88 по error path 0xaccd590 или в proc_exit).

### 25.4 Формат дескриптора s3_4_c15_c15_1 (из 0xa9664a8)

```
значение = (window_VA & 0x1fffffe000000) | (size_code | flags)
size_code = (7 - clz(size-1)) & 0xf   → степень двух
+ nibble 6 если x1(mode)==0
s3_4_c15_c15_4 = 0x61e0c5b (mode≤2) / 0xe1e0c5b (mode>2) / 2 (disable)
```

### 25.5 Что осталось (для живого тестирования)

- msgh_id входа в JIT-enter (runtime-дамп vm op-table).
- Событие callout-списка 0xb335398 (что триггерит 0xa96681c).
- Порядок teardown окна при proc_exit → оценка UAF-окна 25.3.1.
- На устройстве: подтвердить, что подмена proc+0x368/0x370 + bump proc+0x378 перепрограммирует окно (24.4) — девайс с iOS 27.0b + kernel R/W.

---

## 26. Titan (alfiecg24): механизм Dopamine 3.0 SPTM bypass и применимость к 27.0

Источник: github.com/alfiecg24/Titan (публичный, клонирован в /mnt/agents/work/titan). Поддержка: iOS 16.1–17.4b3, A14–A17. Реимплементация Rocket (Coruna kit, GTIG).

### 26.1 Полная цепочка (по исходнику)

**Слой 0 — примитивы (вне Titan):** kernel R/W (в Dopamine 3.0 = DarkSword CVE-2025-43510 CoW-mismap / ClearSword CVE-2025-43520 VFS TOCTOU), `kalloc` (перманентные аллокации), `kmap` (мапить произвольные PA в userspace).

**Слой 1 — доступ к AGX:**
- Кража порта IOGPUDeviceUserClient у backboardd (src/ipc_port.c, техника Coruna; **пофикшено в iOS 26: data PAC на ipc_port->ip_nsrequest**).
- `kmap(0x292050000)` (IORVBAR A17) → reset vector → маркер `0x7777777777777700` → RTKit info-структура: gfx __TEXT PA/size, __DATA const, mangled PA, rtk_patch_page_table_base.
- Копия GFX-прошивки в userspace; gfx_patchfinder (src/gfx_patchfinder.c) находит: гаджеты `rop_invoke/write/spin/tlb_flush/cache_flush/tpidrro_el0`, функцию `arm_hibernate`, данные `power_thread`, `hibernation_context`, `asc_impl`, `asc_dbg`, `gfx_new_mapping_va`, `target_job`.

**Слой 2 — ROP на AGX:**
- Перезапись `power_thread.saved_state` указателем на подготовленный ROP-фрейм в target_job; при пробуждении power-треда AGX загружает контролируемый thread state.
- Главный гаджет — exception-return загрузчик полного регистрового состояния (вкл. X30 и ELR_EL1) + ERET → серия фреймов произвольных вызовов.
- **Патч iOS 18.1b4: instruction PAC на AGX** — PACGA-проверка PC/LR/CPSR/X16/X17 thread state (как в XNU) → ROP мёртв.

**Слой 3 — microPPL bypass (hibernation TTBR):**
- ROP вызывает `arm_hibernate` → AGX кэширует регистры (включая **TTBR0_EL1/TTBR1_EL1**) в hibernation_context.
- Патчим: `hibernation_context+0x108` (cached TTBR1) = PA фейковой L1-таблицы; `+0xD8` (thread state), `+0xC8` (stack) = фрейм write_pte; `ASC_OVRD=0xFFFFFFFF80000000` (форс сброса/гибернации).
- При wake AGX восстанавливает **наш** TTBR1 → фейковые таблицы (fake_l1[56]=fake_l2|3, fake_l2[0]=self_ref_L3|0x20000000000445) мапят **XNU page table** в AGX VA → второй ROP пишет `pte_to_store` в PTE этой таблицы: **таблица мапит сама себя**.
- **Патч iOS 17.4b4: TTBR0 не восстанавливается вообще, TTBR1 = hardcoded** → bypass мёртв.

**Слой 4 — финал на AP:**
- Ждём `kread64(self_ref_pte_kaddr)==pte_to_store` → self-referencing L3 → `titan_map_page`: перезапись PA-битов PTE в phys_window = **произвольный phys R/W**, включая SPTM/PPL-защищённые страницы (SPTM не видит запись, т.к. она идёт через уже-валидный PTE, закреплённый в TLB/таблице).

### 26.2 Соотнесение с нашим анализом

1. **Self-adjacency XNU_PAGE_TABLE → XNU_PAGE_TABLE** (секция 10, аномалия 3) — легальное условие для self-referencing PTE: SPTM разрешает PT-страницам мапить PT-страницы. Titan не ломает SPTM-логику — он **записывает PTE мимо SPTM** (через AGX DMA), и SPTM не может отличить эту запись от легальной.
2. Наша модель «SPTM чист внутри» (секции 13–21) подтверждается выбором атакующих: Coruna/Rocket/Titan пошли в обход через копроцессор, а не через retype/perm-классы.
3. jitbox-вектор (секции 22–25) — **альтернатива слоям 1–3**: вместо AGX-исполнения — аппаратное окно; вместо self-ref PTE — подмена proc+0x368. Требования скромнее: только kernel R/W (kmap/kalloc не нужны).

### 26.3 Статус патчей и что проверить на 27.0

| Компонент Titan | Патч | Статус на 27.0 — проверить |
|---|---|---|
| Port stealing (ip_nsrequest) | iOS 26: data PAC | проверить PAC-дискриминатор на [ipc_port+…] в kc.macho — статика, быстро |
| AGX ROP (thread state) | iOS 18.1b4: PACGA | проверить в AGX-прошивке 27.0 (gfx im4p в IPSW) наличие PACGA на пути saved_state |
| Hibernation TTBR restore | iOS 17.4b4: TTBR1 hardcoded, TTBR0 не восстанавливается | проверить в AGX-прошивке 27.0 код restore hibernation_context |
| Self-ref PTE endgame | **не пофикшен концептуально** | жива, если появится запись в kernel PT мимо SPTM |

### 26.4 Следующий шаг

AGX-прошивка из IPSW 27.0 (24A5390f): извлечь, найти hibernation-restore путь и PACGA-проверку thread state. Если хоть один из двух патчей AGX регрессировал/отсутствует — цепочка Titan частично переоткрывается (слой 2 или 3), и недостающее добирается kernel R/W отдельно. Это чистая статика по имеющемуся IPSW.

---

## 27. AGX-прошивка 27.0 (24A5390f): верификация патчей против Titan

Источник: `Firmware/agx/armfw_g16p.im4p` из IPSW iPhone16,2_27.0_24A5390f (range-скачан с updates.cdn-apple.com без полного IPSW; сохранён как /mnt/agents/work/armfw_g16p.im4p и gfx.macho). Mach-O: __TEXT vm 0xfffffc0000000000 (совпадает с gfx_text_base из Titan для A17 ✓ — прошивка та же по структуре).

### 27.1 Патч microPPL (17.4b4) — ПРИСУТСТВУЕТ

```asm
0xfffffc000005d028: cbz w21, #0x5d03c
0xfffffc000005d02c: msr ttbr1_el1, x20   ; x20 = x1 = аргумент функции
0xfffffc000005d03c: msr ttbr0_el1, x20
```
TTBR0/TTBR1 восстанавливаются из **аргумента кода** (x20=x1), а не из hibernation_context в памяти. Отдельно: init-путь `0x1c84: bl 0x1df0; msr ttbr1_el1, x0` — TTBR1 = вычисленная константа (base таблиц), как и описано в патче «TTBR1 hardcoded». Кэшированные в память значения больше не используются → запись в hibernation_context+0x108 бесполезна. **Вектор Titan слоя 3 мёртв.**

### 27.2 Патч AGX PAC (18.1b4) — ПРИСУТСТВУЕТ

9 кластеров `pacga` в __TEXT (0x1e10, 0x242c, 0x2570, 0x2618, 0x272c, 0x2854, 0x294c и др.), цепочки вида:
```asm
pacga x1, x30, x1
pacga x1, x16, x1
pacga x1, x17, x1
```
— подпись X30/X16/X17 на путях thread state, плюс `msr apiakeyhi_el1` на пути инициализации (0x1c60). Подмена saved_state power-треда бессильна: подписанный state не подделать без ключа. **Вектор Titan слоя 2 мёртв.**

### 27.3 Вывод

AGX-ветка для 27.0 закрыта полностью (ожидалось, но требовало прямой проверки — сделано, регрессий нет). Из четырёх компонентов Titan в iOS 27 не работает ничего, кроме концепции self-ref PTE (которая сама по себе требует записи в kernel page table мимо SPTM — чего AGX больше не даёт). Остаются наши векторы: jitbox-угон (24.4/25.3), MIG-подсистема 0xa960ce0, VMID-confusion, первичный kernel-баг класса DarkSword/ClearSword в 27.0b (vm/VFS слои, не SPTM).

---

## 28. AppleM2ScalerCSCDriver (арена CVE-2025-43510): локализация в 27.0b4

Статус CVE: CVE-2025-43510 (CoW mismap через selector 1 драйвера) и CVE-2025-43520 (VFS TOCTOU) **пофикшены в iOS 26.1** → на 27.0b4 мертвы (подтверждено по бюллетеням Apple/GTIG). FilzaSlop содержит порт DarkSword (pe_v2, ICMP6_FILTER OOB + kalloc_type leak) — на 27.0b4 неприменим как kernel R/W, полезен как userland-доставка (sandbox escape на 27b1-4) и инфраструктура.

### 28.1 Локализация драйвера в kc.macho

- Mach-O @ file 0x5382c0 (uuid 8a442084ab5d3db6ab43c603ad778c63): __TEXT vm 0xfffffff00753c2c0 (cstring, вкл. AppleM2ScalerCSCDriver.cpp/.h, HalMSR21/MSR9/MSR10j), __TEXT_EXEC vm 0xfffffff008f70870 (file 0x1f6c870, size 0x13c630), __DATA_CONST vm 0x7f691f0, __DATA vm 0xb41d438.
- Классы: AppleM2ScalerCSCDriver + AppleM2ScalerCSCHalMSR* + AppleM2ScalerCSCDriverFilters.
- MetaClass/newUserClient-фабрика: **0xfffffff008fc3cfc** (OSMetaClass::alloc("AppleM2ScalerCSCDriver") → init → vtable blraa 0x3a87).
- Шесть функций, ссылающихся на .cpp: 0x8fff96c, 0x9003e34 (в fn ~0x9003xxx), 0x9003e80, 0x90083e4 (в fn ~0x9005xxx/0x9008xxx), 0x90084bc, 0x900fd4c.
- **Ключевая строка: "[IOSA][Boot ] MSR Driver Waiting for IOSurfaceRoot"** — драйвер аттачится к IOSurfaceRoot → **достижим из любого приложения через IOSurfaceUserClient** (идеальная поверхность для первичного бага: sandbox не нужен).
- Panic-строка "Same request can not be enqueued twice %p" (0x7618549) — queue/refcount логика рядом с обработкой запросов (fn 0x90083xx): класс багов, соседний с CVE-2025-43510.

### 28.2 Оценка FilzaSlop для нашей цели

- userland sandbox escape на 27.0b1-4 (MobileHouseArrest/MCM) — **рабочий слой доставки** на твоём билде;
- kexploit (DarkSword-порт) — мёртв на 27.0b4 (оба CVE пофикшены в 26.1);
- вывод: первичный kernel-баг искать самим; драйвер — точка входа №1.

### 28.3 Next steps

- [ ] Перечислить external methods драйвера (dispatch через IOSurface/IOExternalMethod): найти selector-1 impl и соседние — аудит на тот же класс CoW/vm_map_copy-ошибок в 27.0b4 (код новой мажоры).
- [ ] Дифф драйвера 26.0 vs 26.1 (kernelcache'и доступны на Apple CDN) → точное место и форма фикса CVE-2025-43510 → аудит соседей паттерна.
- [ ] Дифф VFS (арена CVE-2025-43520) 26.6 ↔ 27.0b4 — новый vnode-код мажоры.

---

## 29. Дифф-инфраструктура 26.0↔26.1 (арена CVE-2025-43510) — пайплайн готов

### 29.1 Артефакты (все в /mnt/agents/work/)

- **kc_26.0.macho** (23A341) и **kc_26.1.macho** (23B85): range-скачаны с Apple CDN (без полных IPSW), bvx2-декомпрессия через pyimg4, ~70 МБ каждый.
- Драйвер в 26.x: macho @ 0x493c10 (26.0, TEXT_EXEC file 0x24d2df0 size 0x100d00, vm 0x94d6df0) и @ 0x4b2610 (26.1, TEXT_EXEC file 0x251f7a0 size 0x101c2c, vm 0x95237a0). Cstring-регионы: __TEXT 0x7497c10 / 0x74b6610.

### 29.2 Функциональный дифф (по pacibsp-границам, хэш mnemonic-потоков)

- 26.0: 4350 функций; 26.1: 4369. Хэш-дельта: 84 новых/изменённых, 76 удалённых.
- Фаззи-матчинг (difflib ≥0.75): **66 пар** изменённых функций.
- Ранжирование по Δbl (рост числа вызовов): топ — 0x6875c (26.1) +4 bl, но это **OSAction-бойлерплейт** (26.1 перегенерировал OSAction-инфраструктуру — vtable slot 0x398, pacia e4fe), т.е. инфраструктурный шум, не фикс.
- Проверенные кандидаты: 0x5a184 (26.1) vs 0x59924 (26.0) — серия вызовов виртуала (slot 0x78) с command-id 0x1a0/0x1a4/0x1a8 (командная очередь скейлера), +2 bl = две доп. команды; на CoW-фикс не похоже.

### 29.3 Вывод по фиксу CVE-2025-43510

Массовый дифф зашумлён перегенерацией OSAction. Точная локализация требует: (а) распознать OSAction-диспетчеризацию драйвера (selector N = OSAction index N — vtable slots 0x398+), (б) сравнивать target-функции экшенов, а не их обёртки. Selector 1 → OSAction #1 → его target — следующая точка.

### 29.4 Утилиты

- /tmp/get_kc.sh — range-скачивание любого компонента из любого IPSW по URL (zip64 EOCD → CD → entry → deflate) — работает для kernelcache (~21 МБ) и мелких im4p.
- Паттерн функционального диффа (pacibsp + mnemonic-hash + difflib) — готов к применению для любых пар версий и любых kext'ов.

### 29.5 Next steps

- [ ] OSAction-диспетчеризация: найти в DATA_CONST драйвера (26.x и 27.0) таблицу OSAction-регистраций → selector→target маппинг → сравнить target #1 между 26.0/26.1 и проаудировать его соседей в 27.0b4.
- [ ] VFS-дифф 26.6↔27.0b4 (арена CVE-2025-43520) — отдельный заход, тот же пайплайн.

---

## 30. OSAction/vtable-диспетчеризация драйвера и итог маршрута диффа

### 30.1 Раскладка класса (27.0b4)

- getMetaClass AppleM2ScalerCSCDriver: **0x8ffe9d0**; instance size **0x640**; vtable класса = **0xfffffff007f85138** (file 0xf81138).
- Слоты 0x398/0x3a0/0x3a8 — no-op стабы (bti+ret, базовые виртуалы); собственные: 0x3b8 (0x900ff98 — metaclass-бойлерплейт), 0x3e8/0x3f0 (0x901450c/0x9014514); далее базовые из main kernel. Т.е. external methods НЕ лежат в vtable — диспетчеризация через runtime OSAction-массив (создаётся из виртуалов, паттерн виден в 26.1@0x6875c: wrap vtable slot → pacia e4fe OSAction).

### 30.2 Ключевой результат: драйвер в 27.0 ПЕРЕПИСАН

Матчинг четырёх функций драйвера 27.0b4 (те, что ссылаются на AppleM2ScalerCSCDriver.cpp) в 26.0/26.1 по mnemonic-хэшу:

| Функция 27.0 (insns) | лучший матч 26.0 | sim | лучший матч 26.1 | sim |
|---|---|---|---|---|
| 0x8fff6f8 (180) | 0x859d4 | 0.45 | 0x86524 | 0.45 |
| 0x90033e0 (512) | 0x50cec | 0.15 | 0x50f6c | 0.15 |
| 0x9007e38 (446) | 0x7d3a4 | 0.54 | 0x7deec | 0.54 |
| 0x900f8e0 (335) | 0x66d8c | 0.31 | 0x67538 | 0.31 |

Сходство 0.15–0.54 → **код драйвера в 27.0 в значительной степени новый**. Следствия:
1. Маршрут «найти фикс CVE-2025-43510 диффом и аудировать соседей паттерна» **теряет ценность**: окружение фикса переписано, соседние с ним функции 26.x в 27.0 не существуют.
2. Зато сам по себе факт: мажорная переписка = свежий, малоаудированный код. Правильный маршрут — **прямой аудит четырёх функций 27.0** (все достижимы через IOSurface-цепочку), особенно 0x90033e0 (512 insns, дважды ссылается .cpp — core request handling) и 0x9007e38 (446).

### 30.3 Next steps

- [ ] Де компилировать 0x90033e0 и 0x9007e38 (27.0b4): жизненный цикл M2ScalerCSCRequest, работа с IOMemoryDescriptor/CoW — искать класс CVE-2025-43510 в новом коде.
- [ ] Найти runtime OSAction-массив: функция, создающая экшены при init (аналог 26.1@0x6875c в 27.0) → точный список selector→method.
- [ ] Параллельно: VFS-дифф 26.6↔27.0b4 (арена CVE-2025-43520, пайплайн готов).

---

## 31. jitbox teardown: порядок освобождения и вердикт по race

### 31.1 Механика (статика, 27.0b4)

- **Отключение железа явно не вызывается при смерти**: у 0xa9664a8 ровно два вызывающих (dispatch 0xa946d48, callout 0xa966884). Выключение — ленивое: при следующем dispatch задачи с count==0 → x0=0 → magic 2 (off).
- **Дескриптор (proc/owner+0x360..0x378) затирается деструктором IOKit-объекта 0x8758bdc**: серия release OSObject'ов (+0x390..0x3a8, blraa 0x3a87) и последовательный zero +0x360/+0x368/+0x370/+0x378 (сайты 0x8758cf8-0x8758d70 и дубль 0x8758f00-0x8758f78 — два объекта или два пути). Владелец дескриптора — IOKit OSObject (vtable 0x7e192a0), т.е. скорее pmap-companion, чем «голый» proc — уточнение к секции 22.2 (там объект назван proc по цепочке 0xa848c3c; точный тип владельца: OSObject, [uthread+0x28]-derived).
- **Страницы окна** освобождаются отдельно, при vm_map teardown (общий путь, не jitbox-специфичный).

### 31.2 Порядок при exit/exec — выглядит безопасным by design

- exit(): задача суспендит треды (task_terminate) до vm_map_destroy → EL0-писателя нет к моменту освобождения страниц; дескриптор затирается деструктором владельца; следующий dispatch выключает железо (count=0 → magic 2).
- exec(): owner-объект (pmap-companion) пересоздаётся → деструктор старого затирает дескриптор; dispatch нового map: count=0 → disable. Дыра «дескриптор переживает exec» — закрыта (при условии, что владелец действительно пересоздаётся; подтверждено косвенно — дескриптор живёт в OSObject, не в скалярном proc).

### 31.3 Остаточный race (единственный, узкий)

**Соседний тред на другом CPU**: выходящий тред гонит teardown (освобождение страниц окна), а sibling-тред той же задачи в этот момент продолжает исполняться на другом ядре с запрограммированным per-CPU окном → аппаратная запись в уже освобождённые физстраницы (reuse → чужие page tables). Thread-terminate через AST асинхронен — окно реально, но узкое (нужно, чтобы sibling был именно в EL0-записи в момент free).
Эксплуатируемость: second-stage, требует выигрыша тонкой гонки; существенно сложнее прямого угона дескриптора (24.4).

### 31.4 Вердикт по приоритету

Teardown-ветка: **порядок корректен для exit/exec; остаточный race узкий и трудновыигрываемый** — понижаем приоритет. Возвращаемся к первичному багу: аудит драйвера (0x90033e0, 0x9007e38) — главная линия.

---

## 32. Аудит драйвера (27.0b4): карта user-facing пути запроса

### 32.1 Цепочка (всё достижимо из приложения через IOSurface-цепочку)

```
external method 0x90065a8 (this, x1, x2, x3)
  → factory 0x9068f68: M2ScalerCSCRequest alloc (metaclass 0xb4419d0), [req+0xc28]=x3
  → param ingestion из user struct x21:
      0x900642c — bitfield unpack flags (безопасно)
      floats/dims через 0x8fb63e0 (fixed-point conv), [x21+0x68/0x60] → dims (zero-check)
      fixed-count копии (8/4) rect-параметров — без переполнения
  → SURFACE-МАППИНГ (зона класса CVE-2025-43510), 0x9006a10-0x9006a7c:
      ldp [x21] → [req+0x51c/0x7dc]
      если [req+0xd0]!=0:  0x90ac690 (import → 0xa01da14, IOSurface kext)
          ([this+0x130]=IOSurface client obj, w1=[req+0xd0]=surface id) → [req+0xaa0]=descriptor,
          0x90ac970 → [req+0xaa8]=length, [req+0xd8/0xe0] → [req+0xab0/0xab8] (range pair)
      если [req+0xe8]!=0:  аналогично → [req+0xa20]/[req+0xa28]/[req+0xa30/0xa38]
  → enqueue 0x9007e38: sorted insert по компаратору 0x900e3cc; индексы захардены (sxtw+poison csel)
  → process 0x90095xx: length!=0 → 0x9009f7c (HAL attach, surface obj [req+0x520]);
      length==0 → 0x90ac8f0 (import, IOSurface map + completion callback 0x9007c1c)
```

### 32.2 Наблюдения по качеству кода

- Индексация очередей — с poison-паттерном (sxtw check + 0x2bad<<48) — свежая захарденная парадигма;
- «Same request can not be enqueued twice» — panic-guard на дабл-энкью;
- Но: **[req+0xd8/0xe0] (user range pair) хранится и потребляется без видимой сверки с [req+0xaa8] (длина дескриптора)** — если HAL/продолжение использует их для адресации — это классическая OOB-зона. Пока прямых потребителей +0xab0/+0xab8 для адресации не найдено (только bookkeeping?).

### 32.3 Открытые аудит-точки (по приоритету)

1. **0xa04eb8c** (IOSurface kext, вызывается из 0xa01da14): валидация surface id + права на surface — есть ли confusion между client контекстами (другой процесс's surface)?
2. **0x9009f7c** (driver, HAL attach): что делается с descriptor [req+0xaa0] и range pair — адресация DMA?
3. **0x90ac8f0 import target** + completion 0x9007c1c: lifecycle surface-маппинга (UAF при раннем освобождении req vs completion?).
4. Где популятся [req+0xd0/0xd8/0xe0/0xe8/0xf0/0xf8] (второй external method, "set buffers") — там валидация user offsets.

### 32.4 Итог захода

Поверхность полностью отмаплена от external method до IOSurface. Харднинг высокий, но два классических семейства остаются открытыми: (а) surface-id confusion при lookup в IOSurface (32.3.1), (б) range-validation в HAL-attach (32.3.2). Следующий заход — 0xa04eb8c и 0x9009f7c.

---

## 33. Вердикт по аудиту surface-пути драйвера (27.0b4): путь захарден

### 33.1 Проверенные точки

**1. Surface lookup (0xa01da14 → 0xaf553d8 → 0xa8f8b30, IOSurface kext):**
```asm
ldr x16, [x0, #0x310]   ; owner token клиента (PAC'd, disc 0x8280)
bl  0xa8f8b30           ; lookup(surface_id, type=0x26, owner)
; внутри: bounds на id; 0xa79a2e0(owner, id) — поиск В namespace ВЛАДЕЛЬЦА;
; [x20+1] type bits & 7; [x20+0] == 0x26 (type check); PAC'd auth с computed disc (0x5a^type)
```
→ confusion между клиентами закрыта по построению (owner-scoped namespace + type check). **Чисто.**

**2. Range-валидация при HAL-attach (0x9009f7c):**
```asm
ldr x27, [x19, #0x10]   ; user range из запроса
ldr x0,  [x19]          ; дескриптор
bl  0x90ac980           ; → длина дескриптора
cmp x27, x0
cset w8, ls             ; требуется range ≤ length (+ результат 0x90ac670)
tbz w8, #0 → attach; иначе log+skip
```
→ user range pair проверяется против реальной длины дескриптора на attach. **Проверка существует** (именно класс CVE-2025-43510 — и она на месте).

**3. Индексация очередей/массивов:** poison-паттерн (sxtw + 0x2bad<<48) везде: enqueue 0x9007e38, attach 0x9009f7c ([req+0x428]<8 guard). **Чисто.**

### 33.2 Вывод

Surface-путь запроса в 27.0 написан с учётом уроков CVE-2025-43510: владение памятью, namespace, range checks и индексация закрыты. Продолжать аудит этой цепочки — низкая отдача. Остаточные зоны драйвера: lifecycle completion-callback (0x9007c1c) vs раннее освобождение req, второй external method (buffer setter) — не проверены, но маргинальны.

### 33.3 Стратегический поворот

С driver-пути снимаемся. Остаётся подготовленная вторая линия: **VFS-дифф 26.6 ↔ 27.0b4** (арена CVE-2025-43520 — VFS TOCTOU/vnode): код VFS в 27.0 новый (мажор), пайплайн диффа готов (kc_26.0/26.1 в work/, kc 26.6 точечно скачивается тем же методом), цель — найти vnode/namei-паттерны с отсутствующими фиксами или новыми багами. Это следующий заход.

---

## 34. VFS-линия: CVE-2025-43520 раскрыт, инфраструктура диффа ядра построена

### 34.1 CVE-2025-43520 (источник: гист Muirey03)

- Место: **bsd/vfs/vfs_cluster.c**. `cluster_read_ext`/`cluster_write_ext` → `cluster_io_type` → `vm_map_get_upl(UPL_QUERY_OBJECT_TYPE)`; если объект физически непрерывен → IO_CONTIG → `cluster_*_contig` вызывает `vm_map_get_upl` **второй раз** и делает физкопию из `upl_phys_page`.
- **TOCTOU**: между двумя вызовами VA-диапазон ремапится (становится не-непрерывным) → OOBR/OOBW по физпамяти. Эксплуатируется из любого приложения через файловый I/O.
- Фикс (26.1): после второго `vm_map_get_upl` — проверка `upl_flags & UPL_PHYS_CONTIG`, иначе EINVAL (два сайта: read ~6082, write ~3689).

### 34.2 Инфраструктура (готово)

- **kc_26.6.macho** (23G71) в work/ — скачан range-методом, bvx2-декомпрессия.
- Полный индекс функций главного ядра: 106k функций 26.0 и 26.1 (pacibsp + mnemonic-sha1); 617 changed пар (difflib ≥0.8, bucketed).

### 34.3 VFS-ish изменения 26.0→26.1 (10 функций)

Самое заметное: **переработка флагов compound rmdir** в vfs_syscalls.c (26.0@0x4ab0 → 26.1@0x45a54):
- `lsr w20,w3,#6` → `#7` (сдвиг извлечения флага);
- блок вычисления флагов переписан: 26.1 ставит 0x80 всегда + маска 0x5000 из w21<<8, bfi #0x14 (было #0xe);
- добавлен out-param (sub x4,x29,#0x74).
→ семантика compound-rmdir флагов в 26.1 изменена. Отдельная точка для аудита в 27.0 (как эти флаги потребляются VNOP_RENAME/REMOVE downstream — не появился ли confusion старый↔новый layout при смешанных вызывающих).

Прочее: apfs_vfsops/vnops churn, pathmonitor_prepare_rename, vfs dataless-entitlements (26.0@0xb150→26.1@0x4c054, sim 0.84), lifs_vnop_strategy_done.

### 34.4 HFS удалён из 27.0

Все hfs_* функции отсутствуют в 27.0b4 (есть в 26.6) — поверхность VFS в 27.0 сокращена до APFS (+lifs/fifo/specfs).

### 34.5 Next steps

- [ ] Найти cluster_read_ext/cluster_write_ext в 27.0b4 (refs 'vfs_cluster.c'): верифицировать наличие UPL_PHYS_CONTIG-проверки; найти всех двойных вызывающих vm_map_get_upl без re-check (sibling-паттерн CVE-2025-43520).
- [ ] Аудит compound-rmdir флагов в 27.0: потребители поля +0x190 (vnop compound arg) — confusion между layout 26.0/26.1.
- [ ] Полный кластерный дифф 26.6↔27.0b4 по vfs_cluster.c (в 27.0 код новый).

---

## 35. vm_map_get_upl в 27.0b4: TOCTOU закрыт архитектурно

### 35.1 Идентификация

**vm_map_get_upl (27.0b4) = 0xfffffff00a980e90** (7-аргументная сигнатура map/offset/size/upl_size/upl/page_info/count/flags; идентичный пролог с 26.1-версией 0x2ee944). 23 call-сайта во всём kc.

### 35.2 Двойные вызывающие (паттерн CVE-2025-43520)

5 функций с ≥2 вызовами — все в vfs-cluster (0xa982xxx-0xa98cxxx):
0xa985370 (×4), 0xa989e50 (×4), 0xa9838b8 (×3), 0xa9841ec (×2), 0xa987d34 (×2).
**Ни в одной нет caller-side re-check** (окно 12 инструкций после каждого вызова проверено).

### 35.3 Почему re-check не нужен: фикс переехал ВНУТРЬ API

Внутри 0xa980e90 (27.0):
- **0xa9823ec** — object-lock обёртка (casa на глобальном локе 0xb392ef8, per-CPU контекст, обход списка объектов с валидацией [x+0x4c]<0): тип/contig объекта вычисляется **под локом объекта**;
- **0xa976e20** — резолв vm_object ([x0+0x88]→[+0xd8], PAC'd disc 0xd6ef) и чтение state-флагов;
- **identity re-verification**: `cmp x19, x23; b.eq → reuse; иначе str xzr,[x26+0x20]` — объект перепроверяется перед возвратом upl.

Вывод: в 26.1 фикс был caller-side (`upl_flags & UPL_PHYS_CONTIG` после второго вызова); **в 27.0 Apple переработала vm_map_get_upl так, что оценка типа/contiguity идёт под локом объекта с реверификацией идентичности** — TOCTOU-окно между query и use закрыто внутри API. Класс CVE-2025-43520 в текущем виде для 27.0 мёртв.

### 35.4 Следствие для поиска багов

Паттерн «query-then-act без лока» остаётся искать **в других API**, не получивших такой же переделки:
- прямые вызовы upl_create с UPL_QUERY_OBJECT_TYPE из драйверов (scaler/IOSurface-цепочки);
- vnode-операции с двойным чтением user-данных (namei re-lookup);
- compound-rmdir флаг-layout (секция 34.3) — потребители поля +0x190.

### 35.5 Next steps

- [ ] Аудит compound-rmdir флагов (34.3): downstream-потребители +0x190 в 27.0b4.
- [ ] Скан вызывающих upl_create/UPL_QUERY_OBJECT_TYPE в kext'ах (не через vm_map_get_upl) — те же двойные паттерны без лока.
- [ ] (Фон) дифф vfs_cluster.c 26.6↔27.0 на предмет новых IO-путей.

---

## 36. Compound-rmdir флаги: layout по версиям, тред закрыт

### 36.1 Layout флагов compound-аргумента (строитель VNOP)

| Версия | Извлечение | Вычисление flags |
|---|---|---|
| 26.0 (0x4ab0) | `lsr w20,w3,#6` | csel 0x1080/0x80, bfi #0xe |
| 26.1 (0x45a54) | `lsr w20,w3,#7` | `and w8,#0x5000,w21<<8; bfi #0x14; orr #0x80` |
| 27.0b4 (0xa9b150c) | биты из w20 | `w8=(w20<<3&0x200)|(w20>>2&0x20)|(w20>>4&1)` → [x19+0x150]; +0x14=0x80; orr #0x400 |

### 36.2 Проверка confusion

- Якорь [x+0x150]: из 1754 чтений отфильтрованы 2 кандидата с tag==0x15 — оба оказались **другими структурами** (+0x150 = указатели/массивы, не flags word). Потребители compound-аргумента (APFS VNOP_RENAME/REMOVE) компилируются вместе с билдером в одном kernelcache → cross-version confusion невозможна по построению.
- Вывод: изменение layout отражает эволюцию API, но бага из него не следует — **тред закрыт, дальнейший ROI низкий**.

### 36.3 Что реально осталось от VFS-линии

- CVE-2025-43520 pattern мёртв архитектурно (секция 35);
- compound-rmdir закрыт (36);
- compound VNOP семантика (0x80/0x400 биты) — отдельный глубокий аудит APFS vnop handlers, маргинально.

VFS-линия исчерпана на уровне «быстрых побед».

---

## 37. Дифф kext'ов по существованию 26.6↔27.0b4: 4 новых, инвентарь целей

### 37.1 Результат диффа (PRELINK_INFO plist)

| Kext | Оценка |
|---|---|
| **com.apple.driver.VideoProcessing (exec VCPDRM)** | ★ топ-цель: IOUserClient из приложений |
| com.apple.security.Image4 | img4-парсинг (reach ограничен из apps) |
| com.apple.kec.AppleEncryptedArchive | crypto/archive, без userclient |
| com.apple.driver.AFKHIDTBDevice | Thunderbolt HID — нужно железо |
| − com.apple.filesystems.hfs.kext | удалён (HFS кончился) |

### 37.2 VCPDRM — полный разбор (macho @0xc45160, TEXT_EXEC всего 0x1330 ≈ 300 insn)

- **VCPDRMService::newUserClient (0xa554348)**: alloc VCPDRMUserClient (0x98 байт), ТРИ виртуальных гейта (blraa по vtable slots 0x5d0/0x360/0x2b0 — init/разрешения; при фейле — release и 0xe00002c9).
- **Dispatch 0xa554ad8**: selector ≤ 2; требует scalarInputCount==0; таблица методов 0x8268688 + sel*0x18.
- **Handler sel? (0xa554b3c)**: rate-limit ([x+0x100] счётчик, лимит 301/окно — «VCPDRMHitCount»); входной u64 валидируется 1..0x20; индекс в массив слотов [x+0x110] по 0x30 байт (poison-guarded); поиск свободного слота (битскан), запись объекта(ов) и u64; ответ в structureOutput.
- **Handler sel? (0xa554ce4)**: симметричное освобождение слота по id (release двух OSObject, обнуление, сброс бита).
- Третий handler (0xa554e08+).

**Замеченный запах:** работа с таблицей слотов и счётчиком hitcount **без видимых локов** — если externalMethod не сериализуется IOKit per-client, два треда одного клиента могут гонять register/unregister по одному id → race на состоянии слота (проверять на девайсе).

### 37.3 Финальный список целей для девайс-фазы (приоритет)

1. **VCPDRMServiceUserClient** — 3 selector'а, новый код, race в слотах; фаззинг id/размеров + конкурентный hammer sel_register/unregister.
2. **AppleM2ScalerCSCDriver** (через IOSurface) — новый код 27.0, фаззинг request-параметров (карта секции 32).
3. **MIG SPRR-подсистема** (диспетчер 0xa960ce0): перебор msgh_id 0x3000_xxxx–0x3200_xxxx по кандидатным портам.
4. IOGPUDeviceUserClient — классика, для полноты.
5. (Второе) Image4/AppleEncryptedArchive — если окажутся reachable.

### 37.4 Статическая фаза закрыта

Все быстрые статические направления исчерпаны: SPTM (чист), jitbox (вектор готов), AGX (запатчен), драйвер скейлера (захарден), VFS/cluster (архитектурный фикс), VCPDRM+новые kext'ы (отмаплены). Дальше — девайс: харнес по списку 37.3 + паник-сбор. Журнал: 37 секций.
