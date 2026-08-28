#ifndef ApiSaikeH
#define ApiSaikeH

#include <Classes.hpp>
#include "ApiYhteydet.h"

class TApiSaike : public TThread
{
private:
	bool pysaytysPyynnon;
	int viiveMs;
	UnicodeString viestiJono;

	void __fastcall Kasittele(void);
	void __fastcall Paivita(const UnicodeString msg, bool virhe = false);
	void __fastcall LisaaMemo(void);

protected:
	void __fastcall Execute(void);

public:
	__fastcall TApiSaike(bool CreateSuspended);
	__fastcall ~TApiSaike(void);

	void PyynnoPysaytys(void);
	bool OnAktiivinen(void) const { return !pysaytysPyynnon; }
};

// Manual / thread entry points
int ApiSynkkaaLahetaKaikki(void);
int ApiSynkkaaHaeKaikki(void);
UnicodeString ApiRakennaKilpailijatJson(void);
int ApiSovellaKilpailijatJson(const UnicodeString& json);

#endif
