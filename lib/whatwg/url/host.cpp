

#include <wordring/whatwg/url/host.hpp>

#include <wordring/whatwg/infra/infra.hpp>

#include <boost/algorithm/string/classification.hpp> 
#include <boost/algorithm/string/split.hpp>

#include <array>
#include <charconv>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include <unicode/idna.h>

std::u32string wordring::whatwg::domain_to_ascii(std::u32string const& s, bool beStrict, std::error_code& ec)
{
	std::int32_t opt = UIDNA_USE_STD3_RULES | UIDNA_CHECK_BIDI | UIDNA_CHECK_CONTEXTJ | UIDNA_CHECK_CONTEXTO;
	UErrorCode u_ec = U_ZERO_ERROR;
	std::unique_ptr<icu::IDNA> idna(icu::IDNA::createUTS46Instance(opt, u_ec));

	icu::IDNAInfo info;
	icu::UnicodeString s1, s2;
	for (std::uint32_t cp : s) s1.append(static_cast<UChar32>(cp));

	idna->nameToASCII(s1, s2, info, u_ec);

	if (U_FAILURE(u_ec) || info.getErrors()) ec = url_error_name::IdnaTranslationFailed;

	std::u32string tmp;
	encoding_cast(s2.getBuffer(), s2.getBuffer() + s2.length(), std::back_inserter(tmp));

	return tmp;
}

std::u32string wordring::whatwg::domain_to_unicode(std::u32string const& s, std::error_code& ec)
{
	std::int32_t opt = UIDNA_CHECK_BIDI | UIDNA_CHECK_CONTEXTJ | UIDNA_CHECK_CONTEXTO;
	UErrorCode u_ec = U_ZERO_ERROR;
	std::unique_ptr<icu::IDNA> idna(icu::IDNA::createUTS46Instance(opt, u_ec));

	icu::IDNAInfo info;
	icu::UnicodeString s1, s2;
	for (std::uint32_t cp : s) s1.append(static_cast<UChar32>(cp));

	idna->nameToUnicode(s1, s2, info, u_ec);

	if (U_FAILURE(u_ec) || info.getErrors()) ec = url_error_name::IdnaTranslationFailed;

	std::u32string tmp;
	encoding_cast(s2.getBuffer(), s2.getBuffer() + s2.length(), std::back_inserter(tmp));

	return tmp;
}

bool wordring::whatwg::is_valid_domain_string(std::u32string const& s)
{
	// 1.
	std::error_code ec;
	std::u32string result = domain_to_ascii(s, true, ec);
	// 2.
	if (ec) return false;
	// 3.
	std::int32_t opt = UIDNA_USE_STD3_RULES | UIDNA_CHECK_BIDI | UIDNA_CHECK_CONTEXTJ | UIDNA_CHECK_CONTEXTO;
	UErrorCode u_ec = U_ZERO_ERROR;
	std::unique_ptr<icu::IDNA> idna(icu::IDNA::createUTS46Instance(opt, u_ec));

	icu::IDNAInfo info;
	icu::UnicodeString s1, s2;
	for (std::uint32_t cp : s) s1.append(static_cast<UChar32>(cp));

	idna->nameToUnicode(s1, s2, info, u_ec);

	// 4.
	if (U_FAILURE(u_ec) || info.getErrors()) return false;
	// 5.
	return true;
}

bool wordring::whatwg::is_valid_ipv4_address_string(std::u32string const& s)
{
	std::vector<std::u32string> v;
	boost::algorithm::split(v, s, boost::is_any_of(U"."));

	if (v.size() != 4) return false;

	std::string tmp;
	for (std::u32string const& c : v)
	{
		if (3 < c.size()) return false;
		tmp.clear();
		for (char32_t ch : c) tmp.push_back(static_cast<std::uint8_t>(ch));
		std::uint32_t i;
		std::from_chars(tmp.data(), tmp.data() + tmp.length(), i);
		if (255 < i) return false;
	}
	return true;
}

wordring::whatwg::host_base wordring::whatwg::parse_host(
	std::u32string const& in, bool isNotSpecial, std::error_code& ec)
{
	// 1.
	if (!in.empty() && in.front() == U'[')
	{
		if (in.back() != U']')
		{
			ec = url_error_name::ParseFailed;
			return host_base();
		}
		return parse_ipv6(std::u32string(++in.begin(), --in.end()), ec);
	}
	// 2.
	if (isNotSpecial) return parse_opaque_host(in, ec);
	// 3.
	assert(!in.empty());
	// 4.
	std::string tmp;
	string_percent_decode(in.begin(), in.end(), std::back_inserter(tmp));
	encoding::io_queue<char> in_q = encoding::to_io_queue_convert(tmp.begin(), tmp.end());
	encoding::io_queue<char32_t> out_q;
	encoding::utf8_decode_without_bom(in_q, out_q);
	std::u32string domain;
	encoding::from_io_queue_convert(out_q, std::back_inserter(domain));
	// 5.
	std::u32string asciiDomain = domain_to_ascii(domain, ec);
	// 6.
	if (ec) return host_base();
	// 7.
	if (std::find_if(asciiDomain.begin(), asciiDomain.end(), is_forbidden_host_code_point) != asciiDomain.end())
	{
		ec = url_error_name::ParseFailed;
		return host_base();
	}
	// 8.
	host_base ipv4 = parse_ipv4(asciiDomain, ec);
	// 9.
	if (!ipv4 || ipv4.type() == host_type_name::Ipv4Address) return ipv4;
	// 10.
	return domain_base{ asciiDomain };
}

// https://triple-underscore.github.io/URL-ja.html#concept-ipv4-parser

wordring::whatwg::host_base wordring::whatwg::parse_ipv4(std::u32string const& in, std::error_code& ec)
{
	// 1.
	bool validationError = false;
	// 2.
	std::vector<std::u32string> parts;
	strictly_split_on_particular_delimiter(in.begin(), in.end(), std::back_inserter(parts), U'.');
	// 3.
	if (!parts.empty() && parts.back().empty())
	{
		validationError = true;
		if (1 < parts.size()) parts.erase(--parts.end());
	}
	// 4.
	if (4 < parts.size()) return opaque_host_base{ in };
	// 5.
	std::vector<std::uint32_t> numbers;
	// 6.
	for (std::u32string const& part : parts)
	{
		// 6.1.
		if (part.empty()) return opaque_host_base{ in };
		// 6.2.
		std::pair<std::optional<std::uint32_t>, bool> result = parse_ipv4_number(part);
		// 6.3.
		if (!result.first) return opaque_host_base{ in };
		// 6.4.
		if (result.second) validationError = true;
		// 6.5.
		numbers.push_back(*result.first);
	}
	// 7.
	if (validationError) ec = url_error_name::ValidationError;
	// 8.
	for (std::uint32_t n : numbers) if (255 < n) ec = url_error_name::ValidationError;
	// 9.
	for (std::size_t i = 0; i < numbers.size() - 1; ++i) if (255 < numbers.at(i)) return host_base();
	// 10.
	assert(!numbers.empty());
	if (std::pow(256, (5 - numbers.size())) <= numbers.back())
	{
		ec = url_error_name::ValidationError;
		return host_base();
	}
	// 11.
	std::uint32_t ipv4 = numbers.back();
	// 12.
	numbers.pop_back();
	// 13.
	std::uint32_t counter = 0;
	// 14.
	for (std::uint32_t n : numbers)
	{
		ipv4 += static_cast<std::uint32_t>(n * std::pow(256, 3 - counter));
		++counter;
	}
	// 15.
	return host_base(ipv4_address_base{ ipv4 });
}

std::pair<std::optional<std::uint32_t>, bool>
wordring::whatwg::parse_ipv4_number(std::u32string s)
{
	// 1.
	bool validationError = false;
	// 2.
	std::uint32_t R = 10;
	// 3.
	if (2 <= s.size() && s.front() == U'0' && (s.at(1) == U'x' || s.at(1) == U'X'))
	{
		validationError = true;
		s.erase(0, 2);
		R = 16;
	}
	// 4.
	else if (2 <= s.size() && s.front() == U'0')
	{
		validationError = true;
		s.erase(0, 1);
		R = 8;
	}
	// 5.
	if (s.empty()) return std::make_pair(std::optional<std::uint32_t>(), validationError);
	// 6.
	std::u32string_view sv = R == 8 ? U"012345678" : (R == 10 ? U"0123456789" : U"0123456789abcdefABCDEF");
	for (char32_t cp : s) if (std::find(sv.begin(), sv.end(), cp) == sv.end())
	{
		return std::make_pair(std::optional<std::uint32_t>(), validationError);
	}
	// 7.
	std::string tmp;
	for (char32_t ch : s) tmp.push_back(static_cast<std::uint8_t>(ch));
	std::uint32_t i;
	std::from_chars(tmp.data(), tmp.data() + tmp.length(), i, R);
	// 8.
	return std::make_pair(std::optional<std::uint32_t>(i), validationError);
}

wordring::whatwg::host_base wordring::whatwg::parse_ipv6(std::u32string const& in, std::error_code& ec)
{
	// 1.
	ipv6_address_base address{};
	// 2.
	std::uint32_t pieceIndex = 0;
	// 3.
	std::optional<std::uint32_t> compress;
	// 4.
	std::int32_t pointer = 0;
	// 5.
	if (!in.empty() && in[pointer] == U':')
	{
		if (in.size() < 2 || in[pointer + 1] != U':')
		{
			ec = url_error_name::ValidationError;
			return host_base();
		}
		pointer += 2;
		++pieceIndex;
		compress = pieceIndex;
	}
	// 6.
	while (static_cast<std::uint32_t>(pointer) < in.size())
	{
		// 6.1.
		if (pieceIndex == 8) return host_base();
		// 6.2.
		if (static_cast<std::uint32_t>(pointer) < in.size() && in[pointer] == U':')
		{
			if (compress)
			{
				ec = url_error_name::ValidationError;
				return host_base();
			}
			++pointer;
			++pieceIndex;
			compress = pieceIndex;
			continue;
		}
		// 6.3.
		std::uint32_t value = 0;
		std::uint32_t length = 0;
		// 6.4.
		while (length < 4 && static_cast<std::uint32_t>(pointer) < in.size() && is_ascii_hex_digit(in[pointer]))
		{
			char32_t cp = in[pointer];
			std::uint32_t i = (U'0' <= cp && cp <= U'9') ? cp - U'0' : (U'a' <= cp && cp <= U'f' ? cp - U'a' + 10 : cp - U'A' + 10);
			value = value * 0x10 + i;
			++pointer;
			++length;
		}
		// 6.5.
		if (static_cast<std::uint32_t>(pointer) < in.size() && in[pointer] == U'.')
		{
			// 6.5.1.
			if (length == 0)
			{
				ec = url_error_name::ValidationError;
				return host_base();
			}
			// 6.5.2.
			pointer -= length;
			// 6.5.3.
			if (6 < pieceIndex)
			{
				ec = url_error_name::ValidationError;
				return host_base();
			}
			// 6.5.4.
			std::uint32_t numbersSeen = 0;
			// 6.5.5.
			while (0 <= static_cast<std::uint32_t>(pointer) && static_cast<std::uint32_t>(pointer) < in.size())
			{
				// 6.5.5.1.
				std::optional<std::uint32_t> ipv4Piece;
				// 6.5.5.2.
				if (0 < numbersSeen)
				{
					if (in[pointer] == U'.' && numbersSeen < 4) ++pointer;
					else
					{
						ec = url_error_name::ValidationError;
						return host_base();
					}
				}
				// 6.5.5.3.
				if (in.size() <= static_cast<std::uint32_t>(pointer) || !is_ascii_digit(in[pointer]))
				{
					ec = url_error_name::ValidationError;
					return host_base();
				}
				// 6.5.5.4.
				while (static_cast<std::uint32_t>(pointer) < in.size() && is_ascii_digit(in[pointer]))
				{
					// 1.
					std::uint32_t number = in[pointer] - U'0';
					// 2.
					if (!ipv4Piece) *ipv4Piece = number;
					else if (*ipv4Piece == 0)
					{
						ec = url_error_name::ValidationError;
						return host_base();
					}
					else *ipv4Piece = *ipv4Piece * 10 + number;
					// 3.
					if(255 < *ipv4Piece)
					{
						ec = url_error_name::ValidationError;
						return host_base();
					}
					// 4.
					++pointer;
				}
				// 6.5.5.5.
				address[pieceIndex] = address[pieceIndex] * 0x100 + *ipv4Piece;
				// 6.5.5.6.
				++numbersSeen;
				// 6.5.5.7.
				if (numbersSeen == 2 || numbersSeen == 4) ++pieceIndex;
			}
			// 6.5.6.
			if(numbersSeen != 4)
			{
				ec = url_error_name::ValidationError;
				return host_base();
			}
			// 6.5.7.
			break;
		}
		// 6.6.
		else if(static_cast<std::uint32_t>(pointer) < in.size() && in[pointer] == U':')
		{
			// 6.6.1.
			++pointer;
			// 6.6.2.
			if(in.size() <= static_cast<std::uint32_t>(pointer))
			{
				ec = url_error_name::ValidationError;
				return host_base();
			}
		}
		// 6.7.
		else if(static_cast<std::uint32_t>(pointer) < in.size())
		{
			ec = url_error_name::ValidationError;
			return host_base();
		}
		// 6.8.
		address[pieceIndex] = value;
		// 6.9.
		++pieceIndex;
	}
	// 7.
	if (compress)
	{
		std::uint32_t swaps = pieceIndex - *compress;
		pieceIndex = 7;
		while (pieceIndex != 0 && 0 < swaps)
		{
			std::swap(address[pieceIndex], address[*compress + swaps - 1]);
			--pieceIndex;
			--swaps;
		}
	}
	// 8.
	else if(!compress && pieceIndex != 8)
	{
		ec = url_error_name::ValidationError;
		return host_base();
	}
	// 9.
	return address;
}

namespace
{
	// parse_opaque_host() 内で使う。
	// 4.3. URL writing 内に同じ定義があることに注意

	inline bool is_url_code_points_(char32_t cp)
	{
		using namespace wordring::whatwg;
		if (is_ascii_alphanumeric(cp)) return true;
		switch (cp)
		{
		case U'\x21': // (!)
		case U'\x24': // ($)
		case U'\x26': // (&)
		case U'\x27': // (')
		case U'\x28': // LEFT PARENTHESIS
		case U'\x29': // RIGHT PARENTHESIS
		case U'\x2A': // (*)
		case U'\x2B': // (+)
		case U'\x2C': // (,)
		case U'\x2D': // (-)
		case U'\x2E': // (.)
		case U'\x2F': // (/)
		case U'\x3A': // (:)
		case U'\x3B': // (;)
		case U'\x3D': // (=)
		case U'\x3F': // (?)
		case U'\x40': // (@)
		case U'\x5F': // (_)
		case U'\x7E': // (~)
			return true;
		}
		if (U'\xA0' <= cp && cp <= U'\U0010FFFD' && !is_surrogate(cp) && !is_noncharacter(cp)) return true;
		return false;
	}
}

wordring::whatwg::host_base wordring::whatwg::parse_opaque_host(
	std::u32string const& in, std::error_code& ec)
{
	// 1.
	for (char32_t cp : in)
	{
		if (is_forbidden_host_code_point(cp) && cp != U'%')
		{
			ec = url_error_name::ValidationError;
			return host_base();
		}
	}
	// 2.
	for (char32_t cp : in) { if (!is_url_code_points_(cp) && cp != U'%') ec = url_error_name::ValidationError; }
	// 3.
	std::uint32_t state = 0;
	for (std::uint32_t i = 0; i < in.size(); ++i)
	{
		char32_t cp = in[i];
		switch (state)
		{
		case 0:
			if (cp == U'%')
			{
				if (in.size() - i < 3)
				{
					ec = url_error_name::ValidationError;
					break;
				}
				else state = 1;
			}
			break;
		case 1:
			if (!is_ascii_hex_digit(cp))
			{
				ec = url_error_name::ValidationError;
				break;
			}
			else state = 2;
			break;
		case 2:
			if (!is_ascii_hex_digit(cp))
			{
				ec = url_error_name::ValidationError;
				break;
			}
			else state = 0;
			break;
		}
	}
	// 4.
	return opaque_host_base{ utf8_percent_encode(in, is_c0_control_percent_encode_set) };
}

std::u32string wordring::whatwg::serialize_host(wordring::whatwg::host_base const& host)
{
	// 1.
	if (host.type() == host_type_name::Ipv4Address) return serialize_ipv4(host);
	// 2.
	if (host.type() == host_type_name::Ipv6Address)
	{
		std::u32string s;
		s.push_back(U'[');
		s += serialize_ipv6(host);
		s.push_back(U']');
		return s;
	}
	// 3.
	if (host.type() == host_type_name::Domain) return host.get<domain_base>().m_value;
	if (host.type() == host_type_name::OpaqueHost) return host.get<opaque_host_base>().m_value;
	if (host.type() == host_type_name::EmptyHost) return U"";

	assert(false);
	return U"";
}

std::u32string wordring::whatwg::serialize_ipv4(wordring::whatwg::host_base const& ipv4)
{
	// 1.
	std::u32string output;
	// 2.
	std::uint32_t n = ipv4.get<ipv4_address_base>().m_value;
	// 3.
	for (std::uint32_t i = 1; i <= 4; ++i)
	{
		// 3.1.
		std::array<char, 10> a;
		auto [p, ec] = std::to_chars(a.data(), a.data() + a.size(), n % 256);
		output.insert(output.begin(), a.data(), p);
		// 3.2.
		if (i != 4) output.insert(output.begin(), U'.');
		// 3.3.
		n = static_cast<std::uint32_t>(std::floor(n / 256));
	}
	// 4.
	return output;
}

std::u32string wordring::whatwg::serialize_ipv6(wordring::whatwg::host_base const& ipv6)
{
	// 1.
	std::u32string output;
	// 2-3.
	std::optional<std::uint32_t> compress;
	ipv6_address_base const& address = ipv6.get<ipv6_address_base>();
	std::uint32_t longer = 0;
	std::uint32_t length = 0;
	std::uint32_t tail = 0;
	for (std::uint32_t i = 0; i < 8; ++i)
	{
		if (address[i] == 0)
		{
			++length;
			if (i == 7) i = 8;
			else continue;
		}
		
		if (longer < length)
		{
			tail = i;
			longer = length;
			length = 0;
		}
	}
	if (2 <= longer) compress = std::make_optional<std::uint32_t>(tail - longer);
	// 4.
	bool ignore0 = false;
	// 5.
	for (std::uint32_t pieceIndex = 0; pieceIndex < 8; ++pieceIndex)
	{
		// 5.1.
		if (ignore0 && address[pieceIndex] == 0) continue;
		// 5.2.
		ignore0 = false;
		// 5.3.
		if (compress && *compress == pieceIndex)
		{
			std::u32string separator = pieceIndex == 0 ? U"::" : U":";
			output += separator;
			ignore0 = true;
			continue;
		}
		// 5.4.
		std::array<char, 10> a;
		auto [p, ec] = std::to_chars(a.data(), a.data() + a.size(), address[pieceIndex], 0x10);
		output.insert(output.end(), a.data(), p);
		// 5.5.
		if (pieceIndex != 7) output.push_back(U':');
	}
	// 6.
	return output;
}
