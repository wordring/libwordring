#pragma once

// ------------------------------------------------------------------------------------------------
// 6. API
//
// https://url.spec.whatwg.org/#api
// ------------------------------------------------------------------------------------------------

#include <wordring/whatwg/url/url.hpp>

#include <wordring/whatwg/infra/unicode.hpp>

#include <iterator>
#include <optional>
#include <system_error>

namespace wordring::whatwg
{
	/*! @brief URL
	* 
	* @sa https://url.spec.whatwg.org/#url-class
	*/
	template <typename String>
	class basic_url
	{
		template <typename String1>
		friend String1 to_string(basic_url<String1> const& url, bool exclude_fragment);

	public:
		using string_type = String;

	protected:
		void parse(string_type const& url, string_type const* base)
		{
			// 1.
			std::optional<url_record_base> parsedBase;
			// 2.
			if (base != nullptr)
			{
				// 2.1.
				std::error_code ec;
				std::u32string tmp = encoding_cast<std::u32string>(*base);
				basic_url_parser p(tmp, ec);
				parsedBase = p.run();
				// 2.2.
				if (!parsedBase) throw make_error_code(url_error_name::ParseFailed);
			}
			// 3.
			std::error_code ec;
			std::u32string tmp = encoding_cast<std::u32string>(url);
			basic_url_parser p(tmp, ec, parsedBase ? &*parsedBase : nullptr);
			std::optional<url_record_base> parsedURL = p.run();
			// 4.
			if (!parsedURL) throw make_error_code(url_error_name::ParseFailed);
			// 5.
			std::u32string query = parsedURL->m_query ? *parsedURL->m_query : std::u32string();
			// 6.
			m_url = std::move(*parsedURL);
			// 7-9.
			// TODO
		}

	public:
		basic_url() = default;
		basic_url(basic_url const&) = default;
		basic_url(basic_url&&) = default;

		basic_url(string_type const& url)
		{
			parse(url, nullptr);
		}

		basic_url(string_type const& url, string_type const& base)
		{
			parse(url, &base);
		}

		/*! @brief href 取得
		* 
		* @sa https://url.spec.whatwg.org/#dom-url-href
		*/
		string_type href() const { return encoding_cast<string_type>(serialize_url(m_url)); }

		/*! @brief href 設定
		*
		* @sa https://url.spec.whatwg.org/#dom-url-href
		*/
		void href(string_type const& s)
		{
			// 1.
			std::error_code ec;
			basic_url_parser p(encoding_cast<std::u32string>(s), ec);
			std::optional<url_record_base> parsedURL = p.run();
			// 2.
			if (!parsedURL) throw ec;
			// 3.
			m_url = *parsedURL;
			// 4-6.
			// TODO
		}

		/*! @brief origin 取得
		*
		* 未実装。
		* 
		* @sa https://url.spec.whatwg.org/#dom-url-origin
		*/
		string_type origin() const
		{
			assert(false);
			return string_type();
		}

		/*! @brief protocol 取得
		*
		* @sa https://url.spec.whatwg.org/#dom-url-protocol
		*/
		string_type protocol() const
		{
			std::u32string s = m_url.m_scheme;
			s.push_back(U':');
			return encoding_cast<string_type>(s);
		}

		/*! @brief protocol 設定
		*
		* @sa https://url.spec.whatwg.org/#dom-url-protocol
		*/
		void protocol(string_type const& s)
		{
			std::u32string tmp = encoding_cast<std::u32string>(s);
			tmp.push_back(U':');
			std::error_code ec;
			basic_url_parser p(tmp, ec, nullptr, encoding_name::UTF_8, &m_url, url_parse_state::SchemeStartState);	
			p.run();
		}

		/*! @brief username 取得
		*
		* @sa https://url.spec.whatwg.org/#dom-url-username
		*/
		string_type username() const { return encoding_cast<string_type>(m_url.m_username); }

		/*! @brief username 設定
		*
		* @sa https://url.spec.whatwg.org/#dom-url-username
		*/
		void username(string_type const& s)
		{
			if (cannot_have_username_or_password_or_port(m_url)) return;
			std::u32string tmp = encoding_cast<std::u32string>(s);
			set_username(m_url, tmp);
		}

		/*! @brief password 取得
		*
		* @sa https://url.spec.whatwg.org/#dom-url-password
		*/
		string_type password() const { return encoding_cast<string_type>(m_url.m_password); }

		/*! @brief password 設定
		*
		* @sa https://url.spec.whatwg.org/#dom-url-password
		*/
		void password(string_type const& s)
		{
			if (cannot_have_username_or_password_or_port(m_url)) return;
			std::u32string tmp = encoding_cast<std::u32string>(s);
			set_password(m_url, tmp);
		}

		/*! @brief host 取得
		*
		* @sa https://url.spec.whatwg.org/#dom-url-host
		*/
		string_type host() const
		{
			if (!m_url.m_host) return string_type();
			std::u32string s = serialize_host(*m_url.m_host);
			if (m_url.m_port)
			{
				s.push_back(U':');
				serialize_integer(*m_url.m_port, std::back_inserter(s));
			}
			return encoding_cast<string_type>(s);
		}

		/*! @brief host 設定
		*
		* @sa https://url.spec.whatwg.org/#dom-url-host
		*/
		void host(string_type const& s)
		{
			if (m_url.m_cannot_be_base_url) return;
			std::u32string tmp = encoding_cast<std::u32string>(s);
			std::error_code ec;
			basic_url_parser p(tmp, ec, nullptr, encoding_name::UTF_8, &m_url, url_parse_state::HostState);
			p.run();
		}

		/*! @brief hostname 取得
		*
		* @sa https://url.spec.whatwg.org/#dom-url-hostname
		*/
		string_type hostname() const
		{
			if (!m_url.m_host) return string_type();
			return encoding_cast<string_type>(serialize_host(*m_url.m_host));
		}

		/*! @brief hostname 設定
		*
		* @sa https://url.spec.whatwg.org/#dom-url-hostname
		*/
		void hostname(string_type const& s)
		{
			if (m_url.m_cannot_be_base_url) return;
			std::u32string tmp = encoding_cast<std::u32string>(s);
			std::error_code ec;
			basic_url_parser p(tmp, ec, nullptr, encoding_name::UTF_8, &m_url, url_parse_state::HostnameState);
			p.run();
		}

		/*! @brief port 取得
		*
		* @sa https://url.spec.whatwg.org/#dom-url-port
		*/
		string_type port() const
		{
			if (!m_url.m_port) return string_type();
			std::u32string s;
			serialize_integer(*m_url.m_port, std::back_inserter(s));
			return encoding_cast<string_type>(s);
		}

		/*! @brief port 設定
		*
		* @sa https://url.spec.whatwg.org/#dom-url-port
		*/
		void port(string_type const& s)
		{
			if (cannot_have_username_or_password_or_port(m_url)) return;
			if (s.empty()) m_url.m_port.reset();
			else
			{
				std::u32string tmp = encoding_cast<std::u32string>(s);
				std::error_code ec;
				basic_url_parser p(tmp, ec, nullptr, encoding_name::UTF_8, &m_url, url_parse_state::PortState);
				p.run();
			}
		}

		/*! @brief pathname 取得
		*
		* @sa https://url.spec.whatwg.org/#dom-url-pathname
		*/
		string_type pathname() const
		{
			if (m_url.m_cannot_be_base_url) return encoding_cast<string_type>(m_url.m_path.front());
			std::u32string s;
			for (std::u32string const& segment : m_url.m_path)
			{
				s.push_back(U'/');
				s.append(segment);
			}
			return encoding_cast<string_type>(s);
		}

		/*! @brief pathname 設定
		*
		* @sa https://url.spec.whatwg.org/#dom-url-pathname
		*/
		void pathname(string_type const& s)
		{
			if (m_url.m_cannot_be_base_url) return;
			m_url.m_path.clear();
			std::u32string tmp = encoding_cast<std::u32string>(s);
			std::error_code ec;
			basic_url_parser p(tmp, ec, nullptr, encoding_name::UTF_8, &m_url, url_parse_state::PathStartState);
			p.run();
		}

		/*! @brief search 取得
		*
		* @sa https://url.spec.whatwg.org/#dom-url-search
		*/
		string_type search() const
		{
			if (!m_url.m_query || m_url.m_query->empty()) return string_type();
			std::u32string s;
			s.push_back(U'?');
			s.append(*m_url.m_query);
			return encoding_cast<string_type>(s);
		}

		/*! @brief search 設定
		*
		* @sa https://url.spec.whatwg.org/#dom-url-search
		*/
		void search(string_type const& s)
		{
			if (s.empty())
			{
				m_url.m_query.reset();
				return;
			}
			std::u32string tmp = encoding_cast<std::u32string>(s);
			if (tmp.front() == U'?') tmp.erase(tmp.begin());
			m_url.m_query = std::make_optional<std::u32string>();
			std::error_code ec;
			basic_url_parser p(tmp, ec, nullptr, encoding_name::UTF_8, &m_url, url_parse_state::QueryState);
			p.run();
		}

		/*! @brief hash 取得
		*
		* @sa https://url.spec.whatwg.org/#dom-url-hash
		*/
		string_type hash() const
		{
			if (!m_url.m_fragment || m_url.m_fragment->empty()) return string_type();
			std::u32string s;
			s.push_back(U'#');
			s.append(*m_url.m_fragment);
			return encoding_cast<string_type>(s);
		}

		/*! @brief hash 設定
		*
		* @sa https://url.spec.whatwg.org/#dom-url-hash
		*/
		void hash(string_type const& s)
		{
			if (s.empty())
			{
				m_url.m_fragment.reset();
				return;
			}

			std::u32string tmp = encoding_cast<std::u32string>(s);
			if (tmp.front() == U'#') tmp.erase(tmp.begin());
			m_url.m_fragment = std::make_optional<std::u32string>();
			std::error_code ec;
			basic_url_parser p(tmp, ec, nullptr, encoding_name::UTF_8, &m_url, url_parse_state::FragmentState);
			p.run();
		}

	protected:
		url_record_base m_url;
	};

	template <typename String>
	inline String to_string(basic_url<String> const& url, bool exclude_fragment = false)
	{
		return encoding_cast<String>(serialize_url(url.m_url, exclude_fragment));
	}
}
