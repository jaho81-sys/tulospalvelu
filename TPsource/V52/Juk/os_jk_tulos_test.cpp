// Standalone test for osuus/joukkue result formatting (no VCL).
#include <stdio.h>
#include <wchar.h>
#include <string.h>

static int tuloskentta_ok(const wchar_t *s)
{
	return s && s[0] && s[0] != L' ';
}

static void liita_sija_suluissa(wchar_t *aika, const wchar_t *sija)
{
	if (!tuloskentta_ok(aika) || !tuloskentta_ok(sija))
		return;
	if (sija[0] == L'0' && sija[1] == 0)
		return;
	wcscat(aika, L" (");
	wcscat(aika, sija);
	wcscat(aika, L")");
}

static void os_jk_tulos_str(wchar_t *dst, const wchar_t *osaika, const wchar_t *ossija,
	const wchar_t *jkaika, const wchar_t *jksija)
{
	wchar_t a[48], b[48];

	a[0] = 0;
	b[0] = 0;
	dst[0] = 0;
	if (tuloskentta_ok(osaika)) {
		wcscpy(a, osaika);
		liita_sija_suluissa(a, ossija);
		}
	if (tuloskentta_ok(jkaika)) {
		wcscpy(b, jkaika);
		liita_sija_suluissa(b, jksija);
		}
	if (a[0] && b[0]) {
		wcscpy(dst, a);
		wcscat(dst, L"  ");
		wcscat(dst, b);
		}
	else if (a[0])
		wcscpy(dst, a);
	else if (b[0])
		wcscpy(dst, b);
}

static int fail = 0;

static void expect(const wchar_t *got, const wchar_t *want, const char *name)
{
	if (wcscmp(got, want) != 0) {
		fwprintf(stderr, L"FAIL %s: got [%ls] want [%ls]\n", name, got, want);
		fail++;
		}
	else
		fwprintf(stdout, L"OK   %s: %ls\n", name, got);
}

int main(void)
{
	wchar_t dst[80];

	wcscpy(dst, L"12.34");
	liita_sija_suluissa(dst, L"3");
	expect(dst, L"12.34 (3)", "liita osuus");

	wcscpy(dst, L"1.12.34");
	liita_sija_suluissa(dst, L"12");
	expect(dst, L"1.12.34 (12)", "liita joukkue");

	wcscpy(dst, L"12.34");
	liita_sija_suluissa(dst, L"");
	expect(dst, L"12.34", "tyhja sija");

	wcscpy(dst, L"12.34");
	liita_sija_suluissa(dst, L" ");
	expect(dst, L"12.34", "vali sija");

	os_jk_tulos_str(dst, L"12.34", L"3", L"1.12.34", L"1");
	expect(dst, L"12.34 (3)  1.12.34 (1)", "os+jk");

	os_jk_tulos_str(dst, L"", L"", L"1.12.34", L"1");
	expect(dst, L"1.12.34 (1)", "vain jk (1. osuus)");

	os_jk_tulos_str(dst, L"15.02", L"8", L"", L"");
	expect(dst, L"15.02 (8)", "vain os");

	os_jk_tulos_str(dst, L" ", L"3", L" ", L"1");
	expect(dst, L"", "pelkat valit");

	os_jk_tulos_str(dst, L"Kesk.", L"", L"", L"");
	expect(dst, L"Kesk.", "tarkistusmerkinta");

	wcscpy(dst, L"12.34");
	liita_sija_suluissa(dst, L"0");
	expect(dst, L"12.34", "sija nolla");

	if (fail) {
		fwprintf(stderr, L"%d failed\n", fail);
		return 1;
		}
	fwprintf(stdout, L"all tests passed\n");
	return 0;
}
