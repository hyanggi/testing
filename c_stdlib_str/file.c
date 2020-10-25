// This program experiments with the C I/O library.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

int write_b(FILE *fp)
{
	if (fputs("lü绿🍀\n", fp) < 0) {
		fprintf(stderr, "Writing file failed.\n");
		return -1;
	}
	return 0;
}

int write_w(FILE *fp)
{
	if (fputws(L"Lü绿🍀\n", fp) < 0) {
		fprintf(stderr, "Writing file failed.\n");
		return -1;
	}
	return 0;
}

int read_b(FILE *fp)
{
	char read_c[20];
	if (fgets(read_c, sizeof read_c, fp) == NULL) {
		fprintf(stderr, "Reading file failed.\n");
		return -1;
	}
	printf("%s", read_c);
	return 0;
}

int read_w(FILE *fp)
{
	wchar_t read_wc[10];
	if (fgetws(read_wc, sizeof read_wc / sizeof read_wc[0], fp) == NULL) {
		fprintf(stderr, "Reading file failed.\n");
		return -1;
	}
	printf("%ls", read_wc);
	return 0;
}

int open_and_rw(const char *filename, const char *mode, int (*rw_func)(FILE*))
{
	FILE *fp = fopen(filename, mode);
	if (fp == NULL) {
		fprintf(stderr, "Opening file failed.\n");
		return -1;
	}
	if (rw_func(fp) < 0) {
		if (fclose(fp) < 0)
			fprintf(stderr, "Closing file failed.\n");
		return -1;
	}
	if (fclose(fp) < 0) {
		fprintf(stderr, "Closing file failed.\n");
		return -1;
	}
	return 0;
}

int test_rw(const char *filename, int (*w_func)(FILE*), int (*r_func)(FILE*))
{
	printf("%s: ", filename);
	if (open_and_rw(filename, "w", w_func) < 0)
		return -1;
	if (open_and_rw(filename, "r", r_func) < 0)
		return -1;
	return 0;
}

int main(void)
{
	if (setlocale(LC_ALL, "en_US.UTF-8") == NULL) {
		fprintf(stderr, "Setting locale failed.\n");
		return EXIT_FAILURE;
	}

	const char *tmpdir = getenv("TMPDIR");
	if (tmpdir == NULL)
		tmpdir = "/tmp";

	const char *filename_part = "/c-file-io-testfile-";
	size_t filename_len = strlen(tmpdir) + strlen(filename_part) + 1 + 1;

	char *filename = (char*)malloc(filename_len);
	if (filename == NULL) {
		fprintf(stderr, "malloc failed.\n");
		return EXIT_FAILURE;
	}

	strcpy(filename, tmpdir);
	strcat(filename, filename_part);
	filename[filename_len - 1] = '\0';

	filename[filename_len - 2] = '1';
	if (test_rw(filename, write_b, read_w) < 0) {
		free(filename);
		return EXIT_FAILURE;
	}

	filename[filename_len - 2] = '2';
	if (test_rw(filename, write_w, read_b) < 0) {
		free(filename);
		return EXIT_FAILURE;
	}

	free(filename);
	return EXIT_SUCCESS;
}
