// This program implements special cultural numeric formats.

#include <iostream>
#include <string>
#include <locale>

struct my_numpunct: std::numpunct<wchar_t> {
	std::string do_grouping() const { return "\4"; }
	// In East Asian languages, numbers are read in groups of 4 digits,
	// but are usually written in groups of 3 digits, following the Western convention.
	// This causes difficulty in reading large numbers.
	// This function facilitates 4 digit grouping.

	string_type do_truename() const { return L"真"; }
	string_type do_falsename() const { return L"假"; }
};

void print()
{
	std::wcout << 100000000 << '\t' << true << ' ' << false << std::endl;
}

void do_main()
{
	std::locale loc("zh_CN.UTF-8");
	std::locale::global(loc);

	std::wcout << std::boolalpha;
	print();

	std::wcout.imbue(loc);
	print();

	// Create a new locale by replacing the "std::numpunct<wchar_t>" facet in "loc".
	std::locale myloc(loc, new my_numpunct);
	// The new'ed "my_numpunct" facet replaces the original "std::numpunct<wchar_t>" facet in "loc".
	// They can replace each other because they have the same "id"
	// (a static member, inherited from "std::numpunct<wchar_t>", required to have for all facets).
	// The new'ed "my_numpunct" object has a reference count (inherited from "std::locale::facet")
	// of 0 and would be increased/decreased by "std::locale"s that start/stop referencing it.
	// When the count reaches zero again, it would be automatically deleted by a "std::locale".

	std::wcout.imbue(myloc);
	print();
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
