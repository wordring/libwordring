#pragma once

// ------------------------------------------------------------------------------------------------
// 3. Hosts (domains and IP addresses)
//
// https://url.spec.whatwg.org/#hosts-(domains-and-ip-addresses)
// https://triple-underscore.github.io/URL-ja.html#hosts-(domains-and-ip-addresses)
// ------------------------------------------------------------------------------------------------

#include <wordring/whatwg/url/infra.hpp>
#include <wordring/whatwg/url/url_defs.hpp>

#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/infra/infra.hpp>
#include <wordring/whatwg/infra/unicode.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <variant>

namespace wordring::whatwg
{
	
	// --------------------------------------------------------------------------------------------
	// 3.1. Host representation
	//
	// https://url.spec.whatwg.org/#host-representation
	// --------------------------------------------------------------------------------------------

	/*! @brief domain
	* 
	* @sa https://url.spec.whatwg.org/#concept-domain
	*/
	struct domain_base { std::u32string m_value; };

	/*! @brief IPv4 address
	* 
	* @sa https://url.spec.whatwg.org/#concept-ipv4
	*/
	struct ipv4_address_base { std::uint32_t m_value; };

	/*! @brief IPv6 address
	* 
	* @sa https://url.spec.whatwg.org/#concept-ipv6
	*/
	struct ipv6_address_base
	{
		std::array<std::uint16_t, 8> m_value;

		std::uint16_t& operator[](std::uint32_t i) { return m_value[i]; }
		std::uint16_t const& operator[](std::uint32_t i) const { return m_value[i]; }
	};

	/*! @brief opaque host
	* 
	* @sa https://url.spec.whatwg.org/#opaque-host
	*/
	struct opaque_host_base { std::u32string m_value; };

	/*! @brief empty host
	* 
	* @sa https://url.spec.whatwg.org/#empty-host
	*/
	struct empty_host_base {};

	enum class host_type_name : std::uint32_t
	{
		Domain = 1,
		Ipv4Address,
		Ipv6Address,
		OpaqueHost,
		EmptyHost,
		String
	};

	/*! @brief host
	* 
	* @sa https://url.spec.whatwg.org/#concept-host
	*/
	class host_base
	{
	public:
		using value_type = std::variant<
			std::monostate,
			domain_base,
			ipv4_address_base,
			ipv6_address_base,
			opaque_host_base,
			empty_host_base>;// ,
			//std::u32string>;

	public:
		host_base() : m_value() {}

		host_base(host_base const& x) : m_value(x.m_value) {}
		host_base(host_base&& x) noexcept : m_value(std::move(x.m_value)) {}

		template <typename Value>
		host_base(Value&& val) : m_value(val) {}

		host_base& operator=(host_base const&) = default;

		operator bool() const { return std::holds_alternative<std::monostate>(m_value); }

		bool operator !() const { return operator bool(); }

		host_type_name type() const
		{
			switch (m_value.index())
			{
			case 0: return static_cast<host_type_name>(0);
			case 1: return host_type_name::Domain;
			case 2: return host_type_name::Ipv4Address;
			case 3: return host_type_name::Ipv6Address;
			case 4: return host_type_name::OpaqueHost;
			case 5: return host_type_name::EmptyHost;
			//case 6: return host_type_name::String;
			default:
				break;
			}
			return static_cast<host_type_name>(0);
		}

		template <typename Type>
		Type& get() { return std::get<Type>(m_value); }

		template <typename Type>
		Type const& get() const { return std::get<Type>(m_value); }

	protected:
		value_type m_value;
	};

	inline bool operator==(host_base const& lhs, std::u32string const& rhs)
	{
		switch (lhs.type())
		{
		case host_type_name::Domain:
			return lhs.get<domain_base>().m_value == rhs;
		case host_type_name::OpaqueHost:
			return lhs.get<opaque_host_base>().m_value == rhs;
		default:
			break;
		}
		assert(false);
		return false;
	}

	// --------------------------------------------------------------------------------------------
	// 3.2. Host miscellaneous
	//
	// https://url.spec.whatwg.org/#host-miscellaneous
	// --------------------------------------------------------------------------------------------

	/*! @brief forbidden host code point
	* 
	* @sa https://url.spec.whatwg.org/#forbidden-host-code-point
	*/
	inline bool is_forbidden_host_code_point(std::uint32_t cp)
	{
		switch (cp)
		{
		case U'\x00':
		case U'\x09':
		case U'\x0A':
		case U'\x0D':
		case U'\x20':
		case U'\x23':
		case U'\x25':
		case U'\x2F':
		case U'\x3A':
		case U'\x3F':
		case U'\x40':
		case U'\x5B':
		case U'\x5C':
		case U'\x5D':
			return true;
		}
		return false;
	}

	// --------------------------------------------------------------------------------------------
	// 3.3. IDNA
	//
	// https://url.spec.whatwg.org/#idna
	// --------------------------------------------------------------------------------------------

	/*! @brief 国際化ドメイン名を ASCII ドメイン名へ変換する
	* 
	* @param [in]  s        ドメイン名
	* @param [in]  beStrict ラベル長の検査を行う（省略時は false ）
	* @param [out] ec       エラーコード
	* 
	* 変換中、エラーに遭遇すると ec へ url_error_name::IdnaTranslationFailed をセットする。
	* 
	* @sa https://triple-underscore.github.io/URL-ja.html#concept-domain-to-ascii
	* 
	* @internal
	* <hr>
	* 
	* CheckJoiners フラグに相当する、 ICU の UIDNA_CHECK_CONTEXTJ 、 UIDNA_CHECK_CONTEXTO フラグは、
	* カナ文字の中点のような、他の言語でも良くありがちで見間違えやすい文字を利用する攻撃を防ぐために、
	* そのような文字をチェックする指定のようです。
	* 
	* @sa https://wiki.suikawiki.org/n/CheckJoiners
	* 
	* VerifyDnsLength がセットされる場合、ドメイン名の長さがチェックされる。
	* ドメイン名の長さは、ルート・ラベルとそのドット「.」（.jp等？）を除き、1～253。
	* 各ラベルの長さは、1 ～ 63。
	* ICU のエラーコードによると、長さは常にチェックされ、切り替えフラグは無いようです。
	* 
	* @sa https://unicode-org.github.io/icu-docs/apidoc/dev/icu4c/uidna_8h.html#a0411cd49bb5b71852cecd93bcbf0ca2da5071eb574ee6087ed7850c4e5f307ba4
	* 
	* @sa https://www.unicode.org/reports/tr46/#ToASCII
	* 
	*/
	std::u32string domain_to_ascii(std::u32string const& s, bool beStrict, std::error_code& ec);

	inline std::u32string domain_to_ascii(std::u32string const& s, std::error_code& ec)
	{
		return domain_to_ascii(s, false, ec);
	}

	/*! @brief ASCII ドメイン名を国際化ドメイン名へ変換する
	*
	* @param [in]  s  ドメイン名
	* @param [out] ec エラーコード
	*
	* 変換中、エラーに遭遇すると ec へ url_error_name::IdnaTranslationFailed をセットする。
	*
	* @sa https://triple-underscore.github.io/URL-ja.html#concept-domain-to-ascii
	*/
	std::u32string domain_to_unicode(std::u32string const& s, std::error_code& ec);

	
	// --------------------------------------------------------------------------------------------
	// 3.4. Host writing
	//
	// https://url.spec.whatwg.org/#host-writing
	// --------------------------------------------------------------------------------------------

	bool is_valid_domain_string(std::u32string const& s);
	bool is_valid_ipv4_address_string(std::u32string const& s);
	bool is_valid_ipv6_address_string(std::u32string const& s);

	/*! @brief 妥当なホスト文字列
	* 
	* @sa https://url.spec.whatwg.org/#valid-host-string
	*/
	inline bool is_valid_host_string(std::u32string const& s)
	{
		if (is_valid_domain_string(s) || is_valid_ipv4_address_string(s)) return true;
		if (s.size() < 3 || s.front() != U'[' || s.back() != U']') return false;
		return is_valid_ipv6_address_string(std::u32string(++s.begin(), --s.end()));
	}

	/*! @brief 妥当なドメイン文字列
	*
	* @sa https://url.spec.whatwg.org/#valid-domain-string
	*/
	bool is_valid_domain_string(std::u32string const& s);

	/*! @brief 妥当な　IPv4 アドレス文字列
	*
	* @sa https://url.spec.whatwg.org/#valid-ipv4-address-string
	*/
	bool is_valid_ipv4_address_string(std::u32string const& s);

	/*! @brief 妥当な　IPv6 アドレス文字列
	*
	* @sa https://url.spec.whatwg.org/#valid-ipv6-address-string
	*/
	inline bool is_valid_ipv6_address_string(std::u32string const& s)
	{
		return true;
	}

	/*! @brief 妥当な不透明ホスト文字列
	*
	* @sa https://url.spec.whatwg.org/#valid-opaque-host-string
	*/
	inline bool is_valid_opaque_host_string(std::u32string const& s)
	{
		return true;
	}
	
	// --------------------------------------------------------------------------------------------
	// 3.5. Host parsing
	//
	// https://url.spec.whatwg.org/#host-parsing
	// --------------------------------------------------------------------------------------------

	host_base parse_ipv4(std::u32string const& in, std::error_code& ec);
	host_base parse_ipv6(std::u32string const& in, std::error_code& ec);
	host_base parse_opaque_host(std::u32string const& in, std::error_code& ec);

	std::pair<std::optional<std::uint32_t>, bool> parse_ipv4_number(std::u32string s);

	/*! @brief host parser
	* 
	* 解析に失敗した場合、 ec に url_error_name::ParseFailed を設定する。
	* 解析に失敗した場合、空のホストを返す。
	* 
	* @sa https://url.spec.whatwg.org/#concept-host-parser
	*/
	host_base parse_host(std::u32string const& in, bool isNotSpecial, std::error_code& ec);

	inline host_base parse_host(std::u32string const& in, std::error_code& ec)
	{
		return parse_host(in, false, ec);
	}

	/*! @brief IPv4 parser
	*
	* @sa https://url.spec.whatwg.org/#concept-ipv4-parser
	*/
	host_base parse_ipv4(std::u32string const& in, std::error_code& ec);

	std::pair<std::optional<std::uint32_t>, bool> parse_ipv4_number(std::u32string s);

	/*! @brief IPv6 parser
	* 
	* @sa https://url.spec.whatwg.org/#concept-ipv6-parser
	*/
	host_base parse_ipv6(std::u32string const& in, std::error_code& ec);

	/*! @brief IPv6 parser
	*
	* @sa https://url.spec.whatwg.org/#concept-ipv6-parser
	*/
	host_base parse_opaque_host(std::u32string const& in, std::error_code& ec);

	// --------------------------------------------------------------------------------------------
	// 3.6. Host serializing
	//
	// https://url.spec.whatwg.org/#host-serializing
	// --------------------------------------------------------------------------------------------

	/*! @brief host serializer
	*
	* @sa https://url.spec.whatwg.org/#concept-host-serializer
	*/
	std::u32string serialize_host(host_base const& host);

	/*! @brief IPv4 serializer
	* 
	* @sa https://url.spec.whatwg.org/#concept-ipv4-serializer
	*/
	std::u32string serialize_ipv4(host_base const& ipv4);

	/*! @brief IPv6 serializer
	* 
	* @sa https://url.spec.whatwg.org/#concept-ipv6-serializer
	*/
	std::u32string serialize_ipv6(host_base const& ipv6);
	
	// --------------------------------------------------------------------------------------------
	// 3.7. Host equivalence
	//
	// https://url.spec.whatwg.org/#host-equivalence
	// --------------------------------------------------------------------------------------------

	/*! @brief HOST equals
	*
	* @sa https://url.spec.whatwg.org/#concept-host-equals
	*/
	inline bool operator==(host_base const& lhs, host_base const& rhs)
	{
		if (lhs.type() != rhs.type()) return false;
		switch (lhs.type())
		{
		case host_type_name::Domain:      return lhs.get<domain_base>().m_value == rhs.get<domain_base>().m_value;
		case host_type_name::EmptyHost:   return true;
		case host_type_name::Ipv4Address: return lhs.get<ipv4_address_base>().m_value == rhs.get<ipv4_address_base>().m_value;
		case host_type_name::Ipv6Address: return lhs.get<ipv6_address_base>().m_value == rhs.get<ipv6_address_base>().m_value;
		case host_type_name::OpaqueHost:  return lhs.get<opaque_host_base>().m_value == rhs.get<opaque_host_base>().m_value;
		default:
			break;
		}
		assert(false);
		return false;
	}
}
