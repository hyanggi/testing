// This program behaves differently when compiled with or without the -O option because of the use of "restrict".

#include <stdio.h>

// When "restrict" is used on a pointer, the programmer guarantees that,
// if an object is accessed through the pointer and is also modified by any means,
// then all accesses to that object occur through this pointer.

// When "restrict" is used on a pointer, the compiler may assume that,
// the pointer is the only way to access the objects it accesses.
// If an object is modified through this pointer,
// the compiler may assume that any object assessed through other means does not change because of this operation.
// If an object is modified through other means,
// the compiler may assume that any object assessed through this pointer does not change because of that operation.

#define FUNC1 { p[2] = 1;     q[1] = 3; return p[2] == 1; }
// When p+2 == q+1, p[2] does not equal 1 because "q[1] = 3;" has changed p[2] to 3.
// When "restrict" is used on either p or q, the compiler may assume that p[2] does not change because of "q[1] = 3;".
// So the compiler may get a result where p[2] equals 1.

#define FUNC2 { int n = p[2]; q[1] = 3; return p[2] == n; }
// When p+2 == q+1, p[2] does not equal n because "q[1] = 3;" has changed p[2] to 3 (originally 0).
// When "restrict" is used on either p or q, the compiler may assume that p[2] does not change because of "q[1] = 3;".
// So the compiler may get a result where p[2] equals n.

int func10(int *p, int *q)
FUNC1

int func11(int *restrict p, int *q)
FUNC1

int func12(int *p, int *restrict q)
FUNC1

int func20(int *p, int *q)
FUNC2

int func21(int *restrict p, int *q)
FUNC2

int func22(int *p, int *restrict q)
FUNC2

int main(void)
{
	int a[5];
	printf("%d  ", func10(a+0, a+1));
	printf("%d  ", func11(a+1, a+2));
	printf("%d\n", func12(a+2, a+3));

	int b[5] = {0};
	printf("%d  ", func20(b+0, b+1));
	printf("%d  ", func21(b+1, b+2));
	printf("%d\n", func22(b+2, b+3));

	return 0;
}
