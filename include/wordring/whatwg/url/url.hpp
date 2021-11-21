#pragma once

// ------------------------------------------------------------------------------------------------
// 4. URLs
//
// https://url.spec.whatwg.org/#urls
// ------------------------------------------------------------------------------------------------

#include <wordring/whatwg/url/host.hpp>

#include <wordring/whatwg/infra/infra.hpp>

#include <wordring/whatwg/encoding/encoding_defs.hpp>

#include <optional>
#include <string>
#include <vector>

namespace wordring::whatwg
{
	// --------------------------------------------------------------------------------------------
	// 4.1. URL representation
	//
	// https://url.spec.whatwg.org/#url-representation
	// --------------------------------------------------------------------------------------------

	/*! @brief URL record
	*
	* @sa https://url.spec.whatwg.org/#url-representation
	*/
	struct url_record_base
	{
		std::u32string m_scheme;
		std::u32string m_username;
		std::u32string m_password;
		std::optional<host_base> m_host;
		std::optional<std::uint16_t>  m_port;
		std::vector<std::u32string> m_path;
		std::optional<std::u32string> m_query;
		std::optional<std::u32string> m_fragment;
		bool m_cannot_be_base_url = false;
		std::vector<char> m_blob_url_entry;
	};

	// --------------------------------------------------------------------------------------------
	// 4.2. URL miscellaneous
	//
	// https://url.spec.whatwg.org/#url-miscellaneous
	// --------------------------------------------------------------------------------------------

	/*! @brief special scheme
	* 
	* @sa https://url.spec.whatwg.org/#special-scheme
	*/
	bool is_special_scheme(std::u32string const& scheme);

	/*! @brief default port
	* 
	* @sa https://url.spec.whatwg.org/#default-port
	*/
	std::optional<std::uint16_t> default_port(std::u32string const& scheme);

	/*! @brief special URL
	*
	* @sa https://url.spec.whatwg.org/#is-special
	*/
	bool is_special(url_record_base const& url);

	/*! @brief 資格情報を含む
	*
	* @sa　https://url.spec.whatwg.org/#include-credentials
	*/
	bool includes_credentials(url_record_base const& url);

	/*! @brief ユーザー名、パスワード、ポート番号を持つことが出来ない
	*
	* @sa　https://url.spec.whatwg.org/#cannot-have-a-username-password-port
	*/
	bool cannot_have_username_or_password_or_port(url_record_base const& url);

	/*! @brief Windows drive letter
	*
	* @sa https://url.spec.whatwg.org/#windows-drive-letter
	*/
	bool is_windows_drive_letter(char32_t c1, char32_t c2);

	/*! @brief Windows drive letter
	*
	* @sa https://url.spec.whatwg.org/#windows-drive-letter
	*/
	bool is_windows_drive_letter(std::u32string const& s);

	/*! @brief 正規化 Windows drive letter
	*
	* @sa https://url.spec.whatwg.org/#normalized-windows-drive-letter
	*/
	bool is_normalized_windows_drive_letter(char32_t c1, char32_t c2);

	/*! @brief 正規化 Windows drive letter
	*
	* @sa https://url.spec.whatwg.org/#normalized-windows-drive-letter
	*/
	bool is_normalized_windows_drive_letter(std::u32string const& s);

	/*! @brief Windows drive letter から始まる
	*
	* @sa https://url.spec.whatwg.org/#start-with-a-windows-drive-letter
	*/
	bool starts_with_windows_drive_letter(char32_t const* first, char32_t const* last);

	/*! @brief URL の PATH を短くする
	*
	* @sa https://url.spec.whatwg.org/#shorten-a-urls-path
	*/
	void shorten_urls_path(url_record_base& url);

	// --------------------------------------------------------------------------------------------
	// 4.3. URL writing
	//
	// https://url.spec.whatwg.org/#url-writing
	// --------------------------------------------------------------------------------------------

	/*! @brief valid URL 文字列
	* 
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#valid-url-string
	*/
	bool is_valid_url_string(std::u32string const& s);

	/*! @brief absolute-URL-with-fragment string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#absolute-url-with-fragment-string
	*/
	bool is_absolute_url_with_fragment_string(std::u32string const& s);

	/*! @brief absolute-URL string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#absolute-url-string
	*/
	bool is_absolute_url_string(std::u32string const& s);

	/*! @brief absolute-URL string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#absolute-url-string
	*/
	std::u32string::const_iterator starts_with_absolute_url_string(
		std::u32string::const_iterator first, std::u32string::const_iterator last);

	/*! @brief URL-scheme string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#url-scheme-string
	*/
	bool is_url_scheme_string(std::u32string const& s);

	/*! @brief relative-URL-with-fragment string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#relative-url-with-fragment-string
	*/
	bool is_relative_url_with_fragment_string(std::u32string const& s);

	/*! @brief relative-URL string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#relative-url-string
	*/
	bool is_relative_url_string(std::u32string const& s);

	/*! @brief scheme-relative-special-URL string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#scheme-relative-special-url-string
	*/
	bool is_scheme_relative_special_url_string(std::u32string const& s);

	/*! @brief URL-port string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#url-port-string
	*/
	bool is_url_port_string(std::u32string const& s);

	/*! @brief scheme-relative-URL string 
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#scheme-relative-file-url-string
	*/
	bool is_scheme_relative_url_string(std::u32string const& s);

	/*! @brief opaque-host-and-port string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#opaque-host-and-port-string
	*/
	bool is_opaque_host_and_port_string(std::u32string const& s);

	/*! @brief scheme-relative-file-URL string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#scheme-relative-file-url-string
	*/
	bool is_scheme_relative_file_url_string(std::u32string const& s);

	/*! @brief path-absolute-URL string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#path-absolute-url-string
	*/
	bool is_path_absolute_url_string(std::u32string const& s);

	/*! @brief path-absolute-non-Windows-file-URL string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#path-absolute-non-windows-file-url-string
	*/
	bool is_path_absolute_non_windows_file_url_string(std::u32string const& s);

	/*! @brief path-relative-URL string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#path-relative-url-string
	*/
	bool is_path_relative_url_string(std::u32string const& s);

	/*! @brief path-relative-scheme-less-URL string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#path-relative-scheme-less-url-string
	*/
	bool is_path_relative_scheme_less_url_string(std::u32string const& s);

	/*! @brief URL-path-segment string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#url-path-segment-string
	*/
	bool is_url_path_segment_string(std::u32string const& s);

	/*! @brief single-dot path segment
	*
	* @sa https://url.spec.whatwg.org/#single-dot-path-segment
	*/
	bool is_single_dot_path_segment(std::u32string const& s);

	/*! @brief double-dot path segment
	*
	* @sa https://url.spec.whatwg.org/#double-dot-path-segment
	*/
	bool is_double_dot_path_segment(std::u32string const& s);

	/*! @brief URL-query string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#url-query-string
	*/
	bool is_url_query_string(std::u32string const& s);

	/*! @brief URL-fragment string
	*
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#url-fragment-string
	*/
	bool is_url_fragment_string(std::u32string const& s);

	/*! @brief URL code points
	*
	* @sa https://url.spec.whatwg.org/#url-code-points
	*/
	bool is_url_code_point(char32_t c);

	/*! @brief URL units
	* 
	* 未実装。
	* 
	* @sa https://url.spec.whatwg.org/#url-units
	*/
	bool is_url_units(std::u32string const& s);
	
	// --------------------------------------------------------------------------------------------
	// 4.4. URL parsing
	//
	// https://url.spec.whatwg.org/#url-parsing
	// --------------------------------------------------------------------------------------------

	/*! @brief URL parser
	*
	* @sa https://url.spec.whatwg.org/#concept-url-parser
	*/
	std::optional<url_record_base> pase_url(
		std::u32string const& in,
		url_record_base const* base = nullptr,
		encoding_name name = encoding_name::UTF_8);


	/*! @brief basic URL parser
	*
	* @sa https://url.spec.whatwg.org/#concept-basic-url-parser
	*/
	enum class url_parse_state : std::uint32_t
	{
		SchemeStartState = 1,
		SchemeState,
		NoSchemeState,
		SpecialRelativeOrAuthorityState,
		PathOrAuthorityState,
		RelativeState,
		RelativeSlashState,
		SpecialAuthoritySlashesState,
		SpecialAuthorityIgnoreSlashesState,
		AuthorityState,
		HostState,
		HostnameState,
		PortState,
		FileState,
		FileSlashState,
		FileHostState,
		PathStartState,
		PathState,
		CannotBeABaseUrlPathState,
		QueryState,
		FragmentState
	};

	enum class url_parse_result : std::uint32_t
	{
		Failure = 1,
		Return,
		Continue
	};

	/*! @brief basic URL parser
	*
	* 解析前に、入力文字列からタブや改行が取り除かれるため、「\\n」をEOS として使う。
	* 
	* @sa https://url.spec.whatwg.org/#concept-basic-url-parser
	*/
	class basic_url_parser
	{
	public:
		basic_url_parser(
			std::u32string const& input,
			std::error_code& ec,
			url_record_base const* base = nullptr,
			encoding_name name = encoding_name::UTF_8,
			url_record_base* url = nullptr,
			url_parse_state override_state = static_cast<url_parse_state>(0));

		std::optional<url_record_base> run();
		
		url_parse_result on_scheme_start_state(char32_t c);
		url_parse_result on_scheme_state(char32_t c);
		url_parse_result on_no_scheme_state(char32_t c);
		url_parse_result on_special_relative_or_authority_state(char32_t c);
		url_parse_result on_path_or_authority_state(char32_t c);
		url_parse_result on_relative_state(char32_t c);
		url_parse_result on_relative_slash_state(char32_t c);
		url_parse_result on_special_authority_slashes_state(char32_t c);
		url_parse_result on_special_authority_ignore_slashes_state(char32_t c);
		url_parse_result on_authority_state(char32_t c);
		url_parse_result on_host_state(char32_t c);
		url_parse_result on_hostname_state(char32_t c);
		url_parse_result on_port_state(char32_t c);
		url_parse_result on_file_state(char32_t c);
		url_parse_result on_file_slash_state(char32_t c);
		url_parse_result on_file_host_state(char32_t c);
		url_parse_result on_path_start_state(char32_t c);
		url_parse_result on_path_state(char32_t c);
		url_parse_result on_cannot_be_a_base_url_path_state(char32_t c);
		url_parse_result on_query_state(char32_t c);
		url_parse_result on_fragment_state(char32_t c);

	public:
		std::u32string m_input;
		url_record_base const* m_base;
		encoding_name m_encoding_name;
		url_record_base* m_url;
		url_parse_state m_override;

		std::error_code* m_ec;

		url_record_base m_url_;

		url_parse_state m_state;
		std::u32string buffer;
		bool atSignSeen;
		bool insideBrackets;
		bool passwordTokenSeen;
		char32_t* pointer;
	};
	
	/*! @brief URL レコードへユーザー名を設定する
	* 
	* @sa https://url.spec.whatwg.org/#set-the-username
	*/
	void set_username(url_record_base& url, std::u32string const& username);

	/*! @brief URL レコードへパスワードを設定する
	*
	* @sa https://url.spec.whatwg.org/#set-the-password
	*/
	void set_password(url_record_base& url, std::u32string const& password);

	// --------------------------------------------------------------------------------------------
	// 4.5. URL serializing
	//
	// https://url.spec.whatwg.org/#url-serializing
	// --------------------------------------------------------------------------------------------

	/*! @brief URL serializer
	* 
	* @sa https://url.spec.whatwg.org/#concept-url-serializer
	*/
	std::u32string serialize_url(url_record_base const& url, bool exclude_fragment = false);

	// --------------------------------------------------------------------------------------------
	// 4.6. URL equivalence
	// 
	// https://url.spec.whatwg.org/#url-equivalence
	// --------------------------------------------------------------------------------------------

	/*! brief  URL A equals B
	* 
	* @sa https://url.spec.whatwg.org/#concept-url-equals
	*/
	bool equals(url_record_base const& lhs, url_record_base const& rhs, bool exclude_fragments = false);
}
