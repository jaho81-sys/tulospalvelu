#!/usr/bin/env python3
"""Protocol checks for bidirectional online-split sync (Hk + Viesti)."""
import json
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))


def test_tapahtuma_yksilo():
    body = {
        "action": "tapahtuma",
        "kilpailu_id": 11,
        "lahde": "HkKisaWin",
        "tapahtumat": [
            {"numero": 101, "piste": 1, "aika_sec": 1234, "lahde": "online"},
            {"numero": 101, "piste": 0, "aika_sec": 3456, "lahde": "online"},
        ],
    }
    raw = json.dumps(body, ensure_ascii=False)
    assert "tapahtuma" in raw
    assert body["tapahtumat"][0]["piste"] == 1
    assert body["tapahtumat"][1]["piste"] == 0
    print("ok tapahtuma yksilo")


def test_tapahtuma_viesti():
    body = {
        "action": "tapahtuma",
        "kilpailu_id": 11,
        "lahde": "ViestiWin",
        "tyyppi": "viesti",
        "tapahtumat": [
            {"numero": 42, "osuus": 2, "piste": 1, "aika_sec": 890, "lahde": "online"},
        ],
    }
    assert body["tapahtumat"][0]["osuus"] == 2
    print("ok tapahtuma viesti")


def apply_valiajat(local, inbound, vastaanotta=True):
    if not vastaanotta:
        return local
    out = dict(local)
    for va in inbound:
        p = va.get("piste")
        if p is None or p < 1:
            continue
        out[p] = va.get("aika_sec")
    return out


def test_valiajat_apply():
    local = {1: 100, 2: 0}
    inbound = [{"piste": 1, "aika_sec": 111}, {"piste": 2, "aika_sec": 222}]
    got = apply_valiajat(local, inbound, True)
    assert got == {1: 111, 2: 222}
    got2 = apply_valiajat(local, inbound, False)
    assert got2 == local
    print("ok valiajat apply")


def test_cpp_json_actions():
    hk = open(os.path.join(ROOT, "TPsource", "V52", "cbHk", "ApiSaike.cpp"),
              encoding="utf-8", errors="replace").read()
    vi = open(os.path.join(ROOT, "TPsource", "V52", "ViestiWin", "ApiSaike.cpp"),
              encoding="utf-8", errors="replace").read()
    assert r'\"action\":\"tapahtuma\"' in hk
    assert r'\"action\":\"tapahtuma\"' in vi
    assert r'\"tyyppi\":\"yksilo\"' in hk
    assert r'\"tyyppi\":\"viesti\"' in vi
    assert r'\"osuus\":' in vi
    print("ok cpp json actions")


SEK = 1000  # TPsource/V52/Tp/TpDef.h: SEK = 10*KSEK, KSEK = 10*CSEK, CSEK = 10


def api_tulos_sec(tl):
    """Mirror of ApiTulosSec: Pirilä ticks -> JAHOnline seconds."""
    if tl <= 0:
        return 0
    return int(tl // SEK)


def api_sec_to_ticks(sec):
    """Mirror of ApiSecToTicks: JAHOnline seconds -> Pirilä ticks."""
    if sec <= 0:
        return 0
    if sec >= 100000:
        return int(sec)
    return int(sec) * SEK


def test_aika_sec_units():
    ticks_45 = 45 * 60 * SEK
    assert ticks_45 == 2700000
    assert api_tulos_sec(ticks_45) == 2700
    assert api_sec_to_ticks(2700) == ticks_45
    assert api_sec_to_ticks(2700000) == 2700000  # legacy ticks inbound
    assert api_tulos_sec(1234 * SEK) == 1234  # docs example
    start = 10 * 3600 * SEK
    finish_clock = (10 * 3600 + 45 * 60) * SEK
    assert api_tulos_sec(finish_clock - start) == 2700
    # Sending clock ticks as duration would look like 10h45min.
    assert api_tulos_sec(finish_clock) == 38700
    print("ok aika_sec units")


def test_cpp_converts_ticks_to_seconds():
    hk = open(os.path.join(ROOT, "TPsource", "V52", "cbHk", "ApiSaike.cpp"),
              encoding="utf-8", errors="replace").read()
    vi = open(os.path.join(ROOT, "TPsource", "V52", "ViestiWin", "ApiSaike.cpp"),
              encoding="utf-8", errors="replace").read()
    for src, name in ((hk, "Hk"), (vi, "Viesti")):
        assert "static int ApiTulosSec(INT32 tl)" in src, name
        assert "tl / SEK" in src, name
        assert "ApiSecToTicks" in src, name
        assert r'\"aika_sec\":" + IntToStr(tsec)' in src, name
        assert r'\"aika_sec\":" + IntToStr((int)tls)' not in src, name
        assert r'\"aika_sec\":" + IntToStr((int)va)' not in src, name
    assert "ApiViestiTulosTicks" in vi
    assert "osTulos" in vi
    assert "ApiViestiInboundMaali" in vi
    assert "ApiViestiTapahtumaSec" in vi
    print("ok cpp converts ticks to seconds")


def test_source_hooks():
    files = {
        "cbHk/ApiSaike.cpp": ["tapahtuma", "lahetaValiajat", "valiajat", "yksilo"],
        "cbHk/UnitAjanotto.cpp": ["IlmoitaTapahtuma"],
        "cbHk/UnitEmit.cpp": ["IlmoitaTapahtuma", "IlmoitaLasna"],
        "cbHk/WinHk.dfm": ["JAHOnline API (synkka)"],
        "ViestiWin/ApiSaike.cpp": ["viesti", "osuus", "tapahtuma"],
        "ViestiWin/UnitAjanotto.cpp": ["IlmoitaTapahtuma"],
        "ViestiWin/UnitEmit.cpp": ["IlmoitaTapahtuma", "IlmoitaLasna"],
        "ViestiWin/UnitJoukkuetiedot.cpp": ["IlmoitaTapahtuma"],
        "ViestiWin/UnitMain.cpp": ["JahonlineApi1Click", "ApiVIntegration"],
        "ViestiWin/UnitMain.dfm": ["JAHOnline API (synkka)"],
        "cbHk/ApiYhteydet.dfm": ["online-rastit"],
    }
    for rel, needles in files.items():
        path = os.path.join(ROOT, "TPsource", "V52", rel)
        text = open(path, encoding="utf-8", errors="replace").read()
        for n in needles:
            if n not in text:
                raise AssertionError("%s missing %r" % (rel, n))
    print("ok source hooks")


def main():
    test_tapahtuma_yksilo()
    test_tapahtuma_viesti()
    test_valiajat_apply()
    test_cpp_json_actions()
    test_source_hooks()
    test_aika_sec_units()
    test_cpp_converts_ticks_to_seconds()
    print("all ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
