// This program demonstrates copy elision in C++.

#include <iostream>

class A {
public:
	A()           { std::cout << "Constructed"      << std::endl; }
	A(const A &a) { std::cout << "Copy-constructed" << std::endl; }
	A(A &&a)      { std::cout << "Move-constructed" << std::endl; }
	~A()          { std::cout << "Destructed"       << std::endl; }
};

A f1() {
	A a = A(); // Initialize "a" with a prvalue "A()"
	return a; // Return a named object
}

A f2() {
	return A(); // Initialize some object (elsewhere) with a prvalue "A()"
}

void g(A a) {
}

int main() {
	A a = f1(); // Initialize "a" with a prvalue "f1()"
	A b = f2(); // Initialize "a" with a prvalue "f2()"
	g(f2()); // Initialize Parameter "a" of Function "g" with a prvalue "f2()"
	return 0;
}

/* Returning a named object:

The returned object is initialized from the named object.
The copy/move operation is usually optimized out when the compiler is able to (Named Return Value Optimization, NRVO).
Sometimes the compiler is not able to do this optimization, for example, when the function may return different named objects.
When there's no optimization, if there's a move constructor, moving is performed, otherwise copying is performed.

*/

/* Initializing an object with a prvalue:

Until C++17, the object is initialized from a temporary object.
The copy/move operation is usually optimized out.
When the prvalue is the operand of a "return" statement, this optimization is called RVO (Return Value Optimization).
When there's no optimization, if there's a move constructor, moving is performed, otherwise copying is performed.

Since C++17, the object is initialized directly from the prvalue and no temporary object ever exists.

*/
