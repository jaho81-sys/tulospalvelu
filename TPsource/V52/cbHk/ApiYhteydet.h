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

#ifndef ApiYhteydetH
#define ApiYhteydetH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>

//---------------------------------------------------------------------------
// API configuration structure
typedef struct {
	wchar_t apiUrl[256];           // API server URL (e.g., http://localhost:8080)
	int apiPort;                   // API port
	wchar_t apiKey[128];           // API key / authentication token
	int lahetaValiajat;            // Send split times automatically
	int vastaanottaValiajat;       // Receive split times from API
	int lahetaTulokset;            // Publish results to API
	int vastaanottaEiLahteneet;    // Receive "didn't start" from API
	int lahetysvali;               // Send interval in seconds
	int kaynnissa;                 // Connection active
} apiconfigtp;

//---------------------------------------------------------------------------
class TFormApiYhteydet : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabYhteys;
	TTabSheet *TabAsetukset;
	TTabSheet *TabTila;
	
	// Connection tab
	TLabel *LabelUrl;
	TEdit *EditUrl;
	TLabel *LabelPortti;
	TEdit *EditPortti;
	TLabel *LabelApiKey;
	TEdit *EditApiKey;
	TButton *BtnYhteysTesti;
	TLabel *LabelYhteysTila;
	
	// Settings tab
	TCheckBox *CBLahetaValiajat;
	TCheckBox *CBVastaanottaValiajat;
	TCheckBox *CBLahetaTulokset;
	TCheckBox *CBVastaanottaEiLahteneet;
	TLabel *LabelLahetysvali;
	TEdit *EditLahetysvali;
	TLabel *LabelSekunti;
	
	// Status tab
	TMemo *MemoTila;
	TButton *BtnPaivitaTila;
	TButton *BtnTyhjennaMemo;
	TButton *BtnVieInternet;
	
	// Buttons
	TButton *BtnOK;
	TButton *BtnPeruuta;
	TButton *BtnOhje;
	
	void __fastcall FormShow(TObject *Sender);
	void __fastcall BtnOKClick(TObject *Sender);
	void __fastcall BtnPeruutaClick(TObject *Sender);
	void __fastcall BtnYhteysTesti(TObject *Sender);
	void __fastcall BtnPaivitaTilaClick(TObject *Sender);
	void __fastcall BtnTyhjennaMemoClick(TObject *Sender);
	void __fastcall BtnVieInternetClick(TObject *Sender);
	void __fastcall BtnOhjeClick(TObject *Sender);
	void __fastcall EditUrlChange(TObject *Sender);
	
private:	// User declarations
	void LueTiedot(void);
	void KirjoitaTiedot(void);
	void PaivitaTila(UnicodeString viesti);
	void TestaaYhteys(void);
	
public:		// User declarations
	__fastcall TFormApiYhteydet(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormApiYhteydet *FormApiYhteydet;
extern PACKAGE apiconfigtp apiconfig;
//---------------------------------------------------------------------------
#endif
