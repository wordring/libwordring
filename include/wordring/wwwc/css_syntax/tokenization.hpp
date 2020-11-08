#pragma once

#include <wordring/whatwg/infra/infra.hpp>

#include <algorithm>
#include <any>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
//#include <variant>

namespace wordring::wwwc::css
{
	// --------------------------------------------------------------------------------------------
	// 4. Tokenization
	//
	// https://drafts.csswg.org/css-syntax-3/#tokenization
	// https://triple-underscore.github.io/css-syntax-ja.html#tokenization
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename OutputIterator, typename ErrorHandler = std::nullptr_t>
	inline void tokenize(BidirectionalIterator, BidirectionalIterator, OutputIterator output, ErrorHandler = nullptr);

	/*! @brief 整数と小数を識別するためのフラグ値

	@sa number_token
	@sa dimension_token
	*/
	enum class number_type_flag_name { integer = 1, number };

	struct ident_token
	{
		ident_token(std::u32string&& id) : m_value(std::move(id)) {}

		std::u32string m_value;
	};

	struct function_token
	{
		function_token(std::u32string&& s) : m_value(std::move(s)) {}

		std::u32string m_value;
	};
	
	struct at_keyword_token
	{
		at_keyword_token(std::u32string&& s) : m_value(std::move(s)) {}

		std::u32string m_value;
	};

	struct hash_token
	{
		enum class type_flag_name { id = 1, unrestricted };

		hash_token() : m_type_flag(type_flag_name::unrestricted) {}

		type_flag_name m_type_flag;
		std::u32string m_value;
	};

	struct string_token
	{
		string_token(std::u32string&& s) : m_value(std::move(s)) {}

		std::u32string m_value;
	};

	struct bad_string_token {};

	struct url_token
	{
		url_token(std::u32string&& url) : m_value(std::move(url)) {}

		std::u32string m_value;
	};

	struct bad_url_token {};

	struct delim_token
	{
		delim_token(char32_t cp) : m_value(cp) {}

		char32_t m_value;
	};
	
	struct number_token
	{
		number_token(double num, number_type_flag_name type = number_type_flag_name::integer)
			: m_value(num)
			, m_type_flag(type)
		{
		}

		double m_value;
		number_type_flag_name m_type_flag;
	};

	struct percentage_token
	{
		percentage_token(double num) : m_value(num) {}

		double m_value;
	};

	struct dimension_token : number_token
	{
		dimension_token(double num, number_type_flag_name type, std::u32string&& unit)
			: number_token(num, type)
			, m_unit(std::move(unit))
		{
		}

		std::u32string m_unit;
	};
	
	struct whitespace_token {};
	struct CDO_token {};
	struct CDC_token {};
	struct colon_token {};
	struct semicolon_token {};
	struct comma_token {};
	/* < [-css_token > */
	struct open_square_token {};
	/* < ]-css_token > */
	struct close_square_token {};
	/* < (-css_token > */
	struct open_paren_token {};
	/* < )-css_token > */
	struct close_paren_token {};
	/* < {-css_token > */
	struct open_curly_token {};
	/* < }-css_token > */
	struct close_curly_token {};
	struct eof_token {};

/*	using css_token_ = std::variant<
		std::monostate,
		ident_token,
		function_token,
		at_keyword_token,
		hash_token,
		string_token,
		bad_string_token,
		url_token,
		bad_url_token,
		delim_token,
		number_token,
		percentage_token,
		dimension_token,
		whitespace_token,
		CDO_token,
		CDC_token,
		colon_token,
		semicolon_token,
		comma_token,
		open_square_token,
		close_square_token,
		open_paren_token,
		close_paren_token,
		open_curly_token,
		close_curly_token,
		eof_token>;
*/

	using css_token = std::any;

	// --------------------------------------------------------------------------------------------
	// 4.2. Definitions
	//
	// https://drafts.csswg.org/css-syntax-3/#tokenizer-definitions
	// https://triple-underscore.github.io/css-syntax-ja.html#tokenizer-definitions
	// --------------------------------------------------------------------------------------------

	inline bool is_digit(char32_t cp) { return U'\x30' <= cp && cp <= U'\x39'; }

	inline bool is_hex_digit(char32_t cp)
	{
		return is_digit(cp) || (U'\x41' <= cp && cp <= U'\x46') || (U'\x61' <= cp && cp <= U'\x66');
	}

	inline bool is_uppercase_letter(char32_t cp) { return U'\x41' <= cp && cp <= U'\x5A'; }

	inline bool is_lowercase_letter(char32_t cp) { return U'\x61' <= cp && cp <= U'\x7A'; }

	inline bool is_letter(char32_t cp) { return is_uppercase_letter(cp) || is_lowercase_letter(cp); }

	inline bool is_non_ascii_code_point(char32_t cp) { return U'\x80' <= cp && cp <= U'\U0010FFFF'; }

	inline bool is_identifier_start_code_point(char32_t cp)
	{
		return is_letter(cp) || is_non_ascii_code_point(cp) || cp == U'\x5F';
	}

	inline bool is_identifier_code_point(char32_t cp)
	{
		return is_identifier_start_code_point(cp) || is_digit(cp) || cp == U'\x2D';
	}

	inline bool is_non_printable_code_point(char32_t cp)
	{
		return (U'\x00' <= cp && cp <= U'\x08') || cp == U'\x0B' || (U'\x0E' <= cp && cp <= U'\x1F') || cp == U'\x7F';
	}

	inline bool is_newline(char32_t cp) { return cp == U'\x0A'; }

	inline bool is_whitespace(char32_t cp) { return is_newline(cp) || cp == U'\x09' || cp == U'\x20'; }

	inline constexpr char32_t maximum_allowed_code_point = U'\U0010FFFF';

	// --------------------------------------------------------------------------------------------
	// 4.3. Tokenizer Algorithms
	//
	// https://drafts.csswg.org/css-syntax-3/#tokenizer-algorithms
	// https://triple-underscore.github.io/css-syntax-ja.html#tokenizer-algorithms
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename ErrorHandler = std::nullptr_t>
	inline std::pair<BidirectionalIterator, css_token> consume_token(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler = nullptr);

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_comments(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler);

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_numeric_token(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler);

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_ident_like_token(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler);

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_string_token(BidirectionalIterator first, BidirectionalIterator last, char32_t ends, ErrorHandler handler);

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_url_token(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler);

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, char32_t> consume_escaped_code_point(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler);

	template <typename BidirectionalIterator>
	inline bool starts_with_valid_escape(BidirectionalIterator first, BidirectionalIterator last);

	template <typename BidirectionalIterator>
	inline bool starts_with_identifier(BidirectionalIterator first, BidirectionalIterator last);

	template <typename BidirectionalIterator>
	inline bool starts_with_number(BidirectionalIterator first, BidirectionalIterator last);

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, std::u32string> consume_identifier(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler);

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::tuple<BidirectionalIterator, double, number_type_flag_name> consume_number(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler);

	template <typename InputIterator>
	inline double convert_string_to_number(InputIterator first, InputIterator last);

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_remnants_of_bad_url(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler);

	// --------------------------------------------------------------------------------------------
	// 4.3.1. Consume a token
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-token
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-token
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_token(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler)
	{
		auto [it, tkn] = consume_comments(first, last, handler);
		while (it != last)
		{
			char32_t cp = *it++;

			if (is_whitespace(cp))
			{
				while (it != last && is_whitespace(*it)) ++it;
				return { it, whitespace_token() };
			}

			switch (cp)
			{
			case U'\x22':
				return consume_string_token(it, last, cp, handler);
			case U'\x23':
				if (is_identifier_code_point(*it) || starts_with_valid_escape(it, last))
				{
					// 1.
					auto token = hash_token();
					// 2.
					if (starts_with_identifier(it, last))token.m_type_flag = hash_token::type_flag_name::id;
					// 3.
					auto [it1, str] = consume_identifier(it, last, handler);
					token.m_value = str;
					// 4.
					return { it1, token };
				}
				return { it, delim_token(cp) };
			case U'\x27':
				return consume_string_token(it, last, cp, handler);
			case U'\x28':
				return { it, open_paren_token() };
			case U'\x29':
				return { it, close_paren_token() };
			case U'\x2B':
				if (starts_with_number(it, last)) return consume_numeric_token(--it, last, handler);
				return { it, delim_token(cp) };
			case U'\x2C':
				return { it, comma_token() };
			case U'\x2D':
				if (starts_with_number(it, last)) return consume_numeric_token(--it, last, handler);
				if (2 <= std::distance(it, last) && *it == U'\x2D' && *std::next(it) == U'\x3E')
				{
					std::advance(it, 2);
					return { it, CDC_token() };
				}
				if (starts_with_identifier(it, last)) return consume_ident_like_token(--it, last, handler);
				return { it, delim_token(cp) };
			case U'\x2E':
				if (starts_with_number(it, last)) return consume_numeric_token(--it, last, handler);
				return { it, delim_token(cp) };
			case U'\x3A':
				return { it, colon_token() };
			case U'\x3B':
				return { it, semicolon_token() };
			case U'\x3C':
				if (3 <= std::distance(it, last) && *it == U'\x21' && *std::next(it) == U'\x2D' && *std::next(it, 2) == U'\x2D')
				{
					std::advance(it, 3);
					return { it, CDO_token() };
				}
				return { it, delim_token(cp) };
			case U'\x40':
				if (starts_with_identifier(it, last))
				{
					auto [it1, str] = consume_identifier(it, last, handler);
					return { it1, at_keyword_token(std::move(str)) };
				}
				return { it, delim_token(cp) };
			case U'\x5B':
				return { it, open_square_token() };
			case U'\x5C':
				if (starts_with_valid_escape(std::prev(it), last)) return consume_ident_like_token(--it, last, handler);
				if constexpr (std::is_invocable_v<ErrorHandler, BidirectionalIterator>) handler(it);
				return { it, delim_token(cp) };
			case U'\x5D':
				return { it, close_square_token() };
			case U'\x7B':
				return { it, open_curly_token() };
			case U'\x7D':
				return { it, close_curly_token() };
			default:
				break;
			}
			if (is_digit(cp)) return consume_numeric_token(--it, last, handler);
			if (is_identifier_start_code_point(cp)) return consume_ident_like_token(--it, last, handler);
			return  { it, delim_token(cp) };
		}

		return { it, eof_token() };
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.2. Consume comments
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-comment
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-comments
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_comments(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler)
	{
		while (2 <= std::distance(first, last) && *first == U'\x2F' && *std::next(first) == U'\x2A')
		{
			std::advance(first, 2);
			std::u32string_view sv = { U"\x2A\x2F" };
			first = std::search(first, last, sv.begin(), sv.end());
			if (first != last) std::advance(first, 2);
		}

		if constexpr (std::is_invocable_v<ErrorHandler, BidirectionalIterator>)
		{
			if (first == last) handler(first);
		}

		return { first, css_token() };
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.3. Consume a numeric token
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-numeric-token
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-numeric-token
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_numeric_token(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler)
	{
		auto [ it, num, type ] = consume_number(first, last, handler);

		if (starts_with_identifier(it, last))
		{
			auto [it1, unit] = consume_identifier(it, last, handler);
			return { it1, dimension_token(num, type, std::move(unit)) };
		}

		if (it != last && *it == U'\x25') return { ++it, percentage_token(num) };

		return { it, number_token(num, type) };
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.4. Consume an ident-like token
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-ident-like-token
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-ident-like-token
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_ident_like_token(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler)
	{
		using namespace wordring::whatwg;

		auto [it, s] = consume_identifier(first, last, handler);

		std::u32string_view sv = U"url";
		if (is_ascii_case_insensitive_match(s.begin(), s.end(), sv.begin(), sv.end()) && *it == U'\x28')
		{
			++it;
			while (2 <= std::distance(it, last) && is_whitespace(*it) && is_whitespace(*std::next(it))) ++it;
			if ((it != last && (*it == U'\x22' || *it == U'\x27'))
				|| (2 <= std::distance(it, last) && is_whitespace(*it) && (*std::next(it) == U'\x22' || *std::next(it) == U'\x27'))) return { --it, function_token(std::move(s)) };
			return consume_url_token(it, last, handler);
		}

		if (it != last && *it == U'\x28') return { ++it, function_token(std::move(s)) };

		return { it, ident_token(std::move(s)) };
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.5. Consume a string token
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-string-token
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-string-token
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_string_token(BidirectionalIterator first, BidirectionalIterator last, char32_t ends, ErrorHandler handler)
	{
		std::u32string s;

		while (first != last)
		{
			char32_t cp = *first++;

			if (cp == ends) return { first, string_token(std::move(s)) };
			if (is_newline(cp))
			{
				if constexpr (std::is_invocable_v<ErrorHandler, BidirectionalIterator>) handler(first);
				--first;
				return { first, bad_string_token() };
			}
			if (cp == U'\x5C')
			{
				if (first != last)
				{
					if (is_newline(*first)) ++first;
					else
					{
						auto [it, str] = consume_escaped_code_point(first, last, handler);
						s.insert(s.end(), str);
						first = it;
					}
				}
			}
			else s.push_back(cp);
		}

		if constexpr (std::is_invocable_v<ErrorHandler, BidirectionalIterator>) handler(first);

		return { first, string_token(std::move(s)) };
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.6. Consume a url token
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-url-token
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-url-token
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_url_token(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler)
	{
		std::u32string url;

		while (first != last && is_whitespace(*first)) ++first;
		while (first != last)
		{
			char32_t cp = *first++;

			if(cp == U'\x29') return { first, url_token(std::move(url)) };

			if (is_whitespace(cp))
			{
				while (first != last && is_whitespace(*first)) ++first;
				if (first == last)
				{
					if constexpr (std::is_invocable_v<ErrorHandler, BidirectionalIterator>) handler(first);
					return { first, url_token(std::move(url)) };
				}
				if (*first == U'\x29') return { ++first, url_token(std::move(url)) };

				auto [it, e] = consume_remnants_of_bad_url(first, last, handler);
				return { it, bad_url_token() };
			}

			if (cp == U'\x22' || cp == U'\x27' || cp == U'\x28' || is_non_printable_code_point(cp))
			{
				if constexpr (std::is_invocable_v<ErrorHandler, BidirectionalIterator>) handler(first);
				auto [it, e] = consume_remnants_of_bad_url(first, last, handler);
				return { it, bad_url_token() };
			}

			if (cp == U'\x5C')
			{
				if (starts_with_valid_escape(first, last))
				{
					auto [it, ch] = consume_escaped_code_point(first, last, handler);
					url.push_back(ch);
				}
				else
				{
					if constexpr (std::is_invocable_v<ErrorHandler, BidirectionalIterator>) handler(first);
					auto [it, e] = consume_remnants_of_bad_url(first, last, handler);
					return { it, bad_url_token() };
				}
			}
			else url.push_back(cp);
		}

		if constexpr (std::is_invocable_v<ErrorHandler, BidirectionalIterator>) handler(first);
		return { first, url_token(std::move(url)) };
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.7. Consume an escaped code point
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-escaped-code-point
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-escaped-code-point
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, char32_t> consume_escaped_code_point(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler)
	{
		if (first != last)
		{
			char32_t cp = *first++;

			if (is_hex_digit(cp))
			{
				using namespace wordring::whatwg;

				std::string s(1, static_cast<std::uint8_t>(cp));
				for (std::uint32_t i = 0; i < 5 && first != last && is_hex_digit(*first); ++i) s.push_back(static_cast<std::uint8_t>(*first++));
				if (first != last && is_whitespace(*first)) ++first;
				std::uint32_t ch;
				[[maybe_unused]] std::from_chars_result r = std::from_chars(s.data(), s.data() + s.size(), ch, 16);
				assert(r.ec == std::errc());
				if (ch == U'\x00' || is_surrogate(ch) || maximum_allowed_code_point < ch) return { first, U'\U0000FFFD' };
				return { first, ch };
			}

			return { first, cp };
		}

		if constexpr (std::is_invocable_v<ErrorHandler, BidirectionalIterator>) handler(first);

		return { first, U'\U0000FFFD' };
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.8. Check if two code points are a valid escape
	//
	// https://drafts.csswg.org/css-syntax-3/#starts-with-a-valid-escape
	// https://triple-underscore.github.io/css-syntax-ja.html#starts-with-a-valid-escape
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator>
	inline bool starts_with_valid_escape(BidirectionalIterator first, BidirectionalIterator last)
	{
		return (2 <= std::distance(first, last) && *first == U'\x5C' && !is_newline(*std::next(first)));
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.9. Check if three code points would start an identifier
	//
	// https://drafts.csswg.org/css-syntax-3/#would-start-an-identifier
	// https://triple-underscore.github.io/css-syntax-ja.html#would-start-an-identifier
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator>
	inline bool starts_with_identifier(BidirectionalIterator first, BidirectionalIterator last)
	{
		if (first != last)
		{
			if (*first == U'\x2D')
			{
				++first;
				if (first != last && (is_identifier_start_code_point(*first) || *first == U'\x2D')) return true;
				if (starts_with_valid_escape(first, last)) return true;
				return false;
			}
			if (is_identifier_start_code_point(*first)) return true;
			if (*first == U'\x5C') return starts_with_valid_escape(first, last);
		}

		return false;
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.10. Check if three code points would start a number
	//
	// https://drafts.csswg.org/css-syntax-3/#starts-with-a-number
	// https://triple-underscore.github.io/css-syntax-ja.html#starts-with-a-number
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator>
	inline bool starts_with_number(BidirectionalIterator first, BidirectionalIterator last)
	{
		if (first != last)
		{
			char32_t cp1 = *first++;

			if (cp1 == U'\x2B' || cp1 == U'\x2D')
			{
				if (first != last)
				{
					char32_t cp2 = *first++;
					if (is_digit(cp2)) return true;
					if (cp2 == U'\x2E' && first != last && is_digit(*first)) return true;
				}
				return false;
			}

			if (cp1 == U'\x2E')
			{
				if (first != last && is_digit(*first)) return true;
				return false;
			}

			if (is_digit(cp1)) return true;
		}

		return false;
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.11. Consume an identifier
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-name
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-name
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, std::u32string> consume_identifier(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler)
	{
		std::u32string s;

		while (first != last)
		{
			if (is_identifier_code_point(*first)) s.push_back(*first++);
			else if (starts_with_valid_escape(first, last))
			{
				auto [it, ch] = consume_escaped_code_point(++first, last, handler);
				s.push_back(ch);
				first = it;
			}
			else break;
		}

		return { first, std::move(s) };
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.12. Consume a number
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-number
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-number
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::tuple<BidirectionalIterator, double, number_type_flag_name> consume_number(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler)
	{
		// 1.
		number_type_flag_name type = number_type_flag_name::integer;
		std::u32string repr;
		// 2.
		if (first != last && (*first == U'+' || *first == U'-')) repr.push_back(*first++);
		// 3.
		while (first != last && is_digit(*first)) repr.push_back(*first++);
		// 4.
		if (2 <= std::distance(first, last) && *first == U'\x2E' && is_digit(*std::next(first)))
		{
			repr.push_back(*first++);
			repr.push_back(*first++);
			type = number_type_flag_name::number;
			while (first != last && is_digit(*first)) repr.push_back(*first++);
		}
		// 5.
		std::uint32_t n = 0;
		if (2 <= std::distance(first, last) && (*first == U'E' || *first == U'e') && is_digit(*std::next(first))) n = 2;
		else if (3 <= std::distance(first, last) && (*first == U'E' || *first == U'e') && (*std::next(first) == U'+' || *std::next(first) == U'-') && is_digit(*std::next(first, 2))) n = 3;
		if (n != 0)
		{
			for (; n != 0; --n) repr.push_back(*first++);
			type = number_type_flag_name::number;
			while (first != last && is_digit(*first)) repr.push_back(*first++);
		}
		// 6.
		// 7.
		return { first, convert_string_to_number(repr.begin(), repr.end()), type };
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.13. Convert a string to a number
	//
	// https://drafts.csswg.org/css-syntax-3/#convert-string-to-number
	// https://triple-underscore.github.io/css-syntax-ja.html#convert-string-to-number
	// --------------------------------------------------------------------------------------------

	template <typename InputIterator>
	inline double convert_string_to_number(InputIterator first, InputIterator last)
	{
		static_assert(sizeof(*first) == sizeof(char32_t));

		// 1. sign
		double s = 1.0;
		if (first != last && *first == U'+') ++first;
		else if (first != last && *first == U'-')
		{
			s = -1.0;
			++first;
		}
		// 2. integer part
		std::uint32_t i = 0;
		std::string integer_part;
		while (first != last && is_digit(*first)) integer_part.push_back(static_cast<std::uint8_t>(*first++));
		if (!integer_part.empty())
		{
			[[maybe_unused]] std::from_chars_result r1 =
				std::from_chars(integer_part.data(), integer_part.data() + integer_part.size(), i, 10);
			assert(r1.ec == std::errc());
		}
		// 3. decimal point
		if (first != last && *first == U'\x2E') ++first;
		// 4. fractional part
		double d = 0;
		std::uint32_t f = 0;
		std::string fractional_part;
		while (first != last && is_digit(*first)) fractional_part.push_back(static_cast<std::uint8_t>(*first++));
		d = fractional_part.size();
		if (!fractional_part.empty())
		{
			[[maybe_unused]] std::from_chars_result r2 =
				std::from_chars(fractional_part.data(), fractional_part.data() + fractional_part.size(), f, 10);
			assert(r2.ec == std::errc());
		}
		// 5. exponent_indicator
		if (first != last && (*first == U'E' || *first == U'e')) ++first;
		// 6. exponent_sign
		double t = 1.0;
		if (first != last && *first == U'+') ++first;
		else if (first != last && *first == U'-')
		{
			t = -1.0;
			++first;
		}
		// 7.exponent
		std::uint32_t e = 0;
		std::string exponent;
		while (first != last && is_digit(*first)) exponent.push_back(static_cast<std::uint8_t>(*first++));
		if (!exponent.empty())
		{
			[[maybe_unused]] std::from_chars_result r3 =
				std::from_chars(exponent.data(), exponent.data() + exponent.size(), e, 10);
			assert(r3.ec == std::errc());
		}

		return s * (i + f * std::pow(10.0, -d)) * std::pow(10.0, t * e);
	}

	// --------------------------------------------------------------------------------------------
	// 4.3.14. Consume the remnants of a bad url
	//
	// https://drafts.csswg.org/css-syntax-3/#consume-remnants-of-bad-url
	// https://triple-underscore.github.io/css-syntax-ja.html#consume-remnants-of-bad-url
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename ErrorHandler>
	inline std::pair<BidirectionalIterator, css_token> consume_remnants_of_bad_url(BidirectionalIterator first, BidirectionalIterator last, ErrorHandler handler)
	{
		while (first != last)
		{
			char32_t cp = *first++;
			if (cp == U')') return { first, css_token() };
			if (starts_with_valid_escape(std::prev(first), last))
			{
				auto [it, cp] = consume_escaped_code_point(first, last, handler);
				first = it;
			}
		}

		return { first, css_token() };
	}

	// --------------------------------------------------------------------------------------------
	// 実装
	// --------------------------------------------------------------------------------------------

	template <typename BidirectionalIterator, typename OutputIterator, typename ErrorHandler>
	inline void tokenize(BidirectionalIterator first, BidirectionalIterator last, OutputIterator output, ErrorHandler handler)
	{
		while (true)
		{
			auto [it, tkn] = consume_token(first, last, handler);
			if (tkn.type() == typeid(eof_token)) break;
			first = it;
			*output++ = tkn;
		}
	}

}
