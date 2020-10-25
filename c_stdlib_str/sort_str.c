// This program uses different ways to sort strings.

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>

int cmp_strcmp(const void *a, const void *b)
{
	const char *string_a = *(const char *const *)a;
	const char *string_b = *(const char *const *)b;
	return strcmp(string_a, string_b);
}

int cmp_strcoll(const void *a, const void *b)
{
	const char *string_a = *(const char *const *)a;
	const char *string_b = *(const char *const *)b;
	return strcoll(string_a, string_b);
}

void sort_and_print_str(const char *strings[], size_t len, int (*cmp)(const void*, const void*))
{
	if (cmp != NULL)
		qsort(strings, len, sizeof(const char*), cmp);
	for (size_t i=0; i<len; ++i)
		printf(" %s", strings[i]);
	printf("\n");
}

int cmp_wcscmp(const void *a, const void *b)
{
	const wchar_t *string_a = *(const wchar_t *const *)a;
	const wchar_t *string_b = *(const wchar_t *const *)b;
	return wcscmp(string_a, string_b);
}

int cmp_wcscoll(const void *a, const void *b)
{
	const wchar_t *string_a = *(const wchar_t *const *)a;
	const wchar_t *string_b = *(const wchar_t *const *)b;
	return wcscoll(string_a, string_b);
}

void sort_and_print_wcs(const wchar_t *wstrings[], size_t len, int (*cmp)(const void*, const void*))
{
	if (cmp != NULL)
		qsort(wstrings, len, sizeof(const wchar_t*), cmp);
	for (size_t i=0; i<len; ++i)
		printf(" %ls", wstrings[i]);
	printf("\n");
}

int main(void)
{
	if (setlocale(LC_ALL, "zh_CN.UTF-8") == NULL) {
		fprintf(stderr, "Setting locale failed.\n");
		return -1;
	}

	const char *strings[] = {"按", "正确", "的", "方式", "排序"};
	size_t len = sizeof strings / sizeof(const char*);

	printf("Original:");
	sort_and_print_str(strings, len, NULL);

	printf("Sort by strcmp:");
	sort_and_print_str(strings, len, cmp_strcmp); // Sort by internal byte representation

	printf("Sort by strcoll:");
	sort_and_print_str(strings, len, cmp_strcoll); // Sort by dictionary order
	// strcoll sees strings as composed of "collation units" and compares "collation units" one by one.
	// For example in Czech, "ch" is a "collation unit" which sorts after "h" and before "i".

	printf("\n");

	const wchar_t *wstrings[] = {L"按", L"正确", L"的", L"方式", L"排序"};
	size_t wlen = sizeof wstrings / sizeof(const wchar_t*);

	printf("Original:");
	sort_and_print_wcs(wstrings, wlen, NULL);

	printf("Sort by wcscmp:");
	sort_and_print_wcs(wstrings, wlen, cmp_wcscmp); // Sort by internal wchar_t representation

	printf("Sort by wcscoll:");
	sort_and_print_wcs(wstrings, wlen, cmp_wcscoll); // Sort by dictionary order
	// wcscoll sees strings as composed of "collation units" and compares "collation units" one by one.
	// For example in Czech, "ch" is a "collation unit" which sorts after "h" and before "i".

	return 0;
}
