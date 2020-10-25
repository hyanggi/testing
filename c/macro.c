// This program experiments with macros.

#include <stdio.h>

// Demonstration of the # ("stringification") operator in macros
#define print_equation(exp) printf("%s = %d.\n", #exp, exp)

// Demonstration of the ## ("concatenation" or "token pasting") operator in macros
#define call_haha(suffix) haha_##suffix()

// Demonstration of __VA_ARGS__ and #__VA_ARGS__ in macros
#define print1(format, ...) printf(format, __VA_ARGS__), printf("The parameters were %s.\n", #format ", " #__VA_ARGS__)
#define print2(...)         printf(__VA_ARGS__),         printf("The parameters were %s.\n", #__VA_ARGS__)

void haha_hoho(void)
{
	printf("Function \"%s\" is called.\n", __func__);
}

int main(void)
{
	call_haha(hoho);
	print_equation(3 + 5*6);
	print1("%d %d %d\n", 4, 5, 6);
	print2("%d %d %d\n", 4, 5, 6);
	return 0;
}
