// This program implements a macro that can print different types of values.

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

void print_int(int x)
{
	printf("%d", x);
}

void print_double(double x)
{
	printf("%f", x);
}

void print_str(const char *x)
{
	printf("%s", x);
}

// Demonstration of anonymous structs and anonymous unions
union four_byte_pack
{
	uint32_t four_bytes;
	struct {
		union {
			uint16_t first_two_bytes;
			struct {
				uint8_t first_byte;
				uint8_t second_byte;
			};
		};
		union {
			uint16_t second_two_bytes;
			struct {
				uint8_t third_byte;
				uint8_t fourth_byte;
			};
		};
	};
};

void print_four_byte_pack(const union four_byte_pack *fbp)
{
	printf("%" PRIu8 " %" PRIu8 " %" PRIu8 " %" PRIu8, fbp->first_byte, fbp->second_byte, fbp->third_byte, fbp->fourth_byte);
}

// Generic selection
#define print_any(x) _Generic((x), int: print_int, double: print_double, char*: print_str, union four_byte_pack*: print_four_byte_pack)(x)

int main(void)
{
	printf("int: ");
	print_any(3);
	printf("\n");

	printf("double: ");
	print_any(3.5);
	printf("\n");

	printf("str: ");
	print_any("hahaha");
	printf("\n");

	for (unsigned i=3; i<6; ++i) {
		char a[i]; // variable length array (VLA)
		memset(a, 'a', i-1);
		a[i-1] = '\0';
		printf("VLA: ");
		print_any(a);
		printf("\n");
	}

	printf("four_byte_pack: ");
	print_any(&(union four_byte_pack){.four_bytes = 4*256*256*256 + 3*256*256 + 2*256 + 1*1}); // compound literal
	printf("\n");

	return 0;
}
