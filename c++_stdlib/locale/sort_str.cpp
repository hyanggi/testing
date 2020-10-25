// This program demonstrates string collation.

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <locale>

void do_main()
{
	std::locale loc("zh_CN.UTF-8");
	std::locale::global(loc);

	std::vector<std::string> strings = {"按", "正确", "的", "方式", "排序"};
	std::cout << "Original:";
	for (const std::string &str: strings)
		std::cout << ' ' << str;
	std::cout << std::endl;

	std::sort(strings.begin(), strings.end());
	std::cout << "Sort by default:";
	for (const std::string &str: strings)
		std::cout << ' ' << str;
	std::cout << std::endl;

	std::sort(strings.begin(), strings.end(), loc);
	// loc will be called with its function call operator "()", i.e., called as loc(str1, str2).
	// loc(str1, str2) will call std::use_facet<std::collate<char>>(loc).compare(str1, str2).
	std::cout << "Sort correctly:";
	for (const std::string &str: strings)
		std::cout << ' ' << str;
	std::cout << std::endl;

	// Note that providing a custom "Traits" class to "std::basic_string<CharT, Traits, Allocator>"
	// to define a locale-sensitive comparison function, will not work.
	// Because the longer string will be truncated to the length of the shorter string before comparing.
	// Thus, the comparison between "ch" and "d" in Czech will have an incorrect result.
}

int main()
{
	try {
		do_main();
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
