#!/usr/bin/env python3
"""ViestiWin must not embed cbHk DFMs that share a filename."""
import os

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
VI = os.path.join(ROOT, "TPsource", "V52", "ViestiWin")
HK = os.path.join(ROOT, "TPsource", "V52", "cbHk")
PROJ = os.path.join(ROOT, "TPsource", "V52", "RADStudio10", "ViestiWin.cbproj")


def test_colliding_pragmas():
    hk = set(n for n in os.listdir(HK) if n.endswith(".dfm"))
    n = 0
    for fn in os.listdir(VI):
        if not fn.endswith(".cpp"):
            continue
        dfm = fn[:-4] + ".dfm"
        if dfm not in hk:
            continue
        text = open(os.path.join(VI, fn), encoding="utf-8", errors="replace").read()
        needle = '#pragma resource "..\\\\ViestiWin\\\\%s"' % dfm
        if needle not in text:
            raise AssertionError("%s missing %s" % (fn, needle))
        if '#pragma resource "*.dfm"' in text:
            raise AssertionError("%s still uses *.dfm" % fn)
        n += 1
    if n < 30:
        raise AssertionError("too few colliding units: %d" % n)
    print("ok %d colliding pragmas" % n)


def test_status_dfm_no_oncreate():
    dfm = open(os.path.join(VI, "UnitStatus.dfm"), encoding="utf-8", errors="replace").read()
    hk = open(os.path.join(HK, "UnitStatus.dfm"), encoding="utf-8", errors="replace").read()
    assert "OnCreate = FormCreate" not in dfm
    assert "OnCreate = FormCreate" in hk
    print("ok status dfm split")


def test_brcc_path():
    text = open(PROJ, encoding="utf-8", errors="replace").read()
    assert r"<BRCC_IncludePath>..\ViestiWin;" in text
    assert r"-I..\ViestiWin;" in text
    print("ok brcc path")


def main():
    test_colliding_pragmas()
    test_status_dfm_no_oncreate()
    test_brcc_path()
    print("all ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
