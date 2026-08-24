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

#include "ApiSaike.h"
#include <Winsock2.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "ws2_32.lib")

extern apiconfigtp apiconfig;

//---------------------------------------------------------------------------
__fastcall TApiSaike::TApiSaike(bool CreateSuspended)
	: TThread(CreateSuspended), pysaytysPyynnon(false), viiveMs(5000)
{
	Priority = tpLower;
	FreeOnTerminate = false;
}

//---------------------------------------------------------------------------
__fastcall TApiSaike::~TApiSaike(void)
{
	PyynnoPysaytys();
	WaitFor();
}

//---------------------------------------------------------------------------
void TApiSaike::PyynnoPysaytys(void)
{
	pysaytysPyynnon = true;
}

//---------------------------------------------------------------------------
void __fastcall TApiSaike::Paivita(const UnicodeString msg, bool virhe)
{
	if (FormApiYhteydet) {
		UnicodeString logiviesti = virhe ? L"❌ " : L"✓ ";
		logiviesti += msg;
		Synchronize([this, logiviesti]() {
			FormApiYhteydet->PaivitaTila(logiviesti);
		});
	}
}

//---------------------------------------------------------------------------
bool TApiSaike::LahetaHttpGet(const UnicodeString& url, UnicodeString& vastaus)
{
	HINTERNET hInternet = InternetOpen(L"Tulospalvelu/2.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (!hInternet)
		return false;
	
	HINTERNET hConnect = InternetOpenUrl(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (!hConnect) {
		InternetCloseHandle(hInternet);
		return false;
	}
	
	char szBuffer[4096];
	DWORD dwBytesRead = 0;
	vastaus = L"";
	
	while (InternetReadFile(hConnect, szBuffer, sizeof(szBuffer) - 1, &dwBytesRead) && dwBytesRead > 0) {
		szBuffer[dwBytesRead] = 0;
		vastaus += UnicodeString(AnsiString(szBuffer));
	}
	
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);
	return true;
}

//---------------------------------------------------------------------------
bool TApiSaike::LahetaHttpPost(const UnicodeString& url, const UnicodeString& data, UnicodeString& vastaus)
{
	HINTERNET hInternet = InternetOpen(L"Tulospalvelu/2.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (!hInternet)
		return false;
	
	URL_COMPONENTS urlComp = { 0 };
	urlComp.dwStructSize = sizeof(URL_COMPONENTS);
	urlComp.dwSchemeLength = -1;
	urlComp.dwHostNameLength = -1;
	urlComp.dwPathLength = -1;
	
	if (!InternetCrackUrl(url.c_str(), url.Length(), 0, &urlComp)) {
		InternetCloseHandle(hInternet);
		return false;
	}
	
	UnicodeString hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
	HINTERNET hConnect = InternetConnect(hInternet, hostName.c_str(), urlComp.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (!hConnect) {
		InternetCloseHandle(hInternet);
		return false;
	}
	
	UnicodeString pathName(urlComp.lpszUrlPath, urlComp.dwPathLength);
	HINTERNET hRequest = HttpOpenRequest(hConnect, L"POST", pathName.c_str(), HTTP_VERSION, NULL, NULL, 0, 0);
	if (!hRequest) {
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return false;
	}
	
	AnsiString dataAnsi = AnsiString(data);
	DWORD dwDataLen = dataAnsi.Length();
	
	// Add Content-Type header
	HttpAddRequestHeaders(hRequest, L"Content-Type: application/json\r\n", -1L, HTTP_ADDREQ_FLAG_ADD);
	
	// Add API key if configured
	if (apiconfig.apiKey[0] != 0) {
		UnicodeString authHeader = L"Authorization: Bearer " + UnicodeString(apiconfig.apiKey);
		HttpAddRequestHeaders(hRequest, authHeader.c_str(), -1L, HTTP_ADDREQ_FLAG_ADD);
	}
	
	if (!HttpSendRequest(hRequest, NULL, 0, (void*)dataAnsi.c_str(), dwDataLen)) {
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return false;
	}
	
	char szBuffer[4096];
	DWORD dwBytesRead = 0;
	vastaus = L"";
	
	while (InternetReadFile(hRequest, szBuffer, sizeof(szBuffer) - 1, &dwBytesRead) && dwBytesRead > 0) {
		szBuffer[dwBytesRead] = 0;
		vastaus += UnicodeString(AnsiString(szBuffer));
	}
	
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);
	return true;
}

//---------------------------------------------------------------------------
void __fastcall TApiSaike::LaheteValiajat(void)
{
	// Implement sending split times to API
	// This would iterate through competition data and POST to /api/v1/competition/{id}/splits
	
	if (!apiconfig.lahetaValiajat)
		return;
	
	try {
		UnicodeString url = UnicodeString(apiconfig.apiUrl) + L":" + UnicodeString(apiconfig.apiPort) + 
							L"/api/v1/competition/current/splits";
		
		// TODO: Serialize current split times to JSON
		UnicodeString jsonData = L"{"
			L"\"timestamp\": \"" + TDateTime::CurrentDateTime().FormatString(L"yyyy-mm-dd hh:mm:ss") + L"\","
			L"\"splits\": []"
			L"}";
		
		UnicodeString vastaus;
		if (LahetaHttpPost(url, jsonData, vastaus)) {
			Paivita(L"Väliajat lähetetty onnistuneesti");
		} else {
			Paivita(L"Väliaikojen lähettäminen epäonnistui", true);
		}
	} catch (...) {
		Paivita(L"Virhe väliaikojen lähettämisessä", true);
	}
}

//---------------------------------------------------------------------------
void __fastcall TApiSaike::VastaanottaValiajat(void)
{
	// Implement receiving split times from API
	// This would GET from /api/v1/competition/{id}/splits and update local data
	
	if (!apiconfig.vastaanottaValiajat)
		return;
	
	try {
		UnicodeString url = UnicodeString(apiconfig.apiUrl) + L":" + UnicodeString(apiconfig.apiPort) + 
							L"/api/v1/competition/current/splits";
		
		UnicodeString vastaus;
		if (LahetaHttpGet(url, vastaus)) {
			// TODO: Parse JSON and update split times in local database
			Paivita(L"Väliajat vastaanotettu netistä");
		} else {
			Paivita(L"Väliaikojen vastaanottaminen epäonnistui", true);
		}
	} catch (...) {
		Paivita(L"Virhe väliaikojen vastaanottamisessa", true);
	}
}

//---------------------------------------------------------------------------
void __fastcall TApiSaike::LaheteaTulokset(void)
{
	// Implement publishing results to API
	// This would serialize final results and POST to /api/v1/competition/{id}/results
	
	if (!apiconfig.lahetaTulokset)
		return;
	
	try {
		UnicodeString url = UnicodeString(apiconfig.apiUrl) + L":" + UnicodeString(apiconfig.apiPort) + 
							L"/api/v1/competition/current/results";
		
		// TODO: Serialize results to JSON
		UnicodeString jsonData = L"{"
			L"\"format\": \"json\","
			L"\"timestamp\": \"" + TDateTime::CurrentDateTime().FormatString(L"yyyy-mm-dd hh:mm:ss") + L"\","
			L"\"results\": []"
			L"}";
		
		UnicodeString vastaus;
		if (LahetaHttpPost(url, jsonData, vastaus)) {
			Paivita(L"Tulokset julkaistu nettiin");
		} else {
			Paivita(L"Tulosten julkaiseminen epäonnistui", true);
		}
	} catch (...) {
		Paivita(L"Virhe tulosten julkaisemisessa", true);
	}
}

//---------------------------------------------------------------------------
void __fastcall TApiSaike::VastaanottaEiLahteneet(void)
{
	// Implement receiving "didn't start" entries from API
	// This would GET from /api/v1/competition/{id}/no-starts
	
	if (!apiconfig.vastaanottaEiLahteneet)
		return;
	
	try {
		UnicodeString url = UnicodeString(apiconfig.apiUrl) + L":" + UnicodeString(apiconfig.apiPort) + 
							L"/api/v1/competition/current/no-starts";
		
		UnicodeString vastaus;
		if (LahetaHttpGet(url, vastaus)) {
			// TODO: Parse JSON and mark entries as "didn't start"
			Paivita(L"'Ei lähteneet' -merkinnät päivitetty");
		} else {
			Paivita(L"'Ei lähteneet' -merkintöjen vastaanottaminen epäonnistui", true);
		}
	} catch (...) {
		Paivita(L"Virhe 'ei lähteneet' -merkintöjen vastaanottamisessa", true);
	}
}

//---------------------------------------------------------------------------
void __fastcall TApiSaike::KasitteleValiajat(void)
{
	// Main processing routine that calls individual operations
	if (!apiconfig.kaynnissa) {
		Sleep(1000);
		return;
	}
	
	if (apiconfig.lahetaValiajat) {
		LaheteValiajat();
	}
	
	if (apiconfig.vastaanottaValiajat) {
		VastaanottaValiajat();
	}
	
	if (apiconfig.lahetaTulokset) {
		LaheteaTulokset();
	}
	
	if (apiconfig.vastaanottaEiLahteneet) {
		VastaanottaEiLahteneet();
	}
}

//---------------------------------------------------------------------------
void __fastcall TApiSaike::Execute(void)
{
	viiveMs = apiconfig.lahetysvali * 1000;
	int jakso = 0;
	
	Paivita(L"API-säie käynnistetty");
	
	while (!pysaytysPyynnon) {
		try {
			// Process API operations
			if (apiconfig.kaynnissa) {
				KasitteleValiajat();
			}
			
			// Sleep in chunks to allow responsive shutdown
			for (int i = 0; i < apiconfig.lahetysvali && !pysaytysPyynnon; i++) {
				Sleep(1000);
			}
			
			jakso++;
			if (jakso % 10 == 0) {
				// Log status every 10 cycles
				if (apiconfig.kaynnissa) {
					Paivita(L"API-säie aktiivinen. Sykli: " + UnicodeString(jakso));
				}
			}
		} catch (...) {
			Paivita(L"Virhe API-säikeen suoritussa", true);
		}
	}
	
	Paivita(L"API-säie pysäytetty");
}

//---------------------------------------------------------------------------
