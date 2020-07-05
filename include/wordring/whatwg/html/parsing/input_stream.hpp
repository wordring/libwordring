#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/parser_defs.hpp>

#include <wordring/whatwg/infra/infra.hpp>
#include <wordring/whatwg/encoding/encoding_defs.hpp>

#include <algorithm>
#include <cassert>
#include <deque>
#include <limits>
#include <string>
#include <string_view>
#include <utility>

namespace wordring::whatwg::html::parsing
{
	/*! @brief HTML5 パーサー用のユニコード・コード・ポイント入力ストリーム
	
	@par コールバック

	派生クラスは以下のメンバを持たなければならない。

	- on_report_error(error_name e)
	- on_emit_code_point()
	*/
	template <typename T>
	class input_stream
	{
	protected:
		using this_type      = T;
		using value_type     = char32_t;
		using container      = std::deque<value_type>;
		using const_iterator = container::const_iterator;

		static std::uint32_t constexpr null_insertion_point = std::numeric_limits<std::uint32_t>::max();

	protected:
		/*! コード・ポイント・バッファ
		*/
		container  m_c;
		value_type m_current_input_character;

		/*! バッファ埋め待ち検出用状態変数
		*/
		std::uint32_t m_fill_length;

		/*! EOF 検出用状態変数
		*/
		bool m_eof;
		bool m_eof_consumed;

		/*! 改行文字正規化用状態変数
		*/
		bool m_cr_state;

	public:

		/*! @brief 空の入力ストリームを構築する
		*/
		input_stream()
			: m_current_input_character()
			, m_fill_length(0)
			, m_eof(false)
			, m_eof_consumed(false)
			, m_cr_state(false)
		{
		}

		virtual ~input_stream() = default;

		/*! @brief 初期状態に戻し、再利用可能とする
		*/
		void clear()
		{
			m_c.clear();
			m_current_input_character = 0;

			m_fill_length = 0;

			m_eof          = false;
			m_eof_consumed = false;

			m_cr_state = false;
		}

		/*! @brief エラー報告する

		@param [in] ecエラー番号

		この関数は、利用者が用意する on_report_error(error ec) コールバック・メンバ関数を呼び出す。
		*/
		void report_error(error_name ec = static_cast<error_name>(0))
		{
			this_type* P = static_cast<this_type*>(this);
			P->on_report_error(ec);
		}

		/*! @brief コード・ポイントを末尾に追加する

		@param [in] cp コード・ポイント
		*/
		void push_code_point(value_type cp)
		{
			if (is_surrogate(cp)) report_error(error_name::surrogate_in_input_stream);
			if (is_noncharacter(cp)) report_error(error_name::noncharacter_in_input_stream);
			if (!(is_ascii_white_space(cp) || cp == 0) && is_control(cp)) report_error(error_name::control_character_in_input_stream);

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

		/*! @brief ストリーム終端を設定する

		@todo 複数回呼び出された場合の対処
		*/
		void push_eof()
		{
			this_type* P = static_cast<this_type*>(this);

			m_eof = true;

			if (m_cr_state)
			{
				m_c.push_back(U'\n');
				P->on_emit_code_point();
			}

			P->on_emit_code_point();
		}

		/*! @brief コード・ポイントを発送する
		
		@param [in] cp コード・ポイント

		この関数は、tokenizerが用意する on_emit_code_point() コールバック・メンバ関数を呼び出す。

		文字が消費されるかどうかにかかわらず、バッファに文字が追加された回数、コールバックが呼び出される。
		実装において、（スクリプトをサポートしないので）EOFは文字ではないが、push_eof()の呼び出しも、コールバックが呼び出される。
		*/
		void emit_code_point(value_type cp)
		{
			m_c.push_back(cp);
			static_cast<this_type*>(this)->on_emit_code_point();
		}

		/*! @brief ストリーム・バッファ内のコード・ポイントをすべて発送する

		複数文字を待つトークン化の状態がある。中途でマッチに失敗した場合、いくつかのコードポイントがバッファに残る。
		マッチに失敗した時点で、このメンバを実行する。

		このメンバは規格にない。プッシュ型パーサの特性を持たせるため新設した。
		*/
		void flush_code_point()
		{
			this_type* P = static_cast<this_type*>(this);

			assert(!m_eof_consumed); // ?
			if (m_eof_consumed) return;

			while (!m_c.empty())
			{
				auto i = m_c.size();
				P->on_emit_code_point();
				if (i == m_c.size()) return;
			}

			if(m_eof && !m_eof_consumed) P->on_emit_code_point();
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
		*/
		value_type next_input_character() const
		{
			assert(!m_c.empty());
			return m_c.front();
		}

		/*! @brief ストリーム終端に達しているか調べる

		@return ストリーム終端に達している場合 <b>true</b> 、その他の場合 <b>false</b> を返す。
		*/
		bool eof() const { return m_eof_consumed; }

		/*! @brief バッファに指定文字数貯まっているか調べる
		
		@param [in] 文字数

		@return 指定文字数バッファに貯まるかEOFに達した場合 true 、それ以外の場合 false を返す。
		*/
		bool fill(std::uint32_t n)
		{
			m_fill_length += (n - m_c.size());
			return m_eof || m_c.size() == n;
		}

		/*! @brief 次の入力文字を消費する

		@return 現在の入力文字

		消費した文字が現在の入力文字となる。
		*/
		value_type consume()
		{
			if (m_c.empty() && m_eof)
			{
				m_eof_consumed = true;
				return 0;
			}

			if (m_fill_length) --m_fill_length;

			m_current_input_character = m_c.front();
			m_c.pop_front();
			return m_current_input_character;
		}

		/*! @brief バッファの文字を <b>n</b> 個消費する

		トークン化段階で、いくつかの状態は文字列を待ち受ける。
		その間、バッファに文字が溜まっていく。
		もしも期待する文字列が得られたなら、このメンバを呼び出し消費する。

		*/
		void consume(std::uint32_t n)
		{
			for (std::uint32_t i = 0; i < n; ++i) consume();
		}

		/*! @brief 現在の入力文字を再消費する
		*/
		void reconsume()
		{
			if (m_eof_consumed) m_eof_consumed = false;
			else m_c.push_front(m_current_input_character);
		}

		const_iterator begin() const { return m_c.begin(); }

		const_iterator end() const { return m_c.end(); }


		/*! @brief 与えられた文字列とストリーム・バッファ内の文字列を比較する
		
		@param [in] label 文字列
		@param [in] case_insensitive 大文字小文字を無視して比較する場合 true
		
		@return
			完全に一致する場合 succeed　、
			途中まで一致するがバッファの文字列が足りない場合 partial 、
			一致に失敗した場合 failed を返す。

			大文字小文字を無視して比較する場合、 <b>label</b> を小文字で指定すること。
		*/
		bool match(std::u32string_view label, bool with_current, bool case_insensitive)
		{
			auto it1 = label.begin();
			auto it2 = label.end();

			if (with_current)
			{
				char32_t cp = current_input_character();
				if (case_insensitive && is_ascii_upper_alpha(cp)) cp += 0x20;
				if (cp != *it1++) return false;
			}

			auto it3 = m_c.begin();
			auto it4 = m_c.end();

			while (it1 != it2)
			{
				if (it3 == it4) return false;

				char32_t cp = *it3++;
				if (case_insensitive && is_ascii_upper_alpha(cp)) cp += 0x20;

				if(cp != *it1++) return false;
			}

			return true;
		}

		/*! @brief 名前付き文字参照とストリーム・バッファ内の文字列を比較する

		@param [out] len マッチした文字数を返す

		@return 0 ～ 2 個の文字参照コードを返す。

		最長一致を試みる。

		失敗するまで検索し、失敗する前に一致が有ればそれを返す動作となる。
		したがってどの場合でも、バッファにコード・ポイントが残る。
		*/
		std::array<char32_t, 2> match_named_character_reference(std::uint32_t& len)
		{
			len = 0;

			auto it1 = named_character_reference_idx_tbl.begin();
			auto it2 = named_character_reference_idx_tbl.end();

			auto tail = it1;
			std::uint32_t j = 0;

			for (std::uint32_t i = 0; it1 != it2 && i < m_c.size(); ++i)
			{
				it1 = it1[m_c[i]];
				if (it1)
				{
					tail = it1;
					j = i + 1;
				}
			}
			std::u32string s;
			tail.string(s);

			while (tail != named_character_reference_idx_tbl.begin())
			{
				if (tail)
				{
					len = j;
					break;
				}
				tail = tail.parent();
			}

			if (len != 0)
			{
				std::uint32_t idx = named_character_reference_idx_tbl.at(tail);
				std::array<char32_t, 2> a = named_character_reference_map_tbl[idx];
				return a;
			}

			return std::array<char32_t, 2>();
		}

		/*! @brief 名前付き文字参照のコード・ポイントを取得する

		@param [in] idx match_named_character_reference() から返される<b>マップの索引</b>

		@return コード・ポイントの配列

		名前付き文字参照のコード・ポイントは、1個あるいは2個である。
		1個のコード・ポイントを返す場合、二個目を <b>0</b> とすることで示す。
		*/
		std::array<char32_t, 2> named_character_reference(std::uint32_t idx)
		{
			return named_character_reference_map_tbl[idx];
		}
	};
}
