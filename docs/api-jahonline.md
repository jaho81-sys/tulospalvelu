# JAHOnline API — kaksisuuntainen synkka

Pirilä (HkKisaWin ja ViestiWin) ↔ JAHOnline tulospalvelu.

## Miksi tämä tapa

Kilpailijatiedot elävät Pirilässä `kilptietue` / `KILP.DAT`-rakenteessa. Valmiit
CSV/XML/KILP.DAT-vientityökalut sopivat kertatuontiin, mutta kisapäivän
kaksisuuntaiseen synkkaan tarvitaan HTTP + Bearer-avain.

JAHOnline käyttää kilpailukohtaista `api_token`-arvoa (hallinta → kilpailu).
Pirilä lähettää saman avaimen: `Authorization: Bearer <api_token>`.

Erillistä siirto-ohjelmaa, CSV-bridgeä tai MySQL-välikantaa ei tarvita:
kaksisuuntainen synkka on HkKisaWinissä ja ViestiWinissä (valikko
**JAHOnline API (synkka)**). Osanottajat, läsnäolo, maaliajat ja
online-rastit kulkevat samaa siltaa.

- Asennusohje: [docs-site/docs/asentaminen/jahonline.md](../docs-site/docs/asentaminen/jahonline.md)
- JAHOnline-sivun `docs/pirila.md` -korvaus: [docs/jahonline-docs/pirila.md](jahonline-docs/pirila.md)

## Endpoint

Oletus-URL:

`https://jahonline.com/public/api/kilpailijat_bridge.php`

(Sama protokolla toimii myös `public/api/pirila_bridge.php`-polussa, jos se on käytössä.)

## Auth

| Header | Arvo |
|--------|------|
| `Authorization` | `Bearer <kilpailut.api_token>` |
| `X-API-Key` | sama (vaihtoehto) |

Bodyyn aina: `"kilpailu_id": <int>`

## Actions

### `ping`
```json
{"action":"ping","kilpailu_id":11}
```
→ `{ "status":"ok", "kilpailu": {...}, "server_time":"..." }`

### `kilpailijat` (JAHOnline → Pirilä)
```json
{"action":"kilpailijat","kilpailu_id":11}
```
→ lista kentillä: `numero`, `sukunimi`, `etunimi`, `nimi`, `seura`, `maa`,
`sarja_nimi`, `badge`/`emit_koodi`, `lasna`, `status`, `aika_sec`, `sija`,
`lahto_aika` / `pirila_lahto_at` / `lahto_sec`, `valiajat[]`

`lasna` on boolean (`true`, kun kilpailija on läsnä: ei poissa/ei-lähtenyt/vakantti).
`status` on `LASNA`, kun läsnäolo on merkitty mutta tulosta ei vielä ole; muuten
`OK` / `DNS` / `DNF` / `DSQ`.

### `synkkaa` (Pirilä → JAHOnline)
```json
{
  "action":"synkkaa",
  "kilpailu_id":11,
  "lahde":"HkKisaWin",
  "luo_puuttuvat":true,
  "kilpailijat":[
    {
      "numero":101,
      "sukunimi":"Meikäläinen",
      "etunimi":"Matti",
      "seura":"HSK",
      "sarja_nimi":"H21",
      "badge":123456,
      "lasna":true,
      "status":"LASNA",
      "aika_sec":null,
      "sija":null,
      "lahto_aika":"10:05:00",
      "pirila_lahto_at":"10:05:00",
      "lahto_sec":36300,
      "valiajat":[]
    }
  ]
}
```

`luo_puuttuvat:true` luo puuttuvat numerot JAHOnlineen (sarja nimellä).

Emit-luenta (leimantarkastus / `ESILUENTA`) merkitsee kilpailijan läsnäolevaksi
ja lähettää yhden kilpailijan `synkkaa`-sanoman heti (`lasna: true`).

Osanottaja- ja tulosmuutokset (nimi, seura, emit, **lähtöaika**, maali, status)
lähtevät nettiin samalla hetkellä kuin Pirilän sisäinen tiedonsiirto (`laheta`
`tallenna`-funktiossa). Ero: JAHOnline-jono ei vaadi `comfl` / RS-232 / TCP-asemaverkkoa.
Säie tyhjentää jonon noin 200 ms välein, kun kilpailu on auki, **Testaa (ping)**
on onnistunut (`kaynnissa`) ja taustasäie on käynnissä. Ilman avattua kisaa
synkkaa ei käynnistetä. **Lopeta synkka** pysäyttää säikeen ja nollaa `kaynnissa`.

Koko lista lähtee lisäksi Automaatio-välilehden välein (`lahetysvali`, oletus 10 s,
minimi 2 s) kun synkka on käynnissä. Tiedonsiirron
`YHTEYS=` / `COM=` -parametrit koskevat vain asemien välistä liikennettä, ei nettiä.

Lähtöaika menee kentissä `lahto_aika` ja `pirila_lahto_at` (kellonaika `HH:MM:SS`)
sekä `lahto_sec` (sekunnit vuorokaudesta). HkKisaWin käyttää kilpailijan `tlahto`-
arvoa, tai sarjan ensimmäistä lähtöä jos henkilökohtaista ei ole. ViestiWin
käyttää `Lahto(osuus)` (osuuden `ylahto` tai sarjan lähtö). JAHOnline kirjoittaa
nämä `kilpailijat.pirila_lahto_at`-kenttään ja synkkaa sarjan `lahdot`-rivin.

`synkkaa` sisältää myös `valiajat[]` (online-rastit / väliaikapisteet) ja
`"tyyppi":"yksilo"` tai `"tyyppi":"viesti"`. Ajanotto / rastileima lähettää
lisäksi heti:

### `tapahtuma` (live online-rasti)

```json
{
  "action":"tapahtuma",
  "kilpailu_id":11,
  "lahde":"HkKisaWin",
  "tapahtumat":[
    {"numero":101,"piste":1,"aika_sec":1234,"lahde":"online"}
  ]
}
```

Viesti (osuus 1-pohjainen JSON:ssa, 0-pohjainen Pirilässä):

```json
{
  "action":"tapahtuma",
  "kilpailu_id":11,
  "lahde":"ViestiWin",
  "tyyppi":"viesti",
  "tapahtumat":[
    {"numero":42,"osuus":2,"piste":1,"aika_sec":890,"lahde":"online"}
  ]
}
```

- `piste` **0** = maali, **≥ 1** = online-rasti / väliaikapiste
- Haku: `kilpailijat`-vastauksen `valiajat[]` kirjoitetaan paikalliseen kantaan
  kun *Vastaanota väliajat* on päällä
- JAHOnline-bridgen (`kilpailijat_bridge.php`) pitää hyväksyä `action=tapahtuma`
  ja kirjoittaa `valiajat` / maaliaika (viestissä myös `osuus`)

## Käyttö (HkKisaWin ja ViestiWin)

1. Valikko → **JAHOnline API (synkka)**
2. Aseta URL, API-avain, `kilpailu_id`
3. **Testaa (ping)** (vain kun kilpailu on auki — käynnistää taustasynkan)
4. **Lähetä kilpailijat nyt** / **Hae kilpailijat nyt**
5. **Lopeta synkka** pysäyttää taustasäikeen
6. Automaatio: välilehti *Automaatio* + **Testaa (ping)** + OK
   (`lahetysvali` = koko listan lähetysväli, oletus 10 s)
7. Osanottajakaavakkeen tallennus / lähtöajan muutos → `synkkaa` noin 200 ms:ssa
   kun synkka on käynnissä
8. Live-väliajat: rastileima ajanotossa → `tapahtuma` heti

Asetukset tallentuvat kilpailun kansioon: `jahonline_api.ini`.
Tiedosto luetaan automaattisesti, kun kilpailu avataan (`Initialisoi`).
Jos kilpailun kansiossa ei ole tiedostoa, vanha exe-kansion `jahonline_api.ini`
kopioidaan sinne ensimmäisellä avauksella.

## Toteutustiedostot

- `TPsource/V52/cbHk/ApiYhteydet.*` — UI + HTTP + ini (yhteinen)
- `TPsource/V52/cbHk/ApiSaike.*` — Hk-synkka
- `TPsource/V52/ViestiWin/ApiSaike.*` — viesti (joukkue + osuus)
- `TPsource/V52/cbHk/ApiJson.*` — kevyt JSON
- `TPsource/V52/cbHk/ApiIntegration.*` / `ApiHkIntegration.h` / `ApiVIntegration.h`
- Valikko: `WinHk.*` / `UnitMain.*` → `JahonlineApi1`
