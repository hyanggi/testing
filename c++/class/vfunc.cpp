// This program demonstrates virtual functions in C++.

/*
A virtual function is a non-static member function that is declared "virtual"
or overrides another virtual function in a base class.
A class that has (declares or inherits) a virtual function is called a polymorphic class.
*/

#include <iostream>

struct A {
	virtual ~A() {}
	void func()          { std::cout << "A::func" << std::endl; }
	virtual void vfunc() { std::cout << "A::vfunc" << std::endl; }
	virtual void pvfunc() = 0; // Pure virtual function, which makes A an abstract class of which no object can be created
};

struct B: A {
	void func()   { std::cout << "B::func" << std::endl; }
	void vfunc()  { std::cout << "B::vfunc" << std::endl; }
	void pvfunc() { std::cout << "B::pvfunc" << std::endl; }
};

int main() {
	B b;
	A &a = b;

	// Normal function:
	a.func();

	// Virtual function:
	a.vfunc();
	a.A::vfunc(); // The :: operator disables the virtual call [class.virtual]/16

	// Pure virtual function:
	a.pvfunc();

	return 0;
}
