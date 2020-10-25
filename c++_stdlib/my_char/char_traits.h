// This is a definition body of a CharTraits class for MyChar.

{
	using char_type = MyChar;
	static constexpr void assign(char_type& r, const char_type& a) noexcept {
		r = a;
	}
	static constexpr bool eq(char_type a, char_type b) noexcept {
		return a.base == b.base;
	}
	static constexpr bool lt(char_type a, char_type b) noexcept {
		return a.base < b.base;
	}

	// The following 6 functions' behaviors are defined by the standard in terms of the above functions.
	// That means the standard doesn't expect us to arbitrarily change them.
	static constexpr int compare(const char_type* s1, const char_type* s2, std::size_t count) {
		for (std::size_t i=0; i<count; ++i) {
			if (lt(s1[i], s2[i]))
				return -1;
			if (lt(s2[i], s1[i]))
				return 1;
		}
		return 0;
	}
	static constexpr std::size_t length(const char_type* s) {
		for (std::size_t i=0; ; ++i)
			if (eq(s[i], char_type()))
				return i;
	}
	static constexpr const char_type* find(const char_type* p, std::size_t count, const char_type& ch) {
		for (std::size_t i=0; i<count; ++i)
			if (eq(p[i], ch))
				return p+i;
		return nullptr;
	}
	static constexpr char_type* move(char_type* dest, const char_type* src, std::size_t count) {
		if (dest < src)
			for (std::size_t i=0; i<count; ++i)
				assign(dest[i], src[i]);
		else
			for (std::size_t i=count; i>0; --i)
				assign(dest[i-1], src[i-1]);
		return dest;
	}
	static constexpr char_type* copy(char_type* dest, const char_type* src, std::size_t count) {
		// The standard requires src should not be in [dest, dest+count) when called.
		for (std::size_t i=count; i>0; --i)
			assign(dest[i-1], src[i-1]);
		return dest;
	}
	static constexpr char_type* assign(char_type* p, std::size_t count, char_type a) {
		for (std::size_t i=0; i<count; ++i)
			assign(p[i], a);
		return p;
	}

	// The followings are used only in I/O
	using int_type = MyInt; // For representing all characters + EOF.
	using off_type = int; // For representing offsets from stream positions.
	using pos_type = int; // For representing absolute positions in a stream.
	static constexpr char_type to_char_type(int_type c) noexcept {
		return c.ch;
	}
	static constexpr int_type to_int_type(char_type c) noexcept {
		return int_type(true, c);
	}
	static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept {
		if (!c1.valid && !c2.valid)
			return true;
		else if (c1.valid && c2.valid)
			return eq(to_char_type(c1), to_char_type(c2));
		else
			return false;
	}
	static constexpr int_type eof() noexcept {
		return int_type(false, char_type());
	}
	static constexpr int_type not_eof(int_type e) noexcept {
		if (e.valid)
			return e;
		else
			return int_type(true, char_type());
	}
}
