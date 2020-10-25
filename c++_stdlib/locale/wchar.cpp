// This program converts a char string to a wchar_t string and prints it,
// along with the properties of the converted characters.

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <locale>

#define ISTYPE(type, mask) if (mask & std::ctype_base::type) std::wcout << " " #type;
#define TOTRANS(trans, nwc, wc) if (nwc != wc) std::wcout << " to" #trans "=" << nwc;

void analyze(const std::wstring &wstr, const std::locale &loc)
{
	auto &facet = std::use_facet<std::ctype<wchar_t>>(loc);

	std::vector<std::ctype_base::mask> masks(wstr.size());
	facet.is(&*wstr.begin(), &*wstr.end(), &*masks.begin());

	std::wstring upper = wstr, lower = wstr;
	facet.toupper(&*upper.begin(), &*upper.end());
	facet.tolower(&*lower.begin(), &*lower.end());

	for (std::size_t i=0; i<wstr.size(); ++i) {
		std::wcout << ' ' << wstr[i] << " \t" << std::setw(5) << std::wint_t(wstr[i]);
		ISTYPE(cntrl, masks[i])
		ISTYPE(print, masks[i])
		ISTYPE(space, masks[i])
		ISTYPE(blank, masks[i])
		ISTYPE(graph, masks[i])
		ISTYPE(punct, masks[i])
		ISTYPE(alnum, masks[i])
		ISTYPE(alpha, masks[i])
		ISTYPE(lower, masks[i])
		ISTYPE(upper, masks[i])
		ISTYPE(digit, masks[i])
		ISTYPE(xdigit, masks[i])
		TOTRANS(lower, lower[i], wstr[i])
		TOTRANS(upper, upper[i], wstr[i])
		std::wcout << std::endl;
	}
}

std::wstring convert(const std::string &from, const std::locale &loc)
{
	std::wstring to(from.length(), L'\0');
	auto &facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	std::mbstate_t state = std::mbstate_t();
	const char *from_next;
	wchar_t *to_next;
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

void do_main(const char *mbstr)
{
	std::locale loc("");
	std::locale::global(loc);
	std::wcout << "Current locale: " << loc.name().data() << std::endl;

	std::wstring wstr = convert(mbstr, loc);
	std::wcout << "String: " << wstr << std::endl;
	std::wcout << std::hex;
	analyze(wstr, loc);
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		std::cerr << "Usage: " << argv[0] << " string" << std::endl;
		return -1;
	}

	try {
		do_main(argv[1]);
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
