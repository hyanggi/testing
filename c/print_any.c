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
union four_octet_pack
{
	uint32_t four_octets;
	struct {
		union {
			uint16_t first_two_octets;
			struct {
				uint8_t first_octet;
				uint8_t second_octet;
			};
		};
		union {
			uint16_t second_two_octets;
			struct {
				uint8_t third_octet;
				uint8_t fourth_octet;
			};
		};
	};
};

void print_four_octet_pack(const union four_octet_pack *fop)
{
	printf("%" PRIu8 " %" PRIu8 " %" PRIu8 " %" PRIu8, fop->first_octet, fop->second_octet, fop->third_octet, fop->fourth_octet);
}

// Generic selection
#define print_any(x) _Generic((x), int: print_int, double: print_double, char*: print_str, union four_octet_pack*: print_four_octet_pack)(x)

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

	printf("four_octet_pack: ");
	print_any(&(union four_octet_pack){.four_octets = 4*256*256*256 + 3*256*256 + 2*256 + 1*1}); // compound literal
	printf("\n");

	return 0;
}
