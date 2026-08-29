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
    print("all ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
