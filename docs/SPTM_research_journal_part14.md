# Журнал, часть 14 (секции 71–73, 14.08.2026) — Metal format-B resources + faithful replay

## 71. Спека из реверса Metal/IOGPU (macOS 27) и её проверка

### 71.1 new_resource формат B — исходная спека была сдвинута

Заданная спека (type 0x00, +0x18 sysMemSize, +0x24 flags 0x1000470, +0x58 dup size) на устройстве
дала `kr 0xe00002be` (BadArgument) — run-v83.log. Сверка с реальным трейсом Metal
(`/tmp/iogpu_trace.log`, interpose на macOS 27): resource creation идёт через **sel 9,
stInSz 0x68**, и раскладка отличается от спеки сдвигом на одно qword-слово начиная с +0x10.
Реальный формат (33 записи трейса, консистентно):

| Off | Значение | Комментарий |
|---|---|---|
| +0x00 u32 | resourceType: 0x00 чистая аллокация / 0x80 обёртка CPU-указателя | |
| +0x04 u32 | 0 | |
| +0x08 u32 | 0x00010001 | во всех записях |
| +0x0c u32 | 1 | |
| +0x10 u32 | 0x01000101 | во всех записях |
| +0x14 u32 | flags: 0x470 (буферы), 0x430 (shader-cache res0), 0xc30 (обёртки) | спековые 0x1000470 = те же младшие биты |
| +0x18..+0x28 | 0 | |
| +0x30 qword | 1 (чистая аллокация) / 0 (обёртка) | тот самый "+0x30=1" из старого формата A |
| +0x38 qword | 0 / CPU start (обёртка) | |
| +0x40 qword | 0 / CPU ptr (обёртка) | как в формате A |
| +0x48 qword | sysMemSize | как в формате A |
| +0x50 qword | 0 / 4 (обёртка) | |
| +0x58 qword | 0 (буферы) / 0x18xx000000 (некоторые) | не размер |

На iOS тот же формат принимает **sel 8** (macOS sel 9 = iOS sel 8; macOS "sel 8 scIn 1" —
другой вызов, нумерация сдвинута на 1). Хелпер `gpu_resource2()` перебирает {sel8, sel9} ×
{0x470, 0x1000470}; работает **sel8 + 0x470**.

### 71.2 Подтверждение главного ожидания спеки

`gpu_resource2` type 0x00 (чистая shared-аллокация) возвращает **GPUVA в пространстве 0x1**
и **CPU-указатель маппинга** (out+0x08) — читается/пишется с CPU. Аллокатор детерминирован
на свежий процесс и совпал с капчей 1:1:

- res0 (0x10000) → GPUVA 0x1_00000000, cpu out+0x08
- res1 (0x10000) → GPUVA 0x1_00018000 (dest в капче!)
- res2 (0x20000) → GPUVA 0x1_00030000

out struct (0x58): +0x00 GPUVA, +0x08 CPU data ptr, +0x10 CPU ptr ресурс-дескриптора,
+0x20 u32 = rid-счётчик, +0x24 u32 = resourceID, +0x28 qword = size.

### 71.3 Капч Trap4 submit entry (0x40 байт, из interpose PRE-дампа)

```
+0x00 u32 = 2   kernelCmdShmemID  (cmd shmem создана ВТОРОЙ)
+0x04 u32 = 1   segmentListShmemID (seglist создана ПЕРВОЙ)
+0x08 u32 = 0   sideband
+0x0c u32 = 0
+0x10 qword = CPU ptr  } два указателя в ОДНОМ буфере,
+0x18 qword = CPU ptr+0x30 } userspace completion
+0x20..     = 0 (никакого rid @+0x20 у Metal нет — это была наша выдумка v76/v80)
```
Per-entry коды (из part13): 0=ok, 8=нет kernel VA, 9=lookup fail, 0xa=format.

### 71.4 shmem-форматы

Типы sel12 (2-й скаляр): 0=segment list, 1=kernel cmd, 2=debug, 3=sideband — оба типа
создаются на устройстве (kr 0). Kernel cmd shmem: nop rec {0x0f, 0xac} @0, residency list
@+8 (count=0 в капче), AGX-команда с +0xac. Segment list shmem: KernelCommandList
{ts, count=1, 0x40000001, rec{0,0xac}} @0, SegmentList {ts, segCount=1, 0x80000130} @+0x18,
segment {ts, begin=0xac, end=0x404, numResources=0, numResourceGroups=0} @+0x28.
6-pack resource group (0x40): +0x00 u32 rid[6], +0x18 u32 sizeKB[6], +0x30 u16 usage[6],
+0x3e u16 count. В капче групп НЕТ (numResources=0) — Metal кладёт их, видимо, только при
явных hazard'ах; totalSize 0x130 резервирует место под 4 группы.

## 72. Что реализовано (v83)

- `gpu_shmem_t(c, size, type, &va)` — typed sel12 (старый `gpu_shmem` не тронут).
- `gpu_resource2(c, size, &gpuva, &cpu)` — формат B по исправленной раскладке (см. 71.1),
  возвращает rid + GPUVA + CPU-указатель.
- Фаза `p_mtlreplay` — первая в списке фаз. Три ресурса (A=metacache 0x10000,
  B=dest 0x10000, C=pool 0x20000) с **точным содержимым из капча** (дампы
  `/tmp/agx_blit/reg_104b44000/104b6c000/104b80000` → `fuzzer/assets/*.bin`, копируются
  в .app билд-скриптом), GPUVA совпадают с капчей без патчинга.
- FULL replay: agx_A4_image/agx_B4_image verbatim (без единого патча), 6-pack
  {ridA,ridB,ridC}, usage=3, entry в форме капча {2,1,comp,comp+0x30}, post-submit
  bookkeeping (sel17 {1}, sel15 {2}/{1}) — на iOS sel17/sel15{2} = 0xe00002c2 (другая
  нумерация), sel15{1} = 0.
- Лестница S1..S8 (мост от v80): capseg/handsег × cmdshmem type0/type1 × aux ×
  entry+0x20-rid × residency-в-nop × usage {3, 6, 0xff, 0xffff}.
- Инфраструктура: `--console` у devicectl на этой паре Mac/iPhone сломался
  (10002/EINVAL при живом plain-launch; перезапуск remotepairingd не помог). Обход:
  `FUZZ_LOGFILE=1` в env запуска → `main.m` делает freopen stderr в
  `Documents/fuzz.log` контейнера, забор через `devicectl device copy from
  --domain-type appDataContainer`. Логи: results/run-v83{,b,c,d}.log.

## 73. Результаты прогонов (run-v83..v83d)

Все 14 конфигураций: GPU-записи в буфер B НЕТ (B полностью нулевой после каждого
сабмита, опрос до 2 с). Ответы ядра задокументированы:

| Комбо | outw | примечание |
|---|---|---|
| FULL t1/t0 u3 (verbatim images, точные ресурсы) | 0 | принято, исполнения нет |
| FULL t1 u6 | 9 | usage=6 → lookup fail |
| S1 capseg+6pk u3 (+aux, +rid@0x20) | 0 | принято, записи нет |
| S2 +residency {2,ridA,ridB} в nop | 0xa | residency-список ломает формат |
| S3 cmd shmem type 1 | 0 | type 1 валиден |
| S4 usage 0xffff | 9 | lookup fail |
| S5 bare (без aux/rid@0x20) | 0 | принято, записи нет |
| S6–S8 hand-built seglist (KCL count=2, 0xc0000001) | 0xa | ручная сборка KCL невалидна; валидна только capture-форма (count=1, 0x40000001) |

Выводы:
1. Пайплайн сабмита доведён до полностью принимаемого ядром Metal-формата:
   ресурсы в пространстве 0x1 с точным содержимым, верbatim-команда, верbatim-seglist,
   6-pack группы — outw=0. Это закрывает гипотезу "невалидные структуры".
2. Отсутствие записи при принятом сабмите значит, что блокировка глубже: либо
   канал/очередь, созданные нашим нулевым sel6-блобом, не привязаны к реальному
   GPU-контексту (firmware никогда не планирует нашу очередь), либо для исполнения
   нужен ещё один шаг инициализации контекста, который Metal делает до первого submit
   (кандидаты: sel42/sel44/sel45 VM-attach из v74, "set_hazard" вызовы, или
   device-command-stream генерация, которую kext делает только для "своих" очередей).
3. Следующий слой реверса: сравнить sel6 queue-create блоб Metal (0x410) с нашим
   нулевым — в трейсе macOS queue-create не попал в лог (до включения watch);
   переснять трейс с самого старта процесса + разобрать, что делает AGX kext между
   queue create и первым trap0 (контекст канала, doorbell, TA/3D/CL channel attach).
