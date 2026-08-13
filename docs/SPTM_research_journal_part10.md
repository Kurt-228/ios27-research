# Журнал, часть 10 (секции 59–62, 13.08.2026) — IOGPU: путь к submit

## 59. Metal-протокол изнутри (трейс на Mac)

- Интерпозер `~/.kimi-work/sptm-analysis/metaltrace/` (DYLD_INSERT + `__DATA,__interpose` пары {repl, orig} — НЕ строки имён; прямые вызовы оригиналов внутри врапперов безопасны).
- Metal на macOS 27 открывает **AGXAcceleratorG16G** (type 0x100005 = 5|(1<<16)); на iOS сервис **AGXAcceleratorG16P**, открывается type 1 и 0x100001 (оба → AGXDeviceUserClient; hi16 → uc+0x128 variant).
- macOS-трейс: sel 9 = create resource (stIn 104), sel 14 {0x4000, idx} = shmem (возвращает VA в out), sel 16 {0x100, 0x28}, sel 7 = регистрация с путём процесса, **submit = Trap4 sel 0 {1, 0x40, ptr1, ptr2}, doorbell = Trap1 sel 1 (инкремент)**.

## 60. iOS IOGPU таблица (восстановлена полностью)

- AGXDeviceUserClient наследует IOGPUDeviceUserClient, таблица в IOGPUFamily: A @ 0xfffffff00814d8c0 (56 sel), B (restricted, entitlement gpu-restricted) вырезает 21,27–34,36–54.
- Ключевые: **sel 6 = new_command_queue** (stIn == 0x410 обязательно, blob: version u32 @+0x400 < 5, флаги +0x404/405/408; out = {queueID, trace-tag}); **sel 8 = new_resource** (проверено: type 0x80, insz 0x68, outsz 0x58; id в out+0x24); **sel 25 = submit_command_buffers** (scIn 4 {queueID, ?, count, entrySize}; запись 0x40: +0 res id A, +4 res id B = command stream resource, +0x30 event id, +0x38 event value); sel 42/43/44/45 = io_command_queue/notification (WebContent-недоступны); трапы: **trap0 = submit одной записи** (copyin p2 ∈ [0x30,0x41]), trap9/10 = signal/wait shared event, trap1 = resource op (НЕ doorbell).
- clientMemoryForType на iOS-коннекте не поддержан; IOConnectMapMemory64 не работает.

## 61. Shmem в нашей задаче

- После sel6 в задаче появляются **две новые 0x4000-страницы** (region-diff метод) — читаемые/записываемые напрямую. Начальное содержимое: структуры со счётчиками (06e, 0x10, единицы; пары u32). Kernel в init их не трогает (маппинг — не от sel6; вероятно IOGPUClientSharedMachine механика).
- Getters: sel 4 отдаёт {token, 0x104390000, 0x104380000, 0x4000} — адреса device shmem в нашей задаче.

## 62. Текущий блокер

sel 25 с минимальной записью {ridA, ridB, 0...} + command stream в ресурсе B (header @+0xc) → 0x2c2 без kernel-лога. Гипотезы: (а) entrySize ≠ 0x40 (константа в AGXAccelerator+0x260 не вычитана); (б) sc[1] не ноль; (в) entry+0x18 (descriptor init) обязательно; (г) command stream нужен валидный заголовок сегмента (не просто cmd header).
Следующие шаги: (1) вычитать [AGXAccelerator+0x260] из AGXG16P (init путь accelerator'а); (2) посмотреть процессный лог с -m '' (полный syslog) в момент sel25 — возможно лог ниже Fault-уровня; (3) fuzz-матрица sc[1] × entrySize; (4) прочитать processCommandBufferList @ 0x9d879bc полностью (что валидируется ДО парсера стрима — туда ли мы вообще доходим).

### Долгая перспектива (submit path достигнут)
Командный стрим ресурса B — user-writable shmem, ядро парсит его «вживую» (assert-паники «shmemOffset value is corrupt» и т.п. — не безопасные отказы) → исторический класс double-fetch/OOB в IOGPUCommandDescriptor::prepare (0x9d948bc-0x9d94940 регион). Это и есть целевой примитив-кандидат.
