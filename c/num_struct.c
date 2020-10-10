// This program demonstrates the internal binary structure of integers and doubles.

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stddef.h>
#include <limits.h>

#if CHAR_BIT != 8
#error A byte is not 8 bits.
#endif

static_assert(sizeof(int) == 4, "The size of int is incorrect.");
static_assert(sizeof(unsigned int) == 4, "The size of unsigned int is incorrect.");
static_assert(sizeof(double) == 8, "The size of double is incorrect.");

enum num_type
{
	type_uint,
	type_int,
	type_double,
};

struct number
{
	enum num_type type;
	union {
		unsigned int num_uint;
		struct {
			unsigned char bytes[4];
		} struct_uint;

		int num_int;
		struct {
			unsigned char bytes[3];
			unsigned int byte_last: 7;
			bool neg: 1;
		} struct_int;

		double num_double;
		struct {
			unsigned int fraction1;
			unsigned int fraction2: 20;
			unsigned int exponent: 11;
			bool neg: 1;
		} struct_double;
	};
};

void print_num(const struct number *num)
{
	static_assert(sizeof num->struct_uint == sizeof num->num_uint, "The size of struct_uint is incorrect.");
	static_assert(sizeof num->struct_int == sizeof num->num_int, "The size of struct_int is incorrect.");
	static_assert(sizeof num->struct_double == sizeof num->num_double, "The size of struct_double is incorrect.");

	switch (num->type) {
	case type_uint:
		printf("%u\n", num->num_uint);
		break;
	case type_int:
		printf("%d\n", num->num_int);
		break;
	case type_double:
		printf("%f\n", num->num_double);
		break;
	default:
		assert(false); // internal error
	}

	if (num->type == type_double) {
		printf("Sign: %d; ", num->struct_double.neg);

		printf("Exponent: %d; ", (int)num->struct_double.exponent - 1023);

		printf("Fraction:");
		unsigned int fraction2 = num->struct_double.fraction2;
		printf(" %hhx", ((char*)&fraction2)[2]);
		printf(" %02hhx", ((char*)&fraction2)[1]);
		printf(" %02hhx", ((char*)&fraction2)[0]);
		unsigned int fraction1 = num->struct_double.fraction1;
		for (int i=3; i>=0; --i)
			printf(" %02hhx", ((char*)&fraction1)[i]);
		printf(".\n");
	}
	else {
		printf("Binary:");
		for (int i=3; i>=0; --i)
			printf(" %02hhx", num->struct_uint.bytes[i]);
		printf(".\n");
	}
}

bool is_little_endian(void)
{
	union {
		unsigned int value;
		unsigned char bytes[4];
	} checker = {
		.bytes = {0, 1, 2, 3}
	};

	static_assert(sizeof checker.value == sizeof checker.bytes, "The size of unsigned int is incorrect.");

	if (checker.value == 0x03020100) // Little endian
		return true;
	else
		return false;
}

int main(void)
{
	if (!is_little_endian())
		return -1;

	struct number largest_unsigned_num = {.type = type_uint, .struct_uint = {.bytes = {0xff, 0xff, 0xff, 0xff}}};
	print_num(&largest_unsigned_num);

	struct number largest_pos_num = {.type = type_int, .struct_int = {.bytes = {0xff, 0xff, 0xff}, .byte_last = 0x7f, .neg = false}};
	print_num(&largest_pos_num);

	struct number smallest_neg_num = {.type = type_int, .struct_int = {.bytes = {0}, .byte_last = 0, .neg = true}};
	print_num(&smallest_neg_num);

	struct number double_num0 = {.type = type_double, .num_double = 1.0};
	print_num(&double_num0);

	struct number double_num1 = {.type = type_double, .num_double = 1.5};
	print_num(&double_num1);

	struct number double_num2 = {.type = type_double, .num_double = 1.0 + 0.5 + 0.25 + 0.125 + 0.0625};
	print_num(&double_num2);

	struct number double_num3 = {.type = type_double, .num_double = -0.25};
	print_num(&double_num3);

	struct number double_num4 = {.type = type_double, .num_double = -4.0};
	print_num(&double_num4);

	return 0;
}
