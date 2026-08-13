# Журнал, часть 12 (секции 66–67, 13.08.2026) — submit-путь ОТКРЫТ

## 66. IOGPU submit заработал на устройстве

Разгадка цепочки (всё на iPhone 15 Pro Max, iOS 27.0b4, сервис "IOGPU" type 1):
1. **sel 14 {numEntries, entrySize}** — create notification queue. Ключ: sc[1] — это entrySize (не index!): 0 → "Invalid arguments: numEntries=N, entrySize=0" в kernel-логе. Рабочее: {0x100, 0x10} → kr 0, out = {userVA, id}.
2. **sel 6** stIn 0x410 (нулевой блоб ок: version=0) → {qid, trace-tag}.
3. **sel 24 {qid, nqid}** — bind notification queue к командной (queue+0x428). Без него submit = молчаливый 0x2bc.
4. **trap0 (conn, 0, qid, 0x40, entryVA, outVA)** или **sel25 {qid,?,count,0x40}** → **kr 0** — submit исполняется!
   - per-entry код в out: 0 = ок, 9 = invalid resource/stream. Наши фейковые стримы → 9.
   - Запись (0x40): +0x00 rid A (required), +0x04 rid B (command-stream resource), +0x10 trace ref ptr, +0x18 descriptor-init ptr (последние — из Metal-трейса: живые user-указатели; на ошибку не влияют).
- Реестр очередей/ресурсов per-connection; id'шки с 1.
- Полная таблица 56 селекторов + 14 трапов задокументирована (part10/11).

## 67. Текущая стена: формат командного стрима

Error 9 = парсер отвергает наш поток в ресурсе B. Эталон с Mac'а: запись submit содержит указатели на Metal-структуры; сам стрим живёт в shmem-ресурсе (парсится ядром live). Нужен формат: типы команд (header @cmd+0xc: bit31=end, bit30=mod, bits29:0=type), segment list, поля shmemOffset (assert-паники "shmemOffset value is corrupt" — цель для double-fetch/OOB).
Источники формата: (а) дизасм парсера IOGPUCommandQueue::processCommandBuffer @ 0x9d8a5e8 + IOGPUCommandDescriptor::prepare (0x9d948xx) + AGX vt[26] 0x831c940; (б) захват настоящего стрима на Mac (дамп shmem-ресурса после encode — нужно найти VA ресурса B в трейсе: sel9 outs).
Побочно: IOMFB begin-swap = **sel 4** (пустой вход → swap id); crop-wrap пейлоад готов (part9, сек. 57) — ждёт прогона.

### Промежуточный итог сессии
- Подтверждённый 0-day: border-fill wraparound → DART panic (bug 210) — репорт-готов.
- IOGPU: от нуля до работающего submit за сессию; следующая фаза — валидный командный стрим → фазз парсера (исторически уязвимое место).
