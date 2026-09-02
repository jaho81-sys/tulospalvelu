#include <vcl.h>
#pragma hdrstop
#include <stdio.h>
#include <string.h>
#include <vector>

#include "ApiSaike.h"
#include "ApiJson.h"
#include "ApiYhteydet.h"
#include "HkDeclare.h"

#pragma package(smart_init)

extern CRITICAL_SECTION tall_CriticalSection;

static CRITICAL_SECTION lasnaJonoCS;
static bool lasnaJonoAlustettu = false;
static int lasnaJono[256];
static int lasnaJonoN = 0;

typedef struct {
	int numero;
	int piste;
	int aikaSec;
} ApiTapahtuma;

static CRITICAL_SECTION tapJonoCS;
static bool tapJonoAlustettu = false;
static ApiTapahtuma tapJono[256];
static int tapJonoN = 0;

static void TapJonoAlusta(void)
{
	if (!tapJonoAlustettu) {
		InitializeCriticalSection(&tapJonoCS);
		tapJonoAlustettu = true;
	}
}

static void LasnaJonoAlusta(void)
{
	if (!lasnaJonoAlustettu) {
		InitializeCriticalSection(&lasnaJonoCS);
		lasnaJonoAlustettu = true;
	}
}

static int ApiIpv(void)
{
	int ipv = 0;
	if (k_pv > 0)
		ipv = k_pv - 1;
	if (ipv < 0)
		ipv = 0;
	return ipv;
}

__fastcall TApiSaike::TApiSaike(bool CreateSuspended)
	: TThread(CreateSuspended), pysaytysPyynnon(false), viiveMs(10000)
{
	Priority = tpLower;
	FreeOnTerminate = false;
}

__fastcall TApiSaike::~TApiSaike(void)
{
	PyynnoPysaytys();
	WaitFor();
}

void TApiSaike::PyynnoPysaytys(void)
{
	pysaytysPyynnon = true;
}

void __fastcall TApiSaike::LisaaMemo(void)
{
	if (FormApiYhteydet && FormApiYhteydet->MemoTila)
		FormApiYhteydet->MemoTila->Lines->Add(viestiJono);
}

void __fastcall TApiSaike::Paivita(const UnicodeString msg, bool virhe)
{
	viestiJono = (virhe ? UnicodeString(L"! ") : UnicodeString(L"")) + msg;
	Synchronize(&LisaaMemo);
}

static UnicodeString StatusMerkki(wchar_t keskhyl, bool onLasna, bool onTulos)
{
	switch (keskhyl) {
	case L'T': return L"DNS";
	case L'H': return L"DNF";
	case L'K': return L"DSQ";
	case L'E': return L"DNS";
	default:
		if (onLasna && !onTulos)
			return L"LASNA";
		if (onLasna)
			return L"OK";
		return L"DNS";
	}
}

static wchar_t StatusMerkkiin(const UnicodeString& st)
{
	if (st.CompareIC(L"DNS") == 0 || st.CompareIC(L"EI_LAHTENYT") == 0)
		return L'T';
	if (st.CompareIC(L"DNF") == 0 || st.CompareIC(L"KESKEYTTI") == 0)
		return L'H';
	if (st.CompareIC(L"DSQ") == 0 || st.CompareIC(L"HYLATTY") == 0)
		return L'K';
	if (st.CompareIC(L"LASNA") == 0 || st.CompareIC(L"PRESENT") == 0)
		return L'-';
	return L' ';
}

// JAHOnline synkka: lahto_aika / pirila_lahto_at (kellonaika) + lahto_sec (sekunnit vuorokaudesta).
static UnicodeString ApiLahtoKentat(INT32 tl)
{
	if (tl == TMAALI0)
		return L",\"lahto_aika\":null,\"pirila_lahto_at\":null,\"lahto_sec\":null";
	wchar_t st[24];
	aikatowstr_cols_n(st, tl, t0, 0, 8);
	int sec = (int)(tl / SEK + (INT32)t0 * 3600L);
	sec %= 86400;
	if (sec < 0)
		sec += 86400;
	return UnicodeString(L",\"lahto_aika\":") + ApiJsonString(st)
		+ L",\"pirila_lahto_at\":" + ApiJsonString(st)
		+ L",\"lahto_sec\":" + IntToStr(sec);
}

static INT32 ApiKilpailijanLahto(kilptietue& kilp, int ipv)
{
	INT32 tl = kilp.TLahto(ipv);
	if (tl == TMAALI0) {
		int srj = kilp.Sarja(ipv);
		if (srj >= 0 && srj < sarjaluku)
			tl = Sarjat[srj].enslahto[ipv];
	}
	return tl;
}

static UnicodeString ApiKilpailijaObj(kilptietue& kilp, int ipv)
{
	int numero = kilp.id();
	UnicodeString sarjaNimi = L"";
	int srj = kilp.Sarja(ipv);
	if (srj >= 0 && srj < sarjaluku)
		sarjaNimi = Sarjat[srj].sarjanimi;

	int badge = kilp.Badge(ipv, 0, true);
	int badge2 = kilp.Badge(ipv, 1, true);
	INT32 tls = kilp.tulos_pv(ipv, false, 1);
	wchar_t tark = kilp.tark(ipv);
	bool onLasna = kilp.lasna(ipv);

	UnicodeString valia = L"[";
	bool vfirst = true;
	if (kilp.pv && kilp.pv[ipv].va) {
		int nva = 0;
		if (srj >= 0 && srj < sarjaluku)
			nva = Sarjat[srj].valuku[ipv];
		if (nva < 0) nva = 0;
		if (nva > 60) nva = 60;
		for (int p = 1; p <= nva; p++) {
			INT32 va = kilp.pv[ipv].va[p].vatulos;
			if (va <= 0)
				continue;
			if (!vfirst) valia += L",";
			vfirst = false;
			valia += L"{\"piste\":" + IntToStr(p)
				+ L",\"aika_sec\":" + IntToStr((int)va)
				+ L",\"sija\":" + IntToStr((int)kilp.pv[ipv].va[p].vasija)
				+ L"}";
		}
	}
	valia += L"]";

	UnicodeString arr = L"{";
	arr += L"\"numero\":" + IntToStr(numero);
	arr += L",\"sukunimi\":" + ApiJsonString(kilp.sukunimi);
	arr += L",\"etunimi\":" + ApiJsonString(kilp.etunimi);
	arr += L",\"nimi\":" + ApiJsonString(UnicodeString(kilp.sukunimi) + L" " + UnicodeString(kilp.etunimi));
	arr += L",\"seura\":" + ApiJsonString(kilp.seura);
	arr += L",\"maa\":" + ApiJsonString(kilp.maa);
	arr += L",\"sarja_nimi\":" + ApiJsonString(sarjaNimi);
	arr += L",\"badge\":" + IntToStr(badge);
	arr += L",\"badge2\":" + IntToStr(badge2);
	arr += L",\"emit_koodi\":" + IntToStr(badge);
	arr += L",\"emit_koodi2\":" + IntToStr(badge2);
	arr += L",\"lasna\":" + ApiJsonBool(onLasna);
	arr += L",\"status\":" + ApiJsonString(StatusMerkki(tark, onLasna, tls > 0));
	arr += L",\"keskhyl\":" + ApiJsonString(UnicodeString(tark));
	if (tls > 0)
		arr += L",\"aika_sec\":" + IntToStr((int)tls);
	else
		arr += L",\"aika_sec\":null";
	INT16 sija = 0;
	if (kilp.pv)
		sija = kilp.pv[ipv].ysija;
	if (sija > 0)
		arr += L",\"sija\":" + IntToStr((int)sija);
	else
		arr += L",\"sija\":null";
	arr += ApiLahtoKentat(ApiKilpailijanLahto(kilp, ipv));
	arr += L",\"valiajat\":" + valia;
	arr += L",\"tyyppi\":\"yksilo\"";
	arr += L"}";
	return arr;
}

UnicodeString ApiRakennaKilpailijatJson(void)
{
	UnicodeString arr = L"[";
	bool first = true;
	int ipv = ApiIpv();

	for (int d = 1; d < nrec; d++) {
		kilptietue kilp;
		kilp.GETREC(d);
		if (kilp.kilpstatus != 0)
			continue;

		int numero = kilp.id();
		if (numero <= 0)
			continue;

		if (!first) arr += L",";
		first = false;
		arr += ApiKilpailijaObj(kilp, ipv);
	}
	arr += L"]";
	return arr;
}

int ApiSovellaKilpailijatJson(const UnicodeString& json)
{
	std::vector<UnicodeString> objs;
	int n = ApiJsonExtractObjectArray(json, L"kilpailijat", objs);
	if (n <= 0)
		return 0;

	int ipv = 0;
	if (k_pv > 0)
		ipv = k_pv - 1;
	if (ipv < 0)
		ipv = 0;

	int updated = 0;
	EnterCriticalSection(&tall_CriticalSection);
	__try {
		for (size_t i = 0; i < objs.size(); i++) {
			const UnicodeString& o = objs[i];
			int numero = 0;
			if (!ApiJsonFindInt(o, L"numero", numero) || numero <= 0)
				continue;

			UnicodeString sukunimi, etunimi, seura, maa, sarjaNimi, status;
			ApiJsonFindString(o, L"sukunimi", sukunimi);
			ApiJsonFindString(o, L"etunimi", etunimi);
			ApiJsonFindString(o, L"seura", seura);
			ApiJsonFindString(o, L"maa", maa);
			ApiJsonFindString(o, L"sarja_nimi", sarjaNimi);
			ApiJsonFindString(o, L"status", status);

			bool lasnaFlag = false;
			bool lasnaAnnettu = ApiJsonFindBool(o, L"lasna", lasnaFlag);

			int badge = 0, badge2 = 0, aikaSec = -1, sija = -1;
			ApiJsonFindInt(o, L"badge", badge);
			if (badge <= 0)
				ApiJsonFindInt(o, L"emit_koodi", badge);
			ApiJsonFindInt(o, L"badge2", badge2);
			if (badge2 <= 0)
				ApiJsonFindInt(o, L"emit_koodi2", badge2);
			__int64 a64 = 0;
			if (ApiJsonFindInt64(o, L"aika_sec", a64))
				aikaSec = (int)a64;
			ApiJsonFindInt(o, L"sija", sija);

			int d = getpos(numero, true);
			kilptietue kilp;
			bool uusi = false;
			if (d > 0) {
				kilp.GETREC(d);
			} else {
				kilp.nollaa();
				kilp.setId(numero);
				uusi = true;
			}

			if (!sukunimi.IsEmpty())
				wcsncpy(kilp.sukunimi, sukunimi.c_str(), LSNIMI);
			if (!etunimi.IsEmpty())
				wcsncpy(kilp.etunimi, etunimi.c_str(), LENIMI);
			if (!seura.IsEmpty())
				wcsncpy(kilp.seura, seura.c_str(), LSEURA);
			if (!maa.IsEmpty())
				wcsncpy(kilp.maa, maa.c_str(), 3);

			if (!sarjaNimi.IsEmpty()) {
				for (int s = 0; s < sarjaluku; s++) {
					if (sarjaNimi.CompareIC(Sarjat[s].sarjanimi) == 0) {
						kilp.setSarja(s);
						break;
					}
				}
			}

			if (kilp.pv) {
				if (badge > 0)
					kilp.pv[ipv].badge[0] = badge;
				if (badge2 > 0)
					kilp.pv[ipv].badge[1] = badge2;
				if (apiconfig.vastaanottaEiLahteneet && !status.IsEmpty()) {
					wchar_t m = StatusMerkkiin(status);
					if (m != L' ')
						kilp.set_tark(m, ipv);
				}
				if (lasnaAnnettu && lasnaFlag && !kilp.lasna(ipv)) {
					wchar_t t = kilp.tark(ipv);
					if (t == L'E' || t == L'P' || t == L'V' || t == L'B' || t == L'T')
						kilp.set_tark(L'-', ipv);
				}
				if (aikaSec >= 0)
					kilp.tall_tulos_pv(aikaSec, ipv, 0);
				{
					UnicodeString lahtoAika;
					if (ApiJsonFindString(o, L"lahto_aika", lahtoAika) && !lahtoAika.IsEmpty())
						kilp.tall_lajat_pv(wstrtoaika_vap(lahtoAika.c_str(), t0), ipv);
				}
				if (sija >= 0)
					kilp.pv[ipv].ysija = (INT16)sija;
				if (apiconfig.vastaanottaValiajat || apiconfig.vastaanottaKilpailijat) {
					std::vector<UnicodeString> vas;
					if (ApiJsonExtractObjectArray(o, L"valiajat", vas) > 0) {
						for (size_t v = 0; v < vas.size(); v++) {
							int piste = 0, vasija = -1;
							__int64 va64 = 0;
							if (!ApiJsonFindInt(vas[v], L"piste", piste) || piste < 1)
								continue;
							if (piste > 60)
								continue;
							if (ApiJsonFindInt64(vas[v], L"aika_sec", va64) && va64 > 0)
								kilp.pv[ipv].va[piste].vatulos = (INT32)va64;
							if (ApiJsonFindInt(vas[v], L"sija", vasija) && vasija >= 0)
								kilp.pv[ipv].va[piste].vasija = (INT16)vasija;
						}
					}
				}
			}

			if (uusi) {
				int nd = 0;
				kilp.addtall(&nd, 0);
			} else {
				kilp.tallenna(d, 0, 0, 0, 0);
			}
			updated++;
		}
	} __finally {
		LeaveCriticalSection(&tall_CriticalSection);
	}
	return updated;
}

int ApiSynkkaaLahetaKaikki(void)
{
	if (apiconfig.kilpailuId <= 0 || apiconfig.apiKey[0] == 0)
		return -1;

	UnicodeString kilpailijat = ApiRakennaKilpailijatJson();
	UnicodeString body =
		L"{\"action\":\"synkkaa\",\"kilpailu_id\":" + IntToStr(apiconfig.kilpailuId)
		+ L",\"lahde\":\"HkKisaWin\""
		+ L",\"luo_puuttuvat\":true"
		+ L",\"kilpailijat\":" + kilpailijat
		+ L"}";

	UnicodeString vastaus;
	if (!ApiHttpPostJson(ApiBridgeUrl(), body, vastaus))
		return -2;
	if (!ApiJsonStatusOk(vastaus))
		return -3;

	int count = 0;
	// rough count of objects
	std::vector<UnicodeString> objs;
	count = ApiJsonExtractObjectArray(L"{\"kilpailijat\":" + kilpailijat + L"}", L"kilpailijat", objs);
	return count;
}

int ApiSynkkaaHaeKaikki(void)
{
	if (apiconfig.kilpailuId <= 0 || apiconfig.apiKey[0] == 0)
		return -1;

	UnicodeString body =
		L"{\"action\":\"kilpailijat\",\"kilpailu_id\":" + IntToStr(apiconfig.kilpailuId) + L"}";
	UnicodeString vastaus;
	if (!ApiHttpPostJson(ApiBridgeUrl(), body, vastaus))
		return -2;
	if (!ApiJsonStatusOk(vastaus))
		return -3;
	return ApiSovellaKilpailijatJson(vastaus);
}

void ApiIlmoitaLasna(int kilpno)
{
	if (kilpno <= 0)
		return;
	if (apiconfig.kilpailuId <= 0 || apiconfig.apiKey[0] == 0)
		return;

	LasnaJonoAlusta();
	EnterCriticalSection(&lasnaJonoCS);
	if (lasnaJonoN < (int)(sizeof(lasnaJono) / sizeof(lasnaJono[0]))) {
		if (lasnaJonoN == 0 || lasnaJono[lasnaJonoN - 1] != kilpno)
			lasnaJono[lasnaJonoN++] = kilpno;
	}
	LeaveCriticalSection(&lasnaJonoCS);
}

void ApiIlmoitaTapahtuma(int kilpno, int piste, int aikaSec)
{
	if (kilpno <= 0)
		return;
	if (apiconfig.kilpailuId <= 0 || apiconfig.apiKey[0] == 0)
		return;
	if (!apiconfig.lahetaValiajat && piste != 0)
		return;

	TapJonoAlusta();
	EnterCriticalSection(&tapJonoCS);
	if (tapJonoN < (int)(sizeof(tapJono) / sizeof(tapJono[0]))) {
		ApiTapahtuma t;
		t.numero = kilpno;
		t.piste = piste;
		t.aikaSec = aikaSec;
		tapJono[tapJonoN++] = t;
	}
	LeaveCriticalSection(&tapJonoCS);
}

static UnicodeString ApiTapahtumatJson(const ApiTapahtuma* ev, int n)
{
	UnicodeString arr = L"[";
	bool first = true;
	for (int i = 0; i < n; i++) {
		if (!first)
			arr += L",";
		first = false;
		arr += L"{\"numero\":" + IntToStr(ev[i].numero)
			+ L",\"piste\":" + IntToStr(ev[i].piste)
			+ L",\"aika_sec\":" + IntToStr(ev[i].aikaSec)
			+ L",\"lahde\":\"online\"}";
	}
	arr += L"]";
	return arr;
}

static int ApiLahetaTapahtumat(const ApiTapahtuma* ev, int n)
{
	if (n <= 0)
		return 0;
	if (apiconfig.kilpailuId <= 0 || apiconfig.apiKey[0] == 0)
		return -1;

	UnicodeString body =
		L"{\"action\":\"tapahtuma\",\"kilpailu_id\":" + IntToStr(apiconfig.kilpailuId)
		+ L",\"lahde\":\"HkKisaWin\""
		+ L",\"tapahtumat\":" + ApiTapahtumatJson(ev, n)
		+ L"}";
	UnicodeString vastaus;
	if (!ApiHttpPostJson(ApiBridgeUrl(), body, vastaus))
		return -2;
	if (!ApiJsonStatusOk(vastaus))
		return -3;
	return n;
}

int ApiLahetaTapahtumatNyt(void)
{
	ApiTapahtuma ev[256];
	int n = 0;
	if (!tapJonoAlustettu)
		return 0;
	EnterCriticalSection(&tapJonoCS);
	n = tapJonoN;
	if (n > 0) {
		memcpy(ev, tapJono, n * sizeof(ApiTapahtuma));
		tapJonoN = 0;
	}
	LeaveCriticalSection(&tapJonoCS);
	return ApiLahetaTapahtumat(ev, n);
}

static int ApiSynkkaaJonosta(void)
{
	int nums[256];
	int n = 0;

	if (!lasnaJonoAlustettu)
		return 0;
	EnterCriticalSection(&lasnaJonoCS);
	n = lasnaJonoN;
	if (n > 0) {
		memcpy(nums, lasnaJono, n * sizeof(int));
		lasnaJonoN = 0;
	}
	LeaveCriticalSection(&lasnaJonoCS);
	if (n <= 0)
		return 0;
	if (apiconfig.kilpailuId <= 0 || apiconfig.apiKey[0] == 0)
		return -1;

	int ipv = ApiIpv();
	UnicodeString arr = L"[";
	bool first = true;
	for (int i = 0; i < n; i++) {
		int d = getpos(nums[i], true);
		if (d <= 0)
			continue;
		kilptietue kilp;
		kilp.GETREC(d);
		if (kilp.kilpstatus != 0 || kilp.id() <= 0)
			continue;
		if (!first)
			arr += L",";
		first = false;
		arr += ApiKilpailijaObj(kilp, ipv);
	}
	arr += L"]";
	if (first)
		return 0;

	UnicodeString body =
		L"{\"action\":\"synkkaa\",\"kilpailu_id\":" + IntToStr(apiconfig.kilpailuId)
		+ L",\"lahde\":\"HkKisaWin\""
		+ L",\"luo_puuttuvat\":true"
		+ L",\"kilpailijat\":" + arr
		+ L"}";
	UnicodeString vastaus;
	if (!ApiHttpPostJson(ApiBridgeUrl(), body, vastaus))
		return -2;
	if (!ApiJsonStatusOk(vastaus))
		return -3;
	return n;
}

void __fastcall TApiSaike::Kasittele(void)
{
	if (!apiconfig.kaynnissa)
		return;

	if (apiconfig.lahetaKilpailijat || apiconfig.lahetaTulokset || apiconfig.lahetaValiajat) {
		int n = ApiSynkkaaLahetaKaikki();
		if (n >= 0)
			Paivita(L"Lähetetty kilpailijoita: " + IntToStr(n));
		else
			Paivita(L"Lähetys epäonnistui (" + IntToStr(n) + L")", true);
	}

	if (apiconfig.lahetaValiajat) {
		int n = ApiLahetaTapahtumatNyt();
		if (n > 0)
			Paivita(L"Lähetetty online-rasteja: " + IntToStr(n));
		else if (n < 0)
			Paivita(L"Online-rastien lähetys epäonnistui (" + IntToStr(n) + L")", true);
	}

	if (apiconfig.vastaanottaKilpailijat || apiconfig.vastaanottaValiajat) {
		int n = ApiSynkkaaHaeKaikki();
		if (n >= 0)
			Paivita(L"Haettu/päivitetty: " + IntToStr(n));
		else
			Paivita(L"Haku epäonnistui (" + IntToStr(n) + L")", true);
	}
}

void __fastcall TApiSaike::Execute(void)
{
	ApiConfigLataa();
	while (!pysaytysPyynnon) {
		viiveMs = apiconfig.lahetysvali * 1000;
		if (viiveMs < 2000)
			viiveMs = 2000;
		if (apiconfig.kaynnissa)
			Kasittele();
		for (int t = 0; t < viiveMs && !pysaytysPyynnon; t += 200) {
			Sleep(200);
			if (!pysaytysPyynnon) {
				ApiSynkkaaJonosta();
				if (apiconfig.lahetaValiajat)
					ApiLahetaTapahtumatNyt();
			}
		}
	}
}
