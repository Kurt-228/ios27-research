# Журнал, часть 11 (секции 63–65, 13.08.2026) — IOGPU submit: дошли до очереди

## 63. Submit-путь: точная семантика (iOS 27.0b4)

- **sel 6 new_command_queue**: stIn РОВНО 0x410 (`Invalid newCommandQueueArgsSize (1032) expected (1040)` — видно в kernel-логе). Блоб: +0x400 u32 version < 5 — единственное обязательное поле; +0x404/+0x405/+0x408 флаги (bit3 +0x408 требует entitlement agx.performance-spi). Реестр очередей **per-connection** (fDevice): очередь видна только на своём коннекте.
- **sel 25 submit_command_buffers**: scIn ровно 4 {queueID, unused, count, entrySize}; scOut ровно 1; structOutput НЕ нужен (stOut=0; стOut≠0 → 0x2c2). entrySize == 0x40 (AGXAccelerator+0x260, безусловная запись в start; mismatch → лог "bad structureInputSize (N != 64)"). Ошибки записей НЕ в kr — в scOut[0] (sticky код в queue+0x520: 4/6/8/9).
- **Текущая стена**: submit с валидными ridA/ridB → **0xe00002bc (NoSpace)** без лога. Гипотеза: очередь не инициализирована на shmem-уровне — free-лист дескрипторов пуст. Ядро при sel6 не мапит страницы само (в AGX-коде нет createMappingInTask), но в задаче появляются 2 новые 0x4000-страницы (client-shared механика, IOGPUClientSharedMachine).
- **trap0 = submit одной записи** (copyin 0x30..0x41 байт — минимальный путь); trap1 = resource op (НЕ doorbell); kick прошивки делает сам submit (queue vfunc+0xb8). Отдельного doorbell-сисколла нет.

## 64. Эталон с Mac (Metal newCommandQueue, region-dump)

Регионы, появляющиеся при создании очереди (macOS 27, M4):
- 0x4000-страница: `2c 2c 00 00 ...` (notification-queue-подобная).
- 0x20000-регион: заголовок `19 00 6d 0b 00 00 00 20 00 58 00 00 10 00 00 00` (shmem pool header).
- 0x10000-регионы с записями `40 00 00 00` шагом 0x40 (descriptor area, entry size 0x40 ✓) и паттернами `ff ff fe ff ab aa` (stamp/fence таблицы).
Т.е. Metal (юзерспейс) пишет заголовки очереди сам. Для прогресса submit на iOS нужно реплицировать инициализацию этих структур (free-list и т.п.).

## 65. Следующие шаги (готовый план)

1. Вычитать из AGXG16P/IOGPUFamily, какие поля queue shmem читает kernel при submit (free count / write index), т.е. какое содержимое страниц снимает 0x2bc: искать чтения по смещениям shmem-страниц в processCommandBufferList / IOGPUCommandDescriptor::prepare (0x9d8a5e8+, 0x9d948xx). Кандидат: поле free-list в первой странице.
2. После первого kr=0 на submit — фаззить командный стрим ресурса B (user-writable, парсится ядром вживую, assert-паники вместо отказов): типы команд (bits 29:0 заголовка @+0xc), shmemOffset-поля, двойные чтения. Мутатор-поток для double-fetch.
3. Альтернатива без shmem-инициализации: trap0 (submit одной записи через copyin) может обходить free-list проверку — проверить trap0(conn, 0, qid, 0x40, entryPtr, outPtr) напрямую.

### Статус большой задачи
- Write-примитив пока не получен; самый обоснованный кандидат — парсер командного стрима IOGPU (user-writable shmem, читается ядром live, assert-паники). Для него нужен валидный submit, который на 1 шаг ближе: известен точный протокол вплоть до shmem-инициализации очереди.
- IOGPU.framework API (IOGPUDeviceCreate/CommandQueueCreate) на iOS не заводится из sandbox (DeviceCreate → NULL; IOServiceOpen type 5 не принят); прямой путь через селекторы работает.

### Дополнение (v50/v51)
- sel25 соглашения выверены: scIn=4, scOut=1, stOut=0, stIn=count*0x40 (иначе лог "bad structureInputSize").
- **trap0: запись ПРИНЯТА** (out по p4 записан нулём = per-entry успех), но kr 0x2bc — похоже, фейлит kick в firmware (queue vfunc+0xb8) из-за неинициализированного submission ring. Блокер локализован точно: ring-инициализация queue shmem (что Metal пишет в страницы после создания очереди).
- Следующий шаг: снять полный дамп двух страниц очереди на Mac (есть в metaltrace/qdump) + найти в AGX-коде чтения полей ring (free-list/write index) и записать их руками; либо сравнить содержимое страниц до/после первого настоящего Metal-submit на Mac.
