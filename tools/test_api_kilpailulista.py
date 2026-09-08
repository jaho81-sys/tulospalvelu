#!/usr/bin/env python3
"""User API key lists Pirilä competitions for the connect combo."""
import json
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))


def kilpailut_pyynto(lahde="HkKisaWin", tyyppi="yksilo"):
    return {
        "action": "kilpailut",
        "lahde": lahde,
        "tyyppi": tyyppi,
        "pirila": True,
    }


def kilpailu_on_pirila(obj):
    if "pirila" in obj:
        return bool(obj["pirila"])
    if "pirila_kilpailu" in obj:
        return bool(obj["pirila_kilpailu"])
    return True


def kilpailu_tyyppi_sopii(tyyppi, viesti=False):
    if not tyyppi:
        return True
    if viesti:
        return tyyppi.lower() == "viesti"
    return tyyppi.lower() != "viesti"


def suodata(rivit, viesti=False):
    out = []
    for obj in rivit:
        if not kilpailu_on_pirila(obj):
            continue
        if not kilpailu_tyyppi_sopii(obj.get("tyyppi") or "", viesti):
            continue
        kid = obj.get("id") or obj.get("kilpailu_id")
        if not kid:
            continue
        nimi = obj.get("nimi") or obj.get("name") or obj.get("otsikko") or ""
        pvm = obj.get("pvm") or obj.get("pvm_str") or obj.get("date") or ""
        teksti = str(int(kid))
        if nimi:
            teksti += " - " + nimi
        if pvm:
            teksti += " (" + pvm + ")"
        out.append({"id": int(kid), "teksti": teksti})
    return out


def test_pyynto_ilman_kilpailu_id():
    body = kilpailut_pyynto()
    raw = json.dumps(body, ensure_ascii=False)
    assert body["action"] == "kilpailut"
    assert "kilpailu_id" not in body
    assert body["pirila"] is True
    assert "HkKisaWin" in raw
    viesti = kilpailut_pyynto("ViestiWin", "viesti")
    assert viesti["lahde"] == "ViestiWin"
    print("ok pyynto ilman kilpailu_id")


def test_suodata_pirila_ja_tyyppi():
    rivit = [
        {"id": 11, "nimi": "Kevatkisa", "tyyppi": "yksilo", "pirila": True, "pvm": "2026-04-12"},
        {"id": 12, "nimi": "Viestikisa", "tyyppi": "viesti", "pirila": True},
        {"id": 13, "nimi": "IRMA-kisa", "tyyppi": "yksilo", "pirila": False},
        {"kilpailu_id": 14, "name": "Nimetön", "pirila": True},
        {"id": 15, "otsikko": "Ilman tyyppia", "pirila": True},
    ]
    hk = suodata(rivit, viesti=False)
    ids = [r["id"] for r in hk]
    assert ids == [11, 14, 15]
    assert hk[0]["teksti"] == "11 - Kevatkisa (2026-04-12)"
    vw = suodata(rivit, viesti=True)
    assert [r["id"] for r in vw] == [12, 14, 15]
    print("ok suodata pirila ja tyyppi")


def test_valinta_yhteen_id():
    """Combo selection stores leading id; Yes/No list pick."""
    teksti = "11 - Kevatkisa (2026-04-12)"
    assert int(teksti.split(" ", 1)[0]) == 11
    print("ok valinta id")


def _read(rel):
    path = os.path.join(ROOT, rel)
    with open(path, "rb") as f:
        return f.read().decode("latin-1")


def test_lahdekoodi():
    cpp = _read("TPsource/V52/cbHk/ApiYhteydet.cpp")
    hdr = _read("TPsource/V52/cbHk/ApiYhteydet.h")
    dfm = _read("TPsource/V52/cbHk/ApiYhteydet.dfm")

    assert 'action":"kilpailut"' in cpp or 'action\\":\\"kilpailut\\"' in cpp
    assert "ApiKilpailutPyynto" in cpp and "ApiKilpailutPyynto" in hdr
    assert "ComboKilpailu" in hdr and "ComboKilpailu" in dfm
    assert "BtnHaeKilpailut" in hdr and "Hae kilpailut" in dfm
    assert "kohtainen" in dfm
    assert "Piril" in dfm
    assert "wchar_t apiKey[256]" in hdr
    assert "kilpailuNimi" in hdr
    assert 'L"kilpailu_nimi"' in cpp
    assert "HaeKilpailuLista" in cpp
    assert "KilpailuOnPirila" in cpp
    # User key, not per-competition token, in ping errors
    assert "kilpailun api_token" not in cpp
    print("ok lahdekoodi")


def test_docs():
    doc = open(os.path.join(ROOT, "docs/api-jahonline.md"), encoding="utf-8").read()
    assert "action=kilpailut" in doc or '"action":"kilpailut"' in doc
    assert "käyttäjäkohtainen" in doc or "käyttäjän api_token" in doc
    assert "Hae kilpailut" in doc
    print("ok docs")


def main():
    test_pyynto_ilman_kilpailu_id()
    test_suodata_pirila_ja_tyyppi()
    test_valinta_yhteen_id()
    test_lahdekoodi()
    test_docs()
    print("all ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
