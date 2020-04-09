#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/infra/infra.hpp>
#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/parser_defs.hpp>

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

	- on_report_error(error err)
	- on_emit_code_point()
	*/
	template <typename T>
	class input_stream
	{
	protected:
		using this_type = T;
		using value_type = char32_t;
		using container = std::deque<value_type>;
		using const_iterator = container::const_iterator;

		static std::uint32_t constexpr null_insertion_point = std::numeric_limits<std::uint32_t>::max();

		enum class match_result : std::uint32_t
		{
			failed = 0, partial = 1, succeed = 2,
		};

		struct match_state
		{
			std::uint32_t m_offset;
			std::uint32_t m_last_match;

			match_state()
				: m_offset(0)
				, m_last_match(0)
			{
			}

			void clear()
			{
				m_offset = 0;
				m_last_match = 0;
			}
		};

	protected:
		container m_c;

		value_type    m_current_input_character;

		/*! EOF 検出用状態変数
		*/
		bool m_eof;

		/*! 改行文字正規化用状態変数
		*/
		bool m_cr_state;

		/*! 名前付き文字参照用状態変数

		match_named_character_reference() から使用される。
		*/
		match_state m_match_state;

	public:

		/*! @brief 空の入力ストリームを構築する
		*/
		input_stream()
			: m_current_input_character()
			, m_eof(false)
			, m_cr_state(false)
			, m_match_state()
		{
		}

		virtual ~input_stream() = default;

		/*! @brief エラー報告する

		@param [in] ecエラー番号

		この関数は、利用者が用意する on_report_error(error ec) コールバック・メンバ関数を呼び出す。
		*/
		void report_error(error_name ec = static_cast<error_name>(0))
		{
			static_cast<this_type*>(this)->on_report_error(ec);
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

		/*! @brief コード・ポイントを発送する
		
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

		/*! @brief ストリーム・バッファ内のコード・ポイントをすべて発送する
		*/
		void flush_code_point()
		{
			for(std::uint32_t i = 0; i < m_c.size(); ++i) static_cast<this_type*>(this)->on_emit_code_point();
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
		match_result match(std::u32string_view label, bool case_insensitive)
		{
			if (m_c.size() < label.size())
			{
				if (m_eof) return match_result::failed;

				auto it = label.begin();
				for (char32_t cp : m_c)
				{
					char32_t ch = *it++;
					if (case_insensitive && is_ascii_upper_alpha(cp)) cp += 0x20;

					if (cp != ch) return match_result::failed;
				}

				return match_result::partial;
			}
			else if (label.size() <= m_c.size())
			{
				auto it = m_c.begin();
				for (char32_t ch : label)
				{
					char32_t cp = *it++;
					if (case_insensitive && is_ascii_upper_alpha(cp)) cp += 0x20;

					if (cp != ch) return match_result::failed;
				}

				return match_result::succeed;
			}

			return match_result::failed;
		}

		/*! @brief 名前付き文字参照とストリーム・バッファ内の文字列を比較する

		@param [out] idx named_character_reference() に渡す idx 値を返す
		@param [out] len マッチした文字数を返す

		@return
			完全に一致する場合 succeed　、
			途中まで一致するがバッファの文字列が足りない場合 partial 、
			一致に失敗した場合 failed を返す。

		最長一致を試みる。

		失敗するまで検索し、失敗する前に一致が有ればそれを返す動作となる。
		したがってどの場合でも、バッファにコード・ポイントが残る。

		最後に一致した場所を保持する状態変数として m_ncr_state を使う。
		*/
		match_result match_named_character_reference(std::uint32_t& idx, std::uint32_t& len)
		{
			if (!m_eof && m_match_state.m_offset != m_c.size())
			{
				assert(m_match_state.m_offset != m_c.size());

				++m_match_state.m_offset;
				auto r = named_character_reference_idx_tbl.lookup(m_c.begin(), m_c.end());
				len = std::distance(m_c.begin(), r.second);

				if (r.first)
				{
					if (len != m_match_state.m_last_match)
					{
						m_match_state.m_last_match = len;
						return match_result::partial;
					}
				}
				else if(len == m_c.size()) return match_result::partial;
			}

			if (m_match_state.m_last_match != 0)
			{
				auto r = named_character_reference_idx_tbl.lookup(m_c.begin(), m_c.end());
				idx = named_character_reference_idx_tbl.at(r.first);
				len = m_match_state.m_last_match;
				m_match_state.clear();

				return match_result::succeed;
			}

			m_match_state.clear();

			return match_result::failed;
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

		/*! @brief 次の入力文字を消費する
		
		@return 現在の入力文字

		消費した文字が現在の入力文字となる。
		*/
		value_type consume()
		{
			assert(!m_c.empty());

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
			m_c.push_front(m_current_input_character);
		}

		/*! @brief ストリーム終端に達しているか調べる

		@return ストリーム終端に達している場合 <b>true</b> 、その他の場合 <b>false</b> を返す。
		*/
		bool eof() const { return m_c.empty() && m_eof; }

		/*! @brief ストリーム終端を設定する

		@param b [in] 常に <b>true</b> を設定する。

		@todo 複数回呼び出された場合の対処
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
