#pragma once

// ------------------------------------------------------------------------------------------------
// URL
//
// 2.5 URLs
// https://html.spec.whatwg.org/multipage/urls-and-fetching.html#urls
// https://triple-underscore.github.io/HTML-urls-and-fetching-ja.html#urls
// ------------------------------------------------------------------------------------------------

#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/encoding_defs.hpp>
#include <wordring/whatwg/infra/infra.hpp>

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

namespace wordring::whatwg::html
{
	// --------------------------------------------------------------------------------------------
	// 2.6.3 Extracting character encodings from meta elements
	//
	// https://html.spec.whatwg.org/multipage/urls-and-fetching.html#algorithm-for-extracting-a-character-encoding-from-a-meta-element
	// https://triple-underscore.github.io/HTML-urls-and-fetching-ja.html#extracting-character-encodings-from-meta-elements
	// --------------------------------------------------------------------------------------------

	/*! @brief meta 要素からエンコーディングを抽出する

	@param [in] s meta 要素の content 属性値

	meta 要素が http-equiv="Content-Type" と content という属性を持つ時、
	パーサーから content 属性の値を引数に呼び出される。

	@sa https://html.spec.whatwg.org/multipage/urls-and-fetching.html#algorithm-for-extracting-a-character-encoding-from-a-meta-element
	@sa https://triple-underscore.github.io/HTML-urls-and-fetching-ja.html#algorithm-for-extracting-a-character-encoding-from-a-meta-element
	*/
	inline encoding_name extract_character_encoding_from_meta_element(std::u32string const& s)
	{
		std::u32string_view sv = U"charset";
		std::u32string s1;
		to_ascii_lowercase(s.begin(), s.end(), std::back_inserter(s1));

		// 1.
		auto position = s1.begin();
		// 2.
	Loop:
		position = std::search(position, s1.end(), sv.begin(), sv.end());
		if (position != s1.end()) position += sv.size();
		else return static_cast<encoding_name>(0);
		// 3.
		position = skip_ascii_whitespace(position, s1.end());
		// 4.
		if (position == s1.end() || *position != U'=') goto Loop;
		// 5.
		position = skip_ascii_whitespace(++position, s1.end());
		// 6.
		if (position != s1.end())
		{
			if (*position == U'"' || *position == U'\'')
			{
				char32_t cp = *position++;
				auto last = std::find(position, s1.end(), cp);
				if (last != s1.end()) return get_encoding_name(std::u32string(position, last));
			}
			else
			{
				// Otherwise.
				auto last = std::find_if(position, s1.end(), [](char32_t cp) { return cp == U';' || is_ascii_white_space(cp); });
				return get_encoding_name(std::u32string(position, last));
			}
		}

		return static_cast<encoding_name>(0);
	}
}
