// This program demonstrates dynamic dispatch in C++.

#include <iostream>

struct A {
	virtual ~A() {}
private: // Access control doesn't affect dynamic dispatch
	void func0() {
		std::cout << "A::func0" << std::endl;
	}
	virtual void func1() {
		std::cout << "A::func1" << std::endl;
	}
	friend void test();
};

struct B: A {
private: // Access control doesn't affect dynamic dispatch
	void func0() {
		std::cout << "B::func0" << std::endl;
	}
	void func1() {
		std::cout << "B::func1" << std::endl;
	}
};

struct C: B {
private: // Access control doesn't affect dynamic dispatch
	void func0() {
		std::cout << "C::func0" << std::endl;
	}
	void func1() {
		std::cout << "C::func1" << std::endl;
	}
};

void test() {
	C c;
	A &a = c;
	a.func0(); // No dynamic dispatch
	a.func1();
	a.A::func1(); // Disable dynamic dispatch by explicitly specifying the function
}

int main() {
	test();
	return 0;
}
