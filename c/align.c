// This program demonstrates alignment control.

#include <stdio.h>
#include <stddef.h>
#include <stdalign.h>

struct unaligned
{
	char a;
	char b[4];
};

struct aligned
{
	char a;
	alignas(4) char b[4];
};

int main(void)
{
	printf("unaligned: align = %zu, size = %zu, ", alignof(struct unaligned), sizeof(struct unaligned));
	printf("offset a = %zu, offset b = %zu.\n", offsetof(struct unaligned, a), offsetof(struct unaligned, b));

	printf("  aligned: align = %zu, size = %zu, ", alignof(struct aligned), sizeof(struct aligned));
	printf("offset a = %zu, offset b = %zu.\n", offsetof(struct aligned, a), offsetof(struct aligned, b));

	return 0;
}
