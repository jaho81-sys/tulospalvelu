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

#ifndef ApiSaikeH
#define ApiSaikeH

#include <Classes.hpp>
#include "ApiYhteydet.h"

//---------------------------------------------------------------------------
// API Communication thread for background synchronization
class TApiSaike : public TThread
{
private:
	bool pysaytysPyynnon;
	int viiveMs;
	HANDLE hPaivitysCallback;
	
	void __fastcall KasitteleValiajat(void);
	void __fastcall LaheteValiajat(void);
	void __fastcall VastaanottaValiajat(void);
	void __fastcall LaheteaTulokset(void);
	void __fastcall VastaanottaEiLahteneet(void);
	void __fastcall Paivita(const UnicodeString msg, bool virhe = false);
	
	// HTTP Helper methods
	bool LahetaHttpGet(const UnicodeString& url, UnicodeString& vastaus);
	bool LahetaHttpPost(const UnicodeString& url, const UnicodeString& data, UnicodeString& vastaus);
	
protected:
	void __fastcall Execute(void);
	
public:
	__fastcall TApiSaike(bool CreateSuspended);
	__fastcall ~TApiSaike(void);
	
	void PyynnoPysaytys(void);
	bool OnAktiivinen(void) const { return !pysaytysPyynnon; }
};

//---------------------------------------------------------------------------
#endif
