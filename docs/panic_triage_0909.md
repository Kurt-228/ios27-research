# Триаж трёх kernel panic от 2026-09-09

Дата: 2026-09-09. Источник: `/tmp/fzc-today/panic-full-2026-09-09-{124459,135915,140637}.0002.ips`.
Kernelcache: `results/kc27/kernelcache_iphone16.macho` (LC_UUID `ca9a3c24…cbb2d2` совпал с
«Fileset Kernelcache UUID» во всех трёх паниках — символизация точна).

## Вердикт (кратко)

**НАШ ТРИГГЕР поверх чужого (Apple) бага.** Все три паники — детерминированно один и тот же
краш: наш фаззер `fuzz27` вызывает IOGPU external method **sel 6 (`new_command_queue`)** с
полем **version ≥ 5** по смещению `structureInput+0x400`. Ядро проваливает gate `version < 5`,
идёт по error-path в `IOGPUCommandQueue::init`, где диагностический лог-блок безусловно
разыменовывает **неинициализированный** указатель `this+0x488` (device) → `ldr x21,[NULL,#0x38]`
→ panic `far=0x38`. Это баг Apple (NULL-deref в failure/logging пути, reachable из userspace),
спровоцированный нашим sweep'ом версий.

## Сводная таблица

| # | Время | pid | slide | pc (slided) | pc (unslided) | x8 / x9 / x23 | lr offset в init |
|---|-------|-----|-------|-------------|----------------|---------------|------------------|
| 1 | 12:44:59 | 1383 | 0x2d97c000 | 0xfffffff037703490 | …09d87490 | 0 / 0x10e / 0 | +0xb4 |
| 2 | 13:59:15 | 1227 | 0x318d8000 | 0xfffffff03b65f490 | …09d87490 | 0 / 0x10e / 0 | +0xb4 |
| 3 | 14:06:37 | 657  | 0x04198800 | 0xfffffff04b70f490 | …09d87490 | 0 / 0x10e / 0 | +0xb4 |

Все: `bug_type 210` (Kernel data abort), `esr 0x96000006` (EC=0x25 data abort, DFSC=0x6
translation fault уровня 2), `far 0x38`, `Panicked task …: fuzz27`, cpu 5.
Смещение pc внутри функции одинаково (0x1d0) при трёх разных slide — краш сдвиг-независим.

## Символизированный стек (паника 1, offsets подтверждены для 2/3 вычитанием slide)

```
pc  : IOGPUCommandQueue::init(IOGPU*, IOGPUDevice*, IOGPUDeviceNewCommandQueueArgs*)
      IOGPUFamily __TEXT_EXEC, функция @ unslided 0xfffffff009d872c0, краш @ +0x1d0
lr  : init + 0xb4  (последний bl — retain-вызов после проверки gate)
      (PAC-подписан; низкие биты совпадают)
+1  : AGXCommandQueue::init            AGXG16P,  AGX base 0x35c6d060(slide) +0x2b154
+2  : IOGPUDevice newCommandQueue thunk (vtable slot 0xd8), IOGPU +0x6e58
+3..: kernel userclient dispatch → IOGPUDeviceUserClient::s_new_command_queue
```

Символы из старого символьного KC (`results/kc-extract/iogpu_full_disasm.txt`:
`IOGPUCommandQueue::init @ 0xabbb034`, `s_new_command_queue @ 0xab889a0`;
`agx_full_disasm.txt`: `AGXCommandQueue::init @ 0x8b0cbdc`). Логика функций совпала
построчно (byte-match) между старым билдом и kc27.

## Корневая причина — разбор init (kc27, верифицировано капстоуном)

Псевдокод (смещения — от базы функции, unslided 0xfffffff009d872c0):

```c
// x0=this, x1=?, x2=IOGPUDevice (сохраняется в x21), x3=args (сохраняется в x22)
+0x3c  vtable[+0x78] -> создать объект;  w0==0 -> FAIL        // alloc-class
+0x50  str x23(=x1), [x19,#0x530]
+0x5c/+0x64  getter-thunk(x2) -> [x19,#0x458], [x19,#0x528]   // retain глобала 0xb35e710
+0x78  kalloc_type(0x2a8) -> [x19,#0x430]
+0x84..+0x90  if (![x19,#0x458] || ![x19,#0x528] || !kalloc) -> w23=0, skip-gate
+0x94  ldr w8, [x22,#0x400]          // args->version
+0x98  cmp w8, #5
+0x9c  cset w23, lo                  // w23 = (version < 5)
+0xb0  retain(kalloc-объект)
+0xc4  cbz w23 -> FAIL               // version >= 5 -> выход с ошибкой
…успешный путь…
+0x114 str x21, [x19,#0x488]         // device записывается ТОЛЬКО здесь
…
FAIL / хвост функции (общий для всех веток ошибки):
+0x1bc adrp x8, 0xfffffff00b49b000; ldr w8,[x8,#0x538]   // глобальный debug-флаг
+0x1c8 cbz w8, +0x1d4                  // флаг==0 -> лог пропускается
+0x1cc ldr x8, [x19,#0x488]           // !!! БЕЗУСЛОВНО, поле не инициализировано
+0x1d0 ldr x21, [x8,#0x38]  <== PANIC (x8==0, far=0x38)
+0x1d4.. os_log(..., x21=device+0x38, x22=[x19,#0x550], w20=err)
```

Регистры паники согласованы именно с gate-путём: `x23=0` (результат `cset`, т.е.
version ≥ 5), `x20=0` (`mov w20,#0 @ +0xb4`), `x21/x22` ненулевые (device и args),
`lr=init+0xb4`. Пути «getter вернул NULL» исключены: thunk ретейнит глобал и при NULL
упал бы внутри хелпера (`ldr w9,[x0,#4]`, far=0x4), а не в init; kalloc-NULL дал бы
lr=+0x7c. Единственный полностью консистентный путь — **args+0x400 ≥ 5**.

Заметка: краш происходит только когда глобальный флаг `*(uint32*)0xfffffff00b49b538 != 0`
(на этом Beta-билде 24A5390f он установлен — иначе `cbz` обошёл бы лог и вернул ошибку
корректно). То есть баг «спит» до включения диагностического логирования.

## Откуда взялся version ≥ 5 — закрытый вопрос

В **незакоммиченном** изменении `fuzzer/t_iosurface_scaler.m` (+526 строк, класс-фаззер
`p_ios_classes` → targeted sweep для sel 6) есть явный sweep поля +0x400:

```c
// fuzzer/t_iosurface_scaler.m:18831 (working tree, НЕ в git HEAD)
static const uint32_t vers[] = { 0, 1, 4, 5, 6, 0xffffffff };
...
ios_blob410(ib);                        // zeroed 0x800 blob: procName@0, version=2@+0x400
*(uint32_t *)(ib + 0x400) = vers[i];
kr = ios_case(&r, e, "m6v", ..., ib, 0x410);   // sel6, structureInputSize=0x410
```

Значения **5, 6, 0xffffffff** — все ≥ 5 → каждый проход sweep'а детерминированно паникует
ядро. Собственный комментарий в фаззере (`:18517`): «version <5 @+0x400» — поле и есть
version дескриптора очереди, gate `version < 5` реален.

Хронология сходится: бинарь `build/fuzz27.app/fuzz27` собран **12:44:33**, паника #1 —
**12:44:59** (26 с позже). После ребута relay перезапускает фаззер, sweep доходит до
sel6 снова → паники #2 (13:59) и #3 (14:06). Заголовочный комментарий sweep'а ожидал
«[HIT] … version %u >= 5 accepted» — вместо этого ядро падает на failure-пути.

В git HEAD (v142 и всей истории) значений ≥ 5 для +0x400 нет — только 0/2, поэтому
старые сборки не паниковали.

## Класс бага и значимость

- **NULL pointer dereference** (чтение неинициализированного `this+0x488` в error-path)
  в `IOGPUCommandQueue::init`, kext `com.apple.iokit.IOGPUFamily`, reachable из userspace
  одним вызовом `IOConnectCallMethod(sel 6, stIn 0x410, version>=5 @+0x400)`.
- Импакт: kernel panic (DoS). Не memory corruption — повышения привилегий напрямую не даёт.
- Требует доступа к IOGPU userclient (наш entitlement `com.apple.private.*`/development
  подпись). Кандидат на репорт Apple: failure-path диагностики разыменовывает член,
  инициализируемый только на success-пути; включение лог-флага превращает benign error
  return в панику.

## Воспроизведение (минимальный рецепт)

```c
uint8_t in[0x410] = {0};
strlcpy(in, "x", 0x40);
*(uint32_t*)(in + 0x400) = 5;                 // 5/6/0xffffffff — любое >= 5
IOConnectCallMethod(gpu_conn, 6, NULL,0, in, 0x410, out,&osc, outb,&osb);
// -> panic в IOGPUCommandQueue::init, far=0x38 (при установленном debug-флаге)
```

## Рекомендации

1. Для продолжения фаззинга без паник: исключить `vers[i] >= 5` из m6v-sweep'а
   (вернуть только {0,1,4}) или принять панику как известную.
2. Оформить findings для Apple: IOGPUFamily, `IOGPUCommandQueue::init` failure-path,
   uninit `this+0x488` deref под флагом `0xfffffff00b49b538`.
3. Паники НЕ похожи на memory corruption и не дают новых примитивов — закрыть как
   «det DoS, известный механизм», не тратить на них дальнейший триаж.
