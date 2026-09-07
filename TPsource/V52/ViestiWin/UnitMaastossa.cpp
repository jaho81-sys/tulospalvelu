// Pekka Pirila's sports timekeeping program (Finnish: tulospalveluohjelma)
// Copyright (C) 2015 Pekka Pirila

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitMaastossa.h"
#include "VDeclare.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMaastossa *FormMaastossa;
//---------------------------------------------------------------------------
__fastcall TFormMaastossa::TFormMaastossa(TComponent* Owner)
	: TForm(Owner)
{
	Scaled = false;
	if (Screen->PixelsPerInch != 96)
		ScaleBy(Screen->PixelsPerInch, 96);
}
//---------------------------------------------------------------------------
void __fastcall TFormMaastossa::asetaSarakkeet(void)
{
	static const wchar_t *ots[] = {
		L"No", L"Os", L"Sarja", L"Nimi", L"Seura", L"Status",
		L"Lähtö", L"Viim.va", L"Va-aika", L"Va-sija"
	};
	int w[] = {50, 40, 70, 180, 140, 110, 80, 60, 80, 60};
	Grid->ColCount = 10;
	Grid->FixedRows = 1;
	Grid->RowCount = 2;
	for (int c = 0; c < 10; c++) {
		Grid->Cells[c][0] = ots[c];
		Grid->ColWidths[c] = w[c] * Screen->PixelsPerInch / 96;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormMaastossa::tarkStr(wchar_t kh, wchar_t *line)
{
	switch (kh) {
	case L'N': wcscpy(line, L"Ilmoittautunut"); break;
	case L'T': wcscpy(line, L"Tark."); break;
	case L'I': wcscpy(line, L"esItys"); break;
	case L'K': wcscpy(line, L"Kesk"); break;
	case L'H': wcscpy(line, L"Hyl"); break;
	case L'E': wcscpy(line, L"Eiläht."); break;
	case L'P': wcscpy(line, L"Poissa"); break;
	case L'V': wcscpy(line, L"Vakantti"); break;
	default: wcscpy(line, L"Avoin"); break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormMaastossa::haeKilpailijat(void)
{
	int srjVal = CBSarja->ItemIndex - 1;
	int rivi = 1;
	wchar_t line[80], nimi[80];

	asetaSarakkeet();
	if (datf2.numrec < 2) {
		Grid->RowCount = 2;
		for (int c = 0; c < Grid->ColCount; c++)
			Grid->Cells[c][1] = L"";
		LblLkm->Caption = L"0 kilpailijaa";
		return;
	}

	for (int d = 1; d < datf2.numrec; d++) {
		kilptietue kilp;
		kilp.getrec(d);
		if (kilp.kilpstatus != 0 || kilp.KilpNo() <= 0)
			continue;
		int srj = kilp.sarja;
		if (srjVal >= 0 && srj != srjVal)
			continue;
		if (srj < 0 || srj >= sarjaluku)
			continue;
		int nos = Sarjat[srj].osuusluku;
		if (nos < 1)
			nos = 1;
		if (nos > MAXOSUUSLUKU)
			nos = MAXOSUUSLUKU;

		for (int os = 0; os < nos; os++) {
			wchar_t kh = kilp.wTark(os, 0);
			if (kh == L'P' || kh == L'E' || kh == L'V' || kh == L'K' || kh == L'H')
				continue;
			if (kilp.Maali(os, 0) != TMAALI0)
				continue;

			if (rivi >= Grid->RowCount)
				Grid->RowCount = rivi + 1;
			Grid->Cells[0][rivi] = UnicodeString(kilp.KilpNo());
			Grid->Cells[1][rivi] = UnicodeString(os + 1);
			Grid->Cells[2][rivi] = UnicodeString(Sarjat[srj].sarjanimi);
			Grid->Cells[3][rivi] = UnicodeString(kilp.Nimi(nimi, 79, os));
			Grid->Cells[4][rivi] = UnicodeString(kilp.seura);
			tarkStr(kh, line);
			Grid->Cells[5][rivi] = UnicodeString(line);
			INT32 tl = kilp.Lahto(os);
			if (tl != TMAALI0)
				Grid->Cells[6][rivi] = UnicodeString(AIKATOWSTRS(line, tl, t0)).SubString(1, 8);
			else
				Grid->Cells[6][rivi] = L"";

			int lastP = 0;
			INT32 lastT = 0;
			int lastSj = 0;
			int nva = Sarjat[srj].valuku[os];
			if (nva < 0) nva = 0;
			if (nva > VAIKALUKU) nva = VAIKALUKU;
			for (int p = 1; p <= nva; p++) {
				INT32 va = kilp.osTulos(os, p, false);
				if (va <= 0) {
					INT32 ma = kilp.Maali(os, p);
					if (ma == TMAALI0 || ma == 0)
						continue;
					va = ma;
				}
				lastP = p;
				lastT = va;
				lastSj = kilp.Sija(os, p);
			}
			if (lastP > 0) {
				Grid->Cells[7][rivi] = UnicodeString(lastP);
				Grid->Cells[8][rivi] = UnicodeString(AIKATOWSTRS(line, lastT, 0)).SubString(1, 8);
				if (lastSj > 0)
					Grid->Cells[9][rivi] = UnicodeString(lastSj);
				else
					Grid->Cells[9][rivi] = L"";
			}
			else {
				Grid->Cells[7][rivi] = L"";
				Grid->Cells[8][rivi] = L"";
				Grid->Cells[9][rivi] = L"";
			}
			rivi++;
		}
	}

	if (rivi == 1) {
		Grid->RowCount = 2;
		for (int c = 0; c < Grid->ColCount; c++)
			Grid->Cells[c][1] = L"";
	}
	else
		Grid->RowCount = rivi;
	LblLkm->Caption = IntToStr(rivi - 1) + L" kilpailijaa ilman tulosta";
}
//---------------------------------------------------------------------------
void __fastcall TFormMaastossa::FormShow(TObject *Sender)
{
	int vanha = CBSarja->ItemIndex;
	CBSarja->Items->BeginUpdate();
	CBSarja->Clear();
	CBSarja->Items->Add(L"Kaikki sarjat");
	for (int srj = 0; srj < sarjaluku; srj++)
		CBSarja->Items->Add(UnicodeString(Sarjat[srj].sarjanimi));
	CBSarja->Items->EndUpdate();
	if (vanha >= 0 && vanha < CBSarja->Items->Count)
		CBSarja->ItemIndex = vanha;
	else
		CBSarja->ItemIndex = 0;
	asetaSarakkeet();
	haeKilpailijat();
}
//---------------------------------------------------------------------------
void __fastcall TFormMaastossa::FormResize(TObject *Sender)
{
	if (Grid)
		Grid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TFormMaastossa::BtnHaeClick(TObject *Sender)
{
	haeKilpailijat();
}
//---------------------------------------------------------------------------
void __fastcall TFormMaastossa::BtnSuljeClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMaastossa::CBSarjaChange(TObject *Sender)
{
	haeKilpailijat();
}
//---------------------------------------------------------------------------
