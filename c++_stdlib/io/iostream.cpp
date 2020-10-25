// This program implements my own iostream that holds two streambufs.

#include <iostream>

template <typename CharT, typename Traits>
struct basic_istream: std::basic_ios<CharT, Traits> {
	using char_type = CharT;
	using traits_type = Traits;
	using int_type = typename traits_type::int_type;
	struct sentry {
		explicit sentry(basic_istream<CharT, Traits>& is): ok_(false) {
			if (!is.good()) is.setstate(is.failbit);
			else ok_ = true;
		}
		~sentry() {}
		explicit operator bool() const { return ok_; }
	private:
		bool ok_;
	};
	basic_istream(std::basic_streambuf<CharT, Traits>* sb): std::basic_ios<CharT, Traits>(sb) {}
	basic_istream& get(char_type& ch) {
		sentry sen(*this); if (!bool(sen)) return *this;
		int_type res = this->rdbuf()->sbumpc();
		if (traits_type::eq_int_type(res, traits_type::eof()))
			this->setstate(this->failbit | this->eofbit);
		else
			traits_type::assign(ch, traits_type::to_char_type(res));
		return *this;
	}
};

template <typename CharT, typename Traits>
struct basic_ostream: std::basic_ios<CharT, Traits> {
	using char_type = CharT;
	using traits_type = Traits;
	using int_type = typename traits_type::int_type;
	struct sentry {
		explicit sentry(basic_ostream<CharT, Traits>& os): ok_(false) {
			if (!os.good()) os.setstate(os.failbit);
			else ok_ = true;
		}
		~sentry() {}
		explicit operator bool() const { return ok_; }
	private:
		bool ok_;
	};
	basic_ostream(std::basic_streambuf<CharT, Traits>* sb): std::basic_ios<CharT, Traits>(sb) {}
	basic_ostream& put(char_type ch) {
		sentry sen(*this); if (!bool(sen)) return *this;
		int_type res = this->rdbuf()->sputc(ch);
		if (traits_type::eq_int_type(res, traits_type::eof()))
			this->setstate(this->badbit);
		return *this;
	}
	basic_ostream& flush() {
		sentry sen(*this); if (!bool(sen)) return *this;
		int res = this->rdbuf()->pubsync();
		if (res == -1)
			this->setstate(this->badbit);
		return *this;
	}
};

template <typename CharT, typename Traits>
struct basic_iostream: basic_istream<CharT, Traits>, basic_ostream<CharT, Traits> {
	basic_iostream(std::basic_streambuf<CharT, Traits>* isb, std::basic_streambuf<CharT, Traits>* osb)
		: basic_istream<CharT, Traits>(isb), basic_ostream<CharT, Traits>(osb) {}
	bool eof() const { return basic_istream<CharT, Traits>::eof(); }
};

using iostream = basic_iostream<char, std::char_traits<char>>;

int main()
{
	std::ios_base::sync_with_stdio(false); // Enable buffering for standard streambufs
	iostream stream(std::cin.rdbuf(), std::cout.rdbuf());
	char c;
	while (true) {
		stream.get(c);
		if (stream.eof())
			break;
		stream.put(c);
		if (c == '\n')
			stream.flush();
	}
	return 0;
}
