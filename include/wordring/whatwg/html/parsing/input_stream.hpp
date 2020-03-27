#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/infra/infra.hpp>
#include <wordring/whatwg/html/parsing/parser_defs.hpp>

#include <cassert>
#include <deque>
#include <limits>
#include <string>

namespace wordring::whatwg::html::parsing
{
	/*! @brief HTML5 パーサー用のユニコード・コード・ポイント入力ストリーム
	
	@par コールバック

	- on_report_error(error err)
	- on_emit_code_point()
	*/
	template <typename T>
	class input_stream
	{
	protected:
		using this_type = T;

		using value_type = char32_t;

		static std::uint32_t constexpr null_insertion_point = std::numeric_limits<std::uint32_t>::max();

	protected:
		std::deque<value_type> m_c;

		value_type    m_current_input_character;
		//std::uint32_t m_insertion_point;
		bool m_eof;
		//encoding_confidence m_confidence;
		bool m_cr_state;

	public:

		/*! @brief 空の入力ストリームを構築する
		*/
		input_stream()
			: m_current_input_character()
			//, m_insertion_point(null_insertion_point)
			, m_eof(false)
			, m_cr_state(false)
		{
		}

		virtual ~input_stream() = default;

		/*! @brief エラー報告する
		
		@param [in] ecエラー番号

		この関数は、利用者が用意する on_report_error(error ec) コールバック・メンバ関数を呼び出す。
		*/
		void report_error(error ec)
		{
			static_cast<this_type*>(this)->on_report_error(ec);
		}

		/*! @brief 文字を発送する
		
		@param [in] cp コード・ポイント

		この関数は、tokenizerが用意する on_emit_code_point() コールバック・メンバ関数を呼び出す。

		文字が消費されるかどうかにかかわらず、バッファに文字が追加された回数、コールバックが呼び出される。
		実装において、（スクリプトをサポートしないので）EOFは文字ではないが、eof(bool)の呼び出しも、コールバックが呼び出される。
		*/
		void emit_code_point(value_type cp)
		{
			m_c.push_back(cp);
			static_cast<this_type*>(this)->on_emit_code_point();
		}

		/*! @brief コード・ポイントを末尾に追加する
		
		@param [in] cp コード・ポイント
		*/
		void push_code_point(value_type cp)
		{
			if (is_surrogate(cp)) report_error(error::surrogate_in_input_stream);
			if (is_noncharacter(cp)) report_error(error::noncharacter_in_input_stream);
			if (!(is_ascii_white_space(cp) || cp == 0) && is_control(cp)) report_error(error::control_character_in_input_stream);

			// 改行文字正規化
			if (m_cr_state)
			{
				if (cp == U'\r') emit_code_point(U'\n');
				else if (cp == U'\n')
				{
					emit_code_point(U'\n');
					m_cr_state = false;
				}
				else
				{
					emit_code_point(U'\n');
					emit_code_point(cp);
					m_cr_state = false;
				}
			}
			else
			{
				if (cp == U'\r') m_cr_state = true;
				else emit_code_point(cp);
			}
		}

		/*! @brief 現在の入力文字を返す
		
		@return 現在の入力文字
		
		何も消費していない場合、現在の入力文字は不定。
		*/
		value_type current_input_character() const
		{
			return m_current_input_character;
		}

		/*! @brief 次の入力文字を返す
		
		@return 次の入力文字

		バッファに文字が無い場合、次の入力文字は不定。
		eof() が <b>false</b> を返す場合、次の入力文字は存在する。
		*/
		value_type next_input_character() const
		{
			assert(!m_c.empty());
			return m_c.front();
		}

		/*! @brief 次の入力文字を消費する
		
		消費した文字が現在の入力文字となる。
		*/
		value_type consume()
		{
			m_current_input_character = m_c.front();
			m_c.pop_front();
			return m_current_input_character;
		}

		/*! @brief 現在の入力文字を再消費する
		*/
		void reconsume()
		{
			m_c.push_front(m_current_input_character);
		}

		/*! @brief ストリーム終端に達しているか調べる

		@return ストリーム終端に達している場合 <b>true</b> 、その他の場合 <b>false</b> を返す。
		*/
		bool eof() const { return m_c.empty() && m_eof; }

		/*! @brief ストリーム終端を設定する

		@param b [in] 常に <b>true</b> を設定する。
		*/
		void eof(bool b)
		{
			assert(b);
			m_eof = b;

			this_type* p = static_cast<this_type*>(this);
			if (m_cr_state)
			{
				m_c.push_back(U'\n');
				p->on_emit_code_point();
			}
			p->on_emit_code_point();
		}

	};
}
