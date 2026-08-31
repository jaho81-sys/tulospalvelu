#include <vcl.h>
#pragma hdrstop
#include <windows.h>
#include "ApiJson.h"

UnicodeString ApiJsonEscape(const UnicodeString& s)
{
	UnicodeString out;
	out.SetLength(0);
	for (int i = 1; i <= s.Length(); i++) {
		wchar_t c = s[i];
		switch (c) {
		case L'\\': out += L"\\\\"; break;
		case L'"':  out += L"\\\""; break;
		case L'\n': out += L"\\n"; break;
		case L'\r': out += L"\\r"; break;
		case L'\t': out += L"\\t"; break;
		default:
			if (c < 32) {
				out += L"\\u";
				out += IntToHex((int)c, 4);
			} else {
				out += c;
			}
		}
	}
	return out;
}

UnicodeString ApiJsonString(const UnicodeString& s)
{
	return L"\"" + ApiJsonEscape(s) + L"\"";
}

UnicodeString ApiJsonNumber(int v)
{
	return IntToStr(v);
}

UnicodeString ApiJsonNumber64(__int64 v)
{
	return IntToStr((__int64)v);
}

UnicodeString ApiJsonNull()
{
	return L"null";
}

UnicodeString ApiJsonBool(bool v)
{
	return v ? UnicodeString(L"true") : UnicodeString(L"false");
}

AnsiString ApiWideToUtf8(const UnicodeString& s)
{
	if (s.IsEmpty())
		return AnsiString();
	int n = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), s.Length(), NULL, 0, NULL, NULL);
	if (n <= 0)
		return AnsiString(s);
	AnsiString out;
	out.SetLength(n);
	WideCharToMultiByte(CP_UTF8, 0, s.c_str(), s.Length(), out.c_str(), n, NULL, NULL);
	return out;
}

UnicodeString ApiUtf8ToWide(const AnsiString& s)
{
	if (s.IsEmpty())
		return UnicodeString();
	int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.Length(), NULL, 0);
	if (n <= 0)
		return UnicodeString(s);
	UnicodeString out;
	out.SetLength(n);
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.Length(), out.c_str(), n);
	return out;
}

static int FindKeyPos(const UnicodeString& json, const UnicodeString& key)
{
	UnicodeString pat = L"\"" + key + L"\"";
	int p = json.Pos(pat);
	if (p <= 0)
		return 0;
	for (int i = p + pat.Length(); i <= json.Length(); i++) {
		wchar_t c = json[i];
		if (c == L':')
			return i + 1;
		if (c != L' ' && c != L'\t' && c != L'\r' && c != L'\n')
			break;
	}
	return 0;
}

static void SkipWs(const UnicodeString& json, int& i)
{
	while (i <= json.Length()) {
		wchar_t c = json[i];
		if (c != L' ' && c != L'\t' && c != L'\r' && c != L'\n')
			break;
		i++;
	}
}

bool ApiJsonFindString(const UnicodeString& json, const UnicodeString& key, UnicodeString& out)
{
	int i = FindKeyPos(json, key);
	if (i <= 0)
		return false;
	SkipWs(json, i);
	if (i > json.Length() || json[i] != L'"')
		return false;
	i++;
	out = L"";
	while (i <= json.Length()) {
		wchar_t c = json[i++];
		if (c == L'\\') {
			if (i > json.Length())
				break;
			wchar_t n = json[i++];
			if (n == L'n') out += L'\n';
			else if (n == L'r') out += L'\r';
			else if (n == L't') out += L'\t';
			else out += n;
		} else if (c == L'"') {
			return true;
		} else {
			out += c;
		}
	}
	return false;
}

bool ApiJsonFindInt(const UnicodeString& json, const UnicodeString& key, int& out)
{
	__int64 v = 0;
	if (!ApiJsonFindInt64(json, key, v))
		return false;
	out = (int)v;
	return true;
}

bool ApiJsonFindInt64(const UnicodeString& json, const UnicodeString& key, __int64& out)
{
	int i = FindKeyPos(json, key);
	if (i <= 0)
		return false;
	SkipWs(json, i);
	if (i <= json.Length() && json[i] == L'n') // null
		return false;
	bool neg = false;
	if (i <= json.Length() && json[i] == L'-') {
		neg = true;
		i++;
	}
	if (i > json.Length() || json[i] < L'0' || json[i] > L'9')
		return false;
	__int64 v = 0;
	while (i <= json.Length() && json[i] >= L'0' && json[i] <= L'9') {
		v = v * 10 + (json[i] - L'0');
		i++;
	}
	out = neg ? -v : v;
	return true;
}

bool ApiJsonFindBool(const UnicodeString& json, const UnicodeString& key, bool& out)
{
	int i = FindKeyPos(json, key);
	if (i <= 0)
		return false;
	SkipWs(json, i);
	if (i + 3 <= json.Length() && json.SubString(i, 4).CompareIC(L"true") == 0) {
		out = true;
		return true;
	}
	if (i + 4 <= json.Length() && json.SubString(i, 5).CompareIC(L"false") == 0) {
		out = false;
		return true;
	}
	return false;
}

bool ApiJsonStatusOk(const UnicodeString& json)
{
	UnicodeString st;
	if (!ApiJsonFindString(json, L"status", st))
		return false;
	return st.CompareIC(L"ok") == 0 || st.CompareIC(L"partial") == 0;
}

int ApiJsonExtractObjectArray(const UnicodeString& json, const UnicodeString& arrayKey,
	std::vector<UnicodeString>& objects)
{
	objects.clear();
	UnicodeString pat = L"\"" + arrayKey + L"\"";
	int p = json.Pos(pat);
	if (p <= 0)
		return 0;
	int i = p + pat.Length();
	while (i <= json.Length() && json[i] != L'[')
		i++;
	if (i > json.Length())
		return 0;
	i++; // after [
	while (i <= json.Length()) {
		SkipWs(json, i);
		if (i > json.Length())
			break;
		if (json[i] == L']')
			break;
		if (json[i] == L',') {
			i++;
			continue;
		}
		if (json[i] != L'{') {
			i++;
			continue;
		}
		int start = i;
		int depth = 0;
		bool inStr = false;
		bool esc = false;
		for (; i <= json.Length(); i++) {
			wchar_t c = json[i];
			if (inStr) {
				if (esc) esc = false;
				else if (c == L'\\') esc = true;
				else if (c == L'"') inStr = false;
				continue;
			}
			if (c == L'"') {
				inStr = true;
				continue;
			}
			if (c == L'{') depth++;
			else if (c == L'}') {
				depth--;
				if (depth == 0) {
					objects.push_back(json.SubString(start, i - start + 1));
					i++;
					break;
				}
			}
		}
	}
	return (int)objects.size();
}
