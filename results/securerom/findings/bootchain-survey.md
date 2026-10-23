# Обход загрузочной цепи t8130 (24A5390f): SecureROM, SEPROM, PMP, SmartIO

Сессия v226 (статический RE, Ghidra 12.1.4 headless — IDA на хосте мертва:
«Cannot continue without a valid license»). Метод: декомп целей → таргетный
аудит поверхностей, принимающих DFU/USB-ввод → **опровержение кандидатов
через raw-disasm** (правило §144: не доверять единствому инструменту).
Наблюдения отделены от выводов; всё непроверенное помечено явно.

## 1. Инвентарь загрузочной цепи (что вообще анализируемо)

| компонент | форма | статус |
|---|---|---|
| SecureROM t8130si (iBoot-8104.0.0.201.4) | raw dump, base 0 | проаудирован (см. §3) |
| AppleSEPROM 834.0.0.300.5 | raw dump | поверхность снята, глубокий аудит не делан |
| t8130pmp firmware | im4p → **plaintext Mach-O** (`cf fa ed fe`, энтропия ~6.4) | снята поверхность; **не аудирован** |
| SmartIOFirmware_ASCv6 (Realtek ASC) | im4p → **plaintext Mach-O** | снята поверхность; **не аудирован** |
| iBoot/iBEC/iBSS/LLB/SPTM/TXM/sep-fw/DeviceTree | шифрованы | закрыто (§167) |

Наблюдение (новое относительно §167): pmp и smartio **незашифрованы** —
загружаются из IPSW plaintext-im4p; ранее в журнале было только замечание
«не зашифрованы», без извлечения и анализа. Пересо-производимо:

```sh
python3 relay/fetch_ipsw_members.py <member>   # range-fetch из IPSW по CDN
# затем распаковать im4p payload (4K-заголовок im4p → Mach-O с base 0x1000000 для pmp)
```

Использованный IPSW: `iPhone16,2_27.0_24A5390f_Restore.ipsw`
(`updates.cdn-apple.com/2026SpringSeed/fullrestores/140-58754/...`).

## 2. Модель памяти SecureROM/SEProm (главный архитектурный факт)

Обе ROM используют **механическую пространственную безопасность** —
«толстые указатели» (ptr + base + bound) и stack-cookie:

- буфер-дескрипторы передаются тройками `(ptr, base, bound)`; каждая
  запись/чтение идёт через range-check с паникой при нарушении:
  SecureROM — `FUN_0000d498` (коды 0/0x15/7), `FUN_0000d4a8`,
  `FUN_0003c230`, `FUN_0003c3bc`; проверки `CARRY8(...)` перед `ptr+len`
  и sentinel'ов `ptr == 0xffffffffffffffff` с веткой на панику;
- stack-cookie: `local_28 = _DAT_fc038a88` в начале и сверка
  `if (_DAT_fc038a88 == local_28) return; FUN_00033070()` в конце —
  **277 функций** SecureROM (`grep -c fc038a88 securerom_decomp.c`);
  SEPROM — своя канарейка `_DAT_02200008` (FUN_0002aa80);
- «отравленные» инициализации `&DAT_aaaaaaaaaaaaaaaa` для
  неинициализированных выходных структур: 213 (SecureROM), 174 (SEProm).

Вывод: класс багов «переполнение буфера через DFU-парсер» в этих двух
ROM закрыт механически на examined-путях. Остаточные классы —
логические (verifier/crypto) и integer-overflow **до** вызова
checked-copy (проверяется длина, уже вычисленная с переполнением).

## 3. SecureROM: аудит DFU-принимающего пути

### 3.1 DER/TLV-ядро — чисто

`FUN_00038108` (tag+length, строгий режим) / `FUN_00038270` (variant),
вызываются из IMG4-цепочки через обёртки `FUN_00038100/00038678/00038718`:

- long-form tag: максимум до `uVar8 >> 0x39` перед сдвигом `<<7`
  (переполнение аккумулятора невозможно — проверка ДО сдвига),
  плюс финальная `uVar8 >> 0x36 != 0 → return 3`;
- long-form length: число октетов `1..8` (`uVar10 != 0 && uVar10 < 9`),
  запрет ведущих нулей (`*pbVar6 != 0` — канонический DER),
  `uVar12 <= uVar8` (октеты ≤ остатка), каждый чтение в цикле
  bounds-checked (underflow → `breakpoint 0x5515`);
- итог: `uVar11 <= uVar5` (длина ≤ остаток буфера) либо явный
  clamp-режим (`param_3 != 0`): `out_len = min(declared, remaining)`.
  **Ни один путь не даёт срез за пределы буфера.**

`FUN_00038510` (целочисленный ридер): ≤9 байт, reject отрицательных,
`uVar6 <= ~pcVar4` — проверка pointer-overflow в цикле.

### 3.2 IMG4-слой (4CC-константы, не строки)

- `FUN_00034d40`: внешний контейнер должен быть `'IMG4'` (0x494d4734),
  сверка `container_len == sum(children)` перед этим (брауз-контейнер);
- `FUN_00034f14`: извлекает `'IM4P'` (0x494d3450), поля со смещений
  0x50/0x58 и 0x60/0x68 (offset/length-пары) — каждая пара проходит
  `CARRY8`-проверку и nonzero, потом уходит в схемные ридеры;
- `FUN_00035038`: проверка `'PAYP'` (0x50415950);
- `FUN_000387e4`: **схемно-управляемый экстрактор** — ходит по детям TLV
  (`FUN_00038678`), матчит с таблицей `{out_offset, tag, flags}` (шаг 0x18),
  пишет (ptr,len)-вью в выходной буфер; проверка `out_offset + 0x10 ≤
  out_buf_size` перед записью; флаг 0b1000 пересчитывает длину как
  «до конца родителя».
- `FUN_00034c98`: ридер 4CC — len обязан быть ровно 4, иначе ошибка.

Все размеры выходных буферов в вызовах — константы вызывающих
(0x20/0x40/0x90/0xa0), совпадают с проверками схемы. Замечаний нет.

### 3.3 DFU/USB-транспорт (подтверждение §167)

`ta_dfu` (FUN_00029b04) → `rsm-usb` (FUN_00027f38) → copy-loop
(FUN_00028068): underflow/overflow/выравнивание — все через
`FUN_0000d498` с паникой. Скопировано с §167, перепроверено в этой сессии.

### 3.4 ОПРОВЕРЖЕНИЕ кандидата «DER-desync в FUN_00011c38»

Сессия-1 (см. checkpoint) сочла `FUN_00011c38` парсером DER с
возможным desync-багом (цикл пропуска ведущих нулей длины не ограничен
числом октетов). **Raw-disasm (`results/securerom/disasm_rom.py
0x11c38 0x11e60`) опровергает**: по адресу 0x11c38 — цикл проверки
дескрипторных структур (чтения по смещениям 0x80/0x42/0x208/0x400-0x410,
сравнения слов, ветки на panic-вектора 0x3c3bc/0xd4a8/0x161b0/0x161f0),
**без парсинга DER**. «Тег-константы» 0x1f/0x82/0x30 — артефакт
недеконвертированных `.word` в псевдокоде Ghidra (функция не
декомпилировалась: «Type propagation not settling», unreachable-блоки).
Класс ошибок: положительный результат от нерабочего детектора (§144).
Единственный DER-парсер на DFU-пути — 0x38108 (§3.1), он чист.

### 3.5 Что НЕ проверено в SecureROM

- crypto/verifier-логика (RSA/CMAC-верификация манифеста, таблицы
  сертификатов 0x53a20/0x53b10/0x53c30-0x53cc8): проверялся только
  memory-safety, не корректность accept/reject;
- форматировщик `FUN_00022a44` (3452 B, маркер `<<STRING BUG>>`);
  строки-форматы в ROM-константах (`"SIKA %02X"`, `"SRTG %s"`), вход —
  измеренные ROM-данные, риск низкий, но не смотрели;
- ws/tree-библиотека `FUN_000436e8+` (ordered-map с удвоением буфера
  `lVar3 << 1`): не проверено, могут ли attacker-счётчики дойти до
  точки удвоения.

## 4. SEPROM 834

Тот же класс, что SecureROM: poisoned-init 0xAAAA… (174), канарейка
`_DAT_02200008`, свой TLV-кластер `FUN_0002aa80/0002ab04` (1:1 тот же
integer-reader, что SecureROM 0x3848c/0x38510), img4-4CC-проверки
`FUN_0002b130(param, 0x494d3450/0x50415950)` (строки 38487/38538/38626).
Крипто-поверхность: строки `AKE_MACKey`, `AKE_ExtractorKey`,
`Counter_block_MAC_key` (0x35a30/0x35b0b) — AppleKeyExchange-путь.
Не аудирован: AKE (ключевой обмен с AP), сама verifier-логика.

## 5. PMP t8130pmp — ГЛАВНЫЙ остаточный кандидат

**Не использует** checked-pointer дисциплину: 0 совпадений
`aaaaaaaaaaaaaaaa`-инициализаций и 0 канареек-вида fc0xxxxx
(проверено grep'ом по декомпу). 825 функций, plaintext Mach-O.

- `FUN_01012568` (8252 B) — большой диспетчер: таблицы объектов
  `DAT_010969a0`/`DAT_010969b0`, vtable-вызовы `(**(code **)(*plVar5
  + 0x10))()`, счетчики из `*(uint *)(obj+4)/(obj+8)` — похоже на
  обход mailbox/ring-списков (поверхность AP→PMP команд);
- `FUN_0100be20` (5796 B), `FUN_0101caa4` (3920 B) — не смотрели;
- модель памяти обычные сырые указатели → классические баги
  (integer overflow в размерах, off-by-one в командах) достижимы.

Рекомендация: следующая итерация — карта mailbox-протокола PMP и
аудит `FUN_01012568`/`FUN_0100be20` на длины из командных структур.

## 6. SmartIO (Realtek ASCv6)

- `FUN_000182f0` (2984 B) — инициализация Realtek-контроллера
  (`_RTK_dev_asc_config`, PTR-таблицы 0x1ba08/0x1ba28/0x1d3f0):
  два `__platform_memmove(&stack0x00000028, &DAT_000441d4,
  (ulong)DAT_000441d0)` и аналогично 0x441c4/0x441c0 —
  **длина копии берётся из ROM-данных, не из атакующего**, но кладётся
  в конкретный стек-слот: стоит сверить кадр (в дампе это ROM-константы,
  поэтому не attacker-bug, если только дамп не модифицируем);
- `FUN_0000934c` (2160 B), `FUN_00000204` (1576 B) — не смотрели;
- дисциплины проверок, как у PMP, нет.

## 7. Артефакты и воспроизведение

Всё закоммичено в `results/securerom/bootchain-decomp/`:

- `securerom_decomp.c` (2023 функции, 1 fail), `seprom_decomp.c` (1187),
  `pmp_decomp.c` (825), `smartio_decomp.c` (629) — Ghidra headless dumps;
- `DumpDecomp.java` — Ghidra-скрипт (писать в файл, не в консоль);
- `slice_decomp.py <dump> <lo> <hi>` — срез функций по диапазону;
- `rom_strings.tsv`/`sep_strings.tsv` — строки с ADR-xrefs.

Пересоздание декомпов:

```sh
/opt/homebrew/Cellar/ghidra/12.1.4/libexec/support/analyzeHeadless \
    /tmp/ghidra-proj bootrom -import "<ROM-dump>" \
    -postScript results/securerom/bootchain-decomp/DumpDecomp.java <out.c>
```

Сырые дампы (не в git): `~/Developer/ios27-fuzz/dumps/SecureROM for
t8130si, iBoot-8104.0.0.201.4`, `.../AppleSEPROM-834.0.0.300.5`.
PMP/SmartIO macho восстановимы из IPSW через `relay/fetch_ipsw_members.py`
(im4p-распаковка: payload после 4K-заголовка im4p; base pmp=0x1000000,
smartio=0).

## 8. Итог

- подтверждено: SecureROM/SEProm на DFU-пути — memory-safe
  (механически); единственный DER-парсер чист; IMG4-слой чист;
- опровергнуто: кандидат DER-desync 0x11c38 (артефакт декомпилятора,
  raw-disasm);
- живые цели следующей итерации: **PMP mailbox-диспетчер
  `FUN_01012568`** (нет проверок, plaintext, команды от AP),
  SmartIO `FUN_000182f0`/`FUN_0000934c`, SEPROM AKE,
  SecureROM verifier-логика (crypto, не memory-safety).
