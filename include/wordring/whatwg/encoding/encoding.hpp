#pragma once

// https://encoding.spec.whatwg.org/
// https://triple-underscore.github.io/Encoding-ja.html

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <deque>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>

#include <boost/container/static_vector.hpp>
#include <boost/algorithm/string.hpp>

//#include <wordring/string/numeric.hpp>
//#include <wordring/whatwg/encoding/encoding_atom.hpp>
//#include <wordring/whatwg/infra.hpp>

namespace wordring
{
namespace whatwg
{
namespace encoding
{
	// 3. Terminology ---------------------------------------------------------

	using stream    = std::deque<char>;
	using u32stream = std::deque<char32_t>;

	// 4.1. Encoders and decoders ---------------------------------------------

	/*!
	@brief	デコーダ。

	charをコード・ポイント（char32_t）に復号する。
	*/
	class decoder
	{
	public:
		enum class error_mode_enum : uint32_t
		{
			replacement = 1,
			fatal       = 2,
		};

		/*!
		@brief	result_typeに含まれる結果コード。
		*/
		enum class result_code_enum : uint32_t
		{
			finished,
			error,
			continue_,
		};

		/*!
		@brief	decoderのハンドラが返す結果。
		*/
		struct result_type
		{
			/*!
			@brief	結果コード。
			*/
			result_code_enum code_;

			/*!
			@brief	正常な結果の場合、トークン列。
			*/
			boost::container::static_vector<char32_t, 2> tokens_;

			/*!
			@brief	エラー時に返される場合があるコードポイント。
			*/
			char32_t cp_;

			result_type() = default;

			result_type(result_code_enum rc)
				: code_{ rc }
				, tokens_{}
			{
			}

			result_type(char32_t cp)
				: tokens_{ cp }
			{
			}

			bool operator ==(result_code_enum rc) const
			{
				return code_ == rc;
			}

			bool operator !=(result_code_enum rc) const
			{
				return code_ != rc;
			}
		};

	public:
		virtual result_type handler(stream input, char token, bool eos) = 0;
	};

	/*!
	@brief	エンコーダ。

	コード・ポイント（char32_t）をcharに符号化する。
	*/
	class encoder
	{
	public:
		enum class error_mode_enum : uint32_t
		{
			fatal = 1,
			html  = 2,
		};

		/*!
		@brief	result_typeに含まれる結果コード。
		*/
		enum class result_code_enum : uint32_t
		{
			finished,
			error,
			continue_,
		};

		struct result_type
		{
			/*!
			@brief	結果コード。
			*/
			result_code_enum code_;

			/*!
			@brief	正常な結果の場合、トークン列。
			*/
			boost::container::static_vector<char, 4> tokens_;

			/*!
			@brief	エラー時に返される場合があるコードポイント。
			*/
			char32_t cp_;

			result_type() = default;

			result_type(result_code_enum rc)
				: code_{ rc }
			{
			}

			result_type(char32_t cp)
				: tokens_{ static_cast<char>(static_cast<unsigned char>(cp)) }
			{
				assert(cp <= 0xFF);
			}

			template <typename Iterator>
			result_type(Iterator first, Iterator last)
				: tokens_{ first, last }
			{
			}

			bool operator ==(result_code_enum rc) const
			{
				return code_ == rc;
			}

			bool operator !=(result_code_enum rc) const
			{
				return code_ != rc;
			}
		};

	public:
		virtual result_type handler(u32stream input, char32_t token, bool eos) = 0;
	};

	/*!
	@brief	トークンを復号処理する。

	- エラー・モードはreplacementあるいはfatalのいずれか。
	*/
	inline decoder::result_type process_token(
		decoder   & decoder,
		char        token,
		bool        eos,
		stream    & input,
		u32stream & output,
		decoder::error_mode_enum mode = decoder::error_mode_enum::replacement)
	{
		using em = decoder::error_mode_enum;
		using rc = decoder::result_code_enum;

		auto result         = decoder.handler(input, token, eos);
		auto const & tokens = result.tokens_;

		if (result == rc::continue_ || result == rc::finished) return result;
		else if (!tokens.empty()) std::copy(tokens.begin(), tokens.end(), std::back_inserter(output));
		else if (result == rc::error)
		{
			switch (mode)
			{
			case em::replacement:
				output.push_back(0xFFFD);
				break;
			case em::fatal:
				return decoder::result_type{ rc::error };
			}
		}

		return decoder::result_type{ rc::continue_ };
	}

	/*!
	@brief	streamを復号処理する。

	- エラー・モードはreplacementあるいはfatalのいずれか。
	*/
	inline decoder::result_type run_encoding(
		decoder   & dc,
		stream    & input,
		u32stream & output,
		decoder::error_mode_enum mode = decoder::error_mode_enum::replacement)
	{
		// 3.
		while (true)
		{
			bool eos = input.begin() == input.end();
			char token = eos ? 0 : *input.begin();
			if (!eos) input.pop_front();

			auto result = process_token(dc, token, eos, input, output, mode);
			if (result != decoder::result_code_enum::continue_) return result;
		}

		assert(false);
		return decoder::result_type{};
	}

	/*!
	@brief	トークンを符号化処理する。

	- エラー・モードはhtmlあるいはfatalのいずれか。
	*/
	inline encoder::result_type process_token(
		encoder   & ec,
		char32_t    token,
		bool        eos,
		u32stream & input,
		stream    & output,
		encoder::error_mode_enum mode = encoder::error_mode_enum::fatal)
	{
		using em = encoder::error_mode_enum;
		using rc = encoder::result_code_enum;

		auto result = ec.handler(input, token, eos);
		auto const & tokens = result.tokens_;

		if (result == rc::continue_ || result == rc::finished) return result;
		else if (!tokens.empty()) std::copy(tokens.begin(), tokens.end(), std::back_inserter(output));
		else if (result == rc::error)
		{
			switch (mode)
			{
			case em::html:
				for (auto c : result.tokens_)
				{
					// 例）&#1234;
					auto tmp = wordring::string::to_string<10>(c);
					auto it = input.insert(input.begin(), { 0x26, 0x23 });
					it = input.insert(it, tmp.begin(), tmp.end());
					input.insert(it, 0x3B);
				}
				break;
			case em::fatal:
				return encoder::result_type{ rc::error };
			}
		}

		return encoder::result_type{ rc::continue_ };
	}

	/*!
	@brief	streamを符号化処理する。

	- エラー・モードはhtmlあるいはfatalのいずれか。
	*/
	inline encoder::result_type run_encoding(
		encoder   & encoder,
		u32stream & input,
		stream    & output,
		encoder::error_mode_enum mode = encoder::error_mode_enum::fatal)
	{
		// 3.
		while (true)
		{
			bool eos = input.begin() == input.end();
			char32_t token = eos ? 0 : *input.begin();
			if (!eos) input.pop_front();

			auto result = process_token(encoder, token, eos, input, output, mode);
			if (result != encoder::result_code_enum::continue_) return result;
		}

		assert(false);
		return encoder::result_type{};
	}

	std::unique_ptr<encoding> make_encoding(encoding_atom atom);

	inline std::unique_ptr<encoding> make_encoding(std::string_view label)
	{
		return make_encoding(encoding_atom{ label });
	}

	inline std::unique_ptr<decoder> make_decoder(encoding_atom atom)
	{
		return make_encoding(atom)->get_decoder();
	}

	inline std::unique_ptr<encoder> make_encoder(encoding_atom atom)
	{
		return make_encoding(atom)->get_encoder();
	}

	// 6. Specification hooks -------------------------------------------------

	inline u32stream decode(stream & input, encoding_atom enc)
	{
		// 1.
		stream buffer{};
		// 2.
		bool bom_seen_flag{ false };
		// 3.
		for (size_t i = 0; i < 3; i++)
		{
			if (input.empty()) break;
			buffer.push_back(input.front());
			input.pop_front();
		}
		// 4.
		if (boost::algorithm::starts_with(buffer, "\xEF\xBB\xBF"))
		{
			enc = encoding_atom{ encoding_enum::utf_8 };
			bom_seen_flag = true;
		}
		else if (boost::algorithm::starts_with(buffer, "\xFE\xFF"))
		{
			enc = encoding_atom{ encoding_enum::utf_16be };
			bom_seen_flag = true;
		}
		else if (boost::algorithm::starts_with(buffer, "\xFF\xFE"))
		{
			enc = encoding_atom{ encoding_enum::utf_16le };
			bom_seen_flag = true;
		}
		// 5.
		if (bom_seen_flag) input.insert(input.begin(), buffer.begin(), buffer.end());
		// 6.
		else if (enc != encoding_enum::utf_8 && buffer.size() == 3)
			input.push_front(buffer.back());
		// 7.
		u32stream output{};
		// 8.
		std::unique_ptr<decoder> dc = make_decoder(enc);
		run_encoding(*dc, input, output);
		// 9.
		return output;
	}

	inline u32stream utf_8_decode(stream & input)
	{
		// 1.
		stream buffer{};
		// 2.
		for (size_t i = 0; i < 3; i++)
		{
			if (input.empty()) break;
			buffer.push_back(input.front());
			input.pop_front();
		}
		// 3.
		if (!boost::starts_with(buffer, "\xEF\xBB\xBF"))
			input.insert(input.begin(), buffer.begin(), buffer.end());
		// 4.
		u32stream output{};
		// 5.
		std::unique_ptr<decoder> dc = make_decoder(encoding_enum::utf_8);
		run_encoding(*dc, input, output);
		// 6.
		return output;
	}

	inline u32stream utf_8_decode_without_bom(stream & input)
	{
		// 1.
		u32stream output{};
		// 2.
		std::unique_ptr<decoder> dc = make_decoder(encoding_enum::utf_8);
		run_encoding(*dc, input, output);
		// 3.
		return output;
	}

	inline u32stream utf_8_decode_without_bom_or_fail(stream & input)
	{
		// 1.
		u32stream output{};
		// 2.
		std::unique_ptr<decoder> dc = make_decoder(encoding_enum::utf_8);
		auto potential_error = run_encoding(*dc, input, output, decoder::error_mode_enum::fatal);
		// 3.
		// TODO.
		if (potential_error == decoder::result_code_enum::error)
			throw std::runtime_error{ "utf_8_decode_without_bom_or_fail" };
		// 4.
		return output;
	}

	inline stream encode(u32stream & input, encoding_atom enc)
	{
		// 1.
		// TODO.
		assert(enc == encoding_enum::replacement || enc == encoding_enum::utf_16be || enc == encoding_enum::utf_16le);
		// 2.
		stream output{};
		// 3.
		std::unique_ptr<encoder> ec = make_encoder(enc);
		run_encoding(*ec, input, output, encoder::error_mode_enum::html);
		// 4.
		return output;
	}

	inline stream utf_8_encode(u32stream & input)
	{
		return encode(input, encoding_enum::utf_8);
	}
}
}
}
