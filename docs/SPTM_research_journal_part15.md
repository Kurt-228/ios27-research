# Журнал, часть 15 (секции 74–75, 14.08.2026) — notification queue: completion status 5

## 74. Диагностика completion-канала (v84)

Применены данные реверса IOGPU.framework (macOS 27). Новая фаза `p_mtlreplay2`
(вызывается первой; полный прогон — только матрица, без фазз-циклов, режим
`FUZZ_MTLR_ONLY=1` + `FUZZ_MODE=scaler`).

Изменения против v83:
- sel14: оба варианта entrySize {0x10, 0x28}; out: +0x00 u64 = VA data queue
  (kernel-mapped, читается с CPU!), +0x08 u32 = notifyQueueID. На iOS работает,
  VA валиден.
- После bind (sel24) добавлен `IOConnectSetNotificationPort(conn, 0, mach_reply_port(), nqid)` → kr 0.
- Queue-create блоб sel6: procName @+0x000, priority=2 @+0x400 (принимается, kr 0).
- trap0 entrySize: оба варианта {0x30, 0x40}.
- Дамп шапок shmem сразу после sel12: **обе shmem полностью нулевые** — ядро
  шапок не пишет, наша раскладка с +0 корректна.
- Логируется и возврат трапа, и *outVA (outU32).

### Формат data queue (выведен из дампов)

Шапка 0x10: +0x00 u32 tag (0x1414/0x2c2c — стабильный per-queue), +0x08 u32 lo =
write index (байты), +0x0c u32 = entrySize 0x28 (ядро пишет СВОЙ размер; наш
параметр 0x10/0x28 игнорируется). Записи с +0x10, **stride 0x2c**:
{u64 ref, u64 startTime, u64 endTime, u32 status, pad}. Timestamps — mach absolute
(стадия занимает ~80–90 мкс, т.е. реальная обработка происходит).

## 75. Матрица результатов (run-v84.log, run-v84b.log)

| nq entrySize | trap entrySize | kr trap | outU32 | completion records | B write |
|---|---|---|---|---|---|
| 0x10 | 0x30 | 0 | 0 | 2 записи: status 0, затем **status 5** | нет |
| 0x10 | 0x40 | 0 | 0 | то же | нет |
| 0x28 | 0x30 | 0 | 0 | то же | нет |
| 0x28 | 0x40 | 0 | 0 | то же | нет |

Во всех ячейках конфиг = capture-faithful FULL (verbatim images, 3 ресурса с
точными GPUVA капчи, 6-pack {ridA,ridB,ridC}, usage 3). GPUVA снова совпали с
капчей 1:1 на обоих коннекшнах (аллокатор per-connection, детерминирован).

### Главный результат

Completion-канал заработал и дал ответ: **на каждый submit приходят ДВЕ записи** —
- первая: ref = 0x1_04300000-подобный (CPU VA kernel-side буфера), status = **0**
  (трансляция kernel command → device command прошла);
- вторая: ref = первый +0x30 (ровно cmdBufArgsSize!), status = **5**
  (стадия исполнения — отказ).

Т.е. сабмит доходит до стадии исполнения и там отвергается с кодом 5 (ранее этот
канал был слепым — outU32=0 лишь «в очередь принято»). Код 5 — IOGPU-specific
(не IOReturn: по MTLCommandBufferError ближайшие — 4 AccessRevoked/3 PageFault;
5 не документирован в публичных заголовках, следующий шаг — найти таблицу кодов
в IOGPU.framework/AGX kext).

### Следующие шаги
1. Найти расшифровку completion status 5 (IOGPUCommandBuffer error table в
   IOGPU.framework / AGXUserClient::... в kernelcache).
2. Рефы записей (0x1_043xxxxx) — это VA в нашем же процессе? Проверить чтением:
   если читается — это device command buffer после трансляции, можно сравнить
   с капченным device stream (reg_10d460000) и увидеть, что kext сгенерил.
3. Проверить, приходят ли пары {0, 5} и у настоящего Metal на macOS (расширить
   interpose-дамп nq) — возможно, status 5 это норма для «пустого» исполнения,
   а возможно — чёткая ошибка.

## 76. Стресс-тест replay для корреляции с глитчами экрана (v85)

Контекст: при прогоне run-v84b (~04:29) пользователь наблюдал фиолетово-чёрные
глитчи экрана; изолированные scaler kill-shot'ы глитчей не давали — подозрение
на AGX-replay. Добавлен env-режим `FUZZ_MTLR_LOOP=N`: непрерывные verbatim
replay submit'ы (конфиг kr 0 / outU32 0 / completion {0,5}) без пересоздания
ресурсов, usleep(1000) между сабмитами, хеши буферов до/после.

Прогон run-v85.log (90 с, FUZZ_MTLR_LOOP=90, безопасный — без killshot'ов и
scaler-фаз):
- **45072 сабмита**, все kr 0 / outU32 0, процесс жив, устройство не паниковало.
- Completion: пары {status 0, status 5} воспроизводятся; wrIdx замер на 0x13f0
  (184 записи) — ядро перестаёт писать completion'ы при заполнении недренируемой
  очереди (мы read index не двигаем) — само по себе интересно: backpressure есть.
- Хеши A/B/C до и после — **идентичны** (GPU-записи в наши буферы нет даже при
  45k сабмитов).
- Визуальная корреляция (глитчи в контролируемом 90-с окне) — по наблюдению
  пользователя; на момент записи verdict не зафиксирован. Если глитчи были —
  replay исполняет ЧТО-ТО видимое мимо наших буферов (кандидат: IOSurface-backed
  backing store UI в нашем GPU address space, несмотря на status 5); если не
  было — глитчи v84b коррелировали с чем-то иным (напр. установкой/запуском
  приложения или scaler-conn setup до матрицы).
