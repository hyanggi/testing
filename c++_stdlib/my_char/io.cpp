// This program does I/O with my own character type.

#include <iostream>
#include <sstream>
#include <locale>
#include "my_char.h"

void io()
{
	std::basic_string<MyChar, MyCharTraits> init = {to_mc(0), to_mc(1), to_mc(2), to_mc(3)};
	std::basic_stringstream<MyChar, MyCharTraits> stream(init);

	stream.seekp(0, std::ios_base::end);
	auto pos = stream.tellp(); // Remember this place
	stream.put(to_mc(4));
	stream.put(to_mc(5));
	stream.put(to_mc(6));
	stream.put(to_mc(7));
	while (true) {
		MyChar c;
		stream.get(c);
		if (stream.bad()) return;
		if (stream.eof()) break;
		if (stream.fail()) break;
		std::cout << c.base << " ";
	}
	std::cout << std::endl;

	stream.clear();
	stream.seekg(pos); // Go back to the remembered place
	while (true) {
		MyChar c;
		stream.get(c);
		if (stream.bad()) return;
		if (stream.eof()) break;
		if (stream.fail()) break;
		std::cout << c.base << " ";
	}
	std::cout << std::endl;
}

// This facet is needed for formatted I/O (>> and <<).
template <>
struct std::ctype<MyChar>: std::locale::facet, std::ctype_base {
	using char_type = MyChar;
	explicit ctype(std::size_t refs = 0) {}
	~ctype() {}
	bool is(mask m, char_type c) const {
		// Let's make Character No. 2 a whitespace character
		if (c.base == 2) return space & m;
		// Whitespace characters are discarded in formatted input
		return false;
	}
	char_type widen(char c) const {
		return to_mc((unsigned char)c);
	}
	const char* widen(const char* beg, const char* end, char_type* dst) const {
		for (int i=0; i<end-beg; ++i) dst[i] = widen(beg[i]);
		return end;
	}
	static std::locale::id id;
};
std::locale::id std::ctype<MyChar>::id;


void io_fmt_ch()
{
	std::basic_stringstream<MyChar, MyCharTraits> stream;

	std::locale myloc(stream.getloc(), new std::ctype<MyChar>);
	stream.imbue(myloc);

	stream << to_mc(0);
	stream << to_mc(1);
	stream << to_mc(2);
	stream << to_mc(3);
	while (true) {
		MyChar c;
		stream >> c;
		if (stream.bad()) return;
		if (stream.eof()) break;
		if (stream.fail()) break;
		std::cout << c.base << " ";
	}
	std::cout << std::endl;
}

// The following two facets are needed for formatted I/O with numbers.

template<>
struct std::num_put<MyChar, std::ostreambuf_iterator<MyChar, MyCharTraits>>: std::locale::facet {
	using char_type = MyChar;
	using iter_type = std::ostreambuf_iterator<MyChar, MyCharTraits>;
	iter_type put(iter_type out, std::ios_base& str, char_type fill, long v) const {
		// Let's generate two characters per number
		*out++ = to_mc((std::uint16_t)v);
		*out++ = to_mc((std::uint16_t)v);
		return out;
	}
	static std::locale::id id;
};
std::locale::id std::num_put<MyChar, std::ostreambuf_iterator<MyChar, MyCharTraits>>::id;

template<>
struct std::num_get<MyChar, std::istreambuf_iterator<MyChar, MyCharTraits>>: std::locale::facet {
	using char_type = MyChar;
	using iter_type = std::istreambuf_iterator<MyChar, MyCharTraits>;
	iter_type get(iter_type in, iter_type end, std::ios_base& str, std::ios_base::iostate& err, long& v) const {
		if (in != end) {
			char_type c = *in++;
			v = (long)c.base;
		}
		else
			err |= std::ios_base::failbit | std::ios_base::eofbit;
		return in;
	}
	static std::locale::id id;
};
std::locale::id std::num_get<MyChar, std::istreambuf_iterator<MyChar, MyCharTraits>>::id;

void io_fmt_num()
{
	std::basic_stringstream<MyChar, MyCharTraits> stream;

	std::locale myloc1(stream.getloc(), new std::ctype<MyChar>);
	std::locale myloc2(myloc1, new std::num_put<MyChar, std::ostreambuf_iterator<MyChar, MyCharTraits>>);
	std::locale myloc3(myloc2, new std::num_get<MyChar, std::istreambuf_iterator<MyChar, MyCharTraits>>);
	stream.imbue(myloc3);

	stream << 0;
	stream << 1;
	stream << 2;
	stream << 3;
	while (true) {
		int c;
		stream >> c;
		if (stream.bad()) return;
		if (stream.eof()) break;
		if (stream.fail()) break;
		std::cout << c << " ";
	}
	std::cout << std::endl;
}

int main()
{
	io();
	io_fmt_ch();
	io_fmt_num();
	return 0;
}
