// This program converts strings between UTF-8 and UTF-16/UTF-32 encodings.

#include <iostream>
#include <string>
#include <locale>

template <typename CharT>
std::basic_string<CharT> convert(const std::u8string &from)
{
	std::basic_string<CharT> to(from.length(), CharT());
	auto &facet = std::use_facet<std::codecvt<CharT, char8_t, std::mbstate_t>>(std::locale());
	std::mbstate_t state = std::mbstate_t();
	const char8_t *from_next;
	CharT *to_next;
	auto result = facet.in(state, &*from.begin(), &*from.end(), from_next, &*to.begin(), &*to.end(), to_next);
	switch (result) {
	case std::codecvt_base::ok:
		break;
	case std::codecvt_base::partial:
		std::cerr << "Partially converted" << std::endl;
		break;
	case std::codecvt_base::error:
		std::cerr << "Conversion error" << std::endl;
		break;
	case std::codecvt_base::noconv:
		std::cerr << "Not converted" << std::endl;
		break;
	default:
		std::cerr << "Internal error" << std::endl;
	}
	to.resize(std::size_t(to_next - &*to.begin()));
	return to;
}

void do_main()
{
	std::cout << std::hex;

	std::u8string u8str = u8"lü绿🍀";
	std::cout << "UTF-8:";
	for (char8_t c: u8str)
		std::cout << ' ' << c;
	std::cout << std::endl;

	std::u16string u16str = convert<char16_t>(u8str);
	std::cout << "UTF-16:";
	for (char16_t c: u16str)
		std::cout << ' ' << c;
	std::cout << std::endl;

	std::u32string u32str = convert<char32_t>(u8str);
	std::cout << "UTF-32:";
	for (char32_t c: u32str)
		std::cout << ' ' << c;
	std::cout << std::endl;
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
