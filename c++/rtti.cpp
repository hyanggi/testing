// C++ is a statically-typed language, which means types are handled at compile-time.
// However, in some cases C++ also makes use of run-time type information (RTTI).
// This program demonstrates RTTI in C++.

#include <typeinfo>
#include <iostream>

struct Parent {
	virtual ~Parent() {}
};

struct Child1: Parent {
};

struct Child2: Parent {
};

int main() {
	Child1 a0;
	Child2 b0;
	Parent &a1 = a0;
	Parent &b1 = b0;

	// Print the type information of a1 and b1
	std::cout << typeid(a1).name() << std::endl;
	std::cout << typeid(b1).name() << std::endl;

	// Try to dynamically down-cast a1 and b1
	try {
		[[maybe_unused]] Child1 &a2 = dynamic_cast<Child1&>(a1);
		std::cout << "Converted" << std::endl;
	}
	catch (const std::exception &e) {
		std::cout << "Failed" << std::endl;
	}
	try {
		[[maybe_unused]] Child1 &b2 = dynamic_cast<Child1&>(b1);
		std::cout << "Converted" << std::endl;
	}
	catch (const std::exception &e) {
		std::cout << "Failed" << std::endl;
	}

	return 0;
}
