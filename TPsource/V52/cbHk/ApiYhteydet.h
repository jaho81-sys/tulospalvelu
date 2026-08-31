// Pekka Pirila's sports timekeeping program (Finnish: tulospalveluohjelma)
// Copyright (C) 2015 Pekka Pirila
// JAHOnline-bridge extensions © jaho81-sys

#ifndef ApiYhteydetH
#define ApiYhteydetH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>

//---------------------------------------------------------------------------
// API configuration — JAHOnline /public/api/pirila_bridge.php (tai kilpailijat_bridge.php)
typedef struct {
	wchar_t apiUrl[512];           // Full bridge URL (preferred) or host base
	int apiPort;                   // 0 = use URL scheme default; else override
	wchar_t apiKey[128];           // Bearer = kilpailut.api_token
	int kilpailuId;                // JAHOnline kilpailu_id
	int lahetaKilpailijat;         // Push full roster + times (action=synkkaa)
	int vastaanottaKilpailijat;    // Pull roster from server (action=kilpailijat)
	int lahetaValiajat;            // Push live punches / splits (action=tapahtuma)
	int vastaanottaValiajat;       // Pull splits from JAHOnline (kilpailijat.valiajat)
	int lahetaTulokset;            // Alias: also push results with synkkaa
	int vastaanottaEiLahteneet;    // Apply DNS/DNF from pull
	int lahetysvali;               // Interval seconds
	int kaynnissa;                 // Sync thread active intent
} apiconfigtp;

class TFormApiYhteydet : public TForm
{
__published:
	TPageControl *PageControl1;
	TTabSheet *TabYhteys;
	TTabSheet *TabAsetukset;
	TTabSheet *TabTila;

	TLabel *LabelUrl;
	TEdit *EditUrl;
	TLabel *LabelPortti;
	TEdit *EditPortti;
	TLabel *LabelApiKey;
	TEdit *EditApiKey;
	TLabel *LabelKilpailuId;
	TEdit *EditKilpailuId;
	TButton *BtnYhteysTesti;
	TButton *BtnLahetaNyt;
	TButton *BtnHaeNyt;
	TLabel *LabelYhteysTila;

	TCheckBox *CBLahetaKilpailijat;
	TCheckBox *CBVastaanottaKilpailijat;
	TCheckBox *CBLahetaValiajat;
	TCheckBox *CBVastaanottaValiajat;
	TCheckBox *CBLahetaTulokset;
	TCheckBox *CBVastaanottaEiLahteneet;
	TLabel *LabelLahetysvali;
	TEdit *EditLahetysvali;
	TLabel *LabelSekunti;

	TMemo *MemoTila;
	TButton *BtnPaivitaTila;
	TButton *BtnTyhjennaMemo;
	TButton *BtnVieInternet;

	TButton *BtnOK;
	TButton *BtnPeruuta;
	TButton *BtnOhje;

	void __fastcall FormShow(TObject *Sender);
	void __fastcall BtnOKClick(TObject *Sender);
	void __fastcall BtnPeruutaClick(TObject *Sender);
	void __fastcall BtnYhteysTestiClick(TObject *Sender);
	void __fastcall BtnLahetaNytClick(TObject *Sender);
	void __fastcall BtnHaeNytClick(TObject *Sender);
	void __fastcall BtnPaivitaTilaClick(TObject *Sender);
	void __fastcall BtnTyhjennaMemoClick(TObject *Sender);
	void __fastcall BtnVieInternetClick(TObject *Sender);
	void __fastcall BtnOhjeClick(TObject *Sender);
	void __fastcall EditUrlChange(TObject *Sender);

private:
	void __fastcall LueTiedot(void);
	void __fastcall KirjoitaTiedot(void);
	void __fastcall PaivitaTila(UnicodeString viesti);
	void __fastcall TestaaYhteys(void);

public:
	__fastcall TFormApiYhteydet(TComponent* Owner);
};

extern PACKAGE TFormApiYhteydet *FormApiYhteydet;
extern PACKAGE apiconfigtp apiconfig;

// Config I/O + URL helper (usable from ApiSaike)
void ApiConfigNollaa(void);
void ApiConfigLataa(void);
void ApiConfigTallenna(void);
UnicodeString ApiBridgeUrl(void);
bool ApiHttpPostJson(const UnicodeString& url, const UnicodeString& jsonBody, UnicodeString& vastaus);
bool ApiHttpGetAuth(const UnicodeString& url, UnicodeString& vastaus);

#endif
