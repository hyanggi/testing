// This program demonstrates abstract classes in C++.

#include <iostream>

struct A {
	virtual ~A() {}
	virtual void func() = 0; // Pure virtual function
};
// A is an abstract class and cannot be instantiated.

struct B: A {
	void func() {
		std::cout << "B::func" << std::endl;
	}
};

int main() {
	B b;
	A &a = b;
	a.func();

	return 0;
}
