# Pirilä synkka (JAHOnline)

Kilpailijat, läsnäolo, maaliajat ja online-rastit siirtyvät **HkKisaWinistä**
ja **ViestiWinistä** suoraan JAHOnline-tulospalveluun. **Erillistä
siirto-ohjelmaa, CSV-bridgeä tai MySQL-välikantaa ei tarvita.**

Valikko (HkKisaWin ja ViestiWin): **JAHOnline API (synkka)**.

Suunnistuksessa osanottajat elävät Pirilässä (IRMA / CSV / käsin). JAHOnline
näyttää ne synkan jälkeen. Emit-luenta merkitsee lähtijän läsnäolevaksi ja
lähettää tiedon heti nettiin.

## Käyttöönotto

1. Asenna HkKisaWin / ViestiWin tavalliseen tapaan ([asennuksen vaiheet](asennuksen_vaiheet.md)).
2. Avaa kilpailu HkKisaWinissä tai ViestiWinissä.
3. Valitse **JAHOnline API (synkka)**.
4. Täytä:
   - **Bridge-URL:** `https://jahonline.com/public/api/kilpailijat_bridge.php`
     (oletus; yleensä ei tarvitse muuttaa)
   - **API-avain:** JAHOnline-hallinnan kilpailukohtainen `api_token`
     (Hallinta → Kilpailu)
   - **kilpailu_id:** saman kilpailun tunnus JAHOnlinessa
5. Paina **Testaa (ping)**. Onnistunut ping aktivoi yhteyden.
6. **Lähetä kilpailijat nyt** vie Pirilän osanottajat (nimet, emit-koodit, ajat,
   lähtöajat, läsnäolo, online-väliajat) JAHOnlineen.
7. **Hae kilpailijat nyt** vain jos netistä pitää tuoda päivityksiä takaisin
   paikalliseen `KILP.DAT`-kantaan.

Asetukset tallentuvat tiedostoon `jahonline_api.ini` **kilpailun kansioon**
(sama hakemisto kuin `laskenta.cfg` / `KILP.DAT`). Tiedosto luetaan, kun
kilpailu avataan. Jos kansiossa ei ole tiedostoa, vanha exe-kansion ini
kopioidaan sinne.

## Läsnäolo emit-luennasta

Kun kilpailijan emit-kortti luetaan (leimantarkastus tai parametrin
`ESILUENTA` mukainen lähtöluenta), ohjelma:

1. yhdistää kortin kilpailijan tietoihin (`KILP.DAT`)
2. merkitsee kilpailijan **läsnäolevaksi**, jos hänet oli merkitty
   poissaolevaksi, ei-lähteneeksi tai vakantiksi
3. lähettää läsnäolotiedon heti JAHOnlineen (`lasna: true`, status `LASNA`
   kun tulosta ei vielä ole)

Lähdön esiluenta otetaan käyttöön parametrilla `ESILUENTA` yhdessä
lukijamäärityksen `LUKIJA=…` kanssa. Katso myös
[Emitkoodien kirjaus ja lähtijöiden kirjaus](../hkkisawin/6.11_emitkoodien_lukeminen_kilpailijoille.md).

Osanottajakaavakkeen tallennus (nimi, seura, emit, lähtöaika, tulos) jonoittaa
muutoksen samaan tapaan kuin Pirilän sisäinen tiedonsiirto. Jono lähtee nettiin
noin 200 ms välein, kun API-avain ja `kilpailu_id` on asetettu. Asemaverkon
`YHTEYS=` / `comfl` ei ole edellytys.

Automaattinen taustasynkka (välilehti *Automaatio*) lähettää lisäksi koko
osanottajalistan valitun välein (`lahetysvali`, oletus 10 s), kun ping on
onnistunut.

## Online-rastit (kaksisuuntainen vienti)

Kyllä: online-väliajat kulkevat saman JAHOnline-bridgen kautta, sekä
henkilökohtaisessa kisassa että viestissä.

- Ajanotto / rastileima → heti `action=tapahtuma` (`piste` 0 = maali, ≥ 1 = online)
- Täysi lista menee `synkkaa`-sanoman `valiajat[]`-kentässä
- Haku JAHOnlinesta kirjoittaa väliajat takaisin `KILP.DAT`:iin
- Viestissä jokaisella rivillä on `osuus` (1, 2, 3…)

JAHOnlinen tulossivu näyttää online-sarakkeet (`sarjat.valia_lkm`) ja
rastiväliajat vasta leimantarkastuksen jälkeen.

## Mitä ei enää tarvita

- Erillistä CSV-bridgeä (`pirila_live.php`) tai Sync Manager -työpöytäohjelmaa
- MySQL-välikantaa (`UnitMySQL` / `kilpsql.txt`) kisapäivän kilpailijasiirtoon
- Manuaalista tiedostokopiota JAHOnlineen pelkkien osanottajatietojen vuoksi

Keräilylähtöiset CSV/IRMA/Kilmo-tuonnit HkKisaWinissä toimivat edelleen
kertatuontiin **ennen** kisaa.

## Lisätietoja

- JAHOnline-sivun korvaava ohje (kopioitavaksi palvelimelle
  `docs/pirila.md`): [docs/jahonline-docs/pirila.md](https://github.com/jaho81-sys/tulospalvelu/blob/cursor/api-kilpailijat-kaksisuuntainen-90ec/docs/jahonline-docs/pirila.md)
- Protokolla: [JAHOnline API](https://github.com/jaho81-sys/tulospalvelu/blob/cursor/api-kilpailijat-kaksisuuntainen-90ec/docs/api-jahonline.md)
  (ping / synkkaa / kilpailijat, Bearer-avain)
- Valikko (HkKisaWin ja ViestiWin): **JAHOnline API (synkka)**
