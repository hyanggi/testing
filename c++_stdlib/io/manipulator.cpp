// This program implements my own IO manipulator.

#include <iostream>

struct MyStr {
	const char* str;
	MyStr(const char* str) : str(str) {}
};

const int myfmt_id = std::ios_base::xalloc();
// This is the ID of my format option.

std::ostream& operator<<(std::ostream& os, MyStr& mystr)
{
	switch (os.iword(myfmt_id)) {
	case 1: // Red
		return os << "\x1B[31m" << mystr.str << "\x1B[39m";
	case 2: // Green
		return os << "\x1B[32m" << mystr.str << "\x1B[39m";
	default:
		return os << mystr.str;
	}
}

// Implement a I/O manipulator
std::ios_base& reset_myfmt(std::ios_base& os) {
	os.iword(myfmt_id) = 0;
	return os;
}

// Implement a I/O manipulator that takes arguments
struct MyFmtVal { long v; };
std::ostream& operator<<(std::ostream& os, MyFmtVal val) {
	os.iword(myfmt_id) = val.v;
	return os;
}
MyFmtVal set_myfmt(long v) { return {v}; }


int main()
{
	MyStr mystr("My string");
	std::cout << mystr << std::endl;
	std::cout << set_myfmt(1) << mystr << std::endl;
	std::cout << set_myfmt(2) << mystr << std::endl;
	std::cout << reset_myfmt << mystr << std::endl;
	return 0;
}
