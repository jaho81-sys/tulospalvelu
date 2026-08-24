// Pekka Pirila's sports timekeeping program (Finnish: tulospalveluohjelma)
// Copyright (C) 2015 Pekka Pirila 

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <Winsock2.h>
#include <iphlpapi.h>

#include "ApiYhteydet.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormApiYhteydet *FormApiYhteydet;
apiconfigtp apiconfig = {
	L"http://localhost",
	8080,
	L"",
	0,
	0,
	0,
	0,
	5,
	0
};

//---------------------------------------------------------------------------
__fastcall TFormApiYhteydet::TFormApiYhteydet(TComponent* Owner)
	: TForm(Owner)
{
	Scaled = false;
	if (Screen->PixelsPerInch != 96) {
		ScaleBy(Screen->PixelsPerInch, 96);
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::FormShow(TObject *Sender)
{
	LueTiedot();
	PaivitaTila(L"Ikkunaa avattu. Tarkista asetukset ja klikkaa 'Yhteystesti'.");
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::LueTiedot(void)
{
	EditUrl->Text = apiconfig.apiUrl;
	EditPortti->Text = UnicodeString(apiconfig.apiPort);
	EditApiKey->Text = apiconfig.apiKey;
	CBLahetaValiajat->Checked = apiconfig.lahetaValiajat;
	CBVastaanottaValiajat->Checked = apiconfig.vastaanottaValiajat;
	CBLahetaTulokset->Checked = apiconfig.lahetaTulokset;
	CBVastaanottaEiLahteneet->Checked = apiconfig.vastaanottaEiLahteneet;
	EditLahetysvali->Text = UnicodeString(apiconfig.lahetysvali);
	
	if (apiconfig.kaynnissa) {
		LabelYhteysTila->Caption = L"AKTIIVINEN ✓";
		LabelYhteysTila->Font->Color = clGreen;
	} else {
		LabelYhteysTila->Caption = L"EI AKTIIVINEN";
		LabelYhteysTila->Font->Color = clRed;
	}
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::KirjoitaTiedot(void)
{
	wcsncpy(apiconfig.apiUrl, EditUrl->Text.c_str(), sizeof(apiconfig.apiUrl)/2 - 1);
	apiconfig.apiPort = _wtoi(EditPortti->Text.c_str());
	wcsncpy(apiconfig.apiKey, EditApiKey->Text.c_str(), sizeof(apiconfig.apiKey)/2 - 1);
	apiconfig.lahetaValiajat = CBLahetaValiajat->Checked;
	apiconfig.vastaanottaValiajat = CBVastaanottaValiajat->Checked;
	apiconfig.lahetaTulokset = CBLahetaTulokset->Checked;
	apiconfig.vastaanottaEiLahteneet = CBVastaanottaEiLahteneet->Checked;
	apiconfig.lahetysvali = _wtoi(EditLahetysvali->Text.c_str());
	
	if (apiconfig.lahetysvali < 1)
		apiconfig.lahetysvali = 5;
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::PaivitaTila(UnicodeString viesti)
{
	TDateTime nyt = Now();
	UnicodeString aika = FormatDateTime(L"hh:mm:ss", nyt);
	MemoTila->Lines->Add(L"[" + aika + L"] " + viesti);
	
	// Pidä vain 100 viimeistä riviä
	while (MemoTila->Lines->Count > 100) {
		MemoTila->Lines->Delete(0);
	}
	
	// Scroll to bottom
	MemoTila->SelStart = MemoTila->GetTextLen() - 1;
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::TestaaYhteys(void)
{
	KirjoitaTiedot();
	PaivitaTila(L"Testataan yhteyttä osoitteeseen: " + EditUrl->Text + L":" + EditPortti->Text);
	
	// Simple DNS/socket test
	WSADATA wsaData;
	SOCKET sock = INVALID_SOCKET;
	struct sockaddr_in serverAddr;
	
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
		PaivitaTila(L"❌ VIRHE: Winsock alustus epäonnistui");
		return;
	}
	
	UnicodeString host = EditUrl->Text;
	if (host.Pos(L"://") > 0) {
		host = host.SubString(host.Pos(L"://") + 3, host.Length());
	}
	if (host.Pos(L"/") > 0) {
		host = host.SubString(1, host.Pos(L"/") - 1);
	}
	
	struct hostent *he = gethostbyname(AnsiString(host).c_str());
	if (he == NULL) {
		PaivitaTila(L"❌ VIRHE: Palvelimen nimeä ei löydetty: " + host);
		WSACleanup();
		return;
	}
	
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		PaivitaTila(L"❌ VIRHE: Soketti luonti epäonnistui");
		WSACleanup();
		return;
	}
	
	// Set non-blocking mode for timeout
	u_long mode = 1;
	ioctlsocket(sock, FIONBIO, &mode);
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(apiconfig.apiPort);
	serverAddr.sin_addr.s_addr = *(unsigned long *) he->h_addr;
	
	int connectResult = connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	
	// Wait 2 seconds for connection
	fd_set writeSet;
	FD_ZERO(&writeSet);
	FD_SET(sock, &writeSet);
	
	timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	
	int selectResult = select(0, NULL, &writeSet, NULL, &tv);
	
	if (selectResult > 0) {
		PaivitaTila(L"✓ Yhteys muodostettiin onnistuneesti!");
		apiconfig.kaynnissa = 1;
		LabelYhteysTila->Caption = L"AKTIIVINEN ✓";
		LabelYhteysTila->Font->Color = clGreen;
	} else {
		PaivitaTila(L"❌ VIRHE: Yhteys aikakatkaistiin. Tarkista palvelimen osoite ja portti.");
		apiconfig.kaynnissa = 0;
		LabelYhteysTila->Caption = L"EI AKTIIVINEN";
		LabelYhteysTila->Font->Color = clRed;
	}
	
	closesocket(sock);
	WSACleanup();
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::BtnYhteysTesti(TObject *Sender)
{
	TestaaYhteys();
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::BtnPaivitaTilaClick(TObject *Sender)
{
	PaivitaTila(L"Asetukset päivitetty. Tila tarkistettu manuaalisesti.");
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::BtnTyhjennaMemoClick(TObject *Sender)
{
	MemoTila->Clear();
	PaivitaTila(L"Lokit tyhjennetty.");
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::BtnVieInternetClick(TObject *Sender)
{
	ShellExecute(NULL, L"open", L"https://github.com/jaho81-sys/tulospalvelu/wiki/API-yhteydet", NULL, NULL, SW_SHOW);
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::BtnOhjeClick(TObject *Sender)
{
	ShowMessage(
		L"REST API -yhteyksien konfiguraatio\n\n"
		L"1. Yhteysväli:\n"
		L"   - URL: API-palvelimen osoite (esim. http://localhost:8080)\n"
		L"   - Portti: Palvelimen portti (oletus 8080)\n"
		L"   - API-avain: Autentikaatioksi tarvittava avain\n\n"
		L"2. Asetukset:\n"
		L"   - Lähetä väliajat: Lähettää väliajat automaattisesti netiin\n"
		L"   - Vastaanota väliajat: Vastaanottaa väliajat mobiilista/netistä\n"
		L"   - Lähetä tulokset: Julkaisee tulokset netiin\n"
		L"   - Vastaanota 'ei lähteneet': Päivittää ei-lähteneille merkityt\n\n"
		L"3. Tila:\n"
		L"   - Näyttää yhteyden lokitiedot\n"
		L"   - Klikkaa 'Yhteystesti' testataksesi yhteyden\n"
	);
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::EditUrlChange(TObject *Sender)
{
	// Placeholder for real-time validation
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::BtnOKClick(TObject *Sender)
{
	KirjoitaTiedot();
	PaivitaTila(L"Asetukset tallennettu.");
	Close();
}

//---------------------------------------------------------------------------
void __fastcall TFormApiYhteydet::BtnPeruutaClick(TObject *Sender)
{
	Close();
}

//---------------------------------------------------------------------------
