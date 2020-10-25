// This header defines a char-like type (a non-array trivial standard-layout type).
// This header also defines the CharTraits classes for that type.

#define WARN "This function is not guaranteed by the standard to exist."

struct MyChar {
	std::uint16_t base;
	std::uint16_t variant;
	MyChar() = default;
	constexpr MyChar(std::uint16_t base, std::uint16_t variant): base(base), variant(variant) {}
	[[deprecated(WARN)]]
	constexpr MyChar(std::uint16_t base): base(base), variant(0) {}
	[[deprecated(WARN)]]
	constexpr operator std::uint16_t() const { return base; }
};

constexpr MyChar to_mc(std::uint16_t base, std::uint16_t variant = 0) {
	return MyChar(base, variant);
}

struct MyInt {
	MyChar ch;
	bool valid;
	MyInt() = default;
	constexpr MyInt(bool valid, MyChar ch): ch(ch), valid(valid) {}
	[[deprecated(WARN)]]
	constexpr MyInt(MyChar ch): ch(ch), valid(true) {}
	[[deprecated(WARN)]]
	constexpr operator MyChar() { return ch; };
};

struct MyCharTraits
#include "char_traits.h"
;

struct MyCharTraits1
#include "char_traits.h"
;

struct MyCharTraits2
#include "char_traits.h"
;

template <>
struct std::char_traits<MyChar>
#include "char_traits.h"
;

// This macro is a workaround for not being able to use the C++20 "<=>" operator.
#define DEFS(OP) \
template <typename T1, typename T2> \
[[deprecated(WARN)]] \
bool operator OP(T1 c1, T2 c2) { \
	unsigned long long v1, v2; \
	if constexpr (std::is_same_v<T1, MyChar>) \
		v1 = c1.base; \
	else if constexpr (std::is_same_v<T1, MyInt>) \
		v1 = c1.ch.base; \
	else \
		v1 = std::make_unsigned_t<T1>(c1); \
	if constexpr (std::is_same_v<T2, MyChar>) \
		v2 = c2.base; \
	else if constexpr (std::is_same_v<T2, MyInt>) \
		v2 = c2.ch.base; \
	else \
		v2 = std::make_unsigned_t<T2>(c2); \
	return v1 OP v2; \
}

DEFS(==)
DEFS(!=)
DEFS(<)
DEFS(<=)
DEFS(>)
DEFS(>=)

#undef DEFS
