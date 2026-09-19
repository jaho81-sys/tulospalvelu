#ifndef ApiVIntegrationH
#define ApiVIntegrationH

#include <Classes.hpp>
#include "ApiSaike.h"
#include "ApiIntegration.h"

namespace ApiVIntegration
{
	inline void AvaaMuokkausikkuna(HWND parentHandle)
	{
		TApiIntegration::GetInstance()->AvaaMuokkausikkuna(parentHandle);
	}

	inline void Alusta()
	{
		TApiIntegration::GetInstance()->Alusta();
	}

	inline void KilpailuAvattu()
	{
		TApiIntegration::GetInstance()->KilpailuAvattu();
	}

	inline void KilpailuSuljettu()
	{
		TApiIntegration::GetInstance()->KilpailuSuljettu();
	}

	inline void Lopeta()
	{
		TApiIntegration::GetInstance()->Lopeta();
	}

	inline bool OnAktiivinen()
	{
		return TApiIntegration::GetInstance()->OnAktiivinen();
	}

	inline void IlmoitaLasna(int kilpno)
	{
		ApiIlmoitaLasna(kilpno);
	}

	inline void IlmoitaTapahtuma(int kilpno, int osuus, int piste, int aikaSec)
	{
		ApiIlmoitaTapahtuma(kilpno, osuus, piste, aikaSec);
	}
}

#endif
