// This program converts a multibyte string to a wide string and prints it,
// along with the properties of the converted characters.
// A multibyte string's encoding is determined by the locale settings.
// A wide string is a UCS (Unicode) code point string (as long as "__STDC_ISO_10646__" is defined).

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <assert.h>
#include <stdbool.h>

#ifndef __STDC_ISO_10646__
#error Not using UCS (Unicode) code points for wchar_t
#endif

#define ISTYPE(type, wc) if (isw##type((wint_t)wc)) printf(" " #type);
#define ISTYPEE(type, wc) if (iswctype((wint_t)wc, wctype(#type))) printf(" " #type);

#define TOTRANS(trans, wc) { wchar_t nwc = (wchar_t)tow##trans((wint_t)wc); if (nwc != wc) printf(" to" #trans "=%lc", nwc); }
#define TOTRANSE(trans, wc) { wchar_t nwc = (wchar_t)towctrans((wint_t)wc, wctrans("to" #trans)); if (nwc != wc) printf(" to" #trans "=%lc", nwc); }

void analyze(wchar_t wc)
{
	ISTYPE(cntrl, wc)
	ISTYPE(print, wc)
	ISTYPE(space, wc)
	ISTYPE(blank, wc)
	ISTYPE(graph, wc)
	ISTYPE(punct, wc)
	ISTYPE(alnum, wc)
	ISTYPE(alpha, wc)
	ISTYPE(lower, wc)
	ISTYPE(upper, wc)
	ISTYPE(digit, wc)
	ISTYPE(xdigit, wc)
	ISTYPEE(combining, wc)
	ISTYPEE(combining_level3, wc)
	// Japanese
	ISTYPEE(jspace, wc)
	ISTYPEE(jhira, wc)
	ISTYPEE(jkata, wc)
	ISTYPEE(jkanji, wc)
	ISTYPEE(jdigit, wc)

	TOTRANS(lower, wc)
	TOTRANS(upper, wc)
	TOTRANSE(title, wc)
	// Japanese
	TOTRANSE(jhira, wc)
	TOTRANSE(jkata, wc)
}

void print_char(wchar_t wc)
{
	printf(" ");
	if (!iswprint((wint_t)wc))
		if (wc < 0x20)
			printf("%lc", wc + 0x2400);
		else if (wc == 0x7f)
			printf("%lc", 0x2421);
		else
			printf("%s", "N/A");
	else if (iswctype((wint_t)wc, wctype("combining")))
		printf("\u25cc%lc", wc);
	else
		printf("%lc", wc);
	printf(" ");
}

int print_chars(const char *mbstr)
{
	mbstate_t state;
	memset(&state, 0, sizeof state);
	assert(mbsinit(&state));

	const char *cur_p = mbstr;
	size_t remaining_len = strlen(mbstr) + 1;
	while (true) {
		wchar_t wch;
		size_t res = mbrtowc(&wch, cur_p, remaining_len, &state);
		switch (res) {
		case 0:
			assert(mbsinit(&state));
			return 0;
		case (size_t)-1:
			fprintf(stderr, "Encoding error.\n");
			return -1;
		case (size_t)-2: // Incomplete byte sequence
			// Impossible because the string is always terminated by a zero byte.
			// A zero byte is always a complete character (the null character),
			// and is never part of the legal sequence of any other character.
			fprintf(stderr, "Internal error.\n");
			return -1;
		}

		print_char(wch);
		printf("\tUnicode: %5x Multibyte:", wch);
		for (size_t i = 0; i < res; ++i)
			printf(" %02hhx", cur_p[i]);
		for (size_t i = res; i < 4; ++i)
			printf(" %2s", "");
		analyze(wch);
		printf("\n");

		cur_p += res;
		remaining_len -= res;
	}
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		fprintf(stderr, "Usage: %s string\n", argv[0]);
		return -1;
	}

	const char *locale = setlocale(LC_ALL, "");
	if (locale == NULL) {
		fprintf(stderr, "Setting locale failed. Check your locale settings.\n");
		return -1;
	}
	else
		printf("Current locale: %s\n", locale);

	printf("String: %s\n", argv[1]);
	print_chars(argv[1]);

	return 0;
}
