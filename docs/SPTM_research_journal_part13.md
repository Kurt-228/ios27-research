# Журнал, часть 13 (секции 68–70, 13.08.2026) — AGX command grammar

## 68. Submit end-to-end (финальная карта)

Цепочка из userland (всё проверено на устройстве, kr 0):
1. sel14 {numEntries, entrySize} — notification queue (sc[1] = entrySize!). out {userVA, id}.
2. sel6 (блоб 0x410, version u32 @+0x400 < 5) — command queue. out {qid, traceTag}.
3. sel24 {qid, nqid} — bind (иначе submit = 0x2bc молча).
4. sel12 {size, 0} — IOGPUDeviceShmem, out {userVA, size, id}. **id из этого реестра (+0x78) — то, что ждут поля записи submit** (sel8-ресурсы НЕ подходят, error 9).
5. Запись 0x40: +0x00 = kernel command shmem id (A), +0x04 = segment list shmem id (B), +0x08 = sideband (0 = нет). Submit: trap0 (p1=qid, p2=0x40, p3=entryVA, p4=outVA) или sel25 {qid, 0, count, 0x40}. Per-entry код в out: 0=ok, 8=нет kernel VA, 9=lookup fail, 0xa=format.

## 69. Грамматика стримов

**Segment list shmem (B)**: +0x08 u32 count ≥1; +0x0c hdr u32: bit31=end, bit30=has-type, bits29:0=type. type 2 = конец; type 1 = segment list: count пар {lo,hi} u32 по +0x10 — диапазоны в A (молча клампятся); type 0 (только offset 0) = submitCommandBuffer.
**Kernel command shmem (A)**: записи {u32 type @+0x00, u32 len @+0x04 (≥8, 4-align)}, типы 2..0x12 + AGX {3,4,0xb,0xe,0x10002,0x10004}. type 2 = noop (проверено: outw 0).
**AGX inner commands** (vt[20] @ 0x831d84c): magic u32 @+0x00 = 0x00010000; subtype u32 @+0xa8 ∈ {1,2,3,6,7}; must-be-zero @+0x8c; tailLen u32 @+0xa4 > 0, tail @+0xc8; OR-check @+0xac..+0xc3 ≤ 0x3ff. Тело cmd+0x08..+0xc8 → cmdObj 1:1. **tail qword'ы копируются в GPU-дескриптор без проверок диапазона** (case 2: tail+0x278/+0x2c0/+0x388 → desc поля).
- Классы дескрипторов: AGXTA/3D/CL/RemoteNode/IOSurfaceSharedEvent CommandDescriptor (kalloc 0x158..0x638).
- GPU пишет по адресам из этих дескрипторов (firmware submit TA/3D/CL channel).

## 70. Статус и следующее

- v56 (random types): 2M+ раундов, 1 аномалия (outw 7, невоспроизведённая детально — инструментированный билд готов).
- v58 (stateful 3/0xb/4/c/d): 1M раундов, чисто.
- v61 (готов): крафт AGX-команд — свип subtype × tailLen, карта per-case кодов. Дальше: fuzz tail-полей (GPUVA-кандидаты) + попытка пропустить через ресурс с known GPUVA (sel8 out поля) для наблюдаемой записи.
- IOMFB crop-wrap: sel5 принял swap, 0xe00002d1 (нужен активный display mode) — припарковано (OOB-read).
- Инфраструктурное: devicectl install/launch клинит после kill-циклов; лечится ребутом устройства + пайплайн-сторожем. idevicediagnostics restart работает удалённо.

## Дополнение (секция 70.1, 13.08.2026 ~23:30)
- Tail-парсеры разобраны: subtype 1 (min tail 0x9a8) и 2 (0x480) — verbatim copy в дескриптор БЕЗ валидации значений; subtype 3 — всегда принимается (fallback submit); 6 — нужны зарегистрированные events; 7 — вложенные суб-парсеры с проверками desc-полей.
- Динамика: subtype 1/2 с полными zero-tail → всё равно per-entry 0xa. Значит отказ глубже парсера — в channel-submit подготовке (fn_0x831e1ec/fn_0x831ea18): нулевой дескриптор там невалиден. Это следующий слой для статики.
- Текущий фазз (run-v63/64): мутация адресных qword'ов tail (subtype 1: +0x138/+0x140/+0x24c/+0x638; subtype 2: +0x278/+0x2c0/+0x388) — >1.2M раундов, без принятий (0xa) и без паник.
- Практический вывод: слепой фазз AGX-команд не проходит многоуровневую структурную валидацию; дальше — только управляемый крафт по полному layout'у fn_0x831e1ec (следующий раунд реверса) либо захват настоящей GPU-команды из Metal-трейса (дамп command shmem в момент submit на Mac).

## Финальное дополнение (секция 70.2, 14.08.2026 ~00:00)
- Tail-фазз v61c/v64: ~90M раундов мутаций tail (включая документированные адресные qword'ы subtype 1/2) — 0 принятий, 0 паник. Слепой фазз не проходит channel-submit валидацию (fn_0x831e1ec/fn_0x831ea18) — нужны структурно валидные AGFI-дескрипторы.
- Mac-трейс: magic 0x10000 команд в памяти Metal при blit-submit НЕТ — этот класс команд Metal в данном флоу не генерит (используется другими механизмами/внутренне), т.е. поверхность малоутоптанная, но эталона для копирования нет.
- Остаточная неидентифицированная аномалия: per-entry код 7 (дважды в grammar-фаззе, не воспроизведён с логированием типов).

### Состояние на стоп
- Полностью разобранные и работающие на устройстве: скейлер-протокол (все селекторы, легитимные трансформы), IOGPU submit-конвейер (очереди/shmem/segment-list/kernel-command/AGX-command уровни).
- 0-day: border-fill wraparound → DART panic bug 210 (репорт-готов, на GitHub с воспроизводителем).
- Write-примитив: не получен. Последний непройденный слой — валидация AGFI-дескрипторов в channel-submit (fn_0x831e1ec/0x831ea18/0x831efb0). Для продолжения: разобрать их до конца (следующий реверс-раунд) или захватить валидный AGFI-дескриптор из реального рендер-пайпа (Metal compute/render workload, не blit).

## Секция 71: захваченный эталон AGX-команды (с Mac, compute dispatch)
- Настоящая subtype-3 inner command снята с macOS 27 Metal compute: len 0x358, tailLen 0x268, полный дамп в `docs/agx_cmd_template.h`, разбор по смещениям — ниже.
- Segment-list фрейминг: +0x08 count, +0x0c hdr (bit30/31), пары {lo,hi} по +0x10 с timestamp-qword'ами; kernel command shmem: записи {type,len} (type 0x0f = nop/pad).
- Tail команды ссылается на подструктуры через 32-битные offsets в multi-window shmem pool (тег 0x100 в high dword) — НЕ GPUVA. GPUVA-поля живут глубже (в окнах 0x28000/0x2a000 пула; единственный видимый: 0x07aa0177).
- Реплей на iOS (v65): шаблон с санитизированными offset'ами → 0xa. Нужны сами подструктуры (они есть в дампах /tmp/agx_regions/) — следующий шаг: встроить блобы и починить ссылки на них. Это полноценная реконструкция AGFI-дескриптора — объёмная задача.
