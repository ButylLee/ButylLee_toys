#pragma once
#ifndef ENCRYPTEDSTRING_HEADER_
#define ENCRYPTEDSTRING_HEADER_

/*
 * EncryptedString - for easily encrypting string literal in
 * compile-time to prevent from modifying in binary executable
 * How to use:
 * Just add suffix [_crypt] to string literal: "a string"_crypt
 */

#pragma warning(disable:26451) // incorrect overflow warning
#include <climits>
#include <cassert>
#include <string>

#define RANDOM_SEED ((__TIME__[0] - '0') * 1ULL + (__TIME__[1] - '0') * 10ULL + \
					 (__TIME__[3] - '0') * 60ULL + (__TIME__[4] - '0') * 600ULL + \
					 (__TIME__[6] - '0') * 3600ULL + (__TIME__[7] - '0') * 36000ULL)

namespace detail {
	constexpr unsigned long long LinearCongruentialGenerator(size_t times)
	{
		// same as std::minstd_rand
		return 48271 * (times > 0 ? LinearCongruentialGenerator(times - 1) : RANDOM_SEED) % 2147483647;
	}
	constexpr unsigned long long GenerateRandom()
	{
		return LinearCongruentialGenerator((__TIME__[0] - '0') * (__TIME__[1] - '0'));
	}
	constexpr unsigned long long GetRandom(unsigned long long min, unsigned long long max)
	{
		return min + GenerateRandom() % (max - min + 1);
	}
	template<typename TChar>
	constexpr TChar EncryptChar(TChar ch, TChar key)
	{
		return ~(ch ^ key);
	}
	template<typename TChar>
	inline TChar DecryptChar(TChar ch, TChar key) noexcept
	{
		return ~ch ^ key;
	}
}

template<typename TChar>
class EncryptedString
{
public:
	// encrypt in compile-time
	constexpr EncryptedString(const TChar* origin_str, size_t len) noexcept
		:str_len(len)
	{
		for (size_t i = 0; i < len; i++)
		{
			str[i] = detail::EncryptChar(origin_str[i], key);
		}
	}
	// decrypt in run-time
	[[nodiscard]]
	operator std::basic_string<TChar> () const&& noexcept
	{
		assert(str_len <= sizeof str);
		std::basic_string<TChar> decrypted;
		decrypted.reserve(str_len);
		for (size_t i = 0; i < str_len; i++)
		{
			decrypted += detail::DecryptChar(str[i], key);
		}
		return decrypted;
	}

private:
	static constexpr TChar key =
		static_cast<TChar>( // casting to signed type is implementation-defined behavior until C++20
			detail::GetRandom(0x00, (0x01ULL << sizeof(TChar) * CHAR_BIT) - 1)
		);
	size_t str_len;
	TChar str[1024] = {};
};

#ifndef NO_ENCRYPTED_STRING
inline std::string operator""_crypt(const char* str, size_t len) noexcept
{
	return EncryptedString<char>(str, len);
}
inline std::wstring operator""_crypt(const wchar_t* str, size_t len) noexcept
{
	return EncryptedString<wchar_t>(str, len);
}
inline std::u8string operator""_crypt(const char8_t* str, size_t len) noexcept
{
	return EncryptedString<char8_t>(str, len);
}
inline std::u16string operator""_crypt(const char16_t* str, size_t len) noexcept
{
	return EncryptedString<char16_t>(str, len);
}
inline std::u32string operator""_crypt(const char32_t* str, size_t len) noexcept
{
	return EncryptedString<char32_t>(str, len);
}
#else
inline std::string operator""_crypt(const char* str, size_t len) noexcept
{
	return str;
}
inline std::wstring operator""_crypt(const wchar_t* str, size_t len) noexcept
{
	return str;
}
inline std::u8string operator""_crypt(const char8_t* str, size_t len) noexcept
{
	return str;
}
inline std::u16string operator""_crypt(const char16_t* str, size_t len) noexcept
{
	return str;
}
inline std::u32string operator""_crypt(const char32_t* str, size_t len) noexcept
{
	return str;
}
#endif // NO_ENCRYPTED_STRING

#undef RANDOM_SEED
#endif // ENCRYPTEDSTRING_HEADER_