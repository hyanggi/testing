// This file experiments with arrays.

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

// ---------- Testing flexible array members ------------

struct my_str
{
	size_t len;
	char str[]; // flexible array member
};

void print_my_str(const struct my_str *s)
{
	for (size_t i = 0; i < s->len; ++i)
		putchar(s->str[i]);
	putchar('\n');
}

int test_my_str(void)
{
	const char c_str[] = "Hello!";
	size_t c_str_len = strlen(c_str);

	size_t total_size_1 = offsetof(struct my_str, str) + c_str_len;
	size_t total_size_2 = sizeof(struct my_str);
	size_t total_size = total_size_1 > total_size_2 ? total_size_1 : total_size_2;

	struct my_str *s = malloc(total_size);
	if (s == NULL) {
		fprintf(stderr, "malloc failed.\n");
		return -1;
	}
	s->len = c_str_len;
	memcpy(s->str, c_str, c_str_len);

	print_my_str(s);

	free(s);
	return 0;
}

// ---------- Testing variable length arrays ---------------

void print_str(const char *x)
{
	printf("%s\n", x);
}

void test_str(void)
{
	char a[4];
	memset(a, 'a', 3);
	a[3] = '\0';
	print_str(a);

	for (unsigned i=3; i<6; ++i) {
		char vla[i]; // variable length array (VLA)
		memset(vla, 'v', i-1);
		vla[i-1] = '\0';
		print_str(vla);
	}
}

// -------- Testing variably-modified types ------------

void test_vm_types(void)
{
	char a[12];
	int n = 6;
	char (*p)[n] = &a; // VLA is compatible with any array of compatible element type.
	if (&p[1][2] == &a[8]) // p's type has the runtime information of 6. VM types are different from other types because they contain runtime information.
		printf("Expected.\n");
	else
		printf("Unexpected.\n");
}

// ----------- Arrays in function parameters -------------

void func_1(char *);
void func_1(char []); // Arrays are actually pointers in function parameters.
void func_1(char [100]); // 100 has no use here.
void func_1(char [static 100]); // "static" means the array should have at least 100 elements, otherwise it's undefined behavior.
void func_1(char [*]); // * can only appear in function prototypes. * has no use here.
void func_1(char *a)
{ a = NULL; }

// Array of arrays
void func_2(char (*)[4]);
void func_2(char [][4]);
void func_2(char [100][4]);
void func_2(char [static 100][4]);
void func_2(char [*][4]);
void func_2(char (*a)[4])
{ a = NULL; }

// Array of variable length arrays
void func_3(int, char (*)[*]);
void func_3(int, char [][*]);
void func_3(int, char [100][*]);
void func_3(int, char [static 100][*]);
void func_3(int, char [*][*]);
void func_3(int n, char (*a)[n])
{ a = NULL; }

// ------------- The main function -------------------

int main(void)
{
	test_my_str();
	test_str();
	test_vm_types();
	return 0;
}
