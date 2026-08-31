#ifndef ApiVIntegrationH
#define ApiVIntegrationH

#include <Classes.hpp>
#include "ApiIntegration.h"
#include "ApiSaike.h"

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
