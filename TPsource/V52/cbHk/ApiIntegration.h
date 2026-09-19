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

#ifndef ApiIntegrationH
#define ApiIntegrationH

#include <Classes.hpp>
#include "ApiYhteydet.h"

// TApiSaike is defined in program-specific ApiSaike.h (cbHk vs ViestiWin).
// Do not include it here: bcc32 looks in this header's directory first, so
// ViestiWin would get the 3-arg HK ApiIlmoitaTapahtuma and fail E2227.
class TApiSaike;

//---------------------------------------------------------------------------
// Global API integration manager
class TApiIntegration
{
private:
	static TApiIntegration* pInstance;
	TApiSaike* pSaike;
	bool onAloitettu;
	bool kilpailuAvattu;
	
public:
	TApiIntegration();
	virtual ~TApiIntegration();
	
	static TApiIntegration* GetInstance(void);
	
	// Lifecycle methods
	void Alusta(void);
	void Lopeta(void);
	void KilpailuAvattu(void);
	void KilpailuSuljettu(void);
	
	// Query methods
	bool OnAktiivinen(void) const { return onAloitettu && pSaike != NULL; }
	bool OnKilpailuAvattu(void) const { return kilpailuAvattu; }
	TApiSaike* GetSaike(void) const { return pSaike; }
	
	// Configuration methods
	void AvaaMuokkausikkuna(HWND parentHandle);
};

//---------------------------------------------------------------------------
extern TApiIntegration* gApiIntegration;

//---------------------------------------------------------------------------
#endif
