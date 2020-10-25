// This program implements my own streambuf that merges cin and cout.

#include <iostream>
#include <locale>
#include <unistd.h> // Operating system API

#define WBUFSZ 3 // Write buffer size
#define RBUFSZ 2 // Read buffer size
#define EBUFSZ 6 // Encoding buffer size
#define DBUFSZ 10 // Decoding buffer size

template <typename CharT, typename Traits>
struct basic_mybuf: std::basic_streambuf<CharT, Traits> {
	using char_type = CharT;
	using traits_type = Traits;
	using int_type = typename traits_type::int_type;
private:
	char_type writebuf[WBUFSZ];
	char_type readbuf[RBUFSZ];

	std::mbstate_t wstate = std::mbstate_t();
	int do_write(const char *buffer, size_t len) {
		size_t len_written = 0;
		while (len_written < len) {
			ssize_t res = ::write(STDOUT_FILENO, buffer + len_written, len - len_written);
			if (res <= 0)
				return -1;
			len_written += (size_t)res;
		}
		return 0;
	}
	ssize_t do_encode(size_t len) { // Return the remaining length in the write buffer, or negative error code
		if constexpr (std::is_same_v<char_type, char>) {
			if (do_write(writebuf, len) < 0)
				return -1;
			return 0;
		}
		else {
			char encode[EBUFSZ];

			auto &facet = std::use_facet<std::codecvt<char_type, char, std::mbstate_t>>(this->getloc());
			const char_type *from_next;
			char *to_next;
			auto result = facet.out(wstate, writebuf, writebuf + len, from_next, encode, encode + sizeof encode, to_next);
			if (result != std::codecvt_base::ok && result != std::codecvt_base::partial)
				return -1;
			if (to_next - encode == 0) // No target string produced
				return -2; // No more in the write buffer can be written

			if (do_write(encode, size_t(to_next - encode)) < 0)
				return -1;

			ssize_t remaining = writebuf + len - from_next;
			for (ssize_t i = 0; i < remaining; ++i)
				writebuf[i] = from_next[i];
			return remaining;
		}
	}
	ssize_t write(size_t length) { // Return the remaining length in the write buffer, or negative error code
		if (length == 0) // Nothing to write
			return 0;
		size_t remaining = length;
		while (true) {
			ssize_t res = do_encode(remaining);
			if (res == -2) // No more can be written
				return (ssize_t)remaining;
			else if (res < 0) // Error
				return -1;
			else if (res == 0) // No character left in the write buffer
				return 0;
			else
				remaining = (size_t)res;
		}
	}
	int unshift() { // In state-dependent encodings, write the unshift sequence to go back to the initial state.
		if constexpr (std::is_same_v<char_type, char>)
			return 0;
		else {
			char encode[EBUFSZ];

			auto &facet = std::use_facet<std::codecvt<char_type, char, std::mbstate_t>>(this->getloc());
			char *to_next;
			auto result = facet.unshift(wstate, encode, encode + sizeof encode, to_next);
			if (result == std::codecvt_base::noconv)
				return 0;
			if (result != std::codecvt_base::ok)
				return -1;

			if (do_write(encode, size_t(to_next - encode)) < 0)
				return -1;

			return 0;
		}
	}

	char decode[DBUFSZ];
	size_t decode_len = 0;
	std::mbstate_t rstate = std::mbstate_t();
	int do_read() {
		ssize_t len = ::read(STDIN_FILENO, decode + decode_len, sizeof decode - decode_len);
		if (len <= 0)
			return -1;
		decode_len += (size_t)len;
		return 0;
	}
	ssize_t do_decode() { // Return the length made available in the read buffer, or negative error code
		if constexpr (std::is_same_v<char_type, char>) {
			if (decode_len == 0)
				return -2;

			size_t readlen = std::min(decode_len, sizeof readbuf);
			for (size_t i = 0; i < readlen; ++i)
				readbuf[i] = decode[i];

			decode_len = decode_len - readlen;
			for (size_t i = 0; i < decode_len; ++i)
				decode[i] = decode[i + readlen];

			return (ssize_t)readlen;
		}
		else {
			auto &facet = std::use_facet<std::codecvt<char_type, char, std::mbstate_t>>(this->getloc());
			const char *from_next;
			char_type *to_next;
			auto result = facet.in(rstate, decode, decode + decode_len, from_next, readbuf, readbuf + RBUFSZ, to_next);
			if (result != std::codecvt_base::ok && result != std::codecvt_base::partial)
				return -1; // Error
			if (from_next - decode == 0) // No source string decoded
				return -2; // Source string needs to be replenished

			decode_len = size_t(decode + decode_len - from_next);
			for (size_t i = 0; i < decode_len; ++i)
				decode[i] = from_next[i];

			return to_next - readbuf;
		}
	}
	ssize_t read() { // Return the length made available in the read buffer, or negative error code
		while (true) {
			ssize_t res = do_decode();
			if (res == -2) { // Source string needs to be replenished
				if (do_read() < 0) // Replenish the source string
					return -1;
			}
			else if (res < 0) // Error
				return res;
			else if (res == 0) // No new character produced
				; // Try again
			else
				return res;
		}
	}
public:
	basic_mybuf() {
		this->setp(writebuf, writebuf + WBUFSZ - 1); // "- 1" is to accommodate the "overflow" function below
		this->setg(readbuf, readbuf, readbuf);
	};
	~basic_mybuf() {
		sync();
		unshift();
	}
	int_type overflow(int_type ch) {
		size_t len = size_t(this->pptr() - this->pbase());
		if (!traits_type::eq_int_type(ch, traits_type::eof())) {
			traits_type::assign(*this->pptr(), traits_type::to_char_type(ch));
			len += 1;
		}
		ssize_t remaining = write(len);
		if (remaining < 0)
			return traits_type::eof();
		if (remaining >= this->epptr() - this->pbase()) // Still no space after write
			return traits_type::eof();
		this->setp(this->pbase(), this->epptr());
		this->pbump((int)remaining);
		return traits_type::to_int_type(char_type());
	}
	int sync() {
		size_t len = size_t(this->pptr() - this->pbase());
		ssize_t remaining = write(len);
		if (remaining < 0)
			return -1;
		this->setp(this->pbase(), this->epptr());
		this->pbump((int)remaining);
		return 0;
	}
	int_type underflow() {
		if (this->gptr() < this->egptr()) // If there are still characters not read in the buffer
			return traits_type::to_int_type(*this->gptr());
		else { // If all read, read new characters
			ssize_t len = read();
			if (len <= 0)
				return traits_type::eof();
			this->setg(readbuf, readbuf, readbuf + len);
			return traits_type::to_int_type(*readbuf);
		}
	}
};

template <typename CharT, typename Traits>
struct basic_mystream: std::basic_iostream<CharT, Traits> {
	basic_mystream(): std::basic_iostream<CharT, Traits>(&sb), sb() {}
private:
	basic_mybuf<CharT, Traits> sb;
};

using wmystream = basic_mystream<wchar_t, std::char_traits<wchar_t>>;

int main()
{
	std::locale loc("");
	std::locale::global(loc);

	wmystream stream;
	wchar_t c;
	while (true) {
		if (!stream.good()) {
			std::cerr << "Error detected" << std::endl;
			return 1;
		}
		stream.get(c);
		if (stream.eof())
			break;
		stream.put(c);
		if (c == L'\n')
			stream.flush();
	}
	return 0;
}
