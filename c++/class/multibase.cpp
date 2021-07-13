// This program demonstrates multiple inheritance in C++.

#include <iostream>

struct A {
	virtual ~A() {}
	const char *s;
};

// "virtual A" means B1 does not exclusively own A as a base. It needs to be shared.
struct B1: virtual A {};

struct B2: virtual A {};

struct B3: A {};

struct B4: A {};

// C inherits only one copy of virtual A, and each copy of non-virtual A.
struct C: B1, B2, B3, B4 {};

int main() {
	C c;
	c.B1::s = "Virtual";
	c.B3::s = "Normal1";
	c.B4::s = "Normal2";
	std::cout << c.B1::s << " ";
	std::cout << c.B2::s << " ";
	std::cout << c.B3::s << " ";
	std::cout << c.B4::s << std::endl;

	return 0;
}
