// JSON helpers for JAHOnline / Pirilä API bridge (C++Builder, no external lib)
#ifndef ApiJsonH
#define ApiJsonH

#include <System.hpp>
#include <System.SysUtils.hpp>
#include <vector>

UnicodeString ApiJsonEscape(const UnicodeString& s);
UnicodeString ApiJsonString(const UnicodeString& s);
UnicodeString ApiJsonNumber(int v);
UnicodeString ApiJsonNumber64(__int64 v);
UnicodeString ApiJsonNull();
UnicodeString ApiJsonBool(bool v);

// Wide → UTF-8 for HTTP body
AnsiString ApiWideToUtf8(const UnicodeString& s);
UnicodeString ApiUtf8ToWide(const AnsiString& s);

// Minimal extractors for inbound JSON (server responses)
bool ApiJsonFindString(const UnicodeString& json, const UnicodeString& key, UnicodeString& out);
bool ApiJsonFindInt(const UnicodeString& json, const UnicodeString& key, int& out);
bool ApiJsonFindInt64(const UnicodeString& json, const UnicodeString& key, __int64& out);
bool ApiJsonFindBool(const UnicodeString& json, const UnicodeString& key, bool& out);
bool ApiJsonStatusOk(const UnicodeString& json);

// Split top-level array value for key "kilpailijat" into object chunks
int ApiJsonExtractObjectArray(const UnicodeString& json, const UnicodeString& arrayKey,
	std::vector<UnicodeString>& objects);

#endif
