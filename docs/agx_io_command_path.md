# AGX IO-command path (sel41–sel48): форматы, цепочки ошибок и путь исполнения

Разбор iOS `IOGPUFamily` из `results/kc27/com_apple_iokit_IOGPUFamily.macho` (T8122, стрипнутый).
Символьный референс — macOS `results/kc-extract/com_apple_iokit_IOGPUFamily.macho`
(адреса `0xfffffe000ab9…`), структурно идентичен. Девайс/фuzzer не трогали.

Отвечает на вопросы:
1. Форматы sel44/sel45/sel46 и откуда берётся `0xe00002c2`.
2. Чем исполняются команды: отдельный CPU-side путь (vnode read + shared events),
   **не** execution layer trap0/AGX.
3. Почему `{iocq=1}` отклонён и какие id валидны.
4. Минимальный валидный вызов sel45.

---

## 0. Поправка к `docs/agx_queue_execution.md`

§2 того документа неверно называл селекторы 41–43. Правильная карта (подтверждена
дизассемблером iOS-таблицы методов и строками `__FUNCTION__`):

| sel | iOS адрес | метод | sin/strIn/sout/strOut |
|-----|-----------|-------|------------------------|
| 41  | 0x9d5a2a0 | **стаб-destroy**: `ldr x8,[x0,#0x120]; ldr w1,[scalarInput[0]]; bl 0x9d56e70` | 1/0/0/0 |
| 42  | 0x9d5a2c8 | **s_create_io_command_queue** | 2/0/2/16 |
| 43  | 0x9d5a4a8 | **s_destroy_io_command_queue** | 1/0/0/0 |
| 44  | 0x9d5a528 | **s_set_io_notification_queue** | 2/0/0/0 |
| 45  | 0x9d5a674 | **s_submit_io_commands** | 1/VAR/0/0 |
| 46  | 0x9d5a8dc | **s_create_io_command_buffer** | 1/0/2/0 |
| 47  | 0x9d5a994 | **s_destroy_io_command_buffer** | 2/0/0/0 |
| 48  | 0x9d56a4c | s_perform_io (по аналогии с mac 0xab8a2e8; проверить на девайсе) | 2/0/0/0 |

sel41 — **не create**. `0x9d56e70` — «destroy object by id в namespace»
(lock `[ns+0x20]`, lookup, release, вернуть found-индикатор). Любой вызов sel41
возвращает kr 0 и **ничего не создаёт**.

---

## 1. Форматы селекторов

### sel42 create_io_command_queue (iOS 0x9d5a2c8)
- **scalarInput: два u64** `{type = scalar[0], priority = scalar[1]}`
  (`ldr x8,[args,#0x20]; ldp x22,x23,[x8]`). structureInput не используется.
- Вирт-вызов `[uc+0xf8]+0x6b0` (iOS) создаёт `IOGPUIOCommandQueue`;
  `IOGPUIOCommandQueue::init(device, type, priority)` (mac 0xab9ed54) имеет гейты:
  - `type >= 2` → лог «Invalid IO Queue Type (%u)» (строка 0x7a8fd6d mac);
  - `priority > 2` → лог «Invalid IO Queue Priority (%u)».
  Т.е. **type ∈ {0,1}, priority ∈ {0,1,2}**.
- Объект регистрируется: `IOGPUNamespace::addObject([[uc+0x120]+0x110], obj)` → **id**;
  `setIOQueueID(id)`.
- structureOutput, **16 байт**: `{u32 iocq_id @+0, u64 [obj+0x30] @+8}`.

### sel44 set_io_notification_queue (iOS 0x9d5a528)
- structureInput: **два u64**: `[in]=iocq_id (w22)`, `[in+8]=nq_id (w21)`.
- `lookup([uc+0x120], iocq_id)` → null → **0xe00002c2**.
- `lookup([uc+0x120], nq_id)` → null → **0xe00002c2**.
- Иначе `0x9d6c8b4(iocq, nq)` — attach: `IOGPUIOCommandQueue::setNotificationQueue`
  (mac 0xab9f680, сохраняет нить в `[q+0x28]`). Release обоих объектов, kr attach.
- `nq_id` — id `IOGPUNotificationQueue`, выдаёт **sel14 s_create_notificationqueue**
  (см. `agx_queue_execution.md` §таблица селекторов: sel14, out 16 байт).

### sel46 create_io_command_buffer (iOS 0x9d5a8dc)
- scalarInput[0] = iocq_id. `lookup([uc+0x120], iocq_id)` → null → **0xe00002c2**
  (лог «Invalid IO Queue ID (%u)»).
- `0x9d6cf5c(iocq, out, out+8)` = `IOGPUIOCommandQueue::createIOCommandBuffer`
  (mac 0xab9fd28): аллок через `[[q+0x78]+0x48]`+vtable+0xa00, init(vtable+0x150),
  `addObject([q+0x38], buf)` → id в `[buf+0x10]`.
- scalarOutput: **два u64**: `{iocb_id = [buf+0x10], aux = [buf+0x38]}`.
- Ошибки аллокации: `0xe00002bd`, `+0xc = 0xe00002c9` (init fail), `+0x1e = 0xe00002db` (addObject fail).

### sel45 submit_io_commands (iOS 0x9d5a674)
- scalarInput[0] = iocq_id; structureInput — буфер подачи (или
  structureInputDescriptor, мапится RO через `[desc]+0xe8`, длина `[desc]+0x78`).
- Проверки structureInput (размер `w0`):
  1. `(size−8) % 24 == 0`, иначе **0xe00002c2** («Insufficient input struct size (%llu)»);
  2. `u32[0] == commandCount == (size−8)/24`, иначе **0xe00002c2**
     («Command buffer count mismatch (%lld - %u)»).
- `lookup([uc+0x120], iocq_id)` → null → **0xe00002c2** («Invalid IO Queue ID (%u)»).
- Далее `0x9d6c928(cmdbuf)` = `IOGPUIOCommandQueue::submitIOCommands`
  (mac 0xab9f6f4):
  - lock `[q+0x20]`; если `[q+0x78]==0` (device) **или** `[q+0x28]==0`
    (notification queue не приаттачена через sel44) → **0xe00002bc**;
  - `count == 0` → kr 0;
  - цикл по дескрипторам `args+8+i*24`:
    - `u32[0] = iocb_id`: `0` → `processBarrier()`; иначе
      `IOGPUNamespace::retainObject([q+0x38], id)` → буфер **обязан существовать**
      (создаётся sel46); null → **молчаливый skip** (kr итоговый всё равно 0!);
    - `u32[1] = shmem_id` (device shmem от sel12),
      `u64[1] @+8` → `[buf+0x20]`, `byte @+0x10` → `[buf+0x5e]` (флаг);
    - `buf->processCommands(shmem_id, u64, flag)` (mac 0xab9d5f8) →
      `processKernelCommands` (0xab9d714): `retainDeviceShmem(device, shmem_id)`
      → VA/len shmem, walk kernel-команд между `readOffset` и `writeOffset`;
    - успех → буфер встаёт в `[q+0x88]`, `kickSubmitIOThread`; неуспех → release буфера.

### Важно про kr
**kr 0 у sel45 не означает исполнения**: невалидный iocb_id пропускается без
ошибки, count==0 — no-op. Единственная достоверная сигнализация — нотификации
в IOGPUNotificationQueue (ниже).

---

## 2. Почему девайс ответил 0x2c2

- `sel41 {qid} → kr 0` — обманчиво: это destroy-стаб, kr 0 = «объект не найден /
  уничтожен». **Очередь не создана.**
- `sel44 {iocq=1, nqid} → 0x2c2` — lookup iocq=1 в namespace `[uc+0x120]`
  вернул null (объектов нет). Второй lookup (nqid) мог бы дать тот же kr.
- `sel45 нулевой stIn {0x8..0x400} → 0x2c2` — размер проходит %24, но
  `commandCount=0 != (size−8)/24` при size>8 → «count mismatch»; при size=8 —
  count=0, но очередь всё равно не найдена → «Invalid IO Queue ID». Оба пути = 0x2c2.
- **Валидные id**: только те, что реально выдали create-селекторы в этом
  user-client (namespace addObject, счётчик с 1). Для iocq — из out sel42;
  для nq — из out sel14; для iocb — из scalar0 sel46.

---

## 3. Путь исполнения (и чем он является)

```
sel45 submit_io_commands
  → IOGPUIOCommandQueue::submitIOCommands      (парсинг, enqueue буферов в [q+0x88])
  → IOGPUIOCommandBuffer::processCommands
  → processKernelCommands                      (walk команд shmem)
  → processKernelCommand                       (создание IOGPUIOCommandDescriptor)
  → submit_io_thread (thread_call по [q+0x98]) → getIOCommands
       desc type 1 (IO)  → listHead → [q+0x60]
       desc type 2 (SignalEvent): сравнение со счётчиком [buf+0x30], notify+complete
       desc type 3 (Barrier): signalCompleted(event, value), complete
       desc type 4 (WaitSharedEvent): waitCompleted с дедлайном, поллинг 5 с
           через kickSubmitIOThread, status 2=signaled / 3=timeout
  → sel48 s_perform_io (mac 0xab8a2e8; iOS 0x9d56a4c):
       retainIOCommandQueue(id) → queue->performIO()  — синхронно, в нити клиента
  → IOGPUIOCommandQueue::performIO (mac 0xab9f9dc):
       uio_create(1,0,0,0)                       (UIO_USERSPACE, UIO_READ)
       по каждому IO-дескриптору из [q+0x58]:
         retainVnioDesc([desc+0x30]) → null → status 2
         uio_reset(uio, [desc+0x38])             (file offset)
         uio_addiov(uio, [desc+0x48], [desc+0x40])(user_addr, length)
         vnio_read(vnioDesc->getVnioDesc(), uio)  ← РЕАЛЬНАЯ РАБОТА
         status = 2 + (ret==0)  → 3 = успех
         commandDescriptorComplete(buf, status, [desc+0x50], notify=1)
```

**Вердикт по вопросу (2): это НЕ execution layer trap0.** Никаких AGFI, command
buffer'ов GPU, IOGPUWorkQueue/IOGPUCommandQueue здесь нет. Путь целиком CPU-side:
VFS `vnio_read` (чтение привязанного fd в юзер-буфер) + `IOSurfaceSharedEvent`
wait/signal. Общее с trap0-путём — только механизм нотификаций
(IOGPUNotificationQueue, shared-data очередь) и id-пространство namespace.
Практический вывод: **это потенциальный обход «no-op стены»** для end-to-end
проверки (не требует AGFI-образа и device-stream), но поверхность — vnode read,
а не DMA/GPU-память.

### Layout shmem и kernel-команд
shmem header: `u32 readOffset @0, u32 writeOffset @4`; команды между ними.
Каждая команда: `u32 type @0, u32 length @4` (length ≥ 8, %4==0, в пределах
writeOffset), payload с +8:

| type | мин. len | payload |
|------|----------|---------|
| 0 = IO (`kIOGPUIOCommandDescriptorTypeIO`) | 0x30 | `u32 vniodesc_id @+8` (из sel40 create_vniodesc), `u64 file_offset @+0x10`, `u64 length @+0x18`, `u64 user_addr @+0x20`, `u64 notify_value @+0x30` |
| 1 = SignalEvent | 0x10 | `u64 value @+8`, `u8 flag @+0x10` (нужен `[buf+0x28]!=0`) |
| 2 = Barrier | 0x10 | `u32 mach_port_name @+8`, `u64 value @+0x10` → `IOUserClient::copyObjectForPortNameInTask` → `IOSurfaceSharedEvent` |
| 3 = WaitSharedEvent | 0x10 | `u32 mach_port_name @+8`, `u64 value @+0x10` |

Созданные дескрипторы цепляются в `[buf+0x48]`, счётчик `[buf+0x28]++`.
IO-дескриптор (type 1) по полям: `[desc+0x30]=vniodesc_id, [desc+0x38]=offset,
[desc+0x40]=length, [desc+0x48]=user_addr, [desc+0x50]=notify_value` — последнее
передаётся в `sendIOCompletionNotification(q, buf_id, status, notify, 1)`.

### Нотификации
`sendNotification` (0xab9e6d0): запись **16 байт** в shared-data очередь
`IOGPUNotificationQueue`: `{u64 value, u32 status, u32 pad}`.
Статусы: 0/3 = complete/ошибка, 2 = wait-signaled, 1 = отменено.
Завершение буфера: когда все дескрипторы complete (`[buf+0x2c] == [buf+0x28]`),
`commandDescriptorComplete` → `sendIOCompletionNotification` и, по флагу `[q+0x50]`,
`thread_wakeup_prim` (ожидатель в `processBarrier`).

---

## 4. Минимальный валидный вызов sel45

1. **sel14** create_notificationqueue → `nq_id` (формат входа/выхода —
   `agx_queue_execution.md`, таблица селекторов).
2. **sel42** scalarIn `{type=0, priority=0}` → `iocq_id` (out[0], u32; out[1]=`[obj+0x30]`).
3. **sel44** `{iocq_id, nq_id}` (16B in) → kr 0 (attach обязателен: без `[q+0x28]`
   submit даст 0x2bc).
4. **sel46** scalar=`iocq_id` → `{iocb_id, aux}` (2 scalars out).
5. Записать в device shmem (создание — sel12, см. прежний документ):
   header `{readOffset=8, writeOffset=8+cmdlen}`; команда type 0 с валидным
   `vniodesc_id` (без него performIO даст status 2 на дескрипторе).
6. **sel45**: scalar=`iocq_id`, structure=`{u32 count=1, {u32 iocb_id, u32 shmem_id, u64 0, u8 0}}` (32 байта).
7. Дрейн — **sel48** perform_io scalar=`iocq_id` → синхронный `vnio_read`;
   результат смотреть нотификациями в очереди sel14 (запись `{notify_value, status}`).

Ожидание: kr 0 на всех шагах; при валидном vniodesc статус 3 (успех).

## 5. Чеклист фаззера

- **sel42**: type ∈ {0,1,2,0xffffffff}, priority ∈ {0..3} — гейты в init.
- **sel44**: несуществующие iocq/nq; перекрёстные id (iocq↔nq) — оба lookup в одном ns.
- **sel45**: count mismatch, size%24, iocb из чужой очереди, shmem_id чужой/0,
  readOffset>writeOffset, length%4/length<8, type>3, type 0 с len 0x10..0x2f,
  vniodesc_id 0/несуществующий (status 2 на дескрипторе, kr всё равно 0!),
  user_addr неперекрытый/граничный, offset+len за EOF, mach_port_name мусор
  (copyObjectForPortNameInTask на произвольном имени порта).
- **sel48** (perform_io): дрейн без submit; повторный дрейн (пустой [q+0x58] →
  ожидание; флаг `[q+0x74]` выставляется при прерывании сна — проверить сценарий
  сброса).
- Важно: kr 0 у submit/perform_io ≠ исполнение; ориентироваться на нотификации.

## 6. Не закрыто

- iOS-адрес 0x9d56a4c назван sel48 (s_perform_io) по позиции в таблице и аналогии
  с mac; проверить строкой `__FUNCTION__`/на девайсе.
- Кто перекладывает дескрипторы `[q+0x60] → [q+0x58]` (возможно, внешняя
  подкласс-логика или субтильность unlink-списков) — на путь vnio_read не влияет,
  но стоит подтвердить трейсом.
- Точная семантика `aux` (out[1] sel46 = `[buf+0x38]`) и флага `[buf+0x5d]`
  (cancel) — по коду не используются для валидации.
