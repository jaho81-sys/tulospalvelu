#!/usr/bin/env python3
"""Logic and source checks for the Maastossa (in-terrain) window."""
import os
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

TMAALI0 = -24 * 3600 * 1000  # TpDef.h: -24*TUNTI, TUNTI = 3600*SEK, SEK=1000


def in_terrain(tark, tulos, poissa=u"PEVKH"):
    """No finish time; not absent/DNS/vacant/DNF/DSQ."""
    if tark in poissa:
        return False
    if tulos is not None and tulos > 0:
        return False
    return True


def p_aika_index(piste):
    """kilptietue::p_aika: va[piste+1]; va[1]=finish, va[2]=1st split."""
    return piste + 1


def nva_kaytossa(sarja_valuku, kilpparam_valuku, cap=60):
    nva = sarja_valuku if sarja_valuku is not None else 0
    if nva < kilpparam_valuku:
        nva = kilpparam_valuku
    if nva < 0:
        nva = 0
    if kilpparam_valuku >= 0 and nva > kilpparam_valuku:
        nva = kilpparam_valuku
    if nva > cap:
        nva = cap
    return nva


def lahto_aika(tlahto, enslahto):
    if tlahto != TMAALI0:
        return tlahto
    return enslahto


def lahto_paikka(lno, rata_lahto):
    parts = []
    if lno and lno > 0:
        parts.append(str(lno))
    if rata_lahto:
        parts.append(rata_lahto)
    return " ".join(parts)


def va_otsikko(p, va_matka=None):
    if va_matka and va_matka not in (u"", u"0"):
        return va_matka
    return u"%d.va" % p


def fill_row(nva, va_by_piste):
    """va_by_piste is 1-based split times from p_aika (duration)."""
    return [va_by_piste.get(p) for p in range(1, nva + 1)]


def test_filter():
    assert in_terrain(u"-", 0)
    assert in_terrain(u"N", 0)
    assert in_terrain(u"T", 0)
    assert in_terrain(u"I", None)
    assert not in_terrain(u"P", 0)
    assert not in_terrain(u"E", 0)
    assert not in_terrain(u"V", 0)
    assert not in_terrain(u"K", 0)
    assert not in_terrain(u"H", 0)
    assert not in_terrain(u"-", 2700)
    print("ok filter")


def test_va_index():
    # Old window used va[p] for p=1..nva, so p=1 was finish (va[1]).
    assert p_aika_index(0) == 1
    assert p_aika_index(1) == 2
    assert p_aika_index(3) == 4
    va = {0: 36300, 1: 0, 2: 612, 3: 1200}  # va[0]=start clock, [1]=finish empty
    nva = 2
    first_wrong = None
    first_right = None
    for p in range(1, nva + 1):
        if first_wrong is None and va.get(p, 0) > 0:
            first_wrong = p
        if first_right is None and va.get(p_aika_index(p), 0) > 0:
            first_right = p
    # Old va[p] labelled the first split as point 2; p_aika uses va[p+1] as 1.va.
    assert first_wrong == 2
    assert first_right == 1
    print("ok va index")


def test_nva_and_columns():
    assert nva_kaytossa(0, 3) == 3
    assert nva_kaytossa(2, 3) == 3
    assert nva_kaytossa(5, 3) == 3
    assert nva_kaytossa(-1, 2) == 2
    assert va_otsikko(1) == u"1.va"
    assert va_otsikko(2, u"2.4 km") == u"2.4 km"
    assert va_otsikko(1, u"0") == u"1.va"
    row = fill_row(3, {1: 612, 3: 1800})
    assert row == [612, None, 1800]
    print("ok nva columns")


def test_lahto():
    assert lahto_aika(TMAALI0, 10 * 3600 * 1000) == 10 * 3600 * 1000
    assert lahto_aika(11 * 3600 * 1000, 10 * 3600 * 1000) == 11 * 3600 * 1000
    assert lahto_paikka(2, u"S1") == u"2 S1"
    assert lahto_paikka(0, u"S1") == u"S1"
    assert lahto_paikka(1, u"") == u"1"
    assert lahto_paikka(0, u"") == u""
    print("ok lahto")


def test_window_rows():
    """Hk-like: start place + start time + every in-use split column."""
    series = [
        {"valuku": 0, "lno": 1, "enslahto": 10 * 3600 * 1000, "va_matka": [u"", u""]},
        {"valuku": 2, "lno": 2, "enslahto": 11 * 3600 * 1000, "va_matka": [u"3.1", u"5.0"]},
    ]
    nva = max(nva_kaytossa(s["valuku"], 2) for s in series)
    headers = [u"No", u"Sarja", u"Nimi", u"Seura", u"Status", u"Lpaikka", u"Lahto"]
    for p in range(1, nva + 1):
        headers.append(va_otsikko(p, series[1]["va_matka"][p - 1] if nva <= 2 else None))
    assert headers[-2:] == [u"3.1", u"5.0"] or headers[-2:] == [u"1.va", u"2.va"]
    assert nva == 2

    people = [
        {"id": 101, "srj": 0, "tark": u"-", "tulos": 0, "tlahto": TMAALI0,
         "rata_lahto": u"S1", "va": {1: 400}},
        {"id": 102, "srj": 1, "tark": u"P", "tulos": 0, "tlahto": 11 * 3600 * 1000,
         "rata_lahto": u"", "va": {}},
        {"id": 103, "srj": 1, "tark": u"-", "tulos": 2700, "tlahto": 11 * 3600 * 1000,
         "rata_lahto": u"", "va": {1: 800, 2: 1500}},
        {"id": 104, "srj": 1, "tark": u"N", "tulos": 0, "tlahto": 11 * 3600 * 1000,
         "rata_lahto": u"", "va": {1: 900}},
    ]
    rows = []
    for k in people:
        if not in_terrain(k["tark"], k["tulos"]):
            continue
        s = series[k["srj"]]
        rows.append({
            "id": k["id"],
            "lp": lahto_paikka(s["lno"], k["rata_lahto"]),
            "lahto": lahto_aika(k["tlahto"], s["enslahto"]),
            "va": fill_row(nva, k["va"]),
        })
    assert [r["id"] for r in rows] == [101, 104]
    assert rows[0]["lp"] == u"1 S1"
    assert rows[0]["lahto"] == 10 * 3600 * 1000  # series start fallback
    assert rows[0]["va"] == [400, None]
    assert rows[1]["lp"] == u"2"
    assert rows[1]["va"] == [900, None]
    print("ok window rows")


def test_source():
    hk = os.path.join(ROOT, "TPsource", "V52", "cbHk", "UnitMaastossa.cpp")
    vi = os.path.join(ROOT, "TPsource", "V52", "ViestiWin", "UnitMaastossa.cpp")
    hk_t = open(hk, encoding="utf-8", errors="replace").read()
    vi_t = open(vi, encoding="utf-8", errors="replace").read()
    for text, name in ((hk_t, "Hk"), (vi_t, "Viesti")):
        assert "MaastossaLahtoPaikka" in text, name
        assert "MaastossaMaxNva" in text, name
        assert "Lpaikka" in text, name
        assert ".va" in text, name
        assert "haerata" in text, name
        assert "Viim.va" not in text, name
        assert "va[p].vatulos" not in text, name
    assert "p_aika(p)" in hk_t
    assert "enslahto" in hk_t
    assert "lno[k_pv]" in hk_t
    assert "osTulos(os, p, false)" in vi_t
    assert "Lahto(os)" in vi_t
    assert "Sarjat[srj].lno" in vi_t
    for rel in (hk, vi):
        raw = open(rel, "rb").read()
        assert b'L"L\xe4ht\xf6"' in raw, rel
        assert b"L\xc3\xa4ht" not in raw, rel
        assert b"L\xe4ht" in raw, rel
    dfm_hk = open(os.path.join(ROOT, "TPsource", "V52", "cbHk", "UnitMaastossa.dfm"),
                  encoding="utf-8", errors="replace").read()
    dfm_vi = open(os.path.join(ROOT, "TPsource", "V52", "ViestiWin", "UnitMaastossa.dfm"),
                  encoding="utf-8", errors="replace").read()
    assert "Maastossa olevat kilpailijat" in dfm_hk
    assert "Maastossa olevat kilpailijat" in dfm_vi
    print("ok source")


def main():
    test_filter()
    test_va_index()
    test_nva_and_columns()
    test_lahto()
    test_window_rows()
    test_source()
    print("all ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
