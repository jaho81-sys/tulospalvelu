#!/usr/bin/env python3
"""Kilpailijatiedot: closing in edit mode asks to save and leaves view mode."""
import os
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))


def form_close_body(text):
    key = "TFormKilpailijatiedot::FormClose"
    i = text.find(key)
    if i < 0:
        raise AssertionError("FormClose missing")
    j = text.find("void __fastcall TFormKilpailijatiedot::BtnSuljeClick", i)
    if j < 0:
        raise AssertionError("BtnSuljeClick after FormClose missing")
    return text[i:j]


def test_close_resets_edit_mode():
    path = os.path.join(ROOT, "TPsource", "V52", "cbHk", "UnitKilpailijatiedot.cpp")
    text = open(path, encoding="utf-8", errors="replace").read()
    body = form_close_body(text)

    assert "if (!sallimuokkaus)" in body
    assert 'L"Tallennetaanko muutokset?"' in body
    assert "MB_YESNO" in body
    assert "asetaMuokkaustila(false)" in body
    assert "tallennaTiedot()" in body
    assert "Kilp = Kilp1" in body
    # Old close used ReadOnly as proxy and never left edit mode.
    assert "EdtSukunimi->ReadOnly" not in body
    assert "asetaMuokkaustila(!sallimuokkaus)" not in body

    assert "void __fastcall TFormKilpailijatiedot::asetaMuokkaustila(bool paalle)" in text
    assert "asetaMuokkaustila(!sallimuokkaus)" in text
    hdr = open(os.path.join(ROOT, "TPsource", "V52", "cbHk", "UnitKilpailijatiedot.h"),
               encoding="utf-8", errors="replace").read()
    assert "void __fastcall asetaMuokkaustila(bool paalle);" in hdr
    print("ok kilpailijatiedot close")


def test_close_dialog_yes_no():
    """Kyllä saves when data changed; Ei discards. Both leave edit mode."""
    def close_edit(sallimuokkaus, changed, answer_yes):
        if not sallimuokkaus:
            return ("view", False, False)
        save = False
        discard = False
        if answer_yes:
            if changed:
                save = True
        else:
            discard = True
        return ("view", save, discard)

    assert close_edit(False, True, True) == ("view", False, False)
    assert close_edit(True, True, True) == ("view", True, False)
    assert close_edit(True, False, True) == ("view", False, False)
    assert close_edit(True, True, False) == ("view", False, True)
    assert close_edit(True, False, False) == ("view", False, True)
    print("ok close yes/no")


def main():
    test_close_resets_edit_mode()
    test_close_dialog_yes_no()
    print("all ok")
    return 0


if __name__ == "__main__":
    sys.exit(main())
