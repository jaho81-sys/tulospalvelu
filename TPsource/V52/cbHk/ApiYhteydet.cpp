#include <vcl.h>
#pragma hdrstop
#include <wininet.h>
#include <stdio.h>

#include "ApiYhteydet.h"
#include "ApiJson.h"
#include "ApiSaike.h"
#include "ApiIntegration.h"

#pragma comment(lib, "wininet.lib")

#pragma package(smart_init)
#pragma resource "*.dfm"

TFormApiYhteydet *FormApiYhteydet;

apiconfigtp apiconfig;

static UnicodeString PolkuKansioon(UnicodeString dir)
{
	if (dir.IsEmpty())
		return dir;
	wchar_t last = dir[dir.Length()];
	if (last != L'\\' && last != L'/')
		dir += L"\\";
	return dir;
}

UnicodeString ApiConfigExePolku(void)
{
	wchar_t buf[MAX_PATH];
	GetModuleFileNameW(NULL, buf, MAX_PATH);
	return PolkuKansioon(ExtractFilePath(UnicodeString(buf))) + L"jahonline_api.ini";
}

UnicodeString ApiConfigPolku(void)
{
	return PolkuKansioon(GetCurrentDir()) + L"jahonline_api.ini";
}

static void ApiConfigLueTiedostosta(const UnicodeString& polku)
{
	wchar_t tmp[512];
	GetPrivateProfileStringW(L"jahonline", L"url", apiconfig.apiUrl, tmp, 512, polku.c_str());
	wcsncpy(apiconfig.apiUrl, tmp, sizeof(apiconfig.apiUrl)/2 - 1);
	apiconfig.apiUrl[sizeof(apiconfig.apiUrl)/2 - 1] = 0;
	GetPrivateProfileStringW(L"jahonline", L"api_key", L"", tmp, 512, polku.c_str());
	wcsncpy(apiconfig.apiKey, tmp, sizeof(apiconfig.apiKey)/2 - 1);
	apiconfig.apiKey[sizeof(apiconfig.apiKey)/2 - 1] = 0;
	apiconfig.apiPort = GetPrivateProfileIntW(L"jahonline", L"port", 0, polku.c_str());
	apiconfig.kilpailuId = GetPrivateProfileIntW(L"jahonline", L"kilpailu_id", 0, polku.c_str());
	apiconfig.lahetysvali = GetPrivateProfileIntW(L"jahonline", L"vali", 10, polku.c_str());
	apiconfig.lahetaKilpailijat = GetPrivateProfileIntW(L"jahonline", L"laheta_kilpailijat", 1, polku.c_str());
	apiconfig.vastaanottaKilpailijat = GetPrivateProfileIntW(L"jahonline", L"vastaanotta_kilpailijat", 1, polku.c_str());
	apiconfig.lahetaValiajat = GetPrivateProfileIntW(L"jahonline", L"laheta_valiajat", 1, polku.c_str());
	apiconfig.vastaanottaValiajat = GetPrivateProfileIntW(L"jahonline", L"vastaanotta_valiajat", 1, polku.c_str());
	apiconfig.lahetaTulokset = GetPrivateProfileIntW(L"jahonline", L"laheta_tulokset", 1, polku.c_str());
	apiconfig.vastaanottaEiLahteneet = GetPrivateProfileIntW(L"jahonline", L"vastaanotta_dns", 1, polku.c_str());
	apiconfig.kaynnissa = GetPrivateProfileIntW(L"jahonline", L"kaynnissa", 0, polku.c_str());
	if (apiconfig.lahetysvali < 2)
		apiconfig.lahetysvali = 2;
}

void ApiConfigNollaa(void)
{
	memset(&apiconfig, 0, sizeof(apiconfig));
	wcsncpy(apiconfig.apiUrl,
		L"https://jahonline.com/public/api/kilpailijat_bridge.php",
		sizeof(apiconfig.apiUrl)/2 - 1);
	apiconfig.apiPort = 0;
	apiconfig.lahetysvali = 10;
	apiconfig.lahetaKilpailijat = 1;
	apiconfig.vastaanottaKilpailijat = 1;
	apiconfig.lahetaValiajat = 1;
	apiconfig.vastaanottaValiajat = 1;
	apiconfig.lahetaTulokset = 1;
}

void ApiConfigLataa(void)
{
	ApiConfigNollaa();
	UnicodeString polku = ApiConfigPolku();
	UnicodeString exe = ApiConfigExePolku();
	if (!FileExists(polku) && polku.CompareIC(exe) != 0 && FileExists(exe)) {
		CopyFileW(exe.c_str(), polku.c_str(), TRUE);
		if (!FileExists(polku))
			polku = exe;
	}
	if (!FileExists(polku))
		return;
	ApiConfigLueTiedostosta(polku);
}

void ApiConfigTallenna(void)
{
	UnicodeString polku = ApiConfigPolku();
	WritePrivateProfileStringW(L"jahonline", L"url", apiconfig.apiUrl, polku.c_str());
	WritePrivateProfileStringW(L"jahonline", L"api_key", apiconfig.apiKey, polku.c_str());
	WritePrivateProfileStringW(L"jahonline", L"port", IntToStr(apiconfig.apiPort).c_str(), polku.c_str());
	WritePrivateProfileStringW(L"jahonline", L"kilpailu_id", IntToStr(apiconfig.kilpailuId).c_str(), polku.c_str());
	WritePrivateProfileStringW(L"jahonline", L"vali", IntToStr(apiconfig.lahetysvali).c_str(), polku.c_str());
	WritePrivateProfileStringW(L"jahonline", L"laheta_kilpailijat", IntToStr(apiconfig.lahetaKilpailijat).c_str(), polku.c_str());
	WritePrivateProfileStringW(L"jahonline", L"vastaanotta_kilpailijat", IntToStr(apiconfig.vastaanottaKilpailijat).c_str(), polku.c_str());
	WritePrivateProfileStringW(L"jahonline", L"laheta_valiajat", IntToStr(apiconfig.lahetaValiajat).c_str(), polku.c_str());
	WritePrivateProfileStringW(L"jahonline", L"vastaanotta_valiajat", IntToStr(apiconfig.vastaanottaValiajat).c_str(), polku.c_str());
	WritePrivateProfileStringW(L"jahonline", L"laheta_tulokset", IntToStr(apiconfig.lahetaTulokset).c_str(), polku.c_str());
	WritePrivateProfileStringW(L"jahonline", L"vastaanotta_dns", IntToStr(apiconfig.vastaanottaEiLahteneet).c_str(), polku.c_str());
	WritePrivateProfileStringW(L"jahonline", L"kaynnissa", IntToStr(apiconfig.kaynnissa).c_str(), polku.c_str());
}

UnicodeString ApiBridgeUrl(void)
{
	UnicodeString url = apiconfig.apiUrl;
	url = StringReplace(url, L" ", L"", TReplaceFlags() << rfReplaceAll);
	if (url.IsEmpty())
		url = L"https://jahonline.com/public/api/kilpailijat_bridge.php";

	// If only host given, append bridge path
	if (url.Pos(L"/api/") == 0 && url.Pos(L"pirila_bridge") == 0 && url.Pos(L"kilpailijat_bridge") == 0) {
		if (url[url.Length()] == L'/')
			url += L"public/api/kilpailijat_bridge.php";
		else
			url += L"/public/api/kilpailijat_bridge.php";
	}

	// Optional port override when URL has no explicit port and apiPort > 0
	if (apiconfig.apiPort > 0 && url.Pos(L"://") > 0) {
		// leave as-is if user already put :port in URL
		UnicodeString after = url.SubString(url.Pos(L"://") + 3, url.Length());
		if (after.Pos(L":") == 0) {
			int slash = after.Pos(L"/");
			UnicodeString host = slash > 0 ? after.SubString(1, slash - 1) : after;
			UnicodeString path = slash > 0 ? after.SubString(slash, after.Length()) : UnicodeString(L"");
			UnicodeString scheme = url.SubString(1, url.Pos(L"://") + 2);
			url = scheme + host + L":" + IntToStr(apiconfig.apiPort) + path;
		}
	}
	return url;
}

// Avoid WinINet URL_COMPONENTSW: bcc32 sees both the SDK type and
// Winapi::Wininet::URL_COMPONENTSW and reports E2015.
static bool ParseHttpUrl(const UnicodeString& url, UnicodeString& host,
	INTERNET_PORT& port, UnicodeString& path, bool& https)
{
	UnicodeString u = url.Trim();
	https = false;
	port = INTERNET_DEFAULT_HTTP_PORT;
	host = L"";
	path = L"/";

	int prefix = 0;
	if (u.Pos(L"https://") == 1) {
		https = true;
		port = INTERNET_DEFAULT_HTTPS_PORT;
		prefix = 8;
	} else if (u.Pos(L"http://") == 1) {
		prefix = 7;
	} else {
		return false;
	}

	UnicodeString rest = u.SubString(prefix + 1, u.Length() - prefix);
	int hash = rest.Pos(L"#");
	if (hash > 0)
		rest = rest.SubString(1, hash - 1);

	int slash = rest.Pos(L"/");
	int qmark = rest.Pos(L"?");
	int hostEnd = rest.Length() + 1;
	if (slash > 0 && (qmark == 0 || slash < qmark))
		hostEnd = slash;
	else if (qmark > 0)
		hostEnd = qmark;

	UnicodeString hostport = hostEnd > 1 ? rest.SubString(1, hostEnd - 1) : UnicodeString(L"");
	if (hostEnd <= rest.Length())
		path = rest.SubString(hostEnd, rest.Length() - hostEnd + 1);
	else
		path = L"/";
	if (path.IsEmpty() || path[1] == L'?')
		path = UnicodeString(L"/") + path;

	int colon = hostport.LastDelimiter(L":");
	if (colon > 0 && colon < hostport.Length()) {
		host = hostport.SubString(1, colon - 1);
		port = (INTERNET_PORT)_wtoi(hostport.SubString(colon + 1, hostport.Length() - colon).c_str());
		if (port == 0)
			port = https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;
	} else {
		host = hostport;
	}
	return !host.IsEmpty();
}

static bool HttpRequest(const wchar_t* method, const UnicodeString& url,
	const UnicodeString& body, UnicodeString& vastaus)
{
	vastaus = L"";
	HINTERNET hInternet = InternetOpenW(L"Tulospalvelu-JAHOnline/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hInternet)
		return false;

	UnicodeString host, path;
	INTERNET_PORT port = INTERNET_DEFAULT_HTTP_PORT;
	bool https = false;
	if (!ParseHttpUrl(url, host, port, path, https)) {
		InternetCloseHandle(hInternet);
		return false;
	}

	DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
	if (https)
		flags |= INTERNET_FLAG_SECURE;

	HINTERNET hConnect = InternetConnectW(hInternet, host.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (!hConnect) {
		InternetCloseHandle(hInternet);
		return false;
	}

	const wchar_t* acceptTypes[] = { L"*/*", NULL };
	HINTERNET hRequest = HttpOpenRequestW(hConnect, method, path.c_str(), NULL, NULL, acceptTypes, flags, 0);
	if (!hRequest) {
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return false;
	}

	UnicodeString headers = L"Content-Type: application/json; charset=utf-8\r\nAccept: application/json\r\n";
	if (apiconfig.apiKey[0] != 0) {
		headers += L"Authorization: Bearer ";
		headers += apiconfig.apiKey;
		headers += L"\r\n";
		headers += L"X-API-Key: ";
		headers += apiconfig.apiKey;
		headers += L"\r\n";
	}
	HttpAddRequestHeadersW(hRequest, headers.c_str(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);

	AnsiString bodyUtf8 = ApiWideToUtf8(body);
	BOOL ok = HttpSendRequestW(hRequest, NULL, 0,
		body.IsEmpty() ? NULL : (LPVOID)bodyUtf8.c_str(),
		body.IsEmpty() ? 0 : bodyUtf8.Length());

	if (ok) {
		char buf[4096];
		DWORD n = 0;
		AnsiString raw;
		while (InternetReadFile(hRequest, buf, sizeof(buf) - 1, &n) && n > 0) {
			buf[n] = 0;
			raw += AnsiString(buf, n);
		}
		vastaus = ApiUtf8ToWide(raw);
	}

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);
	return ok != FALSE;
}

bool ApiHttpPostJson(const UnicodeString& url, const UnicodeString& jsonBody, UnicodeString& vastaus)
{
	return HttpRequest(L"POST", url, jsonBody, vastaus);
}

bool ApiHttpGetAuth(const UnicodeString& url, UnicodeString& vastaus)
{
	return HttpRequest(L"GET", url, L"", vastaus);
}

__fastcall TFormApiYhteydet::TFormApiYhteydet(TComponent* Owner)
	: TForm(Owner)
{
	Scaled = false;
	if (Screen->PixelsPerInch != 96)
		ScaleBy(Screen->PixelsPerInch, 96);
}

void __fastcall TFormApiYhteydet::FormShow(TObject *Sender)
{
	ApiConfigLataa();
	LueTiedot();
	PaivitaTila(L"Asetukset: " + ApiConfigPolku());
	PaivitaTila(L"Asetukset ladattu. Testaa yhteys Bearer-pingillä.");
}

void __fastcall TFormApiYhteydet::LueTiedot(void)
{
	EditUrl->Text = apiconfig.apiUrl;
	EditPortti->Text = IntToStr(apiconfig.apiPort);
	EditApiKey->Text = apiconfig.apiKey;
	EditKilpailuId->Text = IntToStr(apiconfig.kilpailuId);
	CBLahetaKilpailijat->Checked = apiconfig.lahetaKilpailijat != 0;
	CBVastaanottaKilpailijat->Checked = apiconfig.vastaanottaKilpailijat != 0;
	CBLahetaValiajat->Checked = apiconfig.lahetaValiajat != 0;
	CBVastaanottaValiajat->Checked = apiconfig.vastaanottaValiajat != 0;
	CBLahetaTulokset->Checked = apiconfig.lahetaTulokset != 0;
	CBVastaanottaEiLahteneet->Checked = apiconfig.vastaanottaEiLahteneet != 0;
	EditLahetysvali->Text = IntToStr(apiconfig.lahetysvali);

	if (apiconfig.kaynnissa) {
		LabelYhteysTila->Caption = L"AKTIIVINEN";
		LabelYhteysTila->Font->Color = clGreen;
	} else {
		LabelYhteysTila->Caption = L"EI AKTIIVINEN";
		LabelYhteysTila->Font->Color = clRed;
	}
	if (LabelIniPolku)
		LabelIniPolku->Caption = L"Asetustiedosto (kilpailun kansio):\r\n" + ApiConfigPolku();
}

void __fastcall TFormApiYhteydet::KirjoitaTiedot(void)
{
	wcsncpy(apiconfig.apiUrl, EditUrl->Text.c_str(), sizeof(apiconfig.apiUrl)/2 - 1);
	apiconfig.apiUrl[sizeof(apiconfig.apiUrl)/2 - 1] = 0;
	apiconfig.apiPort = _wtoi(EditPortti->Text.c_str());
	wcsncpy(apiconfig.apiKey, EditApiKey->Text.c_str(), sizeof(apiconfig.apiKey)/2 - 1);
	apiconfig.apiKey[sizeof(apiconfig.apiKey)/2 - 1] = 0;
	apiconfig.kilpailuId = _wtoi(EditKilpailuId->Text.c_str());
	apiconfig.lahetaKilpailijat = CBLahetaKilpailijat->Checked ? 1 : 0;
	apiconfig.vastaanottaKilpailijat = CBVastaanottaKilpailijat->Checked ? 1 : 0;
	apiconfig.lahetaValiajat = CBLahetaValiajat->Checked ? 1 : 0;
	apiconfig.vastaanottaValiajat = CBVastaanottaValiajat->Checked ? 1 : 0;
	apiconfig.lahetaTulokset = CBLahetaTulokset->Checked ? 1 : 0;
	apiconfig.vastaanottaEiLahteneet = CBVastaanottaEiLahteneet->Checked ? 1 : 0;
	apiconfig.lahetysvali = _wtoi(EditLahetysvali->Text.c_str());
	if (apiconfig.lahetysvali < 2)
		apiconfig.lahetysvali = 2;
}

void __fastcall TFormApiYhteydet::PaivitaTila(UnicodeString viesti)
{
	UnicodeString aika = FormatDateTime(L"hh:nn:ss", Now());
	MemoTila->Lines->Add(L"[" + aika + L"] " + viesti);
	while (MemoTila->Lines->Count > 200)
		MemoTila->Lines->Delete(0);
	MemoTila->SelStart = MemoTila->GetTextLen();
}

void __fastcall TFormApiYhteydet::TestaaYhteys(void)
{
	KirjoitaTiedot();
	ApiConfigTallenna();

	if (apiconfig.kilpailuId <= 0) {
		PaivitaTila(L"VIRHE: aseta kilpailu_id (JAHOnline)");
		return;
	}
	if (apiconfig.apiKey[0] == 0) {
		PaivitaTila(L"VIRHE: aseta API-avain (= kilpailun api_token)");
		return;
	}

	UnicodeString url = ApiBridgeUrl();
	UnicodeString body = L"{\"action\":\"ping\",\"kilpailu_id\":" + IntToStr(apiconfig.kilpailuId) + L"}";
	UnicodeString vastaus;
	PaivitaTila(L"POST ping → " + url);

	if (!ApiHttpPostJson(url, body, vastaus)) {
		PaivitaTila(L"VIRHE: HTTP-pyyntö epäonnistui");
		apiconfig.kaynnissa = 0;
		LabelYhteysTila->Caption = L"EI AKTIIVINEN";
		LabelYhteysTila->Font->Color = clRed;
		return;
	}

	if (ApiJsonStatusOk(vastaus)) {
		PaivitaTila(L"OK: " + vastaus.SubString(1, 180));
		apiconfig.kaynnissa = 1;
		LabelYhteysTila->Caption = L"AKTIIVINEN";
		LabelYhteysTila->Font->Color = clGreen;
		ApiConfigTallenna();
		TApiIntegration::GetInstance()->Alusta();
	} else {
		PaivitaTila(L"VIRHE vastaus: " + vastaus.SubString(1, 240));
		apiconfig.kaynnissa = 0;
		LabelYhteysTila->Caption = L"EI AKTIIVINEN";
		LabelYhteysTila->Font->Color = clRed;
	}
}

void __fastcall TFormApiYhteydet::BtnYhteysTestiClick(TObject *Sender)
{
	TestaaYhteys();
}

void __fastcall TFormApiYhteydet::BtnLahetaNytClick(TObject *Sender)
{
	KirjoitaTiedot();
	ApiConfigTallenna();
	int n = ApiSynkkaaLahetaKaikki();
	PaivitaTila(L"Lähetys valmis, rivejä: " + IntToStr(n));
}

void __fastcall TFormApiYhteydet::BtnHaeNytClick(TObject *Sender)
{
	KirjoitaTiedot();
	ApiConfigTallenna();
	int n = ApiSynkkaaHaeKaikki();
	PaivitaTila(L"Haku valmis, päivitetty: " + IntToStr(n));
}

void __fastcall TFormApiYhteydet::BtnPaivitaTilaClick(TObject *Sender)
{
	PaivitaTila(L"URL=" + ApiBridgeUrl() + L" kilpailu_id=" + IntToStr(apiconfig.kilpailuId));
}

void __fastcall TFormApiYhteydet::BtnTyhjennaMemoClick(TObject *Sender)
{
	MemoTila->Clear();
}

void __fastcall TFormApiYhteydet::BtnVieInternetClick(TObject *Sender)
{
	ShellExecuteW(NULL, L"open",
		L"https://github.com/jaho81-sys/tulospalvelu/blob/main/docs/api-jahonline.md",
		NULL, NULL, SW_SHOW);
}

void __fastcall TFormApiYhteydet::BtnOhjeClick(TObject *Sender)
{
	ShowMessage(
		L"JAHOnline API (kaksisuuntainen)\n\n"
		L"URL: https://jahonline.com/public/api/kilpailijat_bridge.php\n"
		L"API-avain: hallinnan kilpailun api_token\n"
		L"kilpailu_id: JAHOnline-kilpailun ID\n\n"
		L"Lähetä kilpailijat → action=synkkaa (osanottajat, ajat, läsnäolo, väliajat)\n"
		L"Hae kilpailijat → action=kilpailijat (päivitys paikalliseen KILP.DAT)\n"
		L"Online-rasti / ajanotto → action=tapahtuma (piste, aika_sec = tuloksen sekunnit)\n"
		L"ViestiWin: sama protokolla + kenttä osuus (1-pohjainen).\n"
		L"Emit-luenta merkitsee lähtijän läsnäolevaksi ja synkkaa heti.\n"
		L"Asetukset tallennetaan kilpailun kansion jahonline_api.ini -tiedostoon\n"
		L"ja luetaan sieltä, kun kilpailu avataan.\n"
		L"Auth: Authorization: Bearer <api_token>\n"
	);
}

void __fastcall TFormApiYhteydet::EditUrlChange(TObject *Sender)
{
}

void __fastcall TFormApiYhteydet::BtnOKClick(TObject *Sender)
{
	KirjoitaTiedot();
	ApiConfigTallenna();
	TApiIntegration::GetInstance()->Alusta();
	PaivitaTila(L"Asetukset tallennettu: " + ApiConfigPolku());
	Close();
}

void __fastcall TFormApiYhteydet::BtnPeruutaClick(TObject *Sender)
{
	Close();
}
