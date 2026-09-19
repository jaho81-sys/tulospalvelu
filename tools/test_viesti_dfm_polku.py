#!/usr/bin/env python3
"""ViestiWin must embed uniquely named DFMs, not cbHk UnitStatus.dfm."""
import os

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
VI = os.path.join(ROOT, "TPsource", "V52", "ViestiWin")
HK = os.path.join(ROOT, "TPsource", "V52", "cbHk")
PROJ = os.path.join(ROOT, "TPsource", "V52", "RADStudio10", "ViestiWin.cbproj")


def test_unique_v_dfms():
    hk = set(n for n in os.listdir(HK) if n.endswith(".dfm"))
    n = 0
    for fn in os.listdir(VI):
        if not fn.endswith(".cpp"):
            continue
        dfm = fn[:-4] + ".dfm"
        if dfm not in hk:
            continue
        vdfm = fn[:-4] + "_v.dfm"
        text = open(os.path.join(VI, fn), encoding="utf-8", errors="replace").read()
        needle = '#pragma resource "%s"' % vdfm
        if needle not in text:
            raise AssertionError("%s missing %s" % (fn, needle))
        if '#pragma resource "*.dfm"' in text:
            raise AssertionError("%s still uses *.dfm" % fn)
        if not os.path.isfile(os.path.join(VI, vdfm)):
            raise AssertionError("missing %s" % vdfm)
        n += 1
    if n < 30:
        raise AssertionError("too few colliding units: %d" % n)
    print("ok %d unique _v.dfm pragmas" % n)


def test_status_formcreate():
    dfm = open(os.path.join(VI, "UnitStatus.dfm"), encoding="utf-8", errors="replace").read()
    vdfm = open(os.path.join(VI, "UnitStatus_v.dfm"), encoding="utf-8", errors="replace").read()
    hdr = open(os.path.join(VI, "UnitStatus.h"), encoding="utf-8", errors="replace").read()
    cpp = open(os.path.join(VI, "UnitStatus.cpp"), encoding="utf-8", errors="replace").read()
    hk = open(os.path.join(HK, "UnitStatus.dfm"), encoding="utf-8", errors="replace").read()
    assert "OnCreate = FormCreate" in dfm
    assert "OnCreate = FormCreate" in vdfm
    assert "OnCreate = FormCreate" in hk
    assert "void __fastcall FormCreate(TObject *Sender);" in hdr
    assert "TFormStatus::FormCreate" in cpp
    assert '#pragma resource "UnitStatus_v.dfm"' in cpp
    print("ok status FormCreate")


def test_brcc_and_formresources():
    text = open(PROJ, encoding="utf-8", errors="replace").read()
    assert r"<BRCC_IncludePath>..\ViestiWin;" in text
    assert r"..\ViestiWin\UnitStatus_v.dfm" in text
    assert r'<FormResources Include="..\ViestiWin\UnitStatus.dfm"/>' not in text
    print("ok project resources")


def main():
    test_unique_v_dfms()
    test_status_formcreate()
    test_brcc_and_formresources()
    print("all ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
