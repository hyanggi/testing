// This program demonstrates various compile-time conditional constructs.

#include <string>
#include <cstring>
#include <iostream>

// ------------ Variable template SFINAE -----------------

// Variable template
template <typename T, typename Enable = void>
constexpr bool check = false;

// Partial template specialization of the variable template
template <typename T>
constexpr bool check<T, std::enable_if_t<std::is_arithmetic_v<T>>> = true;

// ------------ Class template SFINAE -----------------

// Class template
template <typename T, typename Enable = void>
struct Check: std::false_type {
	Check(T) {}
};

// Partial template specialization of the class template
template <typename T>
struct Check<T, std::enable_if_t<std::is_arithmetic_v<T>>>: std::true_type {
	Check(T) {}
};

// ------------ Function template SFINAE -----------------

template <typename T>
std::enable_if_t<std::is_arithmetic_v<T>>
print1(std::ostream &out, const T &current) {
	out << "Arithmetic type (" << sizeof current << "): " << current << std::endl;
}
template <typename T, typename ...Args>
std::enable_if_t<std::is_convertible_v<T, const char*>>
print1(std::ostream &out, const T &current) {
	out << "C string (" << std::strlen(current) << "): " << current << std::endl;
}
template <typename T, typename ...Args>
std::enable_if_t<std::is_same_v<T, std::string>>
print1(std::ostream &out, const T &current) {
	out << "std::string (" << current.length() << "): " << current << std::endl;
}

// -------------- "if constexpr" statement ----------------

template <typename T>
void print2(std::ostream &out, const T &current) {
	if constexpr (std::is_arithmetic_v<T>)
		out << "Arithmetic type (" << sizeof current << "): " << current << std::endl;
	else if constexpr (std::is_convertible_v<T, const char*>)
		out << "C string (" << std::strlen(current) << "): " << current << std::endl;
	else if constexpr (std::is_same_v<T, std::string>)
		out << "std::string (" << current.length() << "): " << current << std::endl;
	else
		out << "Unrecognized type" << std::endl;
}

// ----------------- "main" function --------------------

int main()
{
	std::cout << std::boolalpha;

	std::cout << check<int> << std::endl;
	std::cout << check<int*> << std::endl;
	std::cout << std::endl;

	std::cout << Check<int>::value << std::endl;
	std::cout << Check<int*>::value << std::endl;
	std::cout << Check(9).value << std::endl;
	std::cout << Check(nullptr).value << std::endl;
	std::cout << std::endl;

	print1(std::cout, 9);
	print1(std::cout, "c_str");
	print1(std::cout, std::string("std_string"));
	print2(std::cout, 9);
	print2(std::cout, "c_str");
	print2(std::cout, std::string("std_string"));

	return 0;
}
