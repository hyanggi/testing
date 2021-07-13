// This program demonstrates various ways of unpacking parameter packs.

#include <typeinfo>
#include <iostream>

// ------------ Using recursion -----------------

// Variable template
template <typename ...Args>
constexpr bool check1;
template <> // Explicit template specialization
constexpr bool check1<> = true;
template <typename T, typename ...Args> // Partial template specialization
constexpr bool check1<T, Args...> = std::is_arithmetic_v<T> && check1<Args...>;

// Class template
template <typename ...Args>
struct Check1 {
	Check1(Args...) {}
};
template <> // Explicit template specialization
struct Check1<>: std::true_type {
	Check1() {}
};
template <typename T, typename ...Args> // Partial template specialization
struct Check1<T, Args...>: std::bool_constant<std::is_arithmetic_v<T> && Check1<Args...>::value> {
	Check1(T, Args...) {}
};

// Function template
template <typename T>
void print_one(std::ostream &out, const T &current) {
	out << typeid(T).name() << " | " << sizeof(T) << " | " << current << std::endl;
}
void print1(std::ostream &out) {
}
template <typename T, typename ...Args>
void print1(std::ostream &out, const T &current, const Args &...args) {
	print_one(out, current);
	print1(out, args...);
}

// --------- Using fold expressions -----------

// Variable template
template <typename ...Args>
constexpr bool check2 = (std::is_arithmetic_v<Args> && ...);

// Class template
template <typename ...Args>
struct Check2: std::bool_constant<(std::is_arithmetic_v<Args> && ...)> {
	Check2(Args...) {}
};

// Function template
template <typename ...Args>
void print2(std::ostream &out, const Args &...args) {
	(print_one(out, args) , ...);
}

// ----------------- "main" function --------------------

int main()
{
	std::cout << std::boolalpha;

	std::cout << check1<int, bool, double, double> << std::endl;
	std::cout << check1<int, bool, double, double*> << std::endl;
	std::cout << check2<int, bool, double, double> << std::endl;
	std::cout << check2<int, bool, double, double*> << std::endl;
	std::cout << std::endl;

	std::cout << Check1<int, bool, double, double>::value << std::endl;
	std::cout << Check1<int, bool, double, double*>::value << std::endl;
	std::cout << Check1(3, true, 3.0, 4.0).value << std::endl;
	std::cout << Check1(3, true, 3.0, nullptr).value << std::endl;
	std::cout << Check2<int, bool, double, double>::value << std::endl;
	std::cout << Check2<int, bool, double, double*>::value << std::endl;
	std::cout << Check2(3, true, 3.0, 4.0).value << std::endl;
	std::cout << Check2(3, true, 3.0, nullptr).value << std::endl;
	std::cout << std::endl;

	print1(std::cout, 9, "c_str", std::string("std_string"));
	print2(std::cout, 9, "c_str", std::string("std_string"));

	return 0;
}
