// This program experiments with the qsort and bsearch functions.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DataUnit
{
	const char *str;
};

typedef const struct DataUnit *T;

int cmp(const void *a, const void *b)
{
	T du_a = *(const T *)a;
	T du_b = *(const T *)b;
	return strcmp(du_a->str, du_b->str);
}

int main(void)
{
	struct DataUnit du0 = {"red"};
	struct DataUnit du1 = {"green"};
	struct DataUnit du2 = {"blue"};
	struct DataUnit du3 = {"yellow"};
	struct DataUnit du4 = {"white"};
	struct DataUnit du5 = {"black"};

	T list[] = {&du0, &du1, &du2, &du3, &du4, &du5};
	size_t element_size = sizeof list[0];
	size_t len = sizeof list / element_size;

	printf("Original:");
	for (size_t i=0; i<len; ++i)
		printf(" %s", list[i]->str);
	printf("\n");

	qsort(list, len, element_size, cmp);

	printf("Sorted:");
	for (size_t i=0; i<len; ++i)
		printf(" %s", list[i]->str);
	printf("\n");

	struct DataUnit du = {"red"};
	T du_p = &du;
	T *result = (T*)bsearch(&du_p, list, len, element_size, cmp);

	if (result != NULL)
		printf("Result: %s at position %td\n", (*result)->str, result - list);
	else
		printf("Result: Not found\n");

	return 0;
}
