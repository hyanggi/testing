// This program experiments with the floating-point facilities in the C standard library.

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
#include <errno.h>
#include <string.h>

#pragma STDC FENV_ACCESS ON

#if ! (math_errhandling & MATH_ERREXCEPT)
#error MATH_ERREXCEPT not set.
#endif

#if ! (math_errhandling & MATH_ERRNO)
#error MATH_ERRNO not set.
#endif

int show_fe_exceptions(void)
{
	printf("Floating-point exceptions:");
	if (fetestexcept(FE_DIVBYZERO)) printf(" FE_DIVBYZERO");
	if (fetestexcept(FE_INEXACT))   printf(" FE_INEXACT");
	if (fetestexcept(FE_INVALID))   printf(" FE_INVALID");
	if (fetestexcept(FE_OVERFLOW))  printf(" FE_OVERFLOW");
	if (fetestexcept(FE_UNDERFLOW)) printf(" FE_UNDERFLOW");
	printf("\n");
	if (feclearexcept(FE_ALL_EXCEPT) != 0) {
		fprintf(stderr, "Clearing FP exceptions failed.\n");
		return -1;
	}
	return 0;
}

void show_errno(void)
{
	printf("Current errno: %s\n", strerror(errno));
	errno = 0;
}

void show_fe_rounding_method(void)
{
	printf("Current rounding method: ");
	int rounding_method = fegetround();
	if      (rounding_method < 0)              printf("Unable to determine");
	else if (rounding_method == FE_DOWNWARD)   printf("FE_DOWNWARD");
	else if (rounding_method == FE_TONEAREST)  printf("FE_TONEAREST");
	else if (rounding_method == FE_TOWARDZERO) printf("FE_TOWARDZERO");
	else if (rounding_method == FE_UPWARD)     printf("FE_UPWARD");
	else                                       printf("Other");
	printf("\n");
}

void show_rounding_method(void)
{
	printf("Current rounding method: ");
	switch (FLT_ROUNDS) {
		case -1: printf("Unable to determine");      break;
		case 0:  printf("Toward zero");              break;
		case 1:  printf("To nearest");               break;
		case 2:  printf("Toward positive infinity"); break;
		case 3:  printf("Toward negative infinity"); break;
		default: printf("Other");
	}
	printf("\n");
}

#define calculate(exp) printf("%s = %e\n", #exp, exp); \
                       if (show_fe_exceptions() < 0) return -1; \
                       show_errno(); \
                       printf("\n");

int main(void)
{
	show_fe_rounding_method();
	show_rounding_method();
	printf("\n");

	calculate(0.0/0.0)
	calculate(1.0/0.0)
	calculate(log(-1.0))
	calculate(log(0.0))
	calculate(DBL_MAX * 2.0)
	calculate(nextafter(DBL_MIN, 0.0))

	return 0;
}
