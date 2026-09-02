// Pekka Pirila's sports timekeeping program (Finnish: tulospalveluohjelma)
// REST API Integration moduuli pääikkunaan
// Copyright (C) 2015 Pekka Pirila 

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// KÄYTTO:
// 1. Lisää tämä tiedosto projektiin: TPsource\V52\cbHk\ApiHkIntegration.cpp
// 2. Lisää seuraava koodiin WinHk.cpp:n alussa:
//    #include "ApiHkIntegration.h"
// 3. Lisää WinHk.h:n julkisiin metodeihin:
//    void __fastcall ApiSettingsClick(TObject *Sender);
// 4. Lisää WinHk.cpp:n Initialisoi-funktioon (kilpailu auki):
//    TApiIntegration::GetInstance()->KilpailuAvattu();
// 5. Lisää WinHk.cpp:n FormClose-funktioon:
//    TApiIntegration::GetInstance()->Lopeta();

#ifndef ApiHkIntegrationH
#define ApiHkIntegrationH

#include <Classes.hpp>
#include "ApiIntegration.h"

//---------------------------------------------------------------------------
// Helpperi-funktiot API-integroimiseen pääikkunaan
namespace ApiHkIntegration
{
	// API-ikkunan avaaminen pääikkunasta
	inline void AvaaMuokkausikkuna(HWND parentHandle)
	{
		TApiIntegration::GetInstance()->AvaaMuokkausikkuna(parentHandle);
	}
	
	// API-säikeen käynnistäminen
	inline void Alusta()
	{
		TApiIntegration::GetInstance()->Alusta();
	}

	// Lataa jahonline_api.ini kilpailun kansiosta ja käynnistä säie
	inline void KilpailuAvattu()
	{
		TApiIntegration::GetInstance()->KilpailuAvattu();
	}
	
	// API-säikeen pysäyttäminen
	inline void Lopeta()
	{
		TApiIntegration::GetInstance()->Lopeta();
	}
	
	// API:n tila
	inline bool OnAktiivinen()
	{
		return TApiIntegration::GetInstance()->OnAktiivinen();
	}

	// Emit-luenta: läsnäolo JAHOnlineen (no-op ilman API-asetuksia)
	inline void IlmoitaLasna(int kilpno)
	{
		ApiIlmoitaLasna(kilpno);
	}

	// Ajanotto / online-rasti
	inline void IlmoitaTapahtuma(int kilpno, int piste, int aikaSec)
	{
		ApiIlmoitaTapahtuma(kilpno, piste, aikaSec);
	}
}

//---------------------------------------------------------------------------
#endif
