# JAHOnline — kaksisuuntainen kilpailijasiirto

Kilpailijatiedot siirretään **HkKisaWinistä** suoraan JAHOnline-tulospalveluun
ja takaisin. **Erillistä siirto-ohjelmaa ei tarvita.**

HkKisaWin sisältää valikon *JAHOnline API (kilpailijat)*. Sillä haetaan
ilmoittautumiset netistä, lähetetään osanottajat ja ajat takaisin, ja merkitään
läsnäolo, kun emit-kortti luetaan lähdössä.

## Mitä asennuksen jälkeen tarvitaan

1. Asenna HkKisaWin tavalliseen tapaan ([asennuksen vaiheet](asennuksen_vaiheet.md)).
2. Avaa kilpailu HkKisaWinissä.
3. Valitse **JAHOnline API (kilpailijat)**.
4. Täytä:
   - **Bridge-URL:** `https://jahonline.com/public/api/kilpailijat_bridge.php`
     (oletus; yleensä ei tarvitse muuttaa)
   - **API-avain:** JAHOnline-hallinnan kilpailukohtainen `api_token`
   - **kilpailu_id:** saman kilpailun tunnus JAHOnlinessa
5. Paina **Testaa (ping)**. Onnistunut ping aktivoi yhteyden.
6. **Hae kilpailijat nyt** tuo osanottajat JAHOnlinesta paikalliseen `KILP.DAT`-kantaan.
7. **Lähetä kilpailijat nyt** vie Pirilän osanottajat (nimet, emit-koodit, ajat,
   läsnäolo) JAHOnlineen.

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

- Erillistä CSV/XML-vientiohjelmaa kisapäivän kaksisuuntaiseen synkkaan
- Erillistä “bridge”-työpöytäohjelmaa kilpailijatietojen siirtoon
- Manuaalista tiedostokopiota JAHOnlineen pelkkien osanottajatietojen vuoksi

Keräilylähtöiset CSV/IRMA/Kilmo-tuonnit HkKisaWinissä toimivat edelleen
kertatuontiin ennen kisaa.

## Lisätietoja

- Protokolla: [JAHOnline API](https://github.com/jaho81-sys/tulospalvelu/blob/main/docs/api-jahonline.md)
  (ping / synkkaa / kilpailijat, Bearer-avain)
- HkKisaWin-valikko: **JAHOnline API (kilpailijat)**
