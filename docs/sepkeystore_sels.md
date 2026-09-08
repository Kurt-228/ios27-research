# AppleSEPKeyStore userclient: карта селекторов и фазз-план (iOS 27.0b4, A17 Pro)

Объект разбора: `results/kc27/com_apple_driver_AppleSEPKeyStore.macho` (carve из kernelcache_iphone16.macho,
сборка 2383.0.22.0.2, Jul 14 2026). Все VA — из этого kext'а. Формула декода указателей данных KC:
`VA = 0xfffffff007004000 + (raw & 0xFFFFFFFF)` (работает и для PAC'd слов в DATA_CONST: low32 хранит смещение).

Дизасм: `objdump -d results/kc27/com_apple_driver_AppleSEPKeyStore.macho` (пересоздавать при ребуте).

## 1. Главный диспетчер

`AppleKeyStoreUserClient::externalMethod` @ `__text+0x10498` (VA `0xfffffff009514d88`).

- x0 = this (userclient), x1 = selector, x2 = `IOExternalMethodArguments*`.
- Провайдер (AppleKeyStore) лежит в поле **+0xd8** клиента.
- Кэш entitlement'ов: битмаска u16 @ **+0xf0**, байт @ **+0xf2** (выставляются в start() через
  `IOTaskHasEntitlement`-подобный хелпер @ `0x95106b0`, биты докидываются в рантайме).
- Гейт открытия: selector 1 (close) гейт не проходит; все остальные доходят до `bl 0x9544c20`
  (isOpen); если клиент не открыт → **0xE00002D9** (kIOReturnDeviceNotAttached).
- Основной switch: `w28 = selector - 1; cmp w28, #0xa8` — jump table @ `0x951d33c`, **169 записей**,
  селекторы 1..169 (sparse). Необработанные → `0x951ae88` → **0xE00002C2** (kIOReturnBadArgument).
  selector 0 — отдельная ветка `cbz w25` до switch'а.

### Словарь возвращаемых кодов (по коду kext'а и err_iokit.sub)

| kr | Константа | Где выставляется |
|---|---|---|
| 0xE00002C2 | kIOReturnBadArgument | дефолт switch'а; mismatch scalarInput/OutputCount |
| 0xE00002C1 | **kIOReturnNotPrivileged** | entitlement-гейты (0x951b314 = w20-1) |
| 0xE00002E2 | **kIOReturnNotPermitted** | entitlement-гейты (0x951ac64 = w20+0x20) |
| 0xE00002F0 | not found ("data was not found") | от провайдера (напр. sel 17 с нулевым вводом) |
| 0xE00002D9 | device not attached | вызов до open (sel != 1) |
| 0xE00002C5 | exclusive access | sel 0 при уже открытом клиенте |
| 0 | success | 0x95176e4 и аналоги |

> Поправка к наблюдениям с девайса: 0x2c1 — это **NotPrivileged** (не «bad argument»),
> 0x2e2 — **NotPermitted**, 0x2f0 — «data not found». Значения из таблицы
> libsyscall/mach/err_iokit.sub: 0x2c1 = «privilege violation», 0x2e2 = «not permitted»,
> 0x2f0 = «data was not found».

### Бит-маска entitlement'ов (+0xf0 u16 / +0xf2 u8), построена в start()

| Бит | Entitlement (cstring VA) |
|---|---|
| 0x0001 (b0) | неизвестен (в start() не выставляется; вероятно сессия/keybagd, проверяется отдельно) |
| 0x0002 (b1) | com.apple.keystore.sik.access (0x7734e53) |
| 0x0004 (b2) | com.apple.keystore.class.wku (0x7734e71) |
| 0x0008 (b3) | com.apple.keystore.access-keychain-keys (0x7734e2b) |
| 0x0010 (b4) | com.apple.keystore.device (0x7734e8e) |
| 0x0020 (b5) | com.apple.keystore.device.uuid (0x77355f6) — рантайм-перепроверка |
| 0x0040 (b6) | com.apple.keystore.lockassertion (0x7734f08) |
| 0x0080 (b7) | com.apple.keystore.lockassertion.restore_from_backup (0x7734f57) |
| 0x0100 (b8) | com.apple.keystore.lockassertion.global_assertion (0x7734f8c) |
| 0x0200 (b9) | com.apple.keystore.lockunlock (0x7734fbe) |
| 0x0400 (b10) | com.apple.keystore.device.remote-session (0x7735567) — рантайм |
| 0x0800 (b11) | com.apple.keystore.se.secret_drop (0x7734fdc) |
| 0x1000 (b12) | com.apple.keystore.se.passcode_derivation (0x7734ffe) |
| 0x2000 (b13) | com.apple.keystore.lockassertion.time_machine (0x7734f29) |
| 0x4000 (b14) | com.apple.keystore.stash.access (0x7734ea8) |
| 0x8000 (b15) | com.apple.keystore.keybag.load (0x7734ec8) |
| +0xf2 b0 | com.apple.keystore.keybag.create (0x7734ee7) |

Прочие entitlement-строки в бинаре (проверяются точечно через 0x95106b0, не в битмаске):
com.apple.keystore.dsme_access (0x7734407), com.apple.keystore.obliterate-d-key (0x77355d2),
com.apple.keystore.devicebackup (0x77354fb), com.apple.keystore.stash.persist (0x7735615),
com.apple.keystore.auth-token (0x7735636), com.apple.keystore.fdr-access (0x7735654),
com.apple.keystore.device.verify (0x7735672), com.apple.keystore.escrow.create (0x7735386),
семейство com.apple.keystore.config.set.* (0x7735742–0x77362b6),
com.apple.applekeystore.selector.keybag_load (0x773543a),
com.apple.applekeystore.selector.keybag_create (0x7735028),
com.apple.keystore.class.d.allow (0x7733e97),
com.apple.keystore.allow.background-processing-assertions (0x7734234).

## 2. Разбор конкретных селекторов

Соглашение: scalarIn = `args->scalarInput` (массив u64), scalarInCnt = `args->scalarInputCount`,
scalarOutCnt = `args->scalarOutputCount`. Смещения по IOExternalMethodArguments: scalarInput +0x20,
scalarInputCount +0x28, scalarOutput +0x48, scalarOutputCount +0x50.

### sel 0 — Open (0x9514f84)
Валидации входа нет вообще. Виртуальный вызов провайдера (vtable+0x2c0).
Возврат: 0, либо 0xE00002C5 если уже открыт. Без энтитлементов → kr 0. ✅ совпадает с девайсом.

### sel 1 — Close (0x95170f4)
Валидации входа нет. Снимает ассершены (+0xf8), дерегистрирует нотификации, вызывает clientClose-подобный
хелпер. Всегда kr 0. ✅ совпадает.

### sel 16 — запрос состояния (0x9516b64)
Валидации входа нет. Если есть b4 (device) — дополнительно дергает хелпер 0x9508b5c(провайдер),
иначе сразу в return 0. kr 0 при любых входных данных. ✅ совпадает.

### sel 17 — query (0x9516f9c)
scalarInCnt ∈ {0,1}, scalarOutCnt == 0 (иначе 0x2c2). scalarInCnt==1 → читает scalar[0] как sub-handle.
Дальше вызов провайдера 0x95096a0; с нулевым вводом провайдер возвращает 0xE00002F0. ✅ совпадает.

### sel 5 — device/stash op (0x951748c)
Гейт: биты {b0, b4} маски (0x11), иначе **0x2c1 NotPrivileged**. ✅ (из sandbox → 0x2c1).
Далее: scalarInCnt ∈ {4,5}, scalarOutCnt == 0, scalar[0] >= 1, scalar[1] ∈ {0, -3, -2, ...} (тип операции).

### sel 6 — KeyBagCreateWithData (0x9516f54) ⚠️ WRITE-операция
Гейт: b15 (keybag.load) ИЛИ (b4>>4 в байт) ИЛИ +0xf2 b0 (keybag.create), иначе **0x2c1** +
лог «process … must have com.apple.keystore.keybag.load entitlement for kAppleKeyStoreKeyBagCreateWithData»
(0x7735417). ✅ (из sandbox → 0x2c1). **Фаззить нельзя** — это создание кейбага (ключевой материал).

### sel 8 — stash/data-protection query (0x9516a54)
Гейт: b14 (stash.access) ИЛИ b4 (device), иначе **0x2e2 NotPermitted**. ✅
scalarInCnt ∈ {0,1}; scalarInCnt==1: scalarOutCnt==1, scalar[0] = sub-handle; вызов провайдера 0x9509c3c.

### sel 19 — obliterate/wipe dkey (0x95176c0) ⚠️ WRITE-операция
Гейт: com.apple.keystore.obliterate-d-key (прямая проверка 0x95106b0), иначе **0x2e2**. ✅
При наличии — вызов провайдера 0x950cb98(provider, 1), kr 0. **Фаззить нельзя** (стирает effaceable dkey).

### sel 2, 12, 15, 33, 42, 44 — проверены на OOB
- sel 2: scalarInCnt ∈ {6,7}; cnt==7 → scalarOutCnt==1 и чтение scalar[6]. Забанчено.
- sel 12: чтение scalar[4] только при cnt>=4/5. Забанчено.
- sel 15: scalarInCnt ∈ {0xb,0xc}; cnt==0xc → scalarOutCnt==1, чтение scalar[0xb]. Забанчено.
- sel 33: гейт b4 (device, иначе 0x2c1); cnt ∈ {4,7}; cnt==7 → чтение scalar[5],scalar[6]. Забанчено.
- sel 42: cnt ∈ {1,4,5}; чтение scalar[4] только на ветках cnt>=4. Забанчено.
- sel 44: cnt==2 → чтение scalar[0], scalar[1]; рантайм-гейт remote-session (иначе обновление бита + 0x2c1).

### DSME-хендлер (0x950b608)
Отдельная функция (не из switch'а userclient'а): проверяет com.apple.keystore.dsme_access напрямую,
иначе **0xE00002C1**. Скорее всего обслуживает DSME-селекторы (5/6 у пользователя дают ровно 0x2c1 —
см. выше, их гейты тоже на 0x2c1, так что наблюдение согласуется с обеих сторон).

## 3. Ключевой результат по поверхности атаки

1. **structureInput вообще не используется.** В теле externalMethod нет ни одной загрузки из
   args+0x30 (structureInput), +0x38 (structureInputSize), +0x40 (structureInputDescriptor).
   Все 169 селекторов работают только со скалярами. Поэтому «любой struct 0x8..0x400 нулей → одинаковый kr»:
   payload структуры пользователя kext'ом игнорируется целиком, копирования из него нет.
2. **Все скалярные чтения забанчены** по scalarInputCount (проверка каждого из 6 подозрительных кейсов).
   OOB-read на scalarInput в externalMethod не найдено.
3. Единственные чтения из памяти клиента — scalarInput (копия в kernel-аллокации, сделанная IOKit
   до входа в kext) и scalarOutput (запись в kernel-аллокацию). Прямого memcpy с пользовательской длиной
   в этом kext'е нет.
4. Вывод: userclient — тонкий валидирующий слой. Вся тяжёлая обработка (keybag, SEP-RPC) — за
   виртуальными вызовами провайдера AppleKeyStore (+0xd8), т.е. поверхность со слепой копией данных
   живёт либо в провайдере этого же kext'а (вне разобранного switch'а), либо в AppleKeyStoreHelper/SEP.

## 4. Полная карта селекторов (jump table @ 0x951d33c, base 0x9514e74)

Дефолт (0xE00002C2): 25, 28, 29, 47, 59–62, 99, 102, 103, 134, 136, 158, 163–168.
Кратко (sel → адрес кейса; адреса VA 0xfffffff0095xxxxx, приведены низшие 6 hex):

| sel | case | sel | case | sel | case | sel | case |
|---|---|---|---|---|---|---|---|
|1|170f4|2|175e8|3|16fd4|4|1741c|
|5|1748c|6|16f54|7|17378|8|16a54|
|9|16bac|10|16bec|11|16ce4|12|16980|
|13|1762c|14|16b7c|15|1672c|16|16b64|
|17|16f9c|18|16e08|19|176c0|20|16598|
|21–22|15234|23|17018|24|17750|26,27,100,101|19ddc|
|30|17d0c|31|178b4|32|16a4c|33|174d0|
|34|169bc|35|16f94|36|1729c|37|16ee0|
|38|171b0|39|1647c|40|16e3c|41|15cbc|
|42|17174|43|16248|44|16a84|45,46,48,52,56,64,109,116,148,161|14e80|
|49|1852c|50|17e6c|51|15f9c|53|17ad8|
|54|1c73c|55|15930|57|16764|58|183b0|
|63|18610|65|175a0|66|17a48|67|1797c|
|68|16798|69,140|150a8|70|17260|71|187d8|
|72|183dc|73|18830|74–79,84,85|14fd4|80|15b94|
|81|15dd4|82|15d50|83|16344|86|184e0|
|87|17c44|88|16010|89|18074|90|17b5c|
|91|15b3c|92,157|15030|93|189c8|94|15700|
|95|1848c|96|18b8c|97|17bc8|98|163a0|
|104|18e18|105|186dc|106,156|152a0|107|160c8|
|108|16544|110|18184|111|15f50|112|1888c|
|113|16194|114|157f0|115|18f54|117|18c98|
|118|158ec|119|15c70|120|182f4|121|15460|
|122|1598c|123,154|1516c|124|17938|125|15a04|
|126|1561c|127|18d80|128|181cc|129|18718|
|130|15eb0|131|1568c|132|15878|133|18ad0|
|135|188cc|137|18a78|138|154d0|139|180e0|
|141|18418|142|153e0|143|15300|144|15768|
|145|16128|146|18d24|147|182c0|149|16654|
|150|18f0c|151|172fc|152|15660|153|17810|
|155|15a44|159|1553c|160|176ec|162|18fc4|
|169|166c4| | | | |

## 5. Безопасный фазз-конверт (только формы/размеры, не семантика ключей)

Жёсткие правила: никаких проходов через энтитлемент-гейты write-операций (sel 6 — KeyBagCreateWithData,
sel 19 — obliterate-d-key, всё с config.set.*/escrow/stash.persist); никаких значений scalar,
похожих на реальные хендлы/пасскоды — только 0/1/мусор; стоп при kr != из белого списка.

### Разрешённые цели (из sandbox, без энтитлементов)

| Цель | Что мутировать | Ожидаемый kr |
|---|---|---|
| sel 0 | scalarInCnt 0..16, любые scalar | всегда 0 (первый вызов) / 0x2c5 (повтор) |
| sel 1 | то же | всегда 0 |
| sel 16 | scalarInCnt 0..16, scalar значения 0/1/0xdeadbeef | всегда 0 |
| sel 17 | scalarInCnt 0 (→0x2f0) / 1 со scalar[0] ∈ {0,1,0x7fffffff,0xffffffff} | 0x2f0, реже 0x2c2; **не должно быть kr<0xE0000000 иных** |
| sel 2,5,8,12,15,33,42,44 и пр. | scalarInCnt 0..17 (перебор вокруг валидных значений), scalarOutCnt 0..2 | 0x2c2 на mismatch, 0x2c1/0x2e2 на гейтах; любой другой код — сигнал разбирать кейс |
| дефолт-селекторы (25,28,…) | любые scalar | всегда 0x2c2 |
| вызов до sel 0 | любой селектор != 1 | 0x2d9 |

### Критерии «стоп/эскалация»
- Любой kr ∉ {0, 0x2c1, 0x2c2, 0x2d9, 0x2e2, 0x2f0, 0x2c5} → разобрать кейс вручную.
- Panic/Watchdog/SKTErr в логах → зафиксировать вход.
- 0 из sandbox на селекторе с write-семантикой (6, 19, escrow, obliterate, config.set) → немедленно
  остановить прогон (нарушение ограничения владельца).

### Чего этим добиться нельзя (и почему)
- OOB на struct-пayload: structureInput kext'ом не читается (см. §3).
- OOB на scalarInput: всё забанчено по count (проверено по 6 худшим кейсам; остальные кейсы строятся
  по одному шаблону «cmp cnt → ветка»).
- Интересные срабатывания возможны только через логику провайдера за виртуальными вызовами —
  но туда попадают уже провалидированные значения, и write-пути закрыты entitlement'ами.

## 6. Оценка перспективности

- **Userclient-слой (разобран): низкая перспектива.** Валидация аккуратная, поверхность узкая
  (скаляры), гейты entitlement'ов корректно разделяют read/write. Из sandbox достижимы только
  open/close/state-query и отлупы 0x2c1/0x2e2/0x2c2/0x2f0/0x2d9 — ровно то, что видно на девайсе.
- **Провайдер (AppleKeyStore) в этом же kext'е: средняя, но вне текущего конверта.** Хендлеры за
  vtable-провайдера (0x9508b5c, 0x95096a0, 0x9509c3c, 0x950cb98, 0x950bb04 и семейство aks_*)
  принимают распарсенные u32/u64; там живут encode_list/aks_params (site.struct-имена kalloc).
  Разбор — отдельная задача; фаззить безопасно можно, только начиная с read-only селекторов
  (state/get), с жёстким запретом на keybag-load/create и passcode-семантику.
- **Для текущего ограничения (не ломать keybag):** конверт из §5 — максимум, что имеет смысл.
  Он даст полную карту «селектор → kr» для всех 169 селекторов (полезно само по себе), но вероятность
  memory-corruption находки на этом слое низкая по конструкции кода.
- Более перспективное направление (вне этого задания): io-surface/память-клиента пути
  (`clientMemoryForType`), которые в этом carve не разбирались.
