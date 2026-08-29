# JAHOnline API — kaksisuuntainen kilpailijasiirto

Pirilä (HkKisaWin) ↔ JAHOnline tulospalvelu.

## Miksi tämä tapa

Kilpailijatiedot elävät Pirilässä `kilptietue` / `KILP.DAT`-rakenteessa. Valmiit
CSV/XML/KILP.DAT-vientityökalut sopivat kertatuontiin, mutta kisapäivän
kaksisuuntaiseen synkkaan tarvitaan HTTP + Bearer-avain.

JAHOnline käyttää kilpailukohtaista `api_token`-arvoa (hallinta → kilpailu).
Pirilä lähettää saman avaimen: `Authorization: Bearer <api_token>`.

Erillistä siirto-ohjelmaa, CSV-bridgeä tai MySQL-välikantaa ei tarvita:
kaksisuuntainen synkka on HkKisaWinissä (valikko **JAHOnline API (kilpailijat)**).

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
`sarja_nimi`, `badge`/`emit_koodi`, `lasna`, `status`, `aika_sec`, `sija`, `valiajat[]`

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
      "valiajat":[]
    }
  ]
}
```

`luo_puuttuvat:true` luo puuttuvat numerot JAHOnlineen (sarja nimellä).

Emit-luenta (leimantarkastus / `ESILUENTA`) merkitsee kilpailijan läsnäolevaksi
ja lähettää yhden kilpailijan `synkkaa`-sanoman heti (`lasna: true`).

## Käyttö HkKisaWinissä

1. Valikko → **JAHOnline API (kilpailijat)**
2. Aseta URL, API-avain, `kilpailu_id`
3. **Testaa (ping)**
4. **Lähetä kilpailijat nyt** / **Hae kilpailijat nyt**
5. Automaatio: välilehti *Automaatio* + OK (säie käynnistyy)

Asetukset tallentuvat: `jahonline_api.ini` (exe-kansion viereen).

## Toteutustiedostot

- `TPsource/V52/cbHk/ApiYhteydet.*` — UI + HTTP + ini
- `TPsource/V52/cbHk/ApiSaike.*` — synkka (lähetys/haku)
- `TPsource/V52/cbHk/ApiJson.*` — kevyt JSON
- `TPsource/V52/cbHk/ApiIntegration.*` / `ApiHkIntegration.h` — elinkaari
- Valikko: `WinHk.*` → `JahonlineApi1`
