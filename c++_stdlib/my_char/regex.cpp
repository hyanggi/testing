// This program uses regex with my own character type.

#include <iostream>
#include <regex>
#include "my_char.h"

// The standard doesn't make much use of char_traits in the regex library.
// The only mention of char_traits in the regex chapter is its length function.
// So any use of other functions of char_traits is indirectly through the string type or the length function.

// It implicitly assumes that the characters are directly comparable to each other.
// It doesn't use char_traits for its eq and lt functions for comparison, except when they are indirectly called through the string type or the length function.

// It also implicitly assumes that the characters are comparable with the integer 0.
// It assumes that a character compares equal with 0 using "==", iff the character compares equal with MyChar() using char_traits<MyChar>::eq.

// Also, the character type has to have a way to compare with or convert between other character types,
// otherwise it's not possible to parse regex syntax.
// The standard also say std::basic_regex should rely on its traits class to do localization.
// However the requirements of the traits class doesn't give us a way to do conversion between character types.
// This means an implementation may have to just use type casts to do this.


#ifndef __clang__ // g++ also requires this
template <>
struct std::make_unsigned<MyChar> { using type = MyChar; };
#else // clang also requires these
struct MyLoc {
    std::string name() { return ""; }
};
template <>
void std::__match_any_but_newline<MyChar>::__exec(std::__state<MyChar>&) const {}
#endif


struct MyRegexTraits {
	using char_type = MyChar;
	using string_type = std::basic_string<char_type>; // The standard says it has to be this.
#ifndef __clang__ // g++ requires locale_type = std::locale
	using locale_type = std::locale;
#else // clang requires locale_type has a member function of "std::string name()"
	using locale_type = MyLoc;
#endif
	using char_class_type = unsigned int;

	// Convert a character to its equivalent class
	// This function is called when matching characters and the "collate" option is set
	char_type translate(char_type c) const { return c; }
	// Convert a character to its equivalent class, ignoring case
	// This function is called when matching characters and the "icase" option is set
	char_type translate_nocase(char_type c) const { return c; }

	// Convert a collation element name to the collation element
	// This function is called when matching a collating element (eg. [[.tilde.]], and [[.ch.]] in Czech)
	template<class ForwardIt>
	string_type lookup_collatename(ForwardIt first, ForwardIt last) const {
		return string_type();
	}
	// Convert a character string to its sort key
	// This function is called when matching a range (eg. [a-z]) and the "collate" option is set
	// The input of this function is a string because a range can be [[.ch.]-z] in Czech
	template<class ForwardIt>
	string_type transform(ForwardIt first, ForwardIt last) const {
		return string_type(first, last);
	}
	// Convert a character string to its sort key, ignoring case, diacritics, variants, etc.
	// This function is called when matching an equivalence class (eg. [[=a=]])
	// The input of this function is a string because an equivalence class can be [[=ch=]] in Czech
	template<class ForwardIt>
	string_type transform_primary(ForwardIt first, ForwardIt last) const {
		return string_type(first, last);
	}

	// Convert a character class name (eg. "alnum") to the character class
	// This function is called when matching a character class (eg. [[:alnum:]])
	template<class ForwardIt>
	char_class_type lookup_classname(ForwardIt first, ForwardIt last, bool icase = false) const {
		return char_class_type();
	}
	// Determine if a character belongs to a character class
	// This function is called when matching a character class (eg. [[:alnum:]])
	bool isctype(char_type c, char_class_type f) const { return false; }

	// Convert a digit character to an integer
	// This function is called when processing repetitions (eg. {3}, {2,5}), back-references (eg. \1, \2) and character escapes (eg. \x61, \u5b57)
	int value(char_type ch, int radix) const {
		if (radix < 0) return -1;
		if (ch.base >= radix) return -1;
		return ch.base;
	}

	// Set/get the locale
	locale_type imbue(locale_type loc) { return locale_type(); }
	locale_type getloc() const { return locale_type(); }
};

#ifndef __clang__ // g++ uses "std::locale" as "locale_type", and the "std::locale" needs to have this facet.
template <>
struct std::ctype<MyChar>: std::locale::facet, std::ctype_base {
	using char_type = MyChar;
	explicit ctype(std::size_t refs = 0) {}
	~ctype() {}
	bool is(mask m, char_type c) const {
		return false;
	}
	char_type widen(char c) const {
		return to_mc((unsigned char)c);
	}
	const char* widen(const char* beg, const char* end, char_type* dst) const {
		for (int i=0; i<end-beg; ++i) dst[i] = widen(beg[i]);
		return end;
	}
	char narrow(char_type c, char dflt) const {
		return (char)c.base;
	}
	char_type tolower(char_type c) const {
		return c;
	}
	char_type toupper(char_type c) const {
		return c;
	}
	static std::locale::id id;
};
std::locale::id std::ctype<MyChar>::id;
#endif


void regex()
{
	std::basic_regex<MyChar, MyRegexTraits> re{to_mc(1), to_mc(2)};

	std::basic_string<MyChar, MyCharTraits1> s{to_mc(0), to_mc(1), to_mc(2), to_mc(3)};
	std::basic_string<MyChar, MyCharTraits2> fmt{to_mc(2), to_mc(1)};
	auto res = std::regex_replace(s, re, fmt);
	static_assert(std::is_same_v<decltype(res), decltype(s)>);

	for (MyChar c: res)
		std::cout << c.base << " ";
	std::cout << std::endl;
}

int main()
{
#ifndef __clang__ // g++ uses "std::locale" as "locale_type", and the "std::locale" needs to have a "std::ctype<MyChar>" facet.
	std::locale myloc(std::locale(), new std::ctype<MyChar>);
	std::locale::global(myloc);
#endif
	regex();
	return 0;
}
