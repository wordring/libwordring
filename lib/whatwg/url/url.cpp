
#include <wordring/whatwg/url/url.hpp>

#include <algorithm>
#include <iterator>
#include <string_view>

// ------------------------------------------------------------------------------------------------
// 4.2. URL miscellaneous
//
// https://url.spec.whatwg.org/#url-miscellaneous
// ------------------------------------------------------------------------------------------------

bool wordring::whatwg::is_special_scheme(std::u32string const& scheme)
{
	if (   scheme == U"ftp"
		|| scheme == U"file"
		|| scheme == U"http"
		|| scheme == U"https"
		|| scheme == U"ws"
		|| scheme == U"wss") return true;
	return false;
}

std::optional<std::uint16_t> wordring::whatwg::default_port(std::u32string const& scheme)
{

	if (scheme == U"ftp")   return std::make_optional<std::uint16_t>(21);
	if (scheme == U"file")  return std::optional<std::uint16_t>();
	if (scheme == U"http")  return std::make_optional<std::uint16_t>(80);
	if (scheme == U"https") return std::make_optional<std::uint16_t>(443);
	if (scheme == U"ws")    return std::make_optional<std::uint16_t>(80);
	if (scheme == U"wss")   return std::make_optional<std::uint16_t>(443);
	return std::optional<std::uint16_t>();
}

bool wordring::whatwg::is_special(wordring::whatwg::url_record_base const& url)
{
	return is_special_scheme(url.m_scheme);
}

bool wordring::whatwg::includes_credentials(wordring::whatwg::url_record_base const& url)
{
	return !url.m_username.empty() || !url.m_password.empty();
}

bool wordring::whatwg::cannot_have_username_or_password_or_port(wordring::whatwg::url_record_base const& url)
{
	return !url.m_host
		|| url.m_host == std::u32string(U"")
		|| cannot_have_username_or_password_or_port(url)
		|| url.m_scheme == U"file";
}

bool wordring::whatwg::is_windows_drive_letter(char32_t c1, char32_t c2)
{
	return is_ascii_alpha(c1) && (c2 == U':' || c2 == U'|');
}

bool wordring::whatwg::is_windows_drive_letter(std::u32string const& s)
{
	return s.size() == 2 && is_windows_drive_letter(s[0], s[1]);
}

bool wordring::whatwg::is_normalized_windows_drive_letter(char32_t c1, char32_t c2)
{
	return is_ascii_alpha(c1) && c2 == U':';
}

bool wordring::whatwg::is_normalized_windows_drive_letter(std::u32string const& s)
{
	return s.size() == 2 && is_normalized_windows_drive_letter(s[0], s[1]);
}

bool wordring::whatwg::starts_with_windows_drive_letter(std::u32string::const_iterator first, std::u32string::const_iterator last)
{
	return 2 <= std::distance(first, last)
		&& is_windows_drive_letter(*first, *std::next(first))
		&& (std::distance(first, last) == 2 || *std::next(first, 2) == U'/' || *std::next(first, 2) == U'\\' || *std::next(first, 2) == U'?' || *std::next(first, 2) == U'#');
}

void wordring::whatwg::shorten_urls_path(wordring::whatwg::url_record_base& url)
{
	if (url.m_path.empty()) return;
	if (url.m_scheme == U"file"
		&& url.m_path.size() == 1
		&& (url.m_path.front().size() == 2 && is_normalized_windows_drive_letter(url.m_path.front()))) return;
	url.m_path.pop_back();
}

// ------------------------------------------------------------------------------------------------
// 4.3. URL writing
//
// https://url.spec.whatwg.org/#url-writing
// ------------------------------------------------------------------------------------------------

bool wordring::whatwg::is_valid_url_string(std::u32string const& s)
{
	return is_relative_url_with_fragment_string(s)
		|| is_absolute_url_with_fragment_string(s);
}

bool wordring::whatwg::is_absolute_url_with_fragment_string(std::u32string const& s)
{
	auto it = starts_with_absolute_url_string(s.begin(), s.end());
	if (it == s.begin()) return false;







	return true;
}

bool wordring::whatwg::is_absolute_url_string(std::u32string const& s)
{

}

std::u32string::const_iterator wordring::whatwg::starts_with_absolute_url_string(
	std::u32string::const_iterator first, std::u32string::const_iterator last)
{

}

bool wordring::whatwg::is_url_scheme_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_relative_url_with_fragment_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_relative_url_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_scheme_relative_special_url_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_url_port_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_scheme_relative_url_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_opaque_host_and_port_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_scheme_relative_file_url_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_path_absolute_url_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_path_absolute_non_windows_file_url_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_path_relative_url_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_path_relative_scheme_less_url_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_url_path_segment_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_single_dot_path_segment(std::u32string const& s)
{
	std::u32string_view sv1 = U"%2e";

	return s == U"." || s == U"%2e" || s == U"%2E";
}

bool wordring::whatwg::is_double_dot_path_segment(std::u32string const& s)
{
	std::u32string_view sv1 = U".%2e";
	std::u32string_view sv2 = U"%2e.";
	std::u32string_view sv3 = U"%2e%2e";

	return s == U".."
		|| is_ascii_case_insensitive_match(s.begin(), s.end(), sv1.begin(), sv1.end())
		|| is_ascii_case_insensitive_match(s.begin(), s.end(), sv2.begin(), sv2.end())
		|| is_ascii_case_insensitive_match(s.begin(), s.end(), sv3.begin(), sv3.end());
}

bool wordring::whatwg::is_url_query_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_url_fragment_string(std::u32string const& s)
{
	
}

bool wordring::whatwg::is_url_code_point(char32_t c)
{
	using namespace wordring::whatwg;
	if (is_ascii_alphanumeric(c)) return true;
	switch (c)
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
	if (U'\xA0' <= c && c <= U'\U0010FFFD' && !is_surrogate(c) && !is_noncharacter(c)) return true;
	return false;
}

bool wordring::whatwg::is_url_units(std::u32string const& s)
{
	
}

// ------------------------------------------------------------------------------------------------
// 4.4. URL parsing
//
// https://url.spec.whatwg.org/#url-parsing
// ------------------------------------------------------------------------------------------------

std::optional<wordring::whatwg::url_record_base> wordring::whatwg::pase_url(
	std::u32string const& input,
	wordring::whatwg::url_record_base const* base,
	wordring::whatwg::encoding_name name)
{
	std::error_code ec;
	// 1.
	basic_url_parser parser(input, ec, base, name);
	std::optional<url_record_base> url = parser.run();
	// 2.
	if (!url) return url;
	// 3.
	if (url->m_scheme != U"blob") return url;
	// 4.
	// TODO: BLOB エントリの解決
	// 5.
	return url;
}


wordring::whatwg::basic_url_parser::basic_url_parser(
	std::u32string const& input,
	std::error_code& ec,
	url_record_base const* base,
	encoding_name name,
	url_record_base* url,
	url_parse_state override_state)
		: m_input()
		, m_base(base)
		, m_encoding_name(name)
		, m_url(url)
		, m_override(override_state)
		, m_ec(&ec)
		, m_url_()
{
	auto it1 = input.begin();
	auto it2 = input.end();
	// 1.
	if (url == nullptr)
	{
		m_url = &m_url_;
		while (it1 != it2)
		{
			if (is_c0_control_or_space(*it1))
			{
				ec = url_error_name::ValidationError;
				++it1;
			}
			else break;
		}
		while (it1 != it2)
		{
			if (is_c0_control_or_space(*std::prev(it2)))
			{
				ec = url_error_name::ValidationError;
				--it2;
			}
			else break;
		}
	}
	// 2-3.
	// 4.
	while (it1 != it2)
	{
		if (is_ascii_tab_or_newline(*it1)) ec = url_error_name::ValidationError;
		else m_input.push_back(*it1);
		++it1;
	}

	m_state = override_state != static_cast<url_parse_state>(0)
		? override_state
		: url_parse_state::SchemeStartState;
	// 5.
	m_encoding_name = encoding::get_output_encoding_name(name);
	// 6.
	// 7.
	atSignSeen = insideBrackets = passwordTokenSeen = false;
	// 8.
	pointer = m_input.begin();
}

std::optional<wordring::whatwg::url_record_base> wordring::whatwg::basic_url_parser::run()
{
	// 9.
	while (true)
	{
		url_parse_result ret = static_cast<url_parse_result>(0);
		char32_t c = pointer != m_input.end() ? *pointer : U'\n';
		switch (m_state)
		{
		case url_parse_state::SchemeStartState:
			ret = on_scheme_start_state(c);
			break;
		case url_parse_state::SchemeState:
			ret = on_scheme_state(c);
			break;
		case url_parse_state::NoSchemeState:
			ret = on_no_scheme_state(c);
			break;
		case url_parse_state::SpecialRelativeOrAuthorityState:
			ret = on_special_relative_or_authority_state(c);
			break;
		case url_parse_state::PathOrAuthorityState:
			ret = on_path_or_authority_state(c);
			break;
		case url_parse_state::RelativeState:
			ret = on_relative_state(c);
			break;
		case url_parse_state::RelativeSlashState:
			ret = on_relative_slash_state(c);
			break;
		case url_parse_state::SpecialAuthoritySlashesState:
			ret = on_special_authority_slashes_state(c);
			break;
		case url_parse_state::SpecialAuthorityIgnoreSlashesState:
			ret = on_special_authority_ignore_slashes_state(c);
			break;
		case url_parse_state::AuthorityState:
			ret = on_authority_state(c);
			break;
		case url_parse_state::HostState:
			ret = on_host_state(c);
			break;
		case url_parse_state::HostnameState:
			ret = on_hostname_state(c);
			break;
		case url_parse_state::PortState:
			ret = on_port_state(c);
			break;
		case url_parse_state::FileState:
			ret = on_file_state(c);
			break;
		case url_parse_state::FileSlashState:
			ret = on_file_slash_state(c);
			break;
		case url_parse_state::FileHostState:
			ret = on_file_host_state(c);
			break;
		case url_parse_state::PathStartState:
			ret = on_path_start_state(c);
			break;
		case url_parse_state::PathState:
			ret = on_path_state(c);
			break;
		case url_parse_state::CannotBeABaseUrlPathState:
			ret = on_cannot_be_a_base_url_path_state(c);
			break;
		case url_parse_state::QueryState:
			ret = on_query_state(c);
			break;
		case url_parse_state::FragmentState:
			ret = on_fragment_state(c);
			break;
		}

		switch (ret)
		{
		case url_parse_result::Failure:
		case url_parse_result::Return:
			return std::optional<url_record_base>();
		case url_parse_result::Continue:
		default:
			break;
		}

		if (pointer == m_input.end()) break;
		++pointer;
	}

	return std::make_optional<url_record_base>(*m_url);
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_scheme_start_state(char32_t c)
{
	// 1.
	if (is_ascii_alpha(c))
	{
		to_ascii_lowercase(pointer, std::next(pointer), std::back_inserter(buffer));
		m_state = url_parse_state::SchemeState;
	}
	// 2.
	else if (m_override == static_cast<url_parse_state>(0))
	{
		m_state = url_parse_state::NoSchemeState;
		assert(pointer != m_input.begin());
		--pointer;
	}
	// 3.
	else
	{
		*m_ec = url_error_name::ValidationError;
		return url_parse_result::Failure;
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_scheme_state(char32_t c)
{
	// 1.
	if (is_ascii_alphanumeric(c) || c == U'+' || c == U'-' || c == U'.')
	{
		to_ascii_lowercase(pointer, std::next(pointer), std::back_inserter(buffer));
	}
	// 2.
	else if (c == U':')
	{
		// 2.1.
		if (m_override != static_cast<url_parse_state>(0))
		{
			// 1.
			if (is_special_scheme(m_url->m_scheme) && !is_special_scheme(buffer)) return url_parse_result::Return;
			// 2.
			if (!is_special_scheme(m_url->m_scheme) && is_special_scheme(buffer)) return url_parse_result::Return;
			// 3.
			if ((includes_credentials(*m_url) || m_url->m_port) && buffer == U"file") return url_parse_result::Return;
			// 4.
			if (m_url->m_scheme == U"file" && m_url->m_host->type() == host_type_name::EmptyHost) return url_parse_result::Return;
		}
		// 2.2.
		m_url->m_scheme = buffer;
		// 2.3.
		if (m_override != static_cast<url_parse_state>(0))
		{
			if (m_url->m_port && m_url->m_port == default_port(m_url->m_scheme)) m_url->m_port.reset();
			return url_parse_result::Return;
		}
		// 2.4.
		buffer.clear();
		// 2.5.
		if (m_url->m_scheme == U"file")
		{
			if (std::distance(pointer, m_input.end()) < 3 || *(pointer + 1) != U'/' || *(pointer + 2) != U'/')
			{
				*m_ec = url_error_name::ValidationError;
			}
			m_state = url_parse_state::FileState;
		}
		// 2.6.
		else if (is_special(*m_url) && m_base != nullptr && m_base->m_scheme == m_url->m_scheme)
		{
			m_state = url_parse_state::SpecialRelativeOrAuthorityState;
		}
		// 2.7.
		else if (is_special(*m_url)) m_state = url_parse_state::SpecialAuthoritySlashesState;
		// 2.8.
		else if (2 < std::distance(pointer, m_input.end()) && *(pointer + 1) == U'/')
		{
			m_state = url_parse_state::PathOrAuthorityState;
			++pointer;
		}
		// 2.9.
		else
		{
			m_url->m_cannot_be_base_url = true;
			m_url->m_path.push_back(std::u32string());
			m_state = url_parse_state::CannotBeABaseUrlPathState;
		}
	}
	// 3.
	else if (m_override == static_cast<url_parse_state>(0))
	{
		buffer.clear();
		m_state = url_parse_state::NoSchemeState;
		pointer = m_input.begin();
	}
	// 4.
	else
	{
		*m_ec = url_error_name::ValidationError;
		return url_parse_result::Failure;
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_no_scheme_state(char32_t c)
{
	// 1.
	if (m_base == nullptr || (m_base->m_cannot_be_base_url == true && c != U'#'))
	{
		*m_ec = url_error_name::ValidationError;
		return url_parse_result::Failure;
	}
	// 2.
	else if (m_base->m_cannot_be_base_url == true && c == U'#')
	{
		m_url->m_scheme = m_base->m_scheme;
		m_url->m_path = m_base->m_path;
		m_url->m_query = m_base->m_query;
		m_url->m_fragment = std::make_optional<std::u32string>();
		m_url->m_cannot_be_base_url = true;
		m_state = url_parse_state::FragmentState;
	}
	// 3.
	else if (m_base->m_scheme != U"file")
	{
		m_state = url_parse_state::RelativeState;
		assert(pointer != m_input.begin());
		--pointer;
	}
	// 4.
	else
	{
		m_state = url_parse_state::FileState;
		assert(pointer != m_input.begin());
		--pointer;
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_special_relative_or_authority_state(char32_t c)
{
	// 1.
	if (c == U'/' && 2 <= std::distance(pointer, m_input.end()) && pointer[1] == U'/')
	{
		m_state = url_parse_state::SpecialAuthorityIgnoreSlashesState;
		assert(pointer != m_input.end());
		++pointer;
	}
	// 2.
	else
	{
		*m_ec = url_error_name::ValidationError;
		m_state = url_parse_state::RelativeState;
		assert(pointer != m_input.begin());
		--pointer;
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_path_or_authority_state(char32_t c)
{
	// 1.
	if (c == U'/') m_state = url_parse_state::AuthorityState;
	// 2.
	else
	{
		m_state = url_parse_state::PathState;
		assert(pointer != m_input.begin());
		--pointer;
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_relative_state(char32_t c)
{
	// 1.
	m_url->m_scheme = m_base->m_scheme;
	// 2.
	if (c == U'/') m_state = url_parse_state::RelativeSlashState;
	// 3.
	else if(is_special(*m_url) && c== U'\x5C')
	{
		*m_ec = url_error_name::ValidationError;
		m_state = url_parse_state::RelativeSlashState;
	}
	// 4.
	else
	{
		// 4.1.
		m_url->m_username = m_base->m_username;
		m_url->m_password = m_base->m_password;
		m_url->m_host = m_base->m_host;
		m_url->m_port = m_base->m_port;
		m_url->m_path = m_base->m_path;
		m_url->m_query = m_base->m_query;
		// 4.2.
		if (c == U'?')
		{
			m_url->m_query = std::make_optional<std::u32string>();
			m_state = url_parse_state::QueryState;
		}
		// 4.3.
		else if (c == U'#')
		{
			m_url->m_fragment = std::make_optional<std::u32string>();
			m_state = url_parse_state::FragmentState;
		}
		// 4.4.
		else if(c != U'\n') // ! EOF
		{
			// 4.4.1.
			m_url->m_query.reset();
			// 4.4.2.
			if (!m_url->m_path.empty()) m_url->m_path.erase(--m_url->m_path.end());
			// 4.4.3.
			m_state = url_parse_state::PathState;
			assert(pointer != m_input.begin());
			--pointer;
		}
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_relative_slash_state(char32_t c)
{
	// 1.
	if (is_special(*m_url) && (c == U'/' || c == U'\x5C'))
	{
		// 1.1.
		if (c == U'\x5C') *m_ec = url_error_name::ValidationError;
		// 1.2.
		m_state = url_parse_state::SpecialAuthorityIgnoreSlashesState;
	}
	// 2.
	else if (c == U'/') m_state = url_parse_state::AuthorityState;
	// 3.
	else
	{
		m_url->m_username = m_base->m_username;
		m_url->m_password = m_base->m_password;
		m_url->m_host = m_base->m_host;
		m_url->m_port = m_base->m_port;
		m_state = url_parse_state::PathState;
		assert(pointer != m_input.begin());
		--pointer;
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_special_authority_slashes_state(char32_t c)
{
	// 1.
	if (c == U'/' && 2 <= std::distance(pointer, m_input.end()) && pointer[1] == U'/')
	{
		m_state = url_parse_state::SpecialAuthorityIgnoreSlashesState;
		assert(pointer != m_input.end());
		++pointer;
	}
	// 2.
	else
	{
		*m_ec = url_error_name::ValidationError;
		m_state = url_parse_state::SpecialAuthorityIgnoreSlashesState;
		assert(pointer != m_input.begin());
		--pointer;
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_special_authority_ignore_slashes_state(char32_t c)
{
	// 1.
	if (!(c == U'/' || c == U'\x5C'))
	{
		m_state = url_parse_state::AuthorityState;
		assert(pointer != m_input.begin());
		--pointer;
	}
	// 2.
	else *m_ec = url_error_name::ValidationError;

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_authority_state(char32_t c)
{
	// 1.
	if (c == U'@')
	{
		// 1.1.
		*m_ec = url_error_name::ValidationError;
		// 1.2.
		if (atSignSeen == true)
		{
			std::u32string_view sv = U"%40";
			buffer.insert(buffer.begin(), sv.begin(), sv.end());
		}
		// 1.3.
		atSignSeen = true;
		// 1.4.
		for (char32_t codePoint : buffer)
		{
			// 1.4.1.
			if (codePoint == U':' && passwordTokenSeen == false)
			{
				passwordTokenSeen = true;
				continue;
			}
			// 1.4.2.
			std::u32string encodedCodePoints = utf8_percent_encode(codePoint, is_userinfo_percent_encode_set);
			// 1.4.3.
			if (passwordTokenSeen == true) m_url->m_password += encodedCodePoints;
			// 1.4.4.
			else m_url->m_username += encodedCodePoints;
		}
		// 1.5.
		buffer.clear();
	}
	// 2.
	else if ((c == U'\n' || c == U'/' || c == U'?' || c == U'#') || (is_special(*m_url) && c == U'\x5C'))
	{
		// 2.1.
		if (atSignSeen == true && buffer.empty())
		{
			*m_ec = url_error_name::ValidationError;
			return url_parse_result::Failure;
		}
		// 2.2.
		assert(buffer.size() < std::distance(m_input.begin(), pointer));
		pointer -= (buffer.size() + 1);
		buffer.clear();
		m_state = url_parse_state::HostState;
	}
	// 3.
	else
	{
		assert(c != U'\n');
		buffer.push_back(c);
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_host_state(char32_t c)
{
	return on_hostname_state(c);
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_hostname_state(char32_t c)
{
	// 1.
	if (m_override != static_cast<url_parse_state>(0) && m_url->m_scheme == U"file")
	{
		assert(pointer != m_input.begin());
		--pointer;
		m_state = url_parse_state::FileHostState;
	}
	// 2.
	if (c == U':' && insideBrackets == false)
	{
		// 2.1.
		if (buffer.empty())
		{
			*m_ec = url_error_name::ValidationError;
			return url_parse_result::Failure;
		}
		// 2.2.
		std::error_code ec;
		host_base host = parse_host(buffer, !is_special(*m_url), ec);
		// 2.3.
		if (host.type() == static_cast<host_type_name>(0)) return url_parse_result::Failure;
		// 2.4.
		m_url->m_host = std::make_optional<host_base>(std::move(host));
		buffer.clear();
		m_state = url_parse_state::PortState;
		// 2.5.
		if (m_override == url_parse_state::HostnameState) return url_parse_result::Return;
	}
	// 3.
	else if ((c == U'\n' || c == U'/' || c == U'?' || c == U'#') || (is_special(*m_url) && c == U'\x5C'))
	{
		// 3.1.
		if (is_special(*m_url) && buffer.empty())
		{
			*m_ec = url_error_name::ValidationError;
			return url_parse_result::Failure;
		}
		// 3.2.
		else if (m_override != static_cast<url_parse_state>(0) && buffer.empty() && (includes_credentials(*m_url) || m_url->m_port))
		{
			*m_ec = url_error_name::ValidationError;
			return url_parse_result::Return;
		}
		// 3.3.
		std::error_code ec;
		host_base host = parse_host(buffer, !is_special(*m_url), ec);
		// 3.4.
		if (host.type() == static_cast<host_type_name>(0)) return url_parse_result::Failure;
		// 3.5.
		m_url->m_host = std::make_optional<host_base>(std::move(host));
		buffer.clear();
		m_state = url_parse_state::PathStartState;
		// 3.6.
		if (m_override != static_cast<url_parse_state>(0)) return url_parse_result::Return;
	}
	// 4.
	else
	{
		if (c == U'[') insideBrackets = true;
		if (c == U']') insideBrackets = false;
		buffer.push_back(c);
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_port_state(char32_t c)
{
	// 1.
	if (is_ascii_digit(c)) buffer.push_back(c);
	// 2.
	else if ((c == U'\n' || c == U'/' || c == U'?' || c == U'#') || (is_special(*m_url) && c == U'\x5C')
		|| (m_override != static_cast<url_parse_state>(0)))
	{
		// 2.1.
		if (!buffer.empty())
		{
			// 2.1.1.
			std::uint32_t port = 0;
			for (char32_t cp : buffer) port = port * 10 + (cp - U'0');
			// 2.1.2.
			if (5 < buffer.size() || 0xFFFF <= port)
			{
				*m_ec = url_error_name::ValidationError;
				return url_parse_result::Failure;
			}
			// 2.1.3.
			if (port == default_port(m_url->m_scheme)) m_url->m_port.reset();
			else m_url->m_port = std::make_optional<std::uint16_t>(port);
			// 2.1.4.
			buffer.clear();
		}
		// 2.2.
		if (m_override != static_cast<url_parse_state>(0)) return url_parse_result::Return;
		// 2.3.
		m_state = url_parse_state::PathStartState;
		assert(pointer != m_input.begin());
		--pointer;
	}
	// 3.
	else
	{
		*m_ec = url_error_name::ValidationError;
		return url_parse_result::Failure;
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_file_state(char32_t c)
{
	// 1.
	m_url->m_scheme = U"file";
	// 2.
	m_url->m_host = std::make_optional<host_base>(empty_host_base{});
	// 3.
	if (c == U'/' || c == U'\x5C')
	{
		if (c == U'\x5C') *m_ec = url_error_name::ValidationError;
		m_state = url_parse_state::FileSlashState;
	}
	// 4.
	else if (m_base != nullptr && m_base->m_scheme == U"file")
	{
		// 4.1.
		m_url->m_host = m_base->m_host;
		m_url->m_path = m_base->m_path;
		m_url->m_query = m_base->m_query;
		// 4.2.
		if (c == U'?')
		{
			m_url->m_query = std::make_optional<std::u32string>();
			m_state = url_parse_state::QueryState;
		}
		// 4.3.
		else if (c == U'#')
		{
			m_url->m_fragment = std::make_optional<std::u32string>();
			m_state = url_parse_state::FragmentState;
		}
		// 4.4.
		else if (c != U'\n') // ! EOF
		{
			// 4.4.1.
			m_url->m_query.reset();
			// 4.4.2.
			if (starts_with_windows_drive_letter(pointer, m_input.end())) shorten_urls_path(*m_url);
			// 4.4.3.
			else
			{
				*m_ec = url_error_name::ValidationError;
				m_url->m_path.clear();
			}
			// 4.4.4.
			m_state = url_parse_state::PathState;
			assert(pointer != m_input.begin());
			--pointer;
		}
	}
	// 5.
	else
	{
		m_state = url_parse_state::PathState;
		assert(pointer != m_input.begin());
		--pointer;
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_file_slash_state(char32_t c)
{
	// 1.
	if (c == U'/' || c == U'\x5C')
	{
		if (c == U'\x5C') *m_ec = url_error_name::ValidationError;
		m_state = url_parse_state::FileHostState;
	}
	// 2.
	else
	{
		// 2.1.
		if (m_base != nullptr && m_base->m_scheme == U"file")
		{
			// 2.1.1.
			m_url->m_host = m_base->m_host;
			// 2.1.2.
			if (!starts_with_windows_drive_letter(pointer, m_input.end())
				&& !m_base->m_path.empty() && is_normalized_windows_drive_letter(m_base->m_path.front()))
			{
				m_url->m_path.push_back(m_base->m_path.front());
			}
		}
		// 2.2.
		m_state = url_parse_state::PathState;
		assert(pointer != m_input.begin());
		--pointer;
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_file_host_state(char32_t c)
{
	// 1.
	if (c == U'\n' || c == U'/' || c == U'\x5C' || c == U'?' || c == U'#')
	{
		assert(pointer != m_input.begin());
		--pointer;
		// 1.1.
		if (m_override == static_cast<url_parse_state>(0) && is_windows_drive_letter(buffer))
		{
			*m_ec = url_error_name::ValidationError;
			m_state = url_parse_state::PathState;
		}
		// 1.2.
		else if (buffer.empty())
		{
			// 1.2.1.
			m_url->m_host = std::make_optional<host_base>(empty_host_base{});
			// 1.2.2.
			if (m_override != static_cast<url_parse_state>(0)) return url_parse_result::Return;
			// 1.2.3.
			m_state = url_parse_state::PathStartState;
		}
		// 1.3.
		else
		{
			// 1.3.1.
			std::error_code ec;
			host_base host = parse_host(buffer, !is_special(*m_url), ec);
			// 1.3.2.
			if (host.type() == static_cast<host_type_name>(0)) return url_parse_result::Failure;
			// 1.3.3.
			if (host == std::u32string(U"localhost")) host = empty_host_base{};
			// 1.3.4.
			m_url->m_host = std::make_optional<host_base>(std::move(host));
			// 1.3.5.
			if (m_override != static_cast<url_parse_state>(0)) return url_parse_result::Return;
			// 1.3.6.
			buffer.clear();
			m_state = url_parse_state::PathStartState;
		}
	}
	// 2.
	else buffer.push_back(c);

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_path_start_state(char32_t c)
{
	// 1.
	if (is_special(*m_url))
	{
		if (c == U'\x5C') *m_ec = url_error_name::ValidationError;
		m_state = url_parse_state::PathState;
		if (!(c == U'/' || c == U'\x5C'))
		{
			assert(pointer != m_input.begin());
			--pointer;
		}
	}
	// 2.
	else if (m_override == static_cast<url_parse_state>(0) && c == U'?')
	{
		m_url->m_query = std::make_optional<std::u32string>();
		m_state = url_parse_state::QueryState;
	}
	// 3.
	else if (m_override == static_cast<url_parse_state>(0) && c == U'#')
	{
		m_url->m_fragment = std::make_optional<std::u32string>();
		m_state = url_parse_state::FragmentState;
	}
	// 4.
	else if (c != U'\n') // ! EOF
	{
		m_state = url_parse_state::PathState;
		if (c != U'/')
		{
			assert(pointer != m_input.begin());
			--pointer;
		}
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_path_state(char32_t c)
{
	// 1.
	if ((c == U'\n' || c == U'/') || (is_special(*m_url) && c == U'\x5C')
		|| (m_override == static_cast<url_parse_state>(0) && (c == U'?' || c == u'#')))
	{
		// 1.1.
		if (is_special(*m_url) && c == U'\x5C') *m_ec = url_error_name::ValidationError;
		// 1.2.
		if (is_double_dot_path_segment(buffer))
		{
			// 1.2.1.
			shorten_urls_path(*m_url);
			// 1.2.2.
			if (!(c == U'/' || (is_special(*m_url) && c == U'\x5C'))) m_url->m_path.push_back(U"");
		}
		// 1.3.
		else if (is_single_dot_path_segment(buffer))
		{
			if (!(c == U'/' || (is_special(*m_url) && c == U'\x5C'))) m_url->m_path.push_back(U"");
		}
		// 1.4.
		else if (!is_single_dot_path_segment(buffer))
		{
			// 1.4.1.
			if (m_url->m_scheme == U"file" && m_url->m_path.empty() && is_windows_drive_letter(buffer))
			{
				buffer[1] = U':'; // driveレター「C|」 を 「C:」 へ変える。
			}
			// 1.4.2.
			m_url->m_path.push_back(buffer);
		}
		// 1.5.
		buffer.clear();
		// 1.6.
		if (c == U'?')
		{
			m_url->m_query = std::make_optional<std::u32string>();
			m_state = url_parse_state::QueryState;
		}
		// 1.7.
		if (c == U'#')
		{
			m_url->m_fragment = std::make_optional<std::u32string>();
			m_state = url_parse_state::FragmentState;
		}
	}
	// 2.
	else
	{
		// 2.1.
		if (!is_url_code_point(c) && c == U'%') *m_ec = url_error_name::ValidationError;
		// 2.2.
		if (c == U'%' && (std::distance(pointer, m_input.end()) < 2 || !is_ascii_hex_digit(pointer[1])))
		{
			*m_ec = url_error_name::ValidationError;
		}
		// 2.3.
		buffer += utf8_percent_encode(c, is_path_percent_encode_set);
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_cannot_be_a_base_url_path_state(char32_t c)
{
	// 1.
	if (c == U'?')
	{
		m_url->m_query = std::make_optional<std::u32string>();
		m_state = url_parse_state::QueryState;
	}
	// 2.
	else if (c == U'#')
	{
		m_url->m_fragment = std::make_optional<std::u32string>();
		m_state = url_parse_state::FragmentState;
	}
	// 3.
	else
	{
		// 3.1.
		if (c != U'\n' && !is_url_code_point(c) && c != U'%') *m_ec = url_error_name::ValidationError;
		// 3.2.
		if (c == U'%' && (std::distance(pointer, m_input.end()) < 3 || !is_ascii_hex_digit(pointer[1]) || !is_ascii_hex_digit(pointer[2])))
		{
			*m_ec = url_error_name::ValidationError;
		}
		// 3.3.
		if (c != U'\n') m_url->m_path[0] += utf8_percent_encode(c, is_c0_control_percent_encode_set);
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_query_state(char32_t c)
{
	// 1.
	if (m_encoding_name != encoding_name::UTF_8 && (!is_special(*m_url) || m_url->m_scheme == U"ws" || m_url->m_scheme == U"wss"))
	{
		m_encoding_name = encoding_name::UTF_8;
	}
	// 2.
	if ((m_override == static_cast<url_parse_state>(0) && c == U'#') || (c == U'\n'))
	{
		// 2.1.
		auto queryPercentEncodeSet = is_special(*m_url) ? is_special_query_percent_encode_set : is_query_percent_encode_set;
		// 2.2.
		m_url->m_query->append(percent_encode_after_encoding(m_encoding_name, buffer, queryPercentEncodeSet));
		// 2.3.
		buffer.clear();
		// 2.4.
		if (c == U'#')
		{
			m_url->m_fragment = std::make_optional<std::u32string>();
			m_state = url_parse_state::FragmentState;
		}
	}
	// 3.
	else if (c != U'\n')
	{
		// 3.1.
		if (!is_url_code_point(c) && c != U'%') *m_ec = url_error_name::ValidationError;
		// 3.2.
		if (c == U'%' && (std::distance(pointer, m_input.end()) < 3 || !is_ascii_hex_digit(pointer[1]) || !is_ascii_hex_digit(pointer[2])))
		{
			*m_ec = url_error_name::ValidationError;
		}
		// 3.3.
		buffer.push_back(c);
	}

	return url_parse_result::Continue;
}

wordring::whatwg::url_parse_result wordring::whatwg::basic_url_parser::on_fragment_state(char32_t c)
{
	// 1.
	if (c != U'\n')
	{
		// 1.1.
		if (!is_url_code_point(c) && c != U'%') *m_ec = url_error_name::ValidationError;
		// 1.2.
		if (c == U'%' && (std::distance(pointer, m_input.end()) < 3 || !is_ascii_hex_digit(pointer[1]) || !is_ascii_hex_digit(pointer[2])))
		{
			*m_ec = url_error_name::ValidationError;
		}
		// 1.3.
		m_url->m_fragment->append(utf8_percent_encode(c, is_fragment_percent_encode_set));
	}

	return url_parse_result::Continue;
}


