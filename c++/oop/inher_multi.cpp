// This program demonstrates multiple inheritance in C++.

#include <iostream>

struct A {
	virtual ~A() {}
	const char *s;
};

struct B: virtual A {
};

struct C: virtual A {
};

struct D: A {
};

struct E: A {
};

struct F: B, C, D, E { // F inherits only one copy of virtual A, and each copy of non-virtual A.
};

int main() {
	F obj;
	obj.B::s = "Virtual";
	obj.D::s = "Normal1";
	obj.E::s = "Normal2";
	std::cout << obj.B::s << " ";
	std::cout << obj.C::s << " ";
	std::cout << obj.D::s << " ";
	std::cout << obj.E::s << std::endl;

	return 0;
}
