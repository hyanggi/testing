// This program converts a multibyte string to its UTF-16 and UTF-32 representations.
// A multibyte string's encoding is determined by the locale settings.

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <uchar.h>
#include <assert.h>
#include <stdbool.h>

#ifndef __STDC_UTF_16__
#error Not using UTF-16
#endif
#ifndef __STDC_UTF_32__
#error Not using UTF-32
#endif

#define CHARMB_LEN_MAX 4
#define CHAR16_LEN_MAX 2
#define CHAR32_LEN_MAX 1

int convert_and_print(const char *mbstr)
{
	printf("  UTF-32 - UTF-16    - Multibyte   -  Glyph\n");

	mbstate_t state16;
	memset(&state16, 0, sizeof state16);
	assert(mbsinit(&state16));
	mbstate_t state32;
	memset(&state32, 0, sizeof state32);
	assert(mbsinit(&state32));

	char16_t ac16[CHAR16_LEN_MAX];
	unsigned nc16 = 0;
	char32_t ac32[CHAR32_LEN_MAX];
	unsigned nc32 = 0;

	const char *pre_p = mbstr;
	const char *cur_p = mbstr;
	size_t remaining = strlen(mbstr) + 1;

	while (true) {
		char16_t nextc16;
		size_t res16;
		while (true) {
			res16 = mbrtoc16(&nextc16, cur_p, remaining, &state16);
			if (res16 == (size_t)-3) {
				if (nc16 == CHAR16_LEN_MAX) {
					fprintf(stderr, "Multi-char16_t sequence too long.\n");
					return -1;
				}
				ac16[nc16] = nextc16;
				++nc16;
			}
			else
				break;
		}
		char32_t nextc32;
		size_t res32;
		while (true) {
			res32 = mbrtoc32(&nextc32, cur_p, remaining, &state32);
			if (res32 == (size_t)-3) {
				if (nc32 == CHAR32_LEN_MAX) {
					fprintf(stderr, "Multi-char32_t sequence too long.\n");
					return -1;
				}
				ac32[nc32] = nextc32;
				++nc32;
			}
			else
				break;
		}

		// Conversion of the next character has started, so print out the previous character.
		unsigned nc8 = (unsigned)(cur_p - pre_p);
		for (unsigned i=0; i<nc32; ++i)
			printf("%8x ", ac32[i]);
		for (unsigned i=nc32; i<CHAR32_LEN_MAX; ++i)
			printf("%8s ", "");
		printf("- ");
		for (unsigned i=0; i<nc16; ++i)
			printf("%04hx ", ac16[i]);
		for (unsigned i=nc16; i<CHAR16_LEN_MAX; ++i)
			printf("%4s ", "");
		printf("- ");
		for (unsigned i=0; i<nc8; ++i)
			printf("%02hhx ", pre_p[i]);
		for (unsigned i=nc8; i<CHARMB_LEN_MAX; ++i)
			printf("%2s ", "");
		printf("- ");
		printf(" ");
		for (unsigned i=0; i<nc8; ++i)
			printf("%c", pre_p[i]);
		printf("\n");

		// Start dealing with the next character.
		if (res16 != res32) {
			fprintf(stderr, "Internal error.\n");
			return -1;
		}
		switch (res16) {
		case 0:
			assert(mbsinit(&state16));
			assert(mbsinit(&state32));
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
		if (res16 > CHARMB_LEN_MAX) {
			fprintf(stderr, "Multibyte sequence too long.\n");
			return -1;
		}
		ac32[0] = nextc32;
		nc32 = 1;
		ac16[0] = nextc16;
		nc16 = 1;
		pre_p = cur_p;
		cur_p += res16;
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
	convert_and_print(argv[1]);

	return 0;
}
