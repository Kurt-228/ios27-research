# SPTM-820.0.16 — чекпоинт исследования (A17 Pro / t8130, iOS 27.0 beta 24A5390f)

> Зеркало рабочего журнала. Актуальная полная версия поддерживается в сессии; здесь — снапшот секций 1–20. Секции 21–37 см. docs/SPTM_research_journal_part2.md

**Дата начала:** 2026-08-08
**Источник бинаря:** `Firmware/sptm.t8122.release.im4p` из ipsw 27.0 (24A5390f) для iPhone16,2
**Верификация:** строка версии `SPTM-820.0.16|2026-07-10:21:28:53.721404` и UUID `11D00CBA-901F-3A3D-BEE2-B43E348B4E9C` совпадают с panic-логом устройства — бинарь бит-в-бит тот, что исполняется на железе.
**Состояние устройства:** SoC revision 0x11 (B0/B1) → SecureROM `iBoot-8104.0.0.201.4`; SPTM load address `0xfffffff01c2e4000`, TXM `0xfffffff02c2e4000` (из panic-лога).

---

## 1. Бинарь

Mach-O arm64e (PAK00), PIE, не stripped от строк. Сегменты:

| Сегмент | vmaddr | fileoff | size | назначение |
|---|---|---|---|---|
| `__TEXT` | 0xfffffff027004000 | 0x0 | 0x18000 | строки/константы (не код!) |
| `__DATA_CONST` | 0xfffffff02701c000 | 0x18000 | 0x8000 | таблицы метаданных |
| `__LATE_CONST` | 0xfffffff027024000 | 0x20000 | 0x74000 | |
| `__TEXT_EXEC` | 0xfffffff027098000 | 0x94000 | 0x60000 | весь код |
| `__LAST` | 0xfffffff0270f8000 | 0xf4000 | 0x4000 | |
| `__DATA` | 0xfffffff0270fc000 | 0xf8000 | 0x14000 | |
| `__BOOTDATA` | 0xfffffff027110000 | 0x10c000 | 0x18000 | |

## 2. Доменная модель

Домены вызовов: `XNU`, `TXM`, `SK` (Secure Kernel, гибернация), `XNU_HIB`, `SPTM`. Диспетчер валидирует команду, домен, таблицу, права, entry point и стек TXM (`VIOLATION_ILLEGAL_DISPATCH_*`).

16 именованных dispatch-таблиц: `XNU_BOOTSTRAP`, `TXM_BOOTSTRAP`, `SK_BOOTSTRAP`, `T8110_DART_XNU/SK`, `SART`, `NVME`, `UAT`, `SHART`, `CPUTRACE`, `HIB`, `GEN3_DART_XNU/SK`, `T6000_DART_XNU`, `T8020_DART_XNU`, `RESERVED2`.

## 3. Интерфейс XNU→SPTM: 40 функций

Типизация/страницы: `LOCKDOWN`, `RETYPE`, `MAP_PAGE`, `MAP_TABLE`, `UNMAP_TABLE`, `UPDATE_REGION`, `UPDATE_DISJOINT`, `UPDATE_DISJOINT_MULTIPAGE`, `UNMAP_REGION`, `UNMAP_DISJOINT`, `NEST_REGION`, `UNNEST_REGION`, `SLIDE_REGION`, `CONDEMN_LEAF_TABLE`, `UNCONDEMN_LEAF_TABLE`.
Конфигурация: `CONFIGURE_SHAREDREGION`, `SET_SHARED_REGION`, `CONFIGURE_ROOT`, `SWITCH_ROOT`, `REGISTER_CPU`, `FIXUPS_COMPLETE`, `CPU_ID`.
PAC: `SIGN_USER_POINTER`, `AUTH_USER_POINTER`, `BATCH_SIGN_USER_POINTER`.
Гость/TLB: `GUEST_VA_TO_IPA`, `GUEST_STAGE1_TLBOP`, `GUEST_STAGE2_TLBOP`, `GUEST_DISPATCH`, `GUEST_EXIT`, `REGISTER_EXC_RETURN`.
Гибернация: `HIB_BEGIN`, `HIB_VERIFY_HASH_NON_WIRED`, `HIB_FINALIZE_NON_WIRED`.
Прочее: `IOFILTER_PROTECTED_WRITE`, `SURT_ALLOC/FREE`, `SPTM_SERIAL_PUTC/DISABLE`, `OUTPUT_AREA`.

## 4. Таксономия типов страниц (ядро модели безопасности)

Таблица @ `0xfffffff02701e098` (файл. 0x1a098), 121 запись `{u32 str_off, u32 flags=0x00100000}`. Код ссылается по индексу (`cmp w?,#0x43; ldr x,[tab,idx,lsl#3]`). Индексы 0–67 — типы фреймов, 68+ — строки `VIOLATION_*`.

**SPTM-типы (0–10):** `UNTYPED`, `UNUSED`, `DEFAULT`, `RO`, `CODE`, `TXM_CODE`, `XNU_CODE`, `XNU_CODE_DBG_RW`, `KERNEL_ROOT_TABLE`, `PAGE_TABLE`, `IOMMU_BOOTSTRAP`.
**XNU-типы (11–41):** `DEFAULT`, `RO`, `RO_DBG_RW`, `USER_EXEC`, `USER_DEBUG`, `USER_JIT`, `USER_TPRO`, `USER_ROOT_TABLE`, `SHARED_ROOT_TABLE`, `PAGE_TABLE`, `PAGE_TABLE_SHARED`, `PAGE_TABLE_ROZONE`, `PAGE_TABLE_COMMPAGE`, `IOMMU`, `ROZONE`, `IO`, `PROTECTED_IO`, `COPROCESSOR_RO_IO`, `COMMPAGE_RW/RO/RX`, `TAG_STORAGE`, `STAGE2_ROOT_TABLE`, `STAGE2_PAGE_TABLE`, `KERNEL_RESTRICTED`, `CPUTRACE_PA/VA_BUFFER`, `RESTRICTED_IO(_RO/_TELEMETRY)`, `SUBPAGE_USER_ROOT_TABLES`.
**TXM-типы (42–62):** `DEFAULT`, `RO`, `RW`, `CPU_STACK`, `THREAD_STACK`, `ADDRESS_SPACE_TABLE`, `MALLOC_PAGE`, `FREE_LIST`, `SLAB_TRUST_CACHE`, `SLAB_PROFILE`, `SLAB_CODE_SIGNATURE`, `SLAB_CODE_REGION`, `SLAB_ADDRESS_SPACE`, `BUCKET_1024…8192`, `BULK_DATA(_READ_ONLY)`, `LOG`, `SEP_SECURE_CHANNEL`.
**SK-типы (63–67):** `DEFAULT`, `SHARED_RO`, `SHARED_RW`, `IO`, `XNU_CONTENT`.

Вывод: RETYPE переводит фрейм между типами, монитор форсит инварианты «CODE не writable», «PAGE_TABLE недоступна гостю» и т.д. — это и есть то, что надо обойти. `XNU_CODE_DBG_RW` и `XNU_RO_DBG_RW` — debug-типы, интересны как лазейки при включённой отладке.

## 5. Точка входа sptm_guest_dispatch @ 0xfffffff0270f1570

- Вход только с замаскированными прерываниями (проверка DAIF).
- Function id (x0) — bounds-check против глобального диапазона; максимум 0x43.
- Доступ к проприетарным sysregs `s3_6_c15_c8_0` (per-CPU control) и `s3_6_c15_c11_1` (указатель текущего контекста).
- Атомарные рефкаунты (`ldadd`) на state-структурах; лимит 9 (cmn w9,#9).
- Логирование через единый хелпер с индексом строки в таблице типов/нарушений.

Малый ldrsw-диспетчер @ 0xfffffff0270a5904 (помечен как `sptm_cmd_state_machine`): switch @ 0xfffffff0270a5a30, таблица оффсетов @ 0xfffffff0270a5e30, 91 кейс, cap 0x5a.

## 6. UAT (User Access Table) — page-table сервисы

14 эндпоинтов: `INIT_STATE`, `DESTROY_STATE`, `MAP_TABLE`, `UNMAP_TABLE`, `MAP_BEGIN`, `MAP_CONTINUE`, `PREPARE_FW_UNMAP_BEGIN/CONTINUE`, `UNMAP_BEGIN`, `UNMAP_CONTINUE`, `SET_CTX_ID`, `REMOVE_CTX_ID`, `GET_INFO`, `REVOKE_SAPT_MULTIPAGE`.
44 типа нарушений UAT — фактически спецификация валидации: `ILLEGAL_TTE`, `ILLEGAL_MAP_FOUND_OCCUPIED_PTE`, `ILLEGAL_UNMAP_FOUND_FREE_PTE`, `ILLEGAL_TTBAT_MAPPING/LOCK`, `ILLEGAL_CACHED_MAPPING`, `ILLEGAL_CACHE_FLUSH`, `INVALID_MICROPPL_MAGIC_VALUE`, `SAPT_REVOKE_*`.

## 7. Направления поиска bypass (приоритеты)

1. **T8110_DART гонки** — в мониторе есть детекторы `VIOLATION_T8110_DART_RACE` и `_SID_RACE`: класс известен Apple, вопрос в полноте детекта. Диспетч-таблицы DART для XNU и SK разделены — искать асимметрию.
2. **TOCTOU маппинга**: `VIOLATION_POSSIBLE_PENDING_TLBI` / `PENDING_CACHE_FLUSH`, `ILLEGAL_CACHED_MAPPING` — окна между применением TTE и инвалидацией.
3. **UAT begin/continue-протокол** — двухфазные операции (MAP_BEGIN→MAP_CONTINUE) исторически дают TOCTOU между валидацией и коммитом.
4. **NVMe-эндпоинты** (10 штук) — DMA-устройство, арбитрируемое монитором (прецедент dmaFail).
5. **Гибернация**: `HIB_VERIFY/FINALIZE_HASH_NON_WIRED` + SK-домен — десериализация состояния при wake.
6. **MICROPPL** — внутренний слой, назначение неизвестно, выяснить.

## 8. Открытые задачи

- [ ] Найти PAC-подписанную таблицу хендлеров функций (сырых указателей нет → указатели подписаны).
- [ ] Восстановить нумерацию FUNCTIONID → адрес хендлера (RETYPE, UPDATE_DISJOINT_MULTIPAGE в первую очередь).
- [ ] Разобрать формат аргументов вызова из XNU (сторона kernelcache — `sptm_*` в `kernelcache.release.iPhone16,2`).
- [ ] Реверс TXM (txm.bin, 528 КБ): инициализация SPTM, что аттестуется при загрузке.
- [ ] Дифф SPTM 820.0.16 против SPTM из iOS 17.3.1 (эпоха Titan) и против следующих бет 27.x.
- [ ] Реверс переписанной функции SecureROM 0x4c00–0x5300 (дифф A0→B0/B1) — bootrom-трек.

## 9. Файлы

- `sptm_820_checkpoint.py` — IDAPython-скрипт: разметка таблицы типов/нарушений (121 имя), 78 функций с type-lookup, 226 сайтов-комментариев, именованные строки FUNCTIONID/DISPATCH_TABLE/UAT, ключевые функции. Загрузить sptm.bin в IDA → File → Script file.

## 10. Таблица политики типов 68×144 (декодирована 2026-08-08)

Адрес: `unk_FFFFFFF0270921E0` (__LATE_CONST), 68 записей по 144 байта. Машиночитаемый дамп: `sptm_type_policy_820.0.16.json`.

### Раскладка записи
| off | поле | смысл |
|---|---|---|
| +0x00 u8 | domain | 0=SPTM, 1=XNU, 2=TXM, 3=SK (совпадает с диапазонами типов) |
| +0x01 u8 | class | 0=data, 1=root_pt, 2=leaf_pt, 3=data retype-able, 4=tag_storage, 5=iommu, 6=io/untyped, 7=subpage_root_tables |
| +0x02 u16 | mask2 | 1 у PT и типизированных данных; потребитель не найден |
| +0x04 u16 | perm_class_mask | бит c разрешает mapping-class c: c=(PTE>>53)&3 \| (PTE>>4)&0xC (bit0=UXN, bit1=PXN, bit2=AP1/user, bit3=AP2/RO). Проверка в sptm_map_page |
| +0x06 u8 | category | 3=RW, 0xa=CODE, 0xb=RO/RX, 0xff=SPECIAL/IO; читается bootstrap (построение наборов типов) |
| +0x08 8b | cookie | рандом, ненулевой только у SPTM_UNTYPED |
| +0x28 u64 | ? | ~0 у user/IO/untyped/default; 0x84 CPUTRACE, 0x170 TXM_SEP_SECURE_CHANNEL, 0x120 SK_XNU_CONTENT. Гипотеза: битмаска FUNCTIONID/UAT |
| +0x32 u8 | exec_flag | 1 только у XNU_USER_EXEC, USER_DEBUG, USER_JIT, COMMPAGE_RX — особый путь пермов в map_page |
| +0x33 u8 | gate | почти везде 1 |
| +0x34 u8 | gate | 1 у XNU_DEFAULT и XNU_KERNEL_RESTRICTED |
| +0x50 u64×2 | adjacency | 128-бит маска разрешённых child-типов: root_pt→leaf_pt, leaf_pt→page types |

### Fast-path RETYPE (0xfffffff0270dbb28, sptm_types.c)
Условие: src class==3, FTE.flags&3==0, read_refcnt==0, mapping_refcnt==0.
Маски: lo=0x040002010000c000, hi=0x8 → разрешённые цели:
**14 XNU_USER_EXEC, 15 XNU_USER_DEBUG, 32 XNU_TAG_STORAGE, 41 XNU_SUBPAGE_USER_ROOT_TABLES, 58 TXM_BUCKET_8192, 67 SK_XNU_CONTENT**.

### Иерархия таблиц страниц
- SPTM_KERNEL_ROOT_TABLE(8) → {20 XNU_PAGE_TABLE, 22 PT_ROZONE}
- XNU_USER_ROOT_TABLE(18) → {20, 23 PT_COMMPAGE}; XNU_SHARED_ROOT_TABLE(19) → {21 PT_SHARED}
- XNU_STAGE2_ROOT_TABLE(33) → {34}; XNU_SUBPAGE_USER_ROOT_TABLES(41) → {20, 23}
- SPTM_PAGE_TABLE(9) → ВСЕ 68 типов (доверенный лиф SPTM)
- XNU_PAGE_TABLE(20) → {0,7,11,13,14,15,16,17,20,21,22,23,26,27,28,35,36,37,39,40,64,65,67} (включая сам себя — вложенные PT!)

### Аномалии / поверхность атаки
1. **7 SPTM_XNU_CODE_DBG_RW** (cat=CODE) и **13 XNU_RO_DBG_RW** (cat=RO/RX) — единственные типы с perm_class KRN-RW-XN у code/ro: writable код. Мапятся через XNU_PAGE_TABLE. Вне fast-path retype; защита, видимо, только debug-fuse/otp-валидацией вне таблицы — найти эти проверки.
2. **15 XNU_USER_DEBUG** — в fast-path retype targets прямо из XNU_DEFAULT. Perm-classes: USR-RW-XN + kernel-exec RO-классы. Проверить особый путь exec_flag (rec+0x32) в map_page: не даёт ли комбинация debug-типа и exec-пути writable+executable mapping.
3. **XNU_PAGE_TABLE → XNU_PAGE_TABLE** (self-adjacency): вложенные таблицы страниц — классическая поверхность для confused-deputy/двойного маппинга.
4. q28=~0 у всех user-типов и XNU_IO — если это маска разрешённых FUNCTIONID, user-типы доступны всем операциям; у CPUTRACE/SEP/SK_XNU_CONTENT — узкие маски (0x84, 0x170, 0x120). Подтвердить семантику через читателей +0x28.
5. Открытые поля: +0x02 mask2, пары u32 на +0x60/+0x68 (self-check code offsets?), cookie +0x08.

## 11. Exec-путь map_page и GXF guarded-call ABI (2026-08-08, вторая сессия)

### Exec-путь (exec_flag rec+0x32 == 1: типы 14/15/16/31)
Асм `0xfffffff0270e8a70` (внутри 0xfffffff0270e8860):
- флаги пермов вычисляются data-driven из perm-class c=(PTE>>53)&3|(PTE>>4)&0xC:
  - bit0 = `qword_FFFFFFF027019048[c-3]` — таблица из 13 qword = бит UXN каждого класса (c=3..15);
  - bit1 = 2 если c∈{3,5,7} (user-RW-XN), иначе PTE bit 58;
  - bit2 = 4 если c&7==5;
- v46 = источник refcount: 0 для COMMPAGE_RX(31); FTE+8/FTE+4 если root_pt cls==1;
- коммит PTE уходит через `sub_FFFFFFF0270AC618(state, v46, is_commpage_or_shared, flags, paddr, vaddr)`.
- `word_FFFFFFF027018E78` / `word_FFFFFFF027018E98` (16×u16) — guard-таблицы prev→new perm transition (биты по классам: 0x20@7,13; 0x8@11; 0x22a0@15 / 0x800@3, 0xa000@5, 0x8000@7,9,13).

### GXF guarded-call ABI (новая поверхность)
- `sub_0AC618` — не функция, а **трамплин**: x16=0x0002_0001_0000_0001 → b 0x9ac98. Семейство трамплинов 0xac618/0xac630/0xac648/0xac660/0xac678 = сервисы 1..5 (та же таблица sel=2).
- `0x9ac98` — вход guarded call: маскирует IRQ, переключается на выделенный стек из TPIDR_GL2+0xb20, сохраняет контекст в фреймы по глубине (слоты +0x80/+0x140/+0x200, счётчик +0x38, max 2), вызывает роутер 0xe0d70 с x0=3, x1=call_id. Возврат через 0x9ad6c. Фоллбэк — 0xdead+wfe.
- `0xe0d70` — роутер: читает GXF_STATUS_EL1 (s3_6_c15_c8_0), TPIDR_GL2 (s3_6_c15_c11_1); текущий домен = byte [TPIDR_GL2+0xb30] (<0x17=23 домена!); селектор x0<15 индексирует таблицу доменов `0xfffffff02701f0b0` (stride 0x1e0: {u8 domain, u64 handler, u8 flags, u64 mask}).
- call_id раскладка: bits48-55 = table_sel (1..2 → Table A, >2 → Table B 0xfffffff027091500), bits32-39 + low = индекс; entry = {u64 handler, u64 domain_mask}; domain_mask сдвигается на текущий домен — проверка прав вызывающего.
- **Table A = runtime**: [0xfffffff027091e60] инициализируется как base+0x10 региона **BootKC-ro** (имена регионов: SPTM-ro, SPTM-rm, SPTM-le, BootKC-ro, TXM-ro) — таблица живёт в boot kernelcache, наполняется при загрузке XNU-стороной.

### Что это значит для W^X-вопроса (USER_DEBUG)
Реальное решение о выставлении exec-бита принимает обработчик сервиса 1 внутри BootKC-ro (XNU-side), а не сам SPTM. Статически его найти нельзя — нужен kernelcache: искать таблицу guarded-call по формату {handler, domain_mask} рядом с началом образа либо регистрацию через символы _sptm_*.

### Следующие шаги
- [ ] Kernelcache: найти таблицу BootKC-ro+0x10 (формат {handler, domain_mask}, 60+ слотов на sel) → обработчик PTE-commit.
- [ ] Проверить трамплины сервисов 2..5 (0xac630..0xac678) — чему соответствуют.
- [ ] Доменовая таблица 0xfffffff02701f0b0 (23×0x1e0) — снять статические поля (handler ptrs могут быть 0 до инициализации, но domain ids/flags на месте).

## 12. Kernelcache 24A5390f: __DATA_SPTM, Table A — геометрия и рантайм-модель (2026-08-09)

### Артефакты
- Сырой kernelcache (IM4P, 22.5 МБ → 73 МБ Mach-O) сохранён постоянно: `/mnt/agents/work/kc.macho`.
- Маппинг VA единый: **VA = fileoff + 0xfffffff007004000** для всего файла (проверено по LC_FILESET_ENTRY ×265 и вложенному Mach-O ядра).
- Вложенный Mach-O ядра (fileoff 0x8000) содержит свой сегмент **`__DATA_SPTM`**: vm `0xfffffff00b27c000`, fileoff `0x4278000`, размер `0x4c000` (19 страниц).
- kernelcache.i64 (IDA 9 v910) вскрыт: zstd-поток 943 МБ = ID0 (B-tree) + ID1 (page-flags по 0x26fd4000) + блоки; ID1 покрывает [0xfffffff007004000, 0xfffffff00828c000) и [0xfffffff0082d8000, 0xfffffff00b4e8000).

### Геометрия BootKC-rs (доказана по sptm_init 0xfffffff0270b300c–0xb31e4)
- `BootKC-rs` ⊂ `BootKC-ro` (вложенность проверяется по именам диапазонов iBoot; строки «BootKC-rs»@0x2700665b, «BootKC-ro»@0x2700713b).
- SPTM регистрирует: [0x27090fb8]=base, [0x270914f8]=base+8, **[0x27091e60]=base+0x10 = Table A**, [0x27091010]=base+0x4000, [0x27090fe0]=base+size.
- Валидация структур: Table A занимает окно +0x10..+0x10+0x1e0·N; указатели таблицы должны лежать в [base+0x4000, base+size) — **первые 16 КБ региона = таблицы, далее ~0x48000 = код обработчиков GL2**.
- Идентификация: BootKC-rs = сегмент `__DATA_SPTM` ядра.

### Table A — уточнённый ABI (роутер 0xe0d70 + сервис 0xe0834)
- Запись = **24 байта** {handler@0, domain_mask@8, поле@0x10}; строка на table_sel = 0x1e0 (20 записей на sel); индекс = (call_id>>32 & 0xff) ≤ 0x13, вход = row(sel)·0x1e0 + idx·24.
- Роутер: sel∈{1,2} → Table A, sel>2 → Table B (0x27091500, __LATE_CONST, статическая, в файле почти вся нулевая — тоже runtime-наполняемая/частично fixup-encoded).
- Сервис-резолвер 0xe0834: домены 1,2 (XNU/TXM) резолвят через Table A, остальные через Table B; фоллбэк для domain==3 — статическая таблица 0x27021bd0 (с xpaci-очисткой указателя).
- Дескриптор домена (0x2701f0b0, 23×0x1e0 = 15 подзаписей ×0x20): {u8 target_domain@0, u64 nonzero@8, u8 new_domain@0x10, u64 caps_mask@0x18}; bit0 caps_mask = разбор call_id, bit1 = проверка аргументов-страниц (FTE type byte, cmp 0x2e).

### Table A в файле отсутствует (важный негативный результат)
- Весь `__DATA_SPTM` в файле — **нули**; единственная цепочка chained-fixups сегмента (последняя страница) — один нулевой терминатор. Chained-fixup формат сегментов ядра = 8 (target=low32 vmoff, next=bits56–61 в 8-байтовых слотах; декодирование откалибровано на __DATA_CONST).
- В XNU **нет ни одной ссылки** на `__DATA_SPTM`: ни adrp из кода (kTEXT/kTEXT_EXEC/kTEXT_BOOT_EXEC/кексты), ни указателей из __DATA_CONST/__DATA (оба сканированы по low32-fixup). В sptm.bin — 4 ссылки на указатель Table A (запись в init, чтение в роутере, ребейс адресного пространства), записей через неё нет.
- Вывод: таблицу и GL2-код наполняет **iBoot или ранний boot-код XNU по физическим адресам** (через разбор собственного Mach-O / device-tree), статически из имеющихся файлов содержимое Table A не восстановить. Для получения содержимого: дамп рантайм-памяти `__DATA_SPTM` (0x4c000) на живом устройстве либо анализ iBoot (лежит в IPSW).

### UXN-таблица и transition-guards (дамп, exec-путь)
- `qword_FFFFFFF027019048` (13 qword, классы 3..15), bit0=UXN: **1 для классов 3,5,7,11,13,15**; 0 для 4,6,8,9,10,12,14. (Классы с UXN=1 — user-маппинги.)
- `word_FFFFFFF027018E78` (16×u16): 0x20@{7,13}, 0x8@{11}, **0x22a0@{15}** — у USER_DEBUG самый широкий guard (биты по классам 5,7,9,13?); `word_FFFFFFF027018E98`: 0x800@{3}, 0xa000@{5}, 0x8000@{7,9,13}.

### Ответ на W^X-вопрос (уточнение)
Семантика пермов для 14/15/16/31 целиком определяется SPTM-стороной (perm-class → UXN-таблица → 3-битовые флаги + refcount + transition-guards); GL2-обработчик sel=2/id=1 — механический коммит этих флагов в PTE. Т.о. решающие для USER_DEBUG проверки — это guard-таблицы и refcount-гейты в sptm.bin, а не содержимое Table A. Содержимое Table A нужно только для подтверждения, что обработчик не добавляет своих проверок (например, по boot-args/fuse).

### Следующие шаги
- [ ] Проанализировать точное применение guard-таблиц 0x27018e78/0x27018e98 в retype/perm-change путях для типа 15 (какие переходы prev→new разрешены).
- [ ] Retype fast-path {14,15,32,41,58} + {67}: полный список проверок (refcounts FTE+4/+8/+0xa, gate rec+0x34) для XNU_DEFAULT→USER_DEBUG.
- [ ] Трамплины сервисов 2..5 (0xac630..0xac678): раскладка call_id, соответствие сервисам.
- [ ] (Опционально, с устройства) дамп `__DATA_SPTM` в рантайме → содержимое Table A и GL2-код обработчиков.
- [ ] iBoot из IPSW 24A5390f: поиск кода, наполняющего BootKC-rs (имена диапазонов «BootKC-rs» в iBoot).

---

## 13. W^X через USER_DEBUG (тип 15): полный ответ

Вопрос: может ли страница типа 15 (USER_DEBUG) быть одновременно writable и executable. Ответ получен целиком из `sptm.bin` — **нет, не может; тип 15 даёт только быстрый flip RW↔RX внутри замкнутого набора perm-классов**.

### 13.1 Раскладка perm-класса

Класс c вычисляется из PTE-шаблона: `c = ((PTE>>53)&3) | ((PTE>>4)&0xC)`, т.е.

- бит 0 = PTE.53 (UXN / XN[0])
- бит 1 = PTE.54 (PXN / XN[1])
- бит 2 = PTE.6 (AP[1], доступ EL0)
- бит 3 = PTE.7 (AP[2], 1 = read-only)

Запись = класс **writable**, если бит 3 = 0; **user-exec**, если бит 0 = 0; **kernel-exec**, если бит 1 = 0.

### 13.2 Маска разрешённых классов (policy-запись, 144 байта, +0x4 u16)

| Тип | Имя | Маска | Классы |
|---|---|---|---|
| 14 | USER_EXEC | 0xa800 | {11, 13, 15} |
| **15** | **USER_DEBUG** | **0xa880** | **{7, 11, 13, 15}** |
| 16 | USER_JIT | 0x8820 | {5, 11, 15} |
| 31 | COMMPAGE_RX | 0xa000 | {13, 15} |
| 33 | XNU_DEFAULT | 0x0000 | (проверка пропускается) |

Семантика классов типа 15:

- **7** (0111): UXN=1, PXN=1, AP=01 → **RW, NX на обоих уровнях** — единственный writable класс;
- **13** (1101): UXN=1, PXN=0, AP=11 → **user RO + kernel-exec** — единственный exec-класс;
- 11: kernel RO NX; 15: user RO NX.

Класса с write+exec одновременно в маске нет и быть не может: все разрешённые классы нечётные (бит UXN всегда 1), единственный writable (7) имеет оба XN-бита выставленными, единственный exec (13) — read-only.

### 13.3 Двойной контроль в пути коммита PTE (map_page, 0x270e8860+)

1. **Маска по типу** (0x270e8a28): `lsr w10, w10, w23; tbz → panic 0x270e915c` — запрошенный класс обязан входить в маску типа. Пропуск только для типа 33 (XNU_DEFAULT, доверенный).
2. **Guard-таблицы на переход** (CAS-цикл 0x270e8bbc–0x270e8c58): если старый PTE валиден (`old&3==3`) и класс меняется, то
   `разрешено ⇔ (1<<new_class) & (guardA[old_class] | guardB[old_class])`, иначе panic (0x270e8c5c, код 0x2c3).

Guard-таблицы (16×u16, @0x27018e78 / @0x27018e98) — индексируются **классом**, не типом (прежняя привязка «0x22a0@15 = USER_DEBUG» была ошибочной):

- guardA: [7]=0x0020 (7→5), [11]=0x0008 (11→3), [13]=0x0020 (13→5), **[15]=0x22a0 (15→{5,7,9,13})**
- guardB: [3]=0x0800 (3→11), [5]=0xa000 (5→{13,15}), [7]=0x8000 (**7→15**), [9]=0x8000 (9→15), [13]=0x8000 (**13→15**)

Граф переходов замкнут на нечётных классах {3,5,7,9,11,13,15}: чётные (user-exec) классы недостижимы ни как цель, ни как источник. Для типа 15 дополнительно срабатывает маска, поэтому реально доступны только {7,11,13,15}.

**Разрешённая цепочка для дебага:** `7 (RW NX) → 15 (RO NX) → 13 (user RO + kernel-exec)` и обратно `13 → 15 → 7`. Оба шага проходят guardB[7]&(1<<15) и guardA[15]&(1<<13). То есть Apple санкционирует именно flip «записал → сделал RO → сделал exec», но не одновременность.

3. Дополнительные шлюзы на этом пути: adjacency-маска 128 бит (rec+0x50) по FTE-типам, refcount'ы FTE (COMMPAGE→0; root-pt→u16@+4; иначе u8@+8), спец-ветки типов 8/0x13/0x21/0x1f.

### 13.4 Retype fast-path (0x270dbb28)

Быстрый retype в целевые типы {14, 15, 32, 41, 58} (lo-маска 0x040002010000c000) и {67} (hi-бит 3) требует одновременно:

- `u8[FTE+4] & 3 == 0` (нет активных счётчиков/флагов в младших битах);
- `u32[FTE+8] == 0` (refcount);
- `u32[FTE+0xc] == 0`;
- иначе — panic-ветки (0x270dbc00/0x270dbc50/0x270dbc94, assert-коды 0x10b9/0x10bf/0x10c5/0x10cd).

При успехе — атомарный `ldclrb 3` на [FTE+4] и retab. Т.е. перевод XNU_DEFAULT→USER_DEBUG возможен только для «чистой» страницы без ссылок — но сам по себе retype не даёт W+X, т.к. perm-класс всё равно ограничен маской 0xa880.

### 13.5 Вывод

- **USER_DEBUG ≠ W+X.** Структурно невозможно получить страницу, одновременно writable и executable (ни user, ни kernel): маска типа + замкнутость guard-графа + принудительная установка UXN для нечётных классов (таблица 0x27019048) образуют тройную защиту.
- Что реально даёт тип 15 — дешёвый **RW↔RX flip** страницы (модель «patch code → flip to exec» для отладчика), с panic при любой попытке выйти за пределы класс-графа.
- Для SPTM bypass под Dopamine-подобный сценарий это означает: путь через USER_DEBUG сам по себе исполняемых writable-страниц не даст; интерес представляют (а) retype в тип 15 чужих страниц через счётчики/gate-условия, (б) тип 33 (XNU_DEFAULT), для которого проверки маски и guard пропускаются, (в) доменная авторизация GXF-вызовов (кто может запросить map/retype с произвольным классом для доверенных типов).

### 13.6 Обновлённые next steps

- [x] Guard-таблицы 0x27018e78/0x27018e98: семантика (индекс=старый класс, бит=новый класс), граф переходов — **готово**.
- [x] Retype fast-path: условия (FTE+4&3==0, u32@+8==0, u32@+0xc==0) — **готово**.
- [ ] Тип 33 (XNU_DEFAULT): кто/когда может запросить (домены 1–2, caps), чем ограничен на практике.
- [ ] Доменные дескрипторы 0x2701f0b0: статические поля 23 доменов, caps-биты.
- [ ] Трамплины сервисов 2..5 (0xac630..0xac678): раскладка call_id.
- [ ] (С устройства/iBoot) содержимое Table A в рантайме.

---

## 14. Тип 33 (XNU_DEFAULT): доверенная зона без enforcement

### 14.1 Все спец-ветки типа 0x21 в sptm.bin (9 сайтов `cmp #0x21`)

**map_page (0x270e8860+)** — три снятия проверок:
- `0x270e8a20`: `b.eq` мимо теста маски perm-классов (`rec+4 >> class & 1`) — тип 33 единственный, кто может запросить **любой** из 16 классов, включая чётные (user-exec);
- `0x270e8c28`: `b.eq` мимо обеих guard-проверок в CAS-цикле коммита PTE — на валидном PTE класс меняется свободно, граф переходов не действует;
- `0x270e8b98`: флаг w14 (очистка PTE-бита 59) для 33 берётся из бита 7 старого PTE, а не из принадлежности класса к {3,5,7,9}.

При этом для типа 33 сохраняются: 128-бит adjacency-проверка (rec+0x50), refcount-гейты FTE. Exec-flag (rec+0x32) у типа 33 = 0 → exec-ветка (UXN-таблица, trampoline sel=2/id=1) для него не выполняется — PTE коммитится напрямую CAS-циклом.

**retype_assign (0x270dc38c)** — присвоение типа:
- Выбор маски по «kind» (x2&0xff): для new_type==0x21 допустимы kinds **{3,4,5}** (маска 0x38 @0x27019040), для всех прочих типов — {0,1} (маска 0x3 @0x27019038); kind>5 или несоответствие → panic (assert 0xbc); kind==1 дополнительно гейтится флагом 0x27094830 (в образе = 0, выключен);
- Для 0x21: требуется `(x2>>32) & 0xff01 == 1`, VMID аллоцируется из bitmap 0x27100100, в FTE+4 пишется VMID=1 (хост-XNU); нарушение → panic (assert 0x129 / 0xdcb);
- Типы 0x12/0x13 (VM-типы): VMID из глобального счётчика 0x27094834, bitmap 0x270fe100, нотификация через trampoline 0x270ac630.

**Прочее**:
- `0x270d11d4` (TLB-shootdown по VMID, 0x270d1164): для 33 требуется флаг 0x27093471 (=1 в образе); пишет `vttbr_el2 = VMID<<48`, `tlbi vmalls12e1is`;
- `0x270dbdf8` (release FTE): для 33 — симметричный VMID/TLB путь;
- `0x270eb22c` (lookup/walk, 0x270eb1f0): типы 0x12/0x21 — единственные, по которым walker идёт с w2=2;
- `0x270f057c` (0x270f0518, grab-ссылка на страницу по PA): **только** тип 33, иначе panic (assert 0x284). Т.е. SPTM-side page-table walker'ы оперируют исключительно XNU_DEFAULT-страницами.

### 14.2 Policy-запись типа 33

`rec+1=1` (класс «default»), perm_mask=0 (проверка всё равно пропускается), exec_flag=0, **adjacency = {34}**: страницы типа 33 живут только под FTE типа 34 (page-table pages XNU). У типов 14/15/16 adjacency пуст — это терминальные листья.

### 14.3 Кто может оперировать типом 33

У всего семейства retype/assign (0x270dbb28, 0x270dbdb8, 0x270dc38c) **ноль статических ссылок** — ни bl/b, ни adrp+add, ни сырого указателя в данных. Вызов возможен только через runtime-таблицы обработчиков (Table A/B в BootKC-rs, наполняемые при загрузке) → доменная авторизация GXF-диспетчера: домены 1–2 (XNU/TXM) → Table A. Флаги 0x27093471/0x27092c01 статически =1 (конфиг образа, не fuse).

### 14.4 Вывод для bypass

Тип 33 — это сам XNU: SPTM **сознательно не полициит** perm-классы XNU_DEFAULT-страниц (ни маски, ни guard-графа). Граница доверия SPTM проходит между XNU и user-памятью, а не внутри XNU. Следствия:

1. W^X-анализ типа 15 из секции 13 окончателен: обойти его «изнутри» типа 15 нельзя.
2. Реальная поверхность — **граница между типами**: retype 33→15 и 15→33 (refcount-гейты fast-path + kind-ограничения assign), и корректность VMID-логики (FTE+4=1 для 33; у VM-типов VMID аллоцируемый — классическая почва для confusion).
3. Проверить на XNU-стороне: может ли pmap легально построить user-exec PTE на странице типа 33 (для неё SPTM не препятствие) и потом дать на неё write — тогда W^X падает без всякого SPTM-эксплойта, чисто логикой. Это следующая цель анализа в kc.macho (поиск XNU-обёрток sptm_map/retype через ops-структуру [x+0x340]).
4. Открыто: содержимое Table A/B в рантайме (какие service id ведут в retype_assign и с какими domain_mask) — нужен дамп BootKC-rs с устройства или анализ iBoot.

### 14.5 Next steps

- [x] Тип 33: все спец-ветки, assign-ограничения, adjacency — **готово**.
- [ ] XNU-side: обёртки sptm_* через ops-struct [x+0x340]; какие типы/классы запрашивает pmap (есть ли user-exec на типе 33).
- [ ] VMID-логика: bitmap 0x270fe100/0x27100100, счётчик 0x27094834 — поверхность для type-confusion между 0x12/0x13/0x21.
- [ ] (С устройства/iBoot) Table A/B: service id → retype/assign, domain_mask.

---

## 15. XNU-сторона: архитектура вызовов SPTM (kc.macho 24A5390f)

### 15.1 GXF-вход/выход

- Всего **один** `mrs GXF_STATUS` во всём kernelcache: enter-stub `0xfffffff00a76f9e4` — сохраняет x0–x7, инкрементит per-thread nesting counter `[tpidr_el1+0x1c0]`, крутится на `mrs s3_6_c15_c8_0` пока не 0 (ожидание освобождения GXF), восстанавливает аргументы, `retab`. Leave-stub `0xfffffff00a76fa50` — декремент счётчика, при нуле и включённых прерываниях проверяет AST (`[cpu+0x1b8]->+0x4c & 4`).
- Прямых `b/bl` к стабам нет — вход через frame-диспетчер `0xfffffff00a76f650` (fake exception frame: `[x1+0x20]→elr_el1`, `[x1+0x28]→spsr_el1`, `[x1+0x30]→msr s3_6_c15_c8_3` = GXF enter; далее переход по номеру 0..3 на fleh-обработчики 0xa76e98c/0xa76ea4c/0xa76eb0c/0xa76e858).
- `s3_6_c15_c8_3` сохраняется/восстанавливается в контексте исключений (4 сайта `mrs` в save-area `sp+0x390`) — GXF-контекст per-thread.

### 15.2 Уровень «UAT libsptm» (врапперы в XNU)

- Generic call helper `0xfffffff00a7d8b1c`: берёт дескриптор op'а (x1), state (x2), out (x3); читает `mrs s3_4_c15_c11_7` (монотонный счётчик), отклоняет вызов если `[x3+0x10] > counter`; PAC-закрутка x0 контекстом (`movk #0xc8a2` при нарушении align по `[x1+0x12]`); вызов impl: `ldr x9,[x1+0x18]; blraa x9, x17` с дискриминатором **0x7203**. Второй helper `0xfffffff00a7d8bcc` — wfe-цикл ожидания слота (`ldxrh` на `[x1+0x12]`, ticket lock).
- Формат дескриптора op'а (0x28 байт, __DATA_CONST): `+0x00` u64 id/params (напр. 0x00100000_00055773), `+0x08` u64 limit (напр. 0x00200000_00cf4dd0), `+0x12` u16 flags/lock, `+0x18`/`+0x20` PAC'd fn-pointers (boot-time подпись, дискриминатор виден в сыром значении: 0x80107203_xxxxxxxx).
- Таблица дескрипторов: инсталлер `0xfffffff00affa0d8` заполняет дефолты для **50 (0x32) op'ов** по 0x28 байт начиная с `0xfffffff007ccaaa8` (слоты +0x18/+0x20, PACIA-дискриминаторы 0xd507/0x52f9).
- Врапперы (`sptm_*` API) — кластер `0xfffffff00a7d3600..0xa7d8xxx`: per-CPU op-слоты `0xfffffff00b2fc000 + (1|cpu<<2)<<6` (stride 0x40), ticket-lock `swplh`, submission через descriptor+helper. Строка-assert: «The underlying UAT libsptm function returned %d».

### 15.3 Инвентарь pmap↔SPTM (адреса в kc.macho)

- `sptm_get_frame_type` (`sptm_get_paddr_type`, файл «sptm.h») — 20 call-сайтов: vm_fault/vm-кластер 0xa866b18, 0xa879c44, 0xa8878cc, 0xa8c6360, 0xa8c89bc, 0xa8da9c4, 0xa8dc6b0, 0xa8dd90c; pmap-кластер 0xa91cfc8, 0xa91eb2c, 0xa91fc3c, 0xa91fdd8, 0xa920b30, 0xa9216a8, 0xa9242cc, 0xa92938c, 0xa930450, 0xa933234, 0xa942d24, 0xa942e14.
- `sptm_get_page_table_refcnt` — 0xa92138c, 0xa92413c, 0xa9247b0, 0xa924b4c, 0xa92a38c.
- `sptm_paddr_is_inflight` / `sptm_frame_is_last_mapping` — 0xa933154 / 0xa933234.
- ASID-init: 0xa921eb8 («insufficient number of ASIDs supplied by SPTM»); copy-window map: 0xa929bf0; commpage: 0xa92d6a0 (тип 31); `pmap_sptm_update_cache_attr_ops_collect` 0xa92ba80; bincompat/ops-handshake: чтение `[pmap+0x340]` (PAC data disc **0x1d9a**), версии 0x11ffff±, site 0xa943fcc.
- Имена типов (USER_DEBUG и т.п.) в XNU отсутствуют — типы передаются числом.

### 15.4 Промежуточный вывод

- XNU ходит в SPTM только через таблицу из ~50 дескрипторов; impl-указатели подписаны при загрузке (boot-PAC), статически не разрешимы — но сами дескрипторы статичны, и их id-поля позволят сматчить op'ы с Table A/B service id при следующем заходе.
- Вопрос «запрашивает ли pmap user-exec класс на типе 33» требует трасcировки от pmap_enter → выбор типа кадра → descriptor call. Это следующий шаг: поставить «якоря» на 20 call-сайтах get_frame_type (vm-кластер) и найти парный set/retype.
- Отдельно замечено: per-pmap SPTM state по `[pmap+0x340]` (PAC 0x1d9a) и флаги `[pmap+0x6b0]` (bit1 = SPTM-managed) — поверхность для состояния-рассинхрона pmap↔SPTM.

### 15.5 Next steps

- [ ] Идентифицировать retype/map дескрипторы: снять q0 (id) всех 50 слотов таблицы 0x7ccaaa8 и сопоставить с service id GXF (Table A/B).
- [ ] Найти set-сторону frame type: кто пишет тип перед map (vm-кластер 0xa866b18..0xa8c89bc).
- [ ] pmap_enter_options: конструирование PTE-шаблона (perm-класс) для user RX/RW — есть ли путь user-exec на XNU_DEFAULT.
- [ ] Рассинхрон pmap flags [pmap+0x6b0] bit1 vs фактический тип кадров в SPTM.

### 15.6 Уточнение: дескрипторы op'ов и runtime-инсталляция impl'ов

- Таблица на 0x7ccaaa8 (50×0x28) оказалась **не** sptm-опами (q0 указывают на строки mach port — «port set», «service port»…). Настоящие sptm-дескрипторы находятся по маркеру PAC-дискриминатора **0x7203** в qword `+0x18`: всего **11** в образе — кластер из 6 по 0x7ccb508 (stride 0x20) + одиночные 0x7ccbec8, 0x7cd3cf0, 0x7cd51e0, 0x7cd5220 (+ один в сегменте 0x8107xxx).
- Статические impl-указатели (target = low32 + BIAS) ведут на **дефолтные panic-стабы** (0xaffda68, 0xaffdb5c, 0xaffe094, 0xaffe850, 0xaffe75c, 0xaffe900, 0xa7d914c, 0xb007e80, 0xa7d8abc, 0xb008c34): разбирают arg-блок и зовут panic 0xaffa874 с файлом/строкой (0x704axxx, «sptm.h»). Т.е. в статике слоты impl = «op недоступен».
- Рабочие impl'ы появляются в рантайме: bincompat-handshake (0xa943fcc → 0xa94ac0c) **копирует версионированные блоки** (размеры 0x510/0x44/0x84/0x200/0x110, версия из [pmap+0x6b0] bit1) из SPTM-предоставленной памяти поверх слотов дескрипторов. Это и есть момент, где SPTM диктует XNU набор вызываемых сервисов.
- Следствие: статически виден весь **интерфейс** (11 дескрипторов, ABI helper'ов 0xa7d8b1c/0xa7d8bcc, PAC-схемы), но не привязка op→service id и не impl-код — они runtime (BootKC-rs / handshake), как и Table A. Граница статического анализа по цепочке XNU→GXF→SPTM на этом и замыкается; дальше — только дамп с устройства или iBoot.
- Что всё ещё доступно статически: логика выбора типа кадра на стороне pmap (vm-кластер call-сайтов get_frame_type) и конструирование PTE-шаблона в pmap_enter — это чистый XNU-код, без runtime-зависимостей.

---

## 16. XNU→SPTM: mailbox-механизм и путь pmap_enter (уточнение механики)

### 16.1 Коррекция к 15.6

Дескрипторы с PAC-дискриминатором 0x7203 — **не sptm-опы**, а vtable hw_lock'ов (q0lo = строки «lck_mtx_t (ilk)», «hw_lck_paddr_lock»…); helper 0xa7d8b1c — acquire-блокировки с timeout через `mrs s3_4_c15_c11_7`. Таблица 0x7ccaaa8 — mach port names. Оба ложных следа закрыты.

### 16.2 Реальный механизм вызова: shared-memory mailbox («UAT»)

- В pmap-кластере **нет ни одного** svc/hvc/smc; во всём kernelcache ровно один `mrs GXF_STATUS` (enter-stub). Прямых GXF-call инструкций в pmap-путях нет.
- Вызов идёт через **per-CPU слоты** `0xfffffff00b2fc000 + ((1|cpuid)<<2)<<6` (stride 0x40): XNU пишет arg-указатель в слот (`str x20,[x19]`), тип/статус (`strh`), дёргает doorbell атомарным `swplh`, затем ждёт completion-флаг `wfe`-циклом (helper 0xa7d8bcc: `ldxrh [x1+0x12]==1`). Обработчик на GL2-стороне поллит слоты. Строка assert: «The underlying UAT libsptm function returned %d».
- Enter/leave-stub'ы 0xa76f9e4/0xa76fa50 обслуживают **обратное** направление (SPTM→XNU через синтезированные исключения, fake frame + eret в GL2 на выходе) — 4 сайта `b` из fleh-путей.

### 16.3 Per-pmap SPTM-контекст

- Регистры **s3_6_c15_c1_5 / c15_c1_6**: per-pmap состояние, сохраняется в `[pmap+0x1a0]`/`[pmap+0x1d8]` при переключении pmap (fn 0xa924bec), восстанавливается с проверкой+флагом resync `[cpu_data+0x6b]=1`. В c15_c1_6 пишется канарейка `0x2020a53a302abae6` (после ttbr-switch, fn 0xa9371xx); c15_c1_5 тестируется маской 0x3000000000.
- Указатели pmap-структур под PAC data disc **0x250c** (autda/pacda при каждом разыменовании), per-pmap SPTM state — disc 0x1d9a ([pmap+0x340]), tte-pointers — disc 0x9e54 ([pmap+0x418]).

### 16.4 Путь map: где рождается PTE-шаблон

- **0xa91e634** — билдер PTE-шаблона (аргументы: pmap, PA, level, prot/flags w5, out x6): собирает x8 = PA | attrs, спец-кейсы для kernel_pmap (0xfffffff00b334c50), биты 0xc0/0x800... Это источник x28-шаблона, который SPTM потом классифицирует в map_page.
- Вызывается из двух мест: **0xa88091c** (pmap_enter-путь, vm-кластер 0xa88xxxx: флаги из `[mem_entry+0x2c]` + cache-attr биты) и **0xa91d12c** (map/copy-window путь).
- Сразу за билдером: вызов **0xa91fc3c** — map-wrapper (он же call-сайт sptm_get_frame_type): читает текущий тип кадра, готовит submission в mailbox.
- Флаги шаблона w5 = w23|w22: w22 = `[x4+0x2c]` (prot/тип от VM entry), w23 = cache-attr биты (bit17 → 0x20000). Т.е. **perm-класс страницы решается здесь, на XNU-стороне**, из VM entry — SPTM только валидирует против маски типа.

### 16.5 Что это даёт атакующему

- Точка, где VM entry prot → PTE-шаблон → SPTM map, полностью статична и видна: следующий шаг — трасса от pmap_enter_options до 0xa88091c (какие entry-флаги дают exec-шаблоны) и поиск retype-submission (смена типа кадра при donation в user-pmap).
- Mailbox = shared memory между EL1 и GL2 → классическая поверхность: гонки slot/doorbell/completion, подмена arg-указателя между проверкой и чтением GL2 (double-fetch по shared slots).
- PAC-дискриминаторы 0x250c/0x9e54/0x1d9a документируют, какие указатели pmap подписаны — полезно при подделке структур.

### 16.6 Next steps

- [ ] pmap_enter_options → 0xa88091c: условия, при которых w5 получает exec-биты (VM_PROT_EXECUTE, cs-флаги), и тип кадра, который при этом запрашивается.
- [ ] Найти submission retype (donation страницы в user-pmap): какой op-code слota, аргументы (paddr, new_type, kind?).
- [ ] Mailbox race-анализ: порядок записи arg vs doorbell, проверки GL2-стороны (свести с sptm.bin: FTE checks caps bit1).

---

## 17. Mailbox XNU→SPTM: полная модель и set-side типа кадра

### 17.0 Ложные следы (закрыты)

Два кандидата прошлого шага проверены и **отвергнуты**:

- **0xfffffff00a87bfc8** — не retype, а *vm_page attribute setter* с версионным op-block API: w1 = op ∈ {0xa,0xb,0xc,0xe,0xf}, w3 = размер блока в словах (3/2/3/4/5). Op 0xe: читает из блока [blk+0]→новое значение [page+0x68] (разрешены только {0,2,6}: маска 0x45, 3→2) и [blk+8]→бит 0x800 в [page+0x74]. Никакого mailbox — чисто метаданные vm_page.
- **0xfffffff00a88fe24** — обработчик **CoW-defeatured policy** (vm_map.c): строка паники `"CoW defeatured policy fail (responsible map=%p, guard_code=%u, fail_info=0x%llx)"`. w1=0xf у сайта 0xa893544 — это *guard_code*, не тип кадра. Гейтинг: глобал [0x7cd2324] ∈ {1,2,3} (режим политики), w2≠0, бит 15 в [vm_map+0x30]. Внутри — PAC-auth [proc+0x28] дискриминатором 0x5ef8 и проверка байта [proc+0xf9].

Вывод: тип кадра в XNU **не передаётся явной константой** в pmap-кластере — отсюда переход к mailbox-модели.

### 17.1 Все точки submission (исчерпывающе)

Во всём kTEXT_EXEC ровно **6 инструкций `swplh`** (doorbell), все на регионе **0xfffffff00b2fc000**:

| Адрес doorbell | Функция | op (desc+0x10) | Аргумент (desc+0) |
|---|---|---|---|
| 0xa7d35ac | (boot/init path) | 0 | x0+0xc |
| 0xa7d42ac | 0xa7d424c | 1 | x0+**6** |
| 0xa7d75c0 | 0xa7d7540 | 1 | x0+**6** (вариант) |
| 0xa7d37e0 | 0xa7d36fc | 2 | объект, desc = x0+**0xe** |
| 0xb007b7c | (late path) | 0 | x0+? |

Слот на CPU: `slot = 0xfffffff00b2fc000 + (((1 | cpu_halfword<<2) & 0xffff) << 6)`, cpu_halfword = [tpidr_el1+0x1b0]. Индекс CPU также читается из `s3_4_c15_c11_7` (per-CPU регистр TXM).

### 17.2 Протокол дескрипторов

- Дескрипторы по **0x40 байт**, пул внутри mailbox-региона: голова пула лежит по `base + (cpuidx & 0x3fff)*0x100`, аллокация — bump `+0x40` (0xa7d428c-0xa7d4294).
- Формат дескриптора: `+0x0`: **tagged pointer на shared-объект** (тег в младшем ниббле: наблюдены +6, +0xc, +0xe); `+0x10`: halfword **op ∈ {0,1,2}**.
- Doorbell: `swplh` пишет **индекс дескриптора** `(desc - base) >> 6` в первый halfword целевого объекта. Старый nonzero → slow path (объект занят).
- Completion: `wfe` по флагу; после — CAS по qword **[объект+8]**: биты 0..0x27 = счётчик, бит 0x1c (0x10000000) = in-flight, биты 0x30+ = владелец-CPU (сверяется с текущим слотом, 0xa7d3848-0xa7d3854).

### 17.3 Ключевой вывод: тип кадра = тег указателя

Аргумент дескриптора — указатель с **тегом в младшем ниббле**: +6 (op 1), +0xe (op 2). Набор тегов {1,2,6,9,0xe} в точности совпадает с nibble-типами vm_page+0x2a (см. 16.4) и с SPTM-типами кадров (14=USER_EXEC и т.д.). Т.е. **set-side типа кадра — это не параметр-константа, а тег на shared-указателе**: XNU донates страницу в SPTM, помечая её типом прямо в указателе; SPTM при обработке mailbox читает тег как new_type.

Подтверждение со стороны callers:
- 0xa7d36fc (op 2) вызывается из **аллокатора vm_page buckets** (0xa7d31a4, таблица бакетов 0xfffffff00b2d4870): вход нормализуется маской 0x7ffffffffff (47-битный PA/ptr), проверяется по границам сегментов памяти ([0x7d71348/350/368...]) — это **передача физической страницы в SPTM** с тегом 0xe.
- Callers op-1 (0xa804f40, 0xa805178, 0xa805bc4, 0xa805ca0) ходят по массивам 8-байтных записей с PA в битах 0..0x2d и busy-битом 0x2e (sbfx/ldseta/ldeorl) — **shadow-таблица кадров в shared-памяти**; op 1 = «пнуть SPTM», когда запись занята.

### 17.4 Следствия для эксплуатации

1. **Вся поверхность XNU→SPTM — 3 op-кода.** Retype/assign — это не отдельный сервис, а комбинация (op, тег). Тег формируется в момент donation; подделка тега = подделка типа — но указатель живёт в shared-регионе, недоступном для записи из EL0, и SPTM валидирует FTE на своей стороне (см. 12.x: caps bit1).
2. **TOCTOU-окно**: между `str [slot]` (arg) и `swplh` (doorbell) — а также между чтением тега SPTM'ом и записью FTE — есть гонка на shared-памяти. GL2-сторона должна сама копировать аргумент до обработки; проверить в sptm.bin, читает ли обработчик тег дважды (double-fetch тега = смена типа между проверкой и применением).
3. Тег 0xe при donation из аллокатора страниц — вероятный кандидат на «свежая страница уходит в XNU_DEFAULT/USER-пул»; перехват точки, где vm_page получает nibble 0xe (см. 16.4: 0xa88a5d4 с `ccmp w9,#0xe`), — следующий шаг к пониманию, кто заказывает USER_EXEC/USER_DEBUG.

### 17.5 Next steps

- [ ] Найти, кто зовёт 0xa7d31a4 (аллокатор) с последующим op-2 submit — построить цепочку vm_page_alloc → donation(tag) и найти все места, где тег ≠ 0xe.
- [ ] Сверить с GL2-стороной: обработчик mailbox в sptm.bin — чтение тега, double-fetch, валидация PA по FTE.
- [ ] Точка 0xa88a5d4 (translation nibble 1→0xe при записи +0x2a): кто вызывает и при каких cs-флагах.

---

## 18. GL2-сторона: обработчик retype найден, граф adjacency, ответ по double-fetch

### 18.1 FTE lookup (0x270e9368)

Адрес → FTE: границы tracked-региона [0x270921d0]/[0x270921d8], индекс = `(addr - base) >> 14` (гранула 16КБ), запись 16 байт в таблице [0x270914f0]: +0 = halfword claim/refcount (ldaddah/casah), +2 = **байт типа**, +3 = флаги, +6 = halfword состояния. Таблица FTE — GL2-private (не shared): XNU её не видит ⇒ double-fetch типа кадра со стороны XNU исключён.

### 18.2 Обработчик retype: 0x270e7e78 (трамплин 0x270e8800)

Аргументы (регистры из GXF-gate, полностью под контролем XNU):
- x0 = адрес страницы (16КБ-выравнивание, диапазон tracked-региона)
- x1 = **expected_type** (младший байт, < 0x44)
- x2 = **new_type** (младший байт, < 0x44)

Цепочка проверок (все — после casah-claim FTE, т.е. под эксклюзивной блокировкой):
1. expected_type == [FTE+2] (0x270e7fc8); special-case current==0xb (type 11): сверка с глобальным флагом [0x271021c0] бит 0x16
2. domain: байт [TPIDR_GL2+0xb00] (текущий домен вызывающего) == rec[current].domain (0x270e7f6c-0x270e7f80)
3. **adjacency**: 128-битный битмап rec[current]+0x40, бит new_type (0x270e7fd8-0x270e8000)
4. rec[new]+1 == 6 → skip, иначе требование на [FTE+0] ≠ 0 (0x270e8010-0x270e8020)
5. rec[new]+0x31 флаги vs [FTE+3] бит 1 (0x270e8024-0x270e8054)
6. blraa → **callback текущего типа** rec[current]+0x70 (дискриминатор 0xd507), w1 = new_type — вето «владельца» страницы

**Double-fetch: НЕТ.** Теги живут в регистрах (не перечитываются из shared-памяти), FTE — GL2-private и залочен casah до всех чтений [FTE+2]. TOCTOU-гипотеза по тегу закрыта.

### 18.3 Граф adjacency (дамп всей таблицы политик, 68 записей × 0x90 @ 0x270921e0)

```
type 0  (boot):    → ВСЕ типы 0..127
type 11 (0xb):     → {1,11,14,15,16,17,18,20,21,22,23,24,25,29,30,31,33,34,35,37,41,42,63,67}  ← ХАБ
types 14,15,16,17,24,25,32,33,34,37,41,67: → {11}  (возврат в хаб)
type 18: → {11,19};  type 19,20,21: → {11}
type 35: → {11,35} (self-loop!)
domain 2 (TXM): 42→{49,59,60}; 49→{11,50..58}; 59→{11,60}; 60→{11}
domain 3: 63→{11,64,65}; 64→{63,65}; 65→{63,64}
```

**Вывод:** тип 11 — центральное состояние «свободная/генерическая страница XNU». Все специальные типы (USER_EXEC=14, USER_DEBUG=15, 16, XNU_DEFAULT=33, page-tables=34, VM=0x12/0x13 по отдельному пути) рождаются **только из типа 11** и умирают только в тип 11. Mailbox-тег 0xe у op-2 submit'а (секция 17) — это new_type=14 (USER_EXEC) при donation.

### 18.4 Статические теги на стороне XNU

Из 6 submit-сайтов теги: 0xc (boot/init), 6 (×2, op 1), 0xe (op 2), — **тега 0xf (USER_DEBUG) в статике XNU НЕТ**. Легальный retype 11→15 существует в политике (домен 1 = XNU), но XNU его статически не заказывает ⇒ USER_DEBUG-страницы либо создаются по другому op-коду/пути (runtime-таблицы, BootKC-rs), либо тип 15 присваивается SPTM'ом самим (см. секцию 13: debug flip 7→15→13 — perm-класс, не frame type).

### 18.5 Callback вето

rec+0x70 у всех типов — PAC'd entry points (дискриминатор 0xd507) в один большой «исполнитель retype» (0x270d7454/0x270d78ac/0x270d7a28/0x270d7b28/0x270d7db8 — разные типы, один мега-функционал с общими frame-локалами). Callback **текущего** типа: для хаба 11 это 0x270d7b28 — финальное вето перед любым рождением специальной страницы. Его аудит — следующий шаг: именно там решается, разрешён ли 11→14/15 в данном контексте (вероятно, проверка VMID/владельца и глобальных флагов).

### 18.6 Обновлённая модель угроз

1. GL2-сторона retype чиста: регистры + залоченный FTE + adjacency + домен + callback. Поверхность сужается до двух точек:
   - **callback 0x270d7b28** (type 11 veto) — логические условия, которые можно удовлетворить «не тем» контекстом (confused deputy: попросить retype страницы, чей VMID/владелец подменён на XNU-стороне до submit'а);
   - **XNU-side gating**: кто и при каких cs-флагах заказывает тег 0xe — и можно ли заставить XNU заказать retype для страницы атакующего (секция 16, точка 0xa88a5d4).
2. Self-loop 35→35 и переход 18→19 — аномалии графа, стоит проверить семантику типов 35/18/19.

### 18.7 Next steps

- [ ] Аудит callback 0x270d7b28 (type-11 veto): какие глобалы/VMID проверяет.
- [ ] Типы 18→19 и 35→35: что это за переходы (семантика rec kind: 18 k=1, 19 k=1, 35 k=3).
- [ ] XNU: точка заказа тега 0xe (0xa88a5d4) — cs-флаги процесса, entitlement.

---

## 19. Механизм хуков retype: два blraa, маска сохранения, точка коммита

### 19.1 Полная последовательность retype (0x270e7e78, продолжение 18.2)

После гейтов (expected, домен, adjacency, флаги):

1. **blraa#1** (0x270e8068): `x8 = [rec(current)+0x70]`, дискриминатор **0xd507**, args (x0=FTE, w1=new_type) — **release-хук текущего типа** (вето «владельца»).
2. **Маска сохранения полей** (rec[current]+0x60, 12 бит → байты FTE +4..+0xf): бит n → байт сохраняется, иначе обнуляется (0x270e80c4-0x270e81e8). У типов 11/14/15/16/67 маска = 0xf00 (сохраняются байты +0xb..? — биты 9,10,11); у 32/33/34/41/58 = 0 (всё затирается).
3. **blraa#2** (0x270e8208): `x8 = [rec(new)+0x68]`, дискриминатор **0xe833**, args (x0=FTE, w1=new_type, x2=arg3, x3=&out_halfword) — **accept-хук нового типа** (может вернуть 2 байта в out: [sp+0x4e/0x4f] идут дальше в 0x270d0e18 вместе с rec[new]+6).
4. **Коммит** (0x270e820c): `strb w22, [FTE+2]` — тип сменён. Затем: вызов 0x270d0e18 (page-аккаунтинг, w3 = 3|(out_byte<<6)), и если rec[new]+4 halfword & 0x20202020 → итерация по списку [0x270fdb80] (region list) с ldapr-флагом [0x271021c0] бит 8.

### 19.2 Дискриминаторы — классы vtable-вызовов

- **0xd507**: 4 сайта (0x270db668, 0x270db86c — внутри retype fast-path; 0x270e8064, 0x270e8338). Сайт 0x270db668 зовёт `[[obj+8]+0x28]` — т.е. d507 = общий дискриминатор «ops-структура +0x28». Release-хуки — частный случай.
- **0xe833**: единственный сайт (0x270e8204) — accept-хук уникален для retype.
- **0xc231**: 9+ сайтов; таблица из 4 указателей @ 0x2701ee48-0x2701ee60; цель 0x270de1b4 = **autibsp** (подтверждённая точка входа blraa) — эталон декодирования.

### 19.3 Декодирование PAC-указателей SPTM (установлено)

Формат: `0x80??_disc_oooooooo`, где `oooooooo` = **VA − 0xfffffff027000000** (не low32 VA!). Подтверждено: c231-указатель 0x000de1b4 → autibsp @ 0x270de1b4 ✓. Байт [55:48] (0x00/0x10/0xb0) — часть PAC, на цель не влияет.

### 19.4 Открытая проблема: противоречие x26 в accept/release-хуках

Статические значения хуков (decode-A):
- release(11)=0x270d7b28, release(14/15/16)=0x270d7a28, release(33)=0x270d7db8, release(34)=0x270d78ac, release(41)=0x270d7454, release(58)=0x270d87b8, release(67)=0x270d7198
- accept(11)=0x270d7d54, accept(14/15/16/58)=0x270d87c0, accept(33)=0x270d838c, accept(34)=0x270d79b8, accept(41)=0x270d74c8, accept(32)=0x270ee3a8
- «дефолт» (≈45 типов): 0x270d87b8/0x270d87c0/0x270d87c8

**Противоречие:** в момент blraa#2 x26 = new_type&0xff (проверено: единственная запись x26 в обработчике — 0x270e7f88), но код по 0x270d87c0/0x270d7ba0 делает `stp [x26]` / `ldapr [x26]` — запись по адресу 0xe/0xf невозможна. Цели попадают внутрь boot-функции region-init (`sptm_compute_io_ranges`, строки 'SPTM-ro/rm/le/rx', 'BootKC-ro', 'TXM-ro', структура x26=0x270fc7e0).

**Рабочие гипотезы:**
1. Статический образ = **boot-снимок**: хуки в __LATE_CONST действительны только для boot-фазы (retype boot-страниц в контексте region-init), а runtime-значения переписываются до блокировки LATE_CONST. Проверка: дамп [0x270921e0 + 14*0x90 + 0x68] на живом устройстве после загрузки.
2. Таблица прыжков этих «дефолтов» — case-метки общего switch внутри мега-функции, куда blraa входит с контекстом, который мы не моделируем (маловероятно из-за x26).

Прямая запись в rec+0x68/+0x70 по статике не найдена (скан str по офсетам 0x68/0x70 — только чужие структуры), что косвенно поддерживает гипотезу 2 или запись через вычисляемый базис.

### 19.5 Что это значит для эксплуатации

Между release-хуком и коммитом **нет других проверок типа**, кроме accept-хука. Весь гейт 11→14 сводится к:
- домен == 1 (XNU ✓ по построению);
- adjacency (11→14 ✓ разрешено);
- expected_type == 11 (контролируется XNU-стороной ✓);
- rec-флаги (статика ✓);
- содержимое release(11) и accept(14) — **неразрешённая пока логика** (см. 19.4).

Если release(11)/accept(14) — дефолтные no-op (или почти), то единственный реальный барьер для XNU — **его собственное решение** заказать retype (cs-флаги, секция 16.4), а SPTM лишь исполняет. Тогда направление атаки смещается обратно в XNU: заставить pmap-код заказать donation с тегом 0xe/0xf для страницы атакующего.

### 19.6 Next steps

- [ ] Разрешить хуки: runtime-дамп rec-таблицы (jailbreak/отладчик) или эмуляция blraa (QEMU PAC-off).
- [ ] Проверить гипотезу 19.4.1: найти код, переписывающий rec+0x68/+0x70 (поиск по базису 0x270921e0 в __TEXT_EXEC, не по офсетам).
- [ ] XNU: точка заказа тега 0xe — 0xa88a5d4 (nibble-translation 1→0xe на vm_page+0x2a): кто вызывает, cs-гейты.

---

## 20. XNU-сторона заказа типа: полная цепочка USER_EXEC и недостижимость USER_DEBUG

### 20.1 Кто пишет nibble 0xe (frame type 14) в vm_page+0x2a

Полный скан strb по +0x2a (67 сайтов): nibble **0xe** пишут ровно 3 места:
- **0xa8c9ff4** и **0xa8ca4f8** — внутри **pmap_enter (0xa8c89bc)** (8 аргументов: pmap, pa, template...; итерация страниц шагом 0x4000);
- **0xa8cb9e0** — в **0xa8cb8a0** (batch-обход vm_objects, вызов из 0xaf226b0; гейты: [obj+0x18] & 0x1650 == 0x200, [obj+0x38] ∈ {0xb3855c0, 0xb3856c0} — сравнение с глобалами shared region).

Перед записью 0xe в pmap_enter: вызов grab-хелпера **0xa8d6270** (page, prot, 0): если nibble уже ∈ {1, 0xe} — только refcount++ ([page+0x28]); иначе полная инициализация. Т.е. **страница получает тип 14 в момент enter'а её в pmap с исполняемым шаблоном** — и donation в SPTM с тегом 0xe (секция 17) является отражением этого nibble.

Роутинг: страницы с [page+0x2c] бит 2 обходят пометку (0xa8c9f1c tbnz → 0xa8ca26c); prot/options приезжает в w7 → [sp+0x5c] (VM_PROT_EXECUTE = бит 2 prot-слова). Заказчики pmap_enter: 0xa87c138 (pmap-кластер), обёртка pmap_enter_options (0xa8c8868-0xa8c88c8, обнуляет options-блок 0x60 байт), 0xa966888 (специальная карта, w6=0x1440, w7=0x26, [pmap+0x68]=2).

### 20.2 USER_DEBUG (15): статически недостижим из XNU

Сводка доказательств:
1. **Mailbox-тега 0xf в XNU нет** (все 6 submit-сайтов: теги 0xc, 6, 0xe — секция 17.4).
2. **Adjacency 14→15 запрещён** (rec(14).adjacency = {11}), 11→15 разрешён, но XNU его не заказывает (нет тега 0xf).
3. **Fast-path retype** (0x270dbb28, цели {14,15,32,41,58,67}) — **ноль статических ссылок и ноль указателей** в образе: вызывается только через runtime GXF-таблицы (BootKC Table A/B), недоступные статически.
4. Nibble 0xf в vm_page+0x2a **не пишется нигде** в kTEXT_EXEC.

Вывод: тип 15 возникает только по решению самого SPTM/TXM через GXF-сервис (debug flip), инициируемому вне статики XNU — вероятно, TXM'ом при отладочных операциях (attach через TXM debug). Из userland/EL1 XNU путь отсутствует.

### 20.3 Сравнение rec(14) vs rec(15)

Почти идентичны: dom=1, kind=3, маски perm-классов **0xba80 (14)** vs **0xba88 (15)**:
- type 14: классы {7,9,11,12,13,15} — класс 12 (UXN=0,PXN=0,AP=RO) = user-exec RO; класс 7 = RW NX;
- type 15: + класс 3 (writable, NX оба уровня).
Оба имеют «дефолтный» ops-указатель 0x270d87c8 (d507) в rec+0x38 — второй ops-слот.

### 20.4 Итоговая модель угроз (обновление)

| Путь | Статус |
|---|---|
| W+X через USER_DEBUG perm-flip | ✗ закрыт (секция 13) |
| USER_DEBUG из XNU | ✗ недостижим статически (20.2) |
| Type 33 (XNU_DEFAULT) abuse | ✗ domain/kind-гейты (секция 14) |
| Mailbox double-fetch тега | ✗ закрыт (18.2) |
| Retype-гейты SPTM | ✗ adjacency+domain+ожидаемый тип+хуки (18-19) |
| **Открыто: хуки rec+0x68/0x70** (boot-снимок?) | ? секция 19.4 |
| **Открыто: XNU решает заказать 0xe для чужой страницы** (confused deputy в pmap_enter/vm_object batch) | ? 20.1 гейты [obj+0x18]&0x1650==0x200 |

### 20.5 Перспективные направления (по убыванию)

1. **0xa8cb8a0 (batch vm_object → nibble 0xe)**: гейты слабые (флаги объекта + совпадение с shared region глобалами). Если атакующий может создать/подделать vm_object с нужными флагами (через vm_map_enter с особым pager'ом?) — страница получит тип 14 без cs-проверок. Разобрать caller 0xaf226b0: кто ставит флаги 0x200 в [obj+0x18].
2. **Runtime-дамп rec-таблицы и Table A/B** (разрешить хуки 19.4 + найти, какой op-id зовёт fast-path 0x270dbb28).
3. **VMID-confusion** (типы 0x12/0x13, bitmap 0x270fe100): retype VM-страниц между VMID'ами.
