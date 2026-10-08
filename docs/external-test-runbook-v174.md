# Внешний тест v174: видит ли демон запись через bad_query

Подготовлен, **не запущен**. Цель — закрыть единственный открытый вопрос
§176: реакция привилегированного демона на изменение его файла из песочницы
наблюдаема только снаружи.

## Что уже доказано (не повторять)

- escape bad_query даёт запись в системные контейнеры (§172/§175);
- изменение `DefaultAppQueryState.plist` (add-ключа) переживает fs и
  побайтово откатывается (§176);
- из песочницы реакция демона не наблюдается: mtime молчит,
  `LSApplicationWorkspace.allApplications` отфильтрован (0 записей),
  `LSCopy*` не экспортируются.

## Гипотеза теста

H1: `lsd` (или другой демон, владеющий контейнером AF589EEA) перечитает
изменённый plist при внешнем событии (respring / reboot / launch приложения)
и перезапишет его — CHECK увидит `REWRITTEN-BY-DAEMON`.

H0: файл остаётся `MODIFIED-BY-US` после любых событий — демон читает его
редко/кэширует/не читает вообще. H0 не обесценивает примитив (запись
доказана), но закрывает этот конкретный файл как рычаг наблюдения.

## Не делать

- не обновлять устройство с 24A5390f;
- не трогать `com.apple.launchservices.securepreferences.plist` (имя
  говорит о проверках целостности — не исследовано);
- не запускать MODIFY без плана тут же прогнать CHECK и ROLLBACK;
- после серии крашей и ребута — `relay/resume.sh`.

## Шаги

Устройство подключено к этому Mac (device id `8A8A1D3F-AF75-5493-9585-6374D1BB90D1`).

### 0. Установить свежий билд

```sh
./relay/build.sh
xcrun devicectl device install app --device 8A8A1D3F-AF75-5493-9585-6374D1BB90D1 build/fuzz27.app
```

### 1. CHECK до — должен быть CLEAN

```sh
RUNF_WAIT=120 ./relay/runf.sh bq5 FUZZ_LOGFILE=1 FUZZ_MODE=scaler FUZZ_BQ5_CHECK=1
```

Ожидаемо: `CLEAN (== backup)` либо `UNKNOWN (no backup)` — оба допустимы,
значит изменения нет. `MODIFIED-BY-US` здесь — след прошлого прогона:
сначала ROLLBACK.

### 2. MODIFY — вооружить тест

```sh
RUNF_WAIT=120 ./relay/runf.sh bq5 FUZZ_LOGFILE=1 FUZZ_MODE=scaler FUZZ_BQ5_MODIFY=1
```

Ожидаемо: `MODIFY w=… reparse key=1 — CHANGE LEFT IN PLACE`. Бэкап:
`Documents/bq5-DefaultAppQueryState.bak` в контейнере приложения
(переживает смерть процесса; UUID контейнера между запусками меняется —
путь извлекать из лога, не из памяти).

### 3. Внешние события (по одному, после каждого — CHECK)

1. **Ждать 5–10 минут** без действий — проверка «фонового перечитывания».
2. **Запустить несколько приложений** с экрана (SpringBoard — внешний
   наблюдатель, который не из песочницы).
3. **Respring** (если доступен способ оператора) — самый вероятный
   момент перечитывания LaunchServices.
4. **Reboot** — крайнее событие; после него `relay/resume.sh`.

После каждого события:

```sh
RUNF_WAIT=120 ./relay/runf.sh bq5 FUZZ_LOGFILE=1 FUZZ_MODE=scaler FUZZ_BQ5_CHECK=1
```

Фиксировать, какое событие к какой классификации привело.

### 4. ROLLBACK — обязательно, даже если тест «застрял»

```sh
RUNF_WAIT=120 ./relay/runf.sh bq5 FUZZ_LOGFILE=1 FUZZ_MODE=scaler FUZZ_BQ5_ROLLBACK=1
```

Ожидаемо: `byte-identical=1 (system restored)`.

### 5. Финальный CHECK

```sh
RUNF_WAIT=120 ./relay/runf.sh bq5 FUZZ_LOGFILE=1 FUZZ_MODE=scaler FUZZ_BQ5_CHECK=1
```

Ожидаемо: `CLEAN (== backup)`.

### 6. Параллельно: mDNS с Mac (§174, закрытые гипотезы)

Отдельно от plist-теста, ничего на устройстве не меняя:

```sh
dns-sd -B _services._dns-sd._udp local.
dns-sd -B _airplay._tcp local.
```

Если iPhone отвечает постороннему наблюдателю — mDNSResponder жив и
фильтрует песочницу (гипотеза A); если нет — демон заглох (гипотеза B).

## Запись результата

Результат каждого шага — в журнал `docs/SPTM_research_journal_part19.md`
следующей секцией (§178+): классификация после каждого события, вывод по
H1/H0, итог по mDNS-гипотезам. Логи шагов — в `results/vNNN-bq5-*.log`.
