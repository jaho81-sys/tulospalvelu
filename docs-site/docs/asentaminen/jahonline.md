# Pirilä synkka (JAHOnline)

Kilpailijat, läsnäolo ja ajat siirtyvät **HkKisaWinistä** suoraan
JAHOnline-tulospalveluun. **Erillistä siirto-ohjelmaa, CSV-bridgeä tai
MySQL-välikantaa ei tarvita.**

HkKisaWin-valikko: **JAHOnline API (kilpailijat)**.

Suunnistuksessa osanottajat elävät Pirilässä (IRMA / CSV / käsin). JAHOnline
näyttää ne synkan jälkeen. Emit-luenta merkitsee lähtijän läsnäolevaksi ja
lähettää tiedon heti nettiin.

## Käyttöönotto

1. Asenna HkKisaWin tavalliseen tapaan ([asennuksen vaiheet](asennuksen_vaiheet.md)).
2. Avaa kilpailu HkKisaWinissä.
3. Valitse **JAHOnline API (kilpailijat)**.
4. Täytä:
   - **Bridge-URL:** `https://jahonline.com/public/api/kilpailijat_bridge.php`
     (oletus; yleensä ei tarvitse muuttaa)
   - **API-avain:** JAHOnline-hallinnan kilpailukohtainen `api_token`
     (Hallinta → Kilpailu)
   - **kilpailu_id:** saman kilpailun tunnus JAHOnlinessa
5. Paina **Testaa (ping)**. Onnistunut ping aktivoi yhteyden.
6. **Lähetä kilpailijat nyt** vie Pirilän osanottajat (nimet, emit-koodit, ajat,
   läsnäolo) JAHOnlineen.
7. **Hae kilpailijat nyt** vain jos netistä pitää tuoda päivityksiä takaisin
   paikalliseen `KILP.DAT`-kantaan.

Asetukset tallentuvat tiedostoon `jahonline_api.ini` ohjelman exe-kansion viereen.
Salaisuuksia ei tarvitse asettaa muualle.

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

Automaattinen taustasynkka (välilehti *Automaatio*) lähettää lisäksi koko
osanottajalistan valitun välein, kun yhteys on aktiivinen.

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
- HkKisaWin-valikko: **JAHOnline API (kilpailijat)**
