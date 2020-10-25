// This program demonstrates compile-time recursion in different ways.

#include <iostream>

// Recursion by class templates
template <unsigned N>
struct Factorial {
	static constexpr unsigned value = N * Factorial<N-1>::value;
};
template <>
struct Factorial<0> {
	static constexpr unsigned value = 1;
};

// Recursion by variable templates
template <unsigned N>
constexpr unsigned factorial = N * factorial<N-1>;
template <>
constexpr unsigned factorial<0> = 1;

// Recursion by function templates
template <unsigned N>
constexpr unsigned get_Factorial() {
	return N * get_Factorial<N-1>();
}
template <>
constexpr unsigned get_Factorial<0>() {
	return 1;
}

// Recursion by functions instead of templates
constexpr unsigned get_factorial(unsigned n) {
	if (n == 0)
		return 1;
	return n * get_factorial(n-1);
}

// Iteration instead of recursion
constexpr unsigned get_factorial_iter(unsigned n) {
	unsigned result = 1;
	for (unsigned i=1; i<=n; ++i)
		result *= i;
	return result;
}

int main()
{
	std::cout << "5! = " << Factorial<5>::value << std::endl;
	std::cout << "5! = " << factorial<5> << std::endl;
	std::cout << "5! = " << get_Factorial<5>() << std::endl;
	std::cout << "5! = " << get_factorial(5) << std::endl;
	std::cout << "5! = " << get_factorial_iter(5) << std::endl;

	return 0;
}
