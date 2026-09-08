# IOSurfaceRootUserClient: точные форматы sel 7 / 9 / 27 (macOS 27 BootKC + живые тесты)

Дата: 2026-09-08. Хост: macOS 27.0 (26A5388g), arm64e. Источник: `com.apple.iokit.IOSurface`
402.5 из BootKC (`results/kc-extract/com_apple_iokit_IOSurface.macho`, carve через
`carve_fileset.py`, полный дизасм `results/kc-extract/iosurface_full_disasm.txt`).
Все форматы ниже **проверены живыми вызовами** на этом ядре
(`results/kc-extract/sel_format_test{5,7,8,9}.m`, собираются обычным clang'ом, никаких
entitlements не нужно). Код семейства общий с iOS — оффсеты структур и логика валидации
одинаковые; расхождения помечены.

## 0. Ключевой факт для фаззера: поверхности привязаны к соединению

`IOSurfaceRoot::find_surface(id, task, client)` (0xfffffe000afcea7c): если
`surface->fOwnerTask != task`, доступ только если `client->knows_surface(surface)`
(т.е. поверхность создана **через это же соединение** user client). Иначе — 0x2c2.

**Вывод:** создавать поверхность для фазза нужно тем же connection (sel 0), иначе любой
sel, дёргающий retainSurface, вернёт 0x2c2. Это же объясняет все прошлые отказы:
каркасы со sid поверхностей, созданных чужим клиентом (в т.ч. публичным
`IOSurfaceCreate`), дают 0x2c2 независимо от формата.

## 1. Таблица externalMethod

`IOSurfaceRootUserClient::externalMethod` (0xafda650) → `IOUserClient2022::
dispatchExternalMethod(sel, args, table, 0x3f /*63*/, this, 0)`.
Таблица `sMethodDescs` @ 0x88c1e38, 63 записи по 0x28 байт
(`IOExternalMethodDispatch2022`: func@0, checkScalarIn@8, checkStructIn@0xc,
checkScalarOut@0x10, checkStructOut@0x14, allowAsync@0x18, entitlement@0x20).
ВНИМАНИЕ: пойнтеры func в __DATA **XOR-обфусцированы** — записи 0–9 ключом
`0x7faf42ad0901c000`, записи 10–62 ключом `0x7faf42ad09004000` (проверено: все 120
декодируются ровно в символы). Это артефакт kernelcache, на iOS-девайсе таблица в
памяти уже деобфусцирована.

Выбор таблицы: флаг клиента @ +0x127 = entitlement
`com.apple.developer.gpu-restricted` (init @ 0xafd5aa0). Если entitlement есть —
используется `sMethodDescsRestricted` (0x88c2810): часть селекторов заменена на
`s_restricted` → **0xe00002e2**. Обычный процесс: обычная таблица. Ещё entitlement'ы
init'а: `com.apple.private.iosurfaceinfo` (+0x122),
`com.apple.private.IOSurface.protected-access` (+0x123).

Полная таблица (macOS, 63 селектора; нумерация для общего префикса совпадает с iOS —
подтверждено: sel7=client_mem, sel9=set_value, sel17/40=async, sel27=bulk; на iOS 60
селекторов — хвостовые 60–62 (transactions) и часть расширений отсутствуют, точную
дельту без iOS-кекста не подтвердить):

| sel | функция | sin | sinStruct | sout | soutStruct | async |
|---|---|---|---|---|---|---|
| 0 | s_create_surface | 1 | VAR | 0 | 3176 | |
| 1 | s_release_surface | 1 | 0 | 0 | 0 | |
| 2 | s_lock_surface | 0 | 12 | 0 | 3176 | |
| 3 | s_unlock_surface | 0 | 12 | 0 | 4 | |
| 4 | s_lookup_surface | 1 | 0 | 0 | 3176 | |
| 5 | s_set_ycbcrmatrix | 2 | 0 | 0 | 0 | |
| 6 | s_create_surface_fast_path | 0 | 32 | 0 | 3176 | |
| 7 | s_create_surface_client_mem | 2 | 0 | 0 | 3176 | |
| 8 | s_get_ycbcrmatrix | 1 | 0 | 1 | 0 | |
| 9 | s_set_value | 0 | VAR | 0 | 4 | |
| 10 | s_get_value | 0 | VAR | 0 | VAR | |
| 11 | s_remove_value | 0 | VAR | 0 | 4 | |
| 12 | s_bind_accel | 3 | 0 | 0 | 0 | |
| 13 | s_get_limits | 0 | 0 | 0 | 40 | |
| 14 | s_inc_sfc_use_count_for_category | 2 | 0 | 0 | 0 | |
| 15 | s_dec_sfc_use_count_for_category | 2 | 0 | 0 | 0 | |
| 16 | s_get_surface_use_count | 1 | 0 | 1 | 0 | |
| 17 | s_set_surface_notify | 0 | 24 | 0 | 0 | **да** |
| 18 | s_remove_surface_notify | 0 | 24 | 0 | 0 | |
| 19 | s_log | 0 | VAR | 0 | 0 | |
| 20 | s_set_purgeable | 2 | 0 | 1 | 0 | |
| 21 | s_set_ownership | 4 | 0 | 0 | 0 | |
| 22 | s_set_tiled | 2 | 0 | 0 | 0 | |
| 23 | s_is_tiled | 1 | 0 | 1 | 0 | |
| 24 | s_set_timestamp | 0 | VAR | 0 | 0 | |
| 25 | s_get_tile_format | 1 | 0 | 1 | 0 | |
| 26 | s_get_data_value | 0 | VAR | 0 | VAR | |
| 27 | s_set_bulk_attachments | 0 | **160** | 0 | 0 | |
| 28 | s_get_bulk_attachments | 1 | 0 | 0 | **160** | |
| 29 | s_prefetch_pages | 2 | 0 | 0 | 0 | |
| 30 | s_gather_iosurface_data | 1 | 0 | 0 | VAR | |
| 31 | s_set_cmp_tile_data_used_of_plane | 3 | 0 | 0 | 0 | |
| 32 | s_get_graphics_comm_page_address | 0 | 0 | 1 | 0 | |
| 33 | s_set_indexed_timestamp | 3 | 0 | 0 | 0 | |
| 34 | s_lookup_surface_from_port | 1 | 0 | 0 | 3176 | |
| 35 | s_create_port_from_surface | 2 | 0 | 1 | 0 | |
| 36 | s_create_shared_event | 1 | 0 | 2 | 0 | |
| 37 | s_signal_shared_event | 2 | 0 | 0 | 0 | |
| 38 | s_query_shared_event | 1 | 0 | 4 | 0 | |
| 39 | s_notify_shared_event | 5 | 0 | 0 | 0 | |
| 40 | s_add_shared_event_notify_port | 0 | 0 | 0 | 0 | **да** |
| 41 | s_remove_shared_event_notify_port | 1 | 0 | 0 | 0 | |
| 42 | s_signal_shared_event_operation | 4 | 0 | 0 | 0 | |
| 43 | s_set_gpu_policy_dict | 0 | VAR | 0 | 0 | |
| 44 | s_get_pid_gpu_policy_dict | 1 | 0 | 0 | VAR | |
| 45 | s_set_detach_mode_code | 4 | 0 | 0 | 0 | |
| 46 | s_set_image_origin_and_extents | 0 | 28 | 0 | 0 | |
| 47 | s_set_ownership_identity | 4 | 0 | 0 | 0 | |
| 48 | s_wait_shared_event | 3 | 0 | 0 | 0 | |
| 49 | s_create_memory_pool | 0 | VAR | 2 | 0 | |
| 50 | s_ensure_memory_pool_memory | 1 | VAR | 0 | 0 | |
| 51 | s_flush_memory_pool | 1 | VAR | 0 | 0 | |
| 52 | s_gather_memory_pool_data | 1 | 0 | 0 | VAR | |
| 53 | s_set_data_property | 2 | VAR | 0 | 0 | |
| 54 | s_get_data_property | 2 | 0 | 2 | VAR | |
| 55 | s_clear_data_properties | 1 | 0 | 0 | 0 | |
| 56 | s_invalidate_surface | 1 | 0 | 0 | 0 | |
| 57 | s_set_corevideo_bridged_keys | 0 | VAR | 0 | 0 | |
| 58 | s_remove_corevideo_bridged_values | 1 | 0 | 1 | 0 | |
| 59 | s_remove_bulk_attachments | 2 | 0 | 0 | 0 | |
| 60 | s_append_transaction | 4 | 0 | 0 | 0 | |
| 61 | s_query_transaction_list | 5 | 0 | 3 | 0 | |
| 62 | s_prune_transaction_list | 1 | 0 | 0 | 0 | |

Проверки dispatch'а (общие): scalar counts — точное равенство; structureIn/Out с
фикс. размером — **точное равенство** (159 вместо 160 → 0x2c2 на входе, до handler'а);
VAR = любой размер (но handler сам требит минимумы).

### Ответ sel 0 (IOSurfaceLockResult, 3176 байт)

`sid` — **u32 @ +0x18** (проверено: is_tiled с этим значением → kr 0). Остальное —
адреса/размеры буферов.

## 2. sel 27 — set_bulk_attachments (ИТОГОВЫЙ КАРКАС)

Вход: structureInput, **ровно 160 байт** (`IOSurfaceColorAndSpatialKeysArgs`):

```
+0x00  32B   поле bit0  (пространственные ключи, копируется только при mask bit0)
+0x20  16B   поле bit1
+0x30   8B   поле bit2  (u64)
+0x38   1B   поле bit3  (u8)
+0x39   1B   поле bit4
+0x3a   1B   поле bit5
+0x3b   1B   поле bit6
+0x3c   1B   поле bit7
+0x3d   1B   поле bit8
+0x3e   1B   поле bit9
+0x3f   1B   поле bit10
+0x40  24B   поле bit11 (16B @+0x40 + 8B @+0x50)
+0x58   4B   поле bit12 (u32)
+0x5c   8B   поле bit13 (u64)
+0x64   1B   поле bit14 (u8)
+0x65   1B   поле bit17 (u8)
+0x66   1B   поле bit18 (u8)
+0x68   4B   поле bit15a (u32)
+0x6c   4B   поле bit15b (u32, копируется вместе с bit15)
+0x70   4B   поле bit16 (u32)
+0x74   8B   поле bit19a (u64)
+0x7c   4B   поле bit19b (u32)
+0x80   2B   поле bit20 (u16)
+0x82   2B   поле bit21 (u16)
+0x84  12B   (unused)
+0x90   8B   mask (u64): бит N = "поле bitN присутствует"
+0x98   4B   surfaceID (u32)
```

Логика (`set_bulk_attachments` 0xafd8f88 → `IOSurface::setBulkAttachments`
0xafc18b4): `retainSurface(sid@0x98)` → нет такой (или чужая) → **0x2c2**. Иначе для
каждого бита mask 0..21 копирует соответствующее поле в `IOSurfaceClient` (+0x34..+0xb6)
и возвращает 0. Никакой другой валидации нет: **любые значения полей принимаются**,
mask может быть 0, размер строго 0xa0.

Проверено round-trip через sel 28 (get_bulk_attachments: scalarIn[0]=sid, structOut
160 байт; возвращает payload 0x84 байта теми же оффсетами + sid@0x98; `*outSize=0xa0`
всегда): записали паттерны с mask=0x7 и mask=bits0..39 → kr=0, чтение дало байт-в-байт
то, что послали.

Почему отклонялись старые каркасы: sid не @+4..+24, а **@+0x98**; плюс требование
ровно 160 байт; плюс привязка поверхности к соединению (см. §0).

## 3. sel 9 — set_value (ИТОГОВЫЙ КАРКАС)

Вход: structureInput, минимум **13 байт** (0xd), формат `IOSurfaceValueArgs`:

```
+0x00  u32  surfaceID
+0x04  u32  (unused)
+0x08  u32  (unused для set; для get/remove — flags, 0 = default)
+0x0c  ...  payload
```

Payload для set: **OSArray из ровно двух элементов, OSUnserialize-формат**
(IOCFSerialize с kIOCFSerializeToBinary *или* XML — оба принимаются, проверено):

```
element[0] = value   (любой объект: number/string/data/dict/array/...)
element[1] = key     (ОБЯЗАТЕЛЬНО OSString)
```

Внутри (`set_value` 0xafd7ba4): `OSUnserialize(payload, size-0xc, 0)` →
`safeMetaCast(result, OSArray::metaClass)` → `getObject(0)` = value,
`getObject(1)` → `safeMetaCast(..., OSString::metaClass)` = key →
`IOSurface::setValue(key, value, token)`. Отказ на любом шаге → 0x2c2.

**Ключевое отличие от предположения:** это НЕ dict `{id,key,value}` и НЕ dict
`{key:value}`. DICT отклоняется 0x2c2 (container-cast падает). Публичный
`IOSurfaceSetValue` на лету превращает {key:value} в `[value, key]`.

Выход: structureOutput, **ровно 4 байта** (иначе 0x2c2 ещё на dispatch) — u32 token
(инкрементальный счётчик значений поверхности; по наблюдениям 3, 5, ...).

sel 10 get_value: вход тот же заголовок, но payload @+0xc — **C-string ключа,
обязательно NUL-закрытый** (последний байт всего struct должен быть 0, иначе 0x2c2;
пустой ключ — get-all/ошибка). Выход: structureOutput ≥ 13 байт:

```
+0x00 u32 0 (observed)
+0x04 u32 token
+0x08 u32 0
+0x0c ...  value, сериализовано обратно в binary IOCFSerialize
```

Несуществующий ключ → **kr=0, outSize=0** (не ошибка!). После успешного get ядро
обновляет `max_property_size` поверхности.

sel 11 remove_value: как get, payload = ключ (пустая строка = remove all);
structureOut 4 байта token.

## 4. sel 7 — create_surface_client_mem

Вход: **scalarInput[0]=addr (u64), scalarInput[1]=size (u64)**, выход lock result
3176 байт (sid @+0x18).

Проверки (`create_surface_client_mem` 0xafd7738):
- addr == 0 || size == 0 → 0x2c2 (0xe00002bd+5);
- уже ≥ 0x4000 поверхностей у клиента → 0xe00002be;
- собирает `OSDictionary { "IOSurfaceAddress": addr, "IOSurfaceAllocSize": size }`
  (u64 OSNumber'ы) и зовёт `IOSurfaceRoot::create_surface_internal(owningTask, dict, 0)`.

Дальше (`IOSurface::parse_properties` 0xafb67d8 → `IOSurface::allocate` 0xafb9f28):
`IOSurfaceAddress` → поле surface @+0x150; `IOSurfaceMemoryDescriptor` строится через
`IOMemoryDescriptor::withAddressRange(addr, size, kIODirectionOutIn, owningTask)` —
**zero-copy, память шарится между клиентом и ядром** (не копия!). Перед этим
`validateRange(task, addr, size, &pageCount)` (0xafbdac4) идёт по страницам через
mach_vm query и требует, чтобы весь диапазон был замаплен и readable, иначе
**0xe00002c8** (kIOReturnVMError).

Наблюденное расхождение с iOS: на этом macOS `addr=1, size=0x1000` → 0x2c2
(validateRange бьёт по несмапленной странице 0); на iOS-девайсе тот же вызов,
по прошлым замерам, давал kr 0. Возможные причины: отличия vm-протокола/маппинга
нулевой страницы, либо iOS-путь обходит validateRange (ifdef). Для фазза на iOS это
означает: подсовывать **свою живую mmap'-нутую память** — тогда kr 0, и поверхность
будет алиасить клиентский буфер (реальные данные читаются/пишутся через surface).

UAF-потенциал: дескриптор держит ссылки на страницы клиента; при смерти клиента
task memory уходит — поверхность переживает клиента, доступ через неё после завершения
процесса = классика use-after-death клиентской памяти (на iOS критично, т.к. surface
могут дёргать другие процессы/GPU).

## 5. Оффсеты IOExternalMethodArguments (актуально для relay/фаззера)

```
+0x20 scalarInput        +0x28 scalarInputCount
+0x30 structureInput     +0x38 structureInputSize     +0x40 structureInputDescriptor
+0x48 scalarOutput       +0x50 scalarOutputCount
+0x58 structureOutput    +0x60 structureOutputSize    +0x68 structureOutputDescriptor
+0x70 structureOutputDescriptorSize
```

Если structureInputDescriptor != NULL (OOL-путь), handler'ы мапят его
(vtable+0x228 с w1=0x1000 — read-only map; +0x138 — getLength; +0x1f0 — release) и
работают с мапой; иначе берут structureInput напрямую.

## 6. Как воспроизвести

`results/kc-extract/sel_format_test9.m` — полный round-trip (sel 0 → 9 → 10 → 27 →
28 → 7) с печатью результатов; собирается:

```
clang -framework Foundation -framework IOKit -framework IOSurface \
    -o sel_format_test9 sel_format_test9.m && ./sel_format_test9
```

Ожидаемый вывод: sel0 kr=0, sel9(array) kr=0 token>0, sel10 kr=0 osz=28,
sel27 kr=0 (mask любой), sel28 kr=0 с возвратом тех же байт, sel7 kr=0 newsid>0,
sel7 addr=1 → 0x2c2. Проверено на macOS 27.0 26A5388g.
