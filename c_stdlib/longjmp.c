// This program simulates exception handling in other languages using longjmp.

#include <stdio.h>
#include <setjmp.h>

int func(int num, jmp_buf jmp_info)
{
	if (num < 0)
		longjmp(jmp_info, 1); // throw 1
	return num;
}

int main(void)
{
	jmp_buf jmp_info; // jmp_buf is an array type.
	switch (setjmp(jmp_info)) {
	case 0: // try
		printf("%d\n", func(3, jmp_info));
		printf("%d\n", func(5, jmp_info));
		printf("%d\n", func(-3, jmp_info));
		break;
	case 1: // catch 1
		printf("Exception 1 caught.\n");
		break;
	default: // catch all
		printf("Unknown exception caught.\n");
	}
	return 0;
}
