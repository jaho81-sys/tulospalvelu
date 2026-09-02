# Pirilä synkka

Kilpailijat, läsnäolo, maaliajat ja **online-rastit** siirtyvät **HkKisaWinistä**
ja **ViestiWinistä** suoraan JAHOnlineen HTTP-APIn kautta.

**Erillistä siirto-ohjelmaa, CSV-bridgeä (`pirila_live.php`), Sync Manageria
tai MySQL-välikantaa (`UnitMySQL` / `kilpsql.txt`) ei tarvita.**

Lähdekoodi: [jaho81-sys/tulospalvelu](https://github.com/jaho81-sys/tulospalvelu)
(valikko **JAHOnline API (synkka)**).

## Käyttöönotto (HkKisaWin / ViestiWin)

1. Avaa kilpailu HkKisaWinissä tai ViestiWinissä.
2. Valitse **JAHOnline API (synkka)**.
3. Täytä:
   - **Bridge-URL:** `https://jahonline.com/public/api/kilpailijat_bridge.php`
     (oletus)
   - **API-avain:** tämän kilpailun `api_token` (Hallinta → Kilpailu)
   - **kilpailu_id:** tämän kilpailun tunnus
4. Paina **Testaa (ping)**.
5. **Lähetä kilpailijat nyt** vie osanottajat (nimet, emit, ajat, lähtöajat,
   läsnäolo, online-väliajat) Pirilästä JAHOnlineen.
6. **Hae kilpailijat nyt** vain jos netistä pitää tuoda päivityksiä takaisin
   `KILP.DAT`:iin.

Asetukset tallentuvat tiedostoon `jahonline_api.ini` ohjelman exe-kansion
viereen.

Suunnistuksessa osanottajat elävät Pirilässä (IRMA / CSV / käsin). JAHOnline
näyttää ne synkan jälkeen — erillistä ilmoittautumislomaketta ei käytetä.

## Läsnäolo (emit)

Kun emit-kortti luetaan (leimantarkastus tai `ESILUENTA`-lähtöluenta),
HkKisaWin:

1. yhdistää kortin kilpailijaan
2. merkitsee poissa / ei-lähtenyt / vakantti -osanottajan **läsnäolevaksi**
3. lähettää heti `synkkaa`-sanoman (`lasna: true`, status `LASNA` jos tulosta
   ei vielä ole)

Taustasynkka (välilehti *Automaatio*) lähettää koko listan valitun välein.

## API

| | |
|--|--|
| Endpoint | `POST /public/api/kilpailijat_bridge.php` |
| Auth | `Authorization: Bearer <api_token>` tai `X-API-Key` |
| Body | aina `"kilpailu_id": <int>` |

| Action | Suunta | Käyttö |
|--------|--------|--------|
| `ping` | — | Yhteystesti |
| `synkkaa` | Pirilä → JAHOnline | Osanottajat, ajat, lähtöajat, läsnäolo, `valiajat[]` |
| `kilpailijat` | JAHOnline → Pirilä | Haku `KILP.DAT`:iin (myös väliajat) |
| `tapahtuma` | live molempiin | Online-rasti heti (`piste`, `aika_sec`, viestissä `osuus`) |

HkKisaWin: `"tyyppi":"yksilo"`. ViestiWin: `"tyyppi":"viesti"` + `"osuus"`.

Protokolla: [docs/api-jahonline.md](https://github.com/jaho81-sys/tulospalvelu/blob/cursor/api-kilpailijat-kaksisuuntainen-90ec/docs/api-jahonline.md).

## Kaksi “väliaika”-käsitettä (näyttö)

Nämä koskevat sitä, miten JAHOnline **näyttää** synkatut tiedot — ei erillistä
siirto-ohjelmaa.

1. **Online-väliaikapisteet** (radio / maasto) — sarakkeet `1. va`, `2. va`…
   tuloksissa. Määrä = `sarjat.valia_lkm` (admin).
2. **Rastiväliajat** (koko Emit-rata) — vasta **leimantarkastuksen jälkeen**
   (kilpailija maalissa). Seuranta → Rastiväliajat tai
   `/public/valiajat.php?kilpailu_id=…&numero=…`.

| Käsite | JAHOnline |
|--------|-----------|
| Maali | `ajat` |
| Online-väliaika | `valiajat` (adminin `valia_lkm`) |
| Emit-rastit | rastiväliajat-näkymä, vain maalissa olevilta |
| Läsnä / DNS / DNF / DSQ | `lasna` + `status` (`LASNA` / `OK` / `DNS` / `DNF` / `DSQ`) |
| Lähtöaika | `lahto_aika` / `pirila_lahto_at` / `lahto_sec` → `kilpailijat` + sarjan `lahdot` |

## Seuranta / tulokset

- Lista: **maalissa → maastossa → status** (DNF / DNS / DSQ)
- Tulokset: **online-va + maaliaika + ero kärkeen**
- Rastiväliajat: Emit-sarakkeet vain maalissa olevilta
- Maastossa: kulunut aika suluissa; ei Emit-rastilistaa

## Mitä ei käytetä kisapäivän kilpailijasiirtoon

- Sync Manager / `tools/JAHOnlineSyncManager`
- CSV-bridge `POST /api/pirila_live.php`
- MySQL-välikanta (`UnitMySQL.cpp`, `kilpsql.txt`, `osanotot` / `osottopv`)

MySQL-luku HkKisaWinissä (Tiedostot / MySQL) on yhä olemassa vanhaan
tietokantakäyttöön, mutta JAHOnline-synkka ei kulje sen kautta.

## Online-rastit

Ajanotto ja rastileima lähettävät heti `action=tapahtuma`
(`piste` 0 = maali, ≥ 1 = online). Viestissä mukana `osuus` (1, 2, 3…).
Täysi lista kulkee myös `synkkaa`-sanoman `valiajat[]`-kentässä.

JAHOnline-bridgen pitää hyväksyä `action=tapahtuma` ja kirjoittaa
`valiajat` / maaliaika (viestissä myös `osuus`).

## Ei vielä

- Sakot / tarkemmat leimausvirheet UI:ssa
