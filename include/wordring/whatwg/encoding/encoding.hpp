#pragma once

// ------------------------------------------------------------------------------------------------
// 4. Encodings
//
// https://encoding.spec.whatwg.org/#encodings
// https://triple-underscore.github.io/Encoding-ja.html#encodings
// ------------------------------------------------------------------------------------------------

#include <array>
#include <cassert>
#include <charconv>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <variant>

#include <wordring/whatwg/encoding/coder.hpp>
#include <wordring/whatwg/encoding/encoding_defs.hpp>
#include <wordring/whatwg/encoding/terminology.hpp>

namespace wordring::whatwg::encoding
{
	// --------------------------------------------------------------------------------------------
	// 4.1. Encoders and decoders
	//
	// https://encoding.spec.whatwg.org/#encoders-and-decoders
	// https://triple-underscore.github.io/Encoding-ja.html#encoders-and-decoders
	// --------------------------------------------------------------------------------------------

	template <typename IoItem, typename Coder, typename InQueue, typename OutQueue>
	result_value process_item(IoItem, Coder&, InQueue&, OutQueue&, error_mode_name);
	
	/*! @brief キューを処理する
	* 
	* @param [in]  c    エンコーダ、あるいはデコーダ
	* @param [in]  in   入力キュー
	* @param [out] out  出力イテレータ
	* @param [in]  mode エラーモード
	* 
	* @sa https://encoding.spec.whatwg.org/#concept-encoding-run
	* @sa https://triple-underscore.github.io/Encoding-ja.html#concept-encoding-run
	*/
	template <typename Coder, typename InQueue, typename OutQueue>
	inline result_value process_queue(Coder& c, InQueue& in, OutQueue& out, error_mode_name mode)
	{
		// 1.
		while (true)
		{
			auto item = in.read();
			if (item.m_wait) return result_continue{};
			auto result = process_item(item.m_value, c, in, out, mode);
			if (result.index() != 1) return result;
		}
		assert(false);
		return result_error{};
	}

	/*! @brief 文字アイテムを処理する
	*/
	template <typename IoItem, typename Coder, typename InQueue, typename OutQueue>
	inline result_value process_item(
		IoItem item, Coder& c, InQueue& in, OutQueue& out, error_mode_name mode)
	{
		// 1.
		if constexpr (std::is_base_of_v<encoder, Coder>) assert(mode != error_mode_name::Replacement);
		// 2.
		if constexpr (std::is_base_of_v<decoder, Coder>) assert(mode != error_mode_name::Html);
		// 3.
		if constexpr (std::is_base_of_v<encoder, Coder>) assert(!is_surrogate(*item));
		// 4.
		result_value result = std::visit([&](auto& x) { return x.run(in, item); }, c);
		auto idx = result.index();
		// 5.
		if (idx == 0) // result_finished
		{
			out.push({ 0, true });
			return result;
		}
		// 6.
		else if(idx != 8)
		{
			switch (idx)
			{
			case 2: // result_code_point
				out.push(*std::get_if<2>(&result));
				break;
			case 3: // result_code_points_2
			{
				auto* a = std::get_if<3>(&result);
				out.push(a->operator[](0));
				out.push(a->operator[](1));
				break;
			}
			case 4: // result_byte
				out.push(*std::get_if<4>(&result));
				break;
			case 5: // result_bytes_2
			{
				auto* a = std::get_if<5>(&result);
				out.push(a->operator[](0));
				out.push(a->operator[](1));
				break;
			}
			case 6: // result_bytes_3
			{
				auto* a = std::get_if<6>(&result);
				out.push(a->operator[](0));
				out.push(a->operator[](1));
				out.push(a->operator[](2));
				break;
			}
			case 7: // result_bytes_4
			{
				auto* a = std::get_if<7>(&result);
				out.push(a->operator[](0));
				out.push(a->operator[](1));
				out.push(a->operator[](2));
				out.push(a->operator[](3));
				break;
			}
			}
		}
		else if (idx == 8) // result_error
		{
			switch (mode)
			{
			case error_mode_name::Replacement:
				// Warning 抑制用の if()
				if constexpr (std::is_base_of_v<decoder, Coder>) out.push(U'\xFFFD');
				break;
			case error_mode_name::Html:
			{
				out.push('\x26');
				out.push('\x23');
				std::array<char, 10> a;
				auto [p, ec] = std::to_chars(a.data(), a.data() + a.size(), std::get_if<8>(&result)->code_point);
				out.push(a.data(), p);
				out.push('\x3B');
				break;
			}
			case error_mode_name::Fatal:
				return result;
			default:
				break;
			}
		}
		return result_continue{};
	}

	// --------------------------------------------------------------------------------------------
	// 4.2. Names and labels
	//
	// https://encoding.spec.whatwg.org/#names-and-labels
	// https://triple-underscore.github.io/Encoding-ja.html#names-and-labels
	// --------------------------------------------------------------------------------------------

	encoding_name get_encoding_name(std::u32string label);

	template <typename String, typename std::enable_if_t<
		std::negation_v<std::is_same<String, std::u32string>>, nullptr_t> = nullptr>
	inline encoding_name get_encoding_name(String label)
	{
		return get_encoding_name(encoding_cast<std::u32string>(label));
	}

	/*! @brief エンコーディングに対応するラベルを返す
	* 
	* @sa https://encoding.spec.whatwg.org/#dom-textdecoder-encoding
	*/
	inline std::u32string get_encoding_label(encoding_name name)
	{
		switch (name)
		{
		case encoding_name::UTF_8:          return U"utf-8";
		case encoding_name::IBM866:         return U"ibm866";
		case encoding_name::ISO_8859_2:     return U"iso-8859-2";
		case encoding_name::ISO_8859_3:     return U"iso-8859-3";
		case encoding_name::ISO_8859_4:     return U"iso-8859-4";
		case encoding_name::ISO_8859_5:     return U"iso-8859-5";
		case encoding_name::ISO_8859_6:     return U"iso-8859-6";
		case encoding_name::ISO_8859_7:     return U"iso-8859-7";
		case encoding_name::ISO_8859_8:     return U"iso-8859-8";
		case encoding_name::ISO_8859_8_I:   return U"iso-8859-8-i";
		case encoding_name::ISO_8859_10:    return U"iso-8859-10";
		case encoding_name::ISO_8859_13:    return U"iso-8859-13";
		case encoding_name::ISO_8859_14:    return U"iso-8859-14";
		case encoding_name::ISO_8859_15:    return U"iso-8859-14";
		case encoding_name::ISO_8859_16:    return U"iso-8859-16";
		case encoding_name::KOI8_R:         return U"koi8-r";
		case encoding_name::KOI8_U:         return U"koi8-u";
		case encoding_name::macintosh:      return U"macintosh";
		case encoding_name::windows_874:    return U"windows-874";
		case encoding_name::windows_1250:   return U"windows-1250";
		case encoding_name::windows_1251:   return U"windows-1251";
		case encoding_name::windows_1252:   return U"windows-1252";
		case encoding_name::windows_1253:   return U"windows-1253";
		case encoding_name::windows_1254:   return U"windows-1254";
		case encoding_name::windows_1255:   return U"windows-1255";
		case encoding_name::windows_1256:   return U"windows-1256";
		case encoding_name::windows_1257:   return U"windows-1257";
		case encoding_name::windows_1258:   return U"windows-1258";
		case encoding_name::x_mac_cyrillic: return U"x-mac-cyrillic";
		case encoding_name::GBK:            return U"gbk";
		case encoding_name::gb18030:        return U"gb18030";
		case encoding_name::Big5:           return U"big5";
		case encoding_name::EUC_JP:         return U"euc-jp";
		case encoding_name::ISO_2022_JP:    return U"iso-2022-jp";
		case encoding_name::Shift_JIS:      return U"shift_jis";
		case encoding_name::EUC_KR:         return U"euc-kr";
		case encoding_name::replacement:    return U"replacement";
		case encoding_name::UTF_16BE:       return U"utf-16be";
		case encoding_name::UTF_16LE:       return U"utf-16le";
		case encoding_name::x_user_defined: return U"x-user-defined";
		default:
			break;
		}
		assert(false);
		return U"";
	}

	/*! brief エンコーディング名に対応するデコーダを返す
	*/
	inline coder get_encoding_decoder(encoding_name name)
	{
		switch (name)
		{
		case encoding_name::UTF_8:         return { UTF_8_decoder{} };
		// Legacy single - byte encodings
		case encoding_name::IBM866:        return { IBM866_decoder{} };
		case encoding_name::ISO_8859_2:    return { ISO_8859_2_decoder{} };
		case encoding_name::ISO_8859_3:    return { ISO_8859_3_decoder{} };
		case encoding_name::ISO_8859_4:    return { ISO_8859_4_decoder{} };
		case encoding_name::ISO_8859_5:    return { ISO_8859_5_decoder{} };
		case encoding_name::ISO_8859_6:    return { ISO_8859_6_decoder{} };
		case encoding_name::ISO_8859_7:    return { ISO_8859_7_decoder{} };
		case encoding_name::ISO_8859_8:    return { ISO_8859_8_decoder{} };
		case encoding_name::ISO_8859_8_I:  return { ISO_8859_8_I_decoder{} };
		case encoding_name::ISO_8859_10:   return { ISO_8859_10_decoder{} };
		case encoding_name::ISO_8859_13:   return { ISO_8859_13_decoder{} };
		case encoding_name::ISO_8859_14:   return { ISO_8859_14_decoder{} };
		case encoding_name::ISO_8859_15:   return { ISO_8859_15_decoder{} };
		case encoding_name::ISO_8859_16:   return { ISO_8859_16_decoder{} };
		case encoding_name::KOI8_R:        return { KOI8_R_decoder{} };
		case encoding_name::KOI8_U:        return { KOI8_U_decoder{} };
		case encoding_name::macintosh:     return { macintosh_decoder{} };
		case encoding_name::windows_874:   return { windows_874_decoder{} };
		case encoding_name::windows_1250:  return { windows_1250_decoder{} };
		case encoding_name::windows_1251:  return { windows_1251_decoder{} };
		case encoding_name::windows_1252:  return { windows_1252_decoder{} };
		case encoding_name::windows_1253:  return { windows_1253_decoder{} };
		case encoding_name::windows_1254:  return { windows_1254_decoder{} };
		case encoding_name::windows_1255:  return { windows_1255_decoder{} };
		case encoding_name::windows_1256:  return { windows_1256_decoder{} };
		case encoding_name::windows_1257:  return { windows_1257_decoder{} };
		case encoding_name::windows_1258:  return { windows_1258_decoder{} };
		case encoding_name::x_mac_cyrillic:return { x_mac_cyrillic_decoder{} };
		// Legacy multi - byte Chinese(simplified) encodings
		case encoding_name::GBK:           return { GBK_decoder{} };
		case encoding_name::gb18030:       return { gb18030_decoder{} };
		// Legacy multi - byte Chinese(traditional) encodings
		case encoding_name::Big5:          return { Big5_decoder{} };
		// Legacy multi - byte Japanese encodings
		case encoding_name::EUC_JP:        return { EUC_JP_decoder{} };
		case encoding_name::ISO_2022_JP:   return { ISO_2022_JP_decoder{} };
		case encoding_name::Shift_JIS:     return { Shift_JIS_decoder{} };
		// Legacy multi - byte Korean encodings
		case encoding_name::EUC_KR:        return { EUC_KR_decoder{} };
		// Legacy miscellaneous encodings
		case encoding_name::replacement:   return { replacement_decoder{} };
		case encoding_name::UTF_16BE:      return { UTF_16BE_decoder{} };
		case encoding_name::UTF_16LE:      return { UTF_16LE_decoder{} };
		case encoding_name::x_user_defined:return { x_user_defined_decoder{} };
		default:
			break;
		}
		assert(false);
		return { error_decoder{} };
	}

	/*! brief エンコーディング名に対応するエンコーダを返す
	*/
	inline coder get_encoding_encoder(encoding_name name)
	{
		switch (name)
		{
		case encoding_name::UTF_8:         return { UTF_8_encoder{} };
		// Legacy single - byte encodings
		case encoding_name::IBM866:        return { IBM866_encoder{} };
		case encoding_name::ISO_8859_2:    return { ISO_8859_2_encoder{} };
		case encoding_name::ISO_8859_3:    return { ISO_8859_3_encoder{} };
		case encoding_name::ISO_8859_4:    return { ISO_8859_4_encoder{} };
		case encoding_name::ISO_8859_5:    return { ISO_8859_5_encoder{} };
		case encoding_name::ISO_8859_6:    return { ISO_8859_6_encoder{} };
		case encoding_name::ISO_8859_7:    return { ISO_8859_7_encoder{} };
		case encoding_name::ISO_8859_8:    return { ISO_8859_8_encoder{} };
		case encoding_name::ISO_8859_8_I:  return { ISO_8859_8_I_encoder{} };
		case encoding_name::ISO_8859_10:   return { ISO_8859_10_encoder{} };
		case encoding_name::ISO_8859_13:   return { ISO_8859_13_encoder{} };
		case encoding_name::ISO_8859_14:   return { ISO_8859_14_encoder{} };
		case encoding_name::ISO_8859_15:   return { ISO_8859_15_encoder{} };
		case encoding_name::ISO_8859_16:   return { ISO_8859_16_encoder{} };
		case encoding_name::KOI8_R:        return { KOI8_R_encoder{} };
		case encoding_name::KOI8_U:        return { KOI8_U_encoder{} };
		case encoding_name::macintosh:     return { macintosh_encoder{} };
		case encoding_name::windows_874:   return { windows_874_encoder{} };
		case encoding_name::windows_1250:  return { windows_1250_encoder{} };
		case encoding_name::windows_1251:  return { windows_1251_encoder{} };
		case encoding_name::windows_1252:  return { windows_1252_encoder{} };
		case encoding_name::windows_1253:  return { windows_1253_encoder{} };
		case encoding_name::windows_1254:  return { windows_1254_encoder{} };
		case encoding_name::windows_1255:  return { windows_1255_encoder{} };
		case encoding_name::windows_1256:  return { windows_1256_encoder{} };
		case encoding_name::windows_1257:  return { windows_1257_encoder{} };
		case encoding_name::windows_1258:  return { windows_1258_encoder{} };
		case encoding_name::x_mac_cyrillic:return { x_mac_cyrillic_encoder{} };
		// Legacy multi - byte Chinese(simplified) encodings
		case encoding_name::GBK:           return { GBK_encoder{} };
		case encoding_name::gb18030:       return { gb18030_encoder{} };
		// Legacy multi - byte Chinese(traditional) encodings
		case encoding_name::Big5:          return { Big5_encoder{} };
		// Legacy multi - byte Japanese encodings
		case encoding_name::EUC_JP:        return { EUC_JP_encoder{} };
		case encoding_name::ISO_2022_JP:   return { ISO_2022_JP_encoder{} };
		case encoding_name::Shift_JIS:     return { Shift_JIS_encoder{} };
		// Legacy multi - byte Korean encodings
		case encoding_name::EUC_KR:        return { EUC_KR_encoder{} };
		// Legacy miscellaneous encodings
		case encoding_name::x_user_defined:return { x_user_defined_encoder{} };
		default:
			break;
		}
		assert(false);
		return { error_encoder{} };
	}

	/*! @brief ラベルからエンコーディングを取得する
	*
	* @sa https://encoding.spec.whatwg.org/#concept-encoding-get
	* @sa https://triple-underscore.github.io/Encoding-ja.html#concept-encoding-get
	*/

	// --------------------------------------------------------------------------------------------
	// 4.3. Output encodings
	//
	// https://encoding.spec.whatwg.org/#output-encodings
	// https://triple-underscore.github.io/Encoding-ja.html#output-encodings
	// --------------------------------------------------------------------------------------------

	/*! @brief 出力エンコーディングを取得する
	* 
	* @sa https://triple-underscore.github.io/Encoding-ja.html#get-an-output-encoding
	*/
	inline encoding_name get_output_encoding_name(encoding_name name)
	{
		switch (name)
		{
		case encoding_name::replacement:
		case encoding_name::UTF_16BE:
		case encoding_name::UTF_16LE:
			return encoding_name::UTF_8;
		default:
			break;
		}
		return name;
	}

	// --------------------------------------------------------------------------------------------
	// 他の標準用のフック
	//
	// 6. Hooks for standards
	// https://html.spec.whatwg.org/multipage/parsing.html#the-insertion-mode
	// --------------------------------------------------------------------------------------------

	template <typename InQueue>
	io_result<encoding_name> bom_sniff(InQueue& in);

	template <typename InQueue, typename OutQueue>
	result_value encode(InQueue& in, encoding_name name, OutQueue& out);

	/*! @brief UTF8 をデコードする
	*
	* @return 完了、継続あるいはエラーを示す
	* 
	* - 入力が足りない場合、 result_continue
	* - デコードが完了した場合、 result_finished
	* - エラーの場合、 result_error
	* を返す。
	* 
	* @sa https://encoding.spec.whatwg.org/#utf-8-decode
	* @sa https://triple-underscore.github.io/Encoding-ja.html#decode
	* 
	* @internal
	* <hr>
	* 
	* EOQ によってデコードの完了を表現できるか？
	*/
	template <typename InQueue, typename OutQueue>
	inline result_value utf8_decode(InQueue& in, OutQueue& out)
	{
		// 1.
		auto buffer = in.peek(3);
		if (buffer.m_wait) return { result_continue{} };
		// 2.
		if (3 <= buffer->size()
			&& *(buffer->operator[](0)) == '\xEF'
			&& *(buffer->operator[](1)) == '\xBB'
			&& *(buffer->operator[](2)) == '\xBF')
		{
			in.read(3);
		}
		// 3.
		auto c = get_encoding_decoder(encoding_name::UTF_8);
		result_value rv = process_queue(c, in, out, error_mode_name::Replacement);
		// 4.
		return rv;
	}

	/*! @brief UTF8 をデコードする
	*
	* @sa inline void utf8_decode(InQueue& in, OutQueue& out)
	* 
	* 待機状態をどう伝えるか？
	*/
	template <typename OutQueue, typename InQueue>
	inline OutQueue utf8_decode(InQueue& in)
	{
		assert(false);

		OutQueue out;
		utf8_decode(in, out);
		return out;
	}

	/*! @brief BOM に関知せず UTF8 をデコードする
	* 
	* @sa https://triple-underscore.github.io/Encoding-ja.html#utf-8-decode-without-bom
	*/
	template <typename InQueue, typename OutQueue>
	inline result_value utf8_decode_without_bom(InQueue& in, OutQueue& out)
	{
		// 1.
		auto c = get_encoding_decoder(encoding_name::UTF_8);
		result_value rv = process_queue(c, in, out, error_mode_name::Replacement);
		// 2.
		return rv;
	}

	/*! @brief BOM や失敗に関知せず UTF8 をデコードする
	*
	* @sa https://encoding.spec.whatwg.org/#utf-8-decode-without-bom-or-fail
	*/
	template <typename InQueue, typename OutQueue>
	inline result_value utf8_decode_without_bom_or_fail(InQueue& in, OutQueue& out)
	{
		// 1.
		auto c = get_encoding_decoder(encoding_name::UTF_8);
		auto potentialError = process_queue(c, in, out, error_mode_name::Fatal);
		// 2.
		// 3.
		return potentialError;
	}

	/*! @brief UTF-8 へエンコードする
	* 
	* @param [in]  in char32_t 入力キュー
	* @param [out] in char 出力キュー
	* 
	* @sa https://encoding.spec.whatwg.org/#utf-8-encode
	*/
	template <typename InQueue, typename OutQueue>
	inline result_value utf8_encode(InQueue& in, OutQueue& out)
	{
		static_assert(sizeof(typename InQueue::character_type) == 4);
		static_assert(sizeof(typename OutQueue::character_type) == 1);

		return encode(in, encoding_name::UTF_8, out);
	}

	// --------------------------------------------------------------------------------------------
	// 6.1. Legacy hooks for standards
	//
	// https://encoding.spec.whatwg.org/#legacy-hooks
	// https://triple-underscore.github.io/Encoding-ja.html#legacy-hooks
	// --------------------------------------------------------------------------------------------

	coder get_encoder(encoding_name name);

	/*! @brief デコード
	* 
	* @sa https://triple-underscore.github.io/Encoding-ja.html#decode
	* 
	* @internal
	* <hr>
	* 
	* 規格の戻り値は、I/O Queue となっているが、改変している。
	*/
	template <typename InQueue, typename OutQueue>
	inline result_value decode(InQueue& in, encoding_name name, OutQueue& out)
	{
		// 1.
		io_result<encoding_name> BOMEncoding = bom_sniff(in);
		if (BOMEncoding.m_wait) return { result_continue{} };
		// 2.
		if (BOMEncoding.m_value != static_cast<encoding_name>(0))
		{
			name = BOMEncoding.m_value;
			if (*BOMEncoding == encoding_name::UTF_8) in.read(3);
			else in.read(2);
		}
		// 3.
		auto c = get_encoding_decoder(name);
		result_value rv = process_queue(c, in, out, error_mode_name::Replacement);
		// 4.
		return rv;
	}

	/*! @brief BOM を探知する

	@sa https://encoding.spec.whatwg.org/#bom-sniff
	*/
	template <typename InQueue>
	inline io_result<encoding_name> bom_sniff(InQueue& in)
	{
		using value_type = typename InQueue::value_type;
		// 1.
		io_result<std::vector<value_type>> BOM = in.peek(3);
		if (BOM.m_wait) return { static_cast<encoding_name>(0), true };
		// 2.
		std::array<char, 3> a = { 0, 0, 0 };
		for (std::uint32_t i = 0, n = std::min(static_cast<std::uint32_t>(3u), static_cast<std::uint32_t>(BOM->size())); i < n; ++i)
		{
			a[i] = BOM->at(i).m_value;
		}
		if (a[0] == '\xEF' && a[1] == '\xBB' && a[2] == '\xBF') return { encoding_name::UTF_8, false };
		if (a[0] == '\xFE' && a[1] == '\xFF') return { encoding_name::UTF_16BE, false };
		if (a[0] == '\xFF' && a[1] == '\xFE') return { encoding_name::UTF_16LE, false };
		// 3.
		return { static_cast<encoding_name>(0), false };
	}

	/*! @brief エンコード
	* 
	* @sa https://triple-underscore.github.io/Encoding-ja.html#encode
	*/
	template <typename InQueue, typename OutQueue>
	inline result_value encode(InQueue& in, encoding_name name, OutQueue& out)
	{
		coder c = get_encoder(name);
		result_value rv = process_queue(c, in, out, error_mode_name::Html);
		return rv;
	}

	/*! @brief エンコーダを取得する
	* 
	* @sa https://encoding.spec.whatwg.org/#get-an-encoder
	*/
	inline coder get_encoder(encoding_name name)
	{
		assert(name != encoding_name::replacement && name != encoding_name::UTF_16BE && name != encoding_name::UTF_16LE);

		return get_encoding_encoder(name);
	}

	/*! @brief エンコードするか失敗する
	*
	* @sa https://triple-underscore.github.io/Encoding-ja.html#encode-or-fail
	*/
	template <typename InQueue, typename OutQueue>
	inline result_value encode_or_fail(InQueue& in, coder c, OutQueue& out)
	{
		using value_type = typename OutQueue::value_type;
		// 1.
		result_value potentialError = process_queue(c, in, out, error_mode_name::Fatal);
		// 2.
		value_type eoq;
		eoq.m_eoq = true;
		out.push(eoq);
		// 3.
		if (std::holds_alternative<result_error>(potentialError)) return potentialError;
		// 4.
		return result_finished{};
	}
}

namespace wordring::whatwg
{
	template <typename String>
	inline encoding_name get_encoding_name(String label)
	{
		return encoding::get_encoding_name(label);
	}
}
