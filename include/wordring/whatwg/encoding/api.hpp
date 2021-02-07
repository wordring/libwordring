#pragma once

// ------------------------------------------------------------------------------------------------
// 7. API
//
// https://encoding.spec.whatwg.org/#api
// https://triple-underscore.github.io/Encoding-ja.html#api
// ------------------------------------------------------------------------------------------------

#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/encoding_defs.hpp>

#include <wordring/whatwg/infra/infra.hpp>
#include <wordring/whatwg/infra/unicode.hpp>

#include <stdexcept>
#include <string>

namespace wordring::whatwg::encoding
{
	class text_decoder_common;

	std::u32string serialize_io_queue(text_decoder_common& c, io_queue<char32_t>& in);

	/*! @brief TextDecoderCommon
	* 
	* @sa https://encoding.spec.whatwg.org/#interface-mixin-textdecodercommon
	*/
	class text_decoder_common
	{
		friend std::u32string serialize_io_queue(text_decoder_common&, io_queue<char32_t>&);

	public:
		text_decoder_common() {}
		/*! @brief エンコーディング名
		*
		* @return 小文字のエンコーディング名
		* 
		* @sa https://encoding.spec.whatwg.org/#dom-textdecoder-encoding
		*/
		std::u32string encoding() const
		{
			return get_encoding_label(m_encoding);
		}

		/*! @brief Fatal フラグ
		* 
		* @sa https://encoding.spec.whatwg.org/#dom-textdecoder-fatal
		*/
		bool fatal() const { return m_error_mode == error_mode_name::Fatal; }

		/*! @brief BOM 無視フラグ
		* 
		* @sa https://encoding.spec.whatwg.org/#dom-textdecoder-ignorebom
		*/
		bool ignore_bom() const { return m_ignore_bom; }

	protected:
		encoding_name   m_encoding   = static_cast<encoding_name>(0);
		coder           m_decoder;
		io_queue<char>  m_io_queue;
		bool            m_ignore_bom = false;
		bool            m_bom_seen   = false;
		error_mode_name m_error_mode = error_mode_name::Replacement;
	};

	/*! @brief IO キューを直列化する
	* 
	* @sa https://encoding.spec.whatwg.org/#concept-td-serialize
	*/
	inline std::u32string serialize_io_queue(text_decoder_common& c, io_queue<char32_t>& in)
	{
		// 1.
		std::u32string output;
		// 2.
		while (true)
		{
			io_result<io_item<char32_t>> item = in.read();
			assert(!item.m_wait);
			if (item->m_eoq) return output;
			if (   (c.m_encoding == encoding_name::UTF_8 || c.m_encoding == encoding_name::UTF_16BE || c.m_encoding == encoding_name::UTF_16LE)
				&& (c.m_ignore_bom == false)
				&& (c.m_bom_seen == false))
			{
				c.m_bom_seen = true;
				if (item->m_value == U'\xFEFF') continue;
			}
			output.push_back(item->m_value);
		}
		assert(false);
		return std::u32string{};
	}

	/*! @brief TextDecoderCommon
	*
	* @sa https://encoding.spec.whatwg.org/#interface-mixin-textdecodercommon
	*/
	class text_decoder : public text_decoder_common
	{
	public:
		text_decoder(encoding_name name, bool fatal, bool ignore_bom)
		{
			// 1.
			encoding_name encoding = name;
			// 2.
			if (encoding == static_cast<encoding_name>(0) || encoding == encoding_name::replacement) throw std::range_error("bad encoding label.");
			// 3.
			m_encoding = encoding;
			// 4.
			if (fatal == true) m_error_mode = error_mode_name::Fatal;
			// 5.
			m_ignore_bom = ignore_bom;
		}

		/*! @brief 構築
		* 
		* @sa https://encoding.spec.whatwg.org/#dom-textdecoder
		*/
		explicit text_decoder(std::u32string_view label = U"utf-8", bool fatal = false, bool ignore_bom = false)
		{
			// 1.
			encoding_name encoding = get_encoding_name(label);
			// 2.
			if (encoding == static_cast<encoding_name>(0) || encoding == encoding_name::replacement) throw std::range_error("bad encoding label.");
			// 3.
			m_encoding = encoding;
			// 4.
			if (fatal == true) m_error_mode = error_mode_name::Fatal;
			// 5.
			m_ignore_bom = ignore_bom;
		}

		/*! @brief デコード
		* 
		* 変換中エラーに遭遇した場合、std::range_error を送出します。
		* 
		* @sa https://triple-underscore.github.io/Encoding-ja.html#dom-textdecoder-decode
		*/
		template <typename InputIterator = std::nullptr_t>
		std::u32string decode(InputIterator first = nullptr, InputIterator last = nullptr, bool stream = false)
		{
			// 1.
			if (m_do_not_flush == false)
			{
				m_decoder = get_encoding_decoder(m_encoding);
				m_io_queue = io_queue<char>{};
				io_item<char> eoq{ '\0', true };
				m_io_queue.push(eoq);
				m_bom_seen = false;
			}
			// 2.
			m_do_not_flush = stream;
			// 3.
			if constexpr (std::negation_v<std::is_same<InputIterator, std::nullptr_t>>)
			{
				m_io_queue.push(first, last);
			}
			// 4.
			io_queue<char32_t> output;
			io_item<char32_t> eoq{ U'\0', true };
			output.push(eoq);
			// 5.
			while (true)
			{
				// 5.1.
				io_result<io_item<char>> item = m_io_queue.read();
				// 5.2.
				if (item->m_eoq && m_do_not_flush) return serialize_io_queue(*this, output);
				// 5.3.
				else
				{
					result_value rv = process_item(*item, m_decoder, m_io_queue, output, m_error_mode);
					if (std::holds_alternative<result_finished>(rv)) return serialize_io_queue(*this, output);
					if (std::holds_alternative<result_error>(rv)) throw std::range_error("decode error.");
				}
			}
			return U"";
		}

	protected:
		bool m_do_not_flush = false;
	};
}
