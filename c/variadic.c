// This program demonstrates variadic functions.

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// When calling variadic functions, each argument in the variable argument list undergoes the "default argument promotions".
// When doing the "default argument promotions":
// "float" is converted to "double". (In C++ this is called the "floating-point promotion".)
// "signed char", "unsigned char", "short", "unsigned short" are converted to "int" or "unsigned int" (in C called the "integer promotions", in C++ called the "integral promotions").

int print(FILE *out, const char types[], ...)
{
	va_list args;
	va_start(args, types);

	const char *type = types;
	while (*type != '\0') {
		if (*type == 'd') {
			int current = va_arg(args, int);
			fprintf(out, "int: %d\n", current);
		}
		else if (*type == 'f') {
			double current = va_arg(args, double);
			fprintf(out, "double: %f\n", current);
		}
		else if (*type == 's') {
			const char *current = va_arg(args, char*);
			fprintf(out, "string: %s\n", current);
		}
		else {
			fprintf(stderr, "Unrecognized type %c.\n", *type);
			va_end(args);
			return -1;
		}
		++type;
	}

	va_end(args);
	return 0;
}

#define PARAM_LIST "dfs", 3, 3.0f, "str"

int main(void)
{
	if (print(stdout, PARAM_LIST) < 0)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
