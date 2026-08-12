# Журнал, часть 3 (секции 38–39, 12.08.2026)

## 38. Девайс-фаза I: relay-инфраструктура и фазз v1–v6

### 38.1 Инфраструктура
- fuzz27.app (cancer9725.turquoise1323), clang-only build, devicectl install/launch --console, idevicesyslog + idevicecrashreport, результаты в results/.
- Подводные: пустой UILaunchStoryboardName = молчаливый краш; sandbox режет file-write в /private/var/tmp; symptomsd CPU 90s/180s + wake-limit 45000/300s (нужен throttle); запуск требует разблокированный экран (FBSOpenApplicationErrorDomain 7).

### 38.2 VCPDRM: мёртво
deny iokit-open-user-client VCPDRMUserClient + IOUC failed MACF. Entitlement-gated. Припаркован.

### 38.3 MIG-sweep: порты существуют, всё sandbox-gated
com.apple.sprr / jitbox / pmap / vmapple / kernel.sprr / private.sprr / vm.map / memory.control / jit — deny mach-lookup. Припаркован.

### 38.4 Scaler: путь проб
- open ОК из app; IOSurface через IOSurfaceCreate (sid 18+).
- v4: sel 2,3,12–31 при structureOutput ≥0x2000 → 0xe00002bf; sel 11 sync → фиксированный блоб 000e000400000013 0101… 0000004001010101 0000002000000004.
- v5: слепой лог (только нестандартные kr) + 30-мин spin → watchdog. Методологическая ошибка.
- v6: полная kr-матрица — все async-вызовы 0xe00002bf, включая sel 11; out не трогается; completions = 0. Вывод: 0x2bf = «принято, ответ позже» — legacy-async через sendAsyncResult64 на notification port, который мы не ставили.

### 38.5 Паники
results/panics (~700МБ) = forceReset btn_rst stackshots (AppleM68Buttons) + jetsam. Настоящих kernel panic нет.

---

## 39. Scaler static II: точная таблица методов (PROVED)

Fileset com.apple.driver.AppleM2ScalerCSCDriver извлечён из kc.macho (LC_FILESET_ENTRY, macho @0x5382c0).

### Формат указателей kernelcache
- vtable/fn: `0x8011|diversity16|fileoff32` (low32 = файловый offset!).
- adrp immhi = (insn>>5)&0x7ffff (НЕ >>3 — ранняя ошибка давала ложные «мёртвые строки»).

### Класс
- OSMetaClass("IOSurfaceAcceleratorClient") @0xfffffff008fc36f4, instance size 0x178, pacda disc 0xcda1.
- Строки: user_transform_surface, transformSurface_asynchronous, submit, IOAsynchronousScheduler, asynchronousUserClientCompletionCallback, sendAsyncResult64 failed, Client closed with requests not notified!, Mapped shared event %d md=%p dva=...

### Таблица методов @0xfffffff007f75848
14 fn-entries + 11 метод-дескрипторов (stride 6 qword: {fn, 0, ver=3, structInputSize, 0, 0}):

| m | in size | семантика |
|---|---|---|
| m0 | -1 | stub 0x2c7 |
| m1 | 0x1b0 | submit request descriptor; [in+8]: ≠0 → request-obj + action-API; =0 → scheduler path |
| m2/m3 | 0 | stubs 0x2c7 |
| m4 | 0x20 | пара значений |
| m5 | - | getter, пишет u32 в out |
| m6 | 0xfa8 | batch: [0]=count ≤ 0x3e8 + массив → 0x9006360 |
| m7 | 8 | u64 → mapper, type 3 (map shared event?) |
| m8 | 8 | u64 → DVA-арифметика |
| m9 | 0x10 | два u64, alloc 0x1b0 (create+bind) |
| m10 | 0x18 | [0]=enum < 4 |

Мёртвые проверки размеров: TransformSurfaceData / TransformEstimationData / IOSurfaceAcceleratorClientProperty.

### Нумерация
11 методов vs живые sel 2,3,11 → нижние селекторы заняты базовым IOUserClient2022; сдвиг выяснит v7.

### v7 (3a9d08b)
IOConnectSetNotificationPort + слушатель asyncResult64; kr-матрица sync sel 0..15 × {0,8,0x10,0x18,0x20,0x1b0,0xfa8}; async sel 0..13 × 11 размеров; семантический fuzz (count/enum/sid) с throttle. Ждём прогон.
