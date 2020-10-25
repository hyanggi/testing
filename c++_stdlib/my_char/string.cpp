// This program uses std::basic_string with my own character type.

#include <iostream>
#include <string>
#include "my_char.h"

void str()
{
	std::basic_string<MyChar, MyCharTraits> str1{to_mc(0, 0), to_mc(1, 0), to_mc(2, 0), to_mc(3, 0)};
	std::basic_string<MyChar, MyCharTraits> str2{to_mc(0, 1), to_mc(1, 1), to_mc(2, 1), to_mc(3, 1)};
	std::basic_string<MyChar, MyCharTraits> str3{to_mc(0, 1), to_mc(1, 1), to_mc(2, 1), to_mc(0, 1)};
	std::cout << std::boolalpha;
	std::cout << (str1 == str2) << std::endl;
	std::cout << (str1 > str3) << std::endl;

	MyChar cstr[4] = {to_mc(1, 1), to_mc(2, 1), to_mc(3, 1), to_mc(0, 1)};
	std::basic_string<MyChar, MyCharTraits> str4 = cstr;
	std::cout << str4.length() << std::endl;
}

int main()
{
	str();
	return 0;
}
