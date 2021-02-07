#pragma once

#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/terminology.hpp>
#include <wordring/whatwg/infra/infra.hpp>

#include <wordring/encoding/encoding_defs.hpp>

#include <algorithm>
#include <string_view>

namespace wordring::wwwc
{
	// --------------------------------------------------------------------------------------------
	// 3.2. The input byte stream
	//
	// https://drafts.csswg.org/css-syntax-3/#input-byte-stream
	// https://triple-underscore.github.io/css-syntax-ja.html#input-byte-stream
	// --------------------------------------------------------------------------------------------

	template <typename ForwardIterator>
	inline encoding_name determine_fallback_encoding(ForwardIterator first, ForwardIterator last, encoding_name fallback = static_cast<encoding_name>(0));

	/*! @brief バイト列を復号する
	
	@sa https://drafts.csswg.org/css-syntax-3/#css-decode-bytes
	@sa https://triple-underscore.github.io/css-syntax-ja.html#css-decode-bytes
	*/
	template <typename ForwardIterator, typename OutputIterator>
	void decode(ForwardIterator first, ForwardIterator last, OutputIterator out, encoding_name fallback = static_cast<encoding_name>(0))
	{
		static_assert(sizeof(decltype(*first)) == sizeof(std::uint8_t));

		using namespace wordring;

		fallback = determine_fallback_encoding(first, last, fallback);
		wordring::whatwg::encoding::io_queue<char> in_q = wordring::whatwg::encoding::to_io_queue_convert(first, last);
		wordring::whatwg::encoding::io_queue<char32_t> out_q;
		wordring::whatwg::encoding::decode(in_q, fallback, out_q);
		wordring::whatwg::encoding::from_io_queue_convert(out_q, out);
	}

	/*! @brief 代替エンコーディングを決定する

	@param [in] first バイトストリームの始端を指すイテレータ
	@param [in] last  バイトストリームの終端を指すイテレータ
	@param [in] fallback 代替エンコーディング（省略可）

	@return エンコーディング名

	@sa https://drafts.csswg.org/css-syntax-3/#determine-the-fallback-encoding
	@sa https://triple-underscore.github.io/css-syntax-ja.html#determine-the-fallback-encoding
	*/
	template <typename ForwardIterator>
	inline encoding_name determine_fallback_encoding(ForwardIterator first, ForwardIterator last, encoding_name fallback)
	{
		static_assert(sizeof(decltype(*first)) == sizeof(std::uint8_t));

		std::uint32_t state = 0;
		std::string label;
		// 1.
		if (fallback != static_cast<encoding_name>(0)) return fallback;
		// 2.
		std::string_view sv = "@charset \"";
		for (std::uint32_t i = 0; first != last && i < sv.size(); ++i) { if (sv[i] != *first++) goto Abort; }

		for (std::uint32_t i = 0; first != last && i <= 1014; ++i)
		{
			std::uint8_t ch = *first++;
			switch (state)
			{
			case 0:
				if ((0 <= ch && ch <= 0x21) || (0x23 <= ch && ch <= 0x7F))
				{
					label.push_back(ch);
					continue;
				}
				if (ch != '\"') goto Abort;
				state = 1;
				break;
			case 1:
				if (ch == ';')
				{
					encoding_name enc = get_encoding_name(label);
					if (enc == static_cast<encoding_name>(0)) goto Abort;
					return enc;
				}
				label.push_back('\"');
				label.push_back(ch);
				state = 0;
				break;
			}
		}
		// 4.
	Abort:
		return encoding_name::UTF_8;
	}

	// --------------------------------------------------------------------------------------------
	// 3.3. Preprocessing the input stream
	//
	// https://drafts.csswg.org/css-syntax-3/#input-preprocessing
	// https://triple-underscore.github.io/css-syntax-ja.html#input-byte-stream
	// --------------------------------------------------------------------------------------------

	/*! @brief コード・ポイント列をフィルタする
	
	@param [in] first 文字列の先頭を指すイテレータ
	@param [in] last  文字列の終端を指すイテレータ

	@return フィルター後の文字列終端を指すイテレータ

	この関数は、CR、LF、FF、NULL、サロゲートを適切に置き換えする。
	STLのミュータブルなアルゴリズムのように、コンテナを変更する。
	返されたイテレータより後ろをコンテナからeraseする必要が有る。

	@sa https://drafts.csswg.org/css-syntax-3/#css-filter-code-points
	@sa https://triple-underscore.github.io/css-syntax-ja.html#input-preprocessing
	*/
	template <typename ForwardIterator>
	ForwardIterator filter_code_points(ForwardIterator first, ForwardIterator last)
	{
		static_assert(sizeof(decltype(*first)) == sizeof(char32_t));

		using namespace wordring::whatwg;

		ForwardIterator out = first;
		while (first != last)
		{
			char32_t cp = *first++;
			switch (cp)
			{
			case U'\x0D':
				if (first != last && *first == U'\x0A') break;
			case U'\x0C':
				*out++ = U'\x0A';
				break;
			case U'\x00':
				*out++ = U'\xFFFD';
				break;
			default:
				if (is_surrogate(cp)) cp = U'\xFFFD';
				*out++ = cp;
			}
		}

		return out;
	}
}
