#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/input_stream.hpp>
#include <wordring/whatwg/html/parsing/open_element_stack.hpp>
#include <wordring/whatwg/html/parsing/parser_defs.hpp>
#include <wordring/whatwg/html/parsing/token.hpp>

#include <wordring/whatwg/infra/infra.hpp>

#include <wordring/string/matcher.hpp>

#include <type_traits>

namespace wordring::whatwg::html::parsing
{

	template <typename T, typename Policy>
	class tokenizer : public input_stream<T>
	{
		friend input_stream<T>;

	protected:
		using base_type = input_stream<T>;
		using this_type = T;

		using policy = Policy;

		using state_type = void(tokenizer::*)();

		using typename base_type::match_result;

		using node_pointer = typename policy::node_pointer;

		using base_type::flush_code_point;
		using base_type::fill;
		using base_type::current_input_character;
		using base_type::next_input_character;
		using base_type::begin;
		using base_type::end;
		using base_type::match_named_character_reference;
		using base_type::named_character_reference;
		using base_type::consume;
		using base_type::match;

		using base_type::report_error;
		using base_type::eof;

	public:
		state_type m_state;
		state_type m_return_state;
		std::u32string m_temporary_buffer;

		// トークン -----------------------------------------------------------

		DOCTYPE_token     m_DOCTYPE_token;     // 1
		start_tag_token   m_start_tag_token;   // 2
		end_tag_token     m_end_tag_token;     // 3
		comment_token     m_comment_token;     // 4
		character_token   m_character_token;   // 5
		end_of_file_token m_end_of_file_token; // 6

		/*! @brief 現在のタグ・トークンを識別する

		開始タグの場合 <b>2</b> 、終了タグの場合、 <b>3</b> 。
		*/
		std::uint32_t m_current_tag_token_id;

		std::u32string m_last_start_tag_name;

		char32_t m_character_reference_code;

		//

	public:
		tokenizer()
			: m_state(data_state)
			, m_return_state(nullptr)
			, m_current_tag_token_id(0)
			, m_character_reference_code(0)
		{
		}

		// トークン -----------------------------------------------------------

		tag_token& create_start_tag_token()
		{
			m_current_tag_token_id = 2;
			m_start_tag_token.clear();
			return m_start_tag_token;
		}

		tag_token& create_end_tag_token()
		{
			m_current_tag_token_id = 3;
			m_end_tag_token.clear();
			return m_start_tag_token;
		}

		void create_comment_token(char32_t const* data = U"")
		{
			m_comment_token.m_data = data;
		}

		DOCTYPE_token& create_DOCTYPE_token()
		{
			m_DOCTYPE_token.clear();
			return m_DOCTYPE_token;
		}

		/*! @brief 現在のタグ・トークンを返す
		*/
		tag_token& current_tag_token()
		{
			if (m_current_tag_token_id == 2) return m_start_tag_token;
			if (m_current_tag_token_id == 3) return m_end_tag_token;

			assert(false);
			return m_start_tag_token;
		}

		bool is_appropriate_end_tag_token(tag_token const& token)
		{
			assert(m_current_tag_token_id == 3);

			if (m_last_start_tag_name.empty() || (m_last_start_tag_name != token.m_tag_name)) return false;
			return true;
		}

		/*! @brief 現在のタグ・トークン上で新しい属性を開始する
		*/
		token_attribute& create_attribute()
		{
			return current_tag_token().m_attributes.create();
		}

		/*! @brief 現在の属性を返す
		*/
		token_attribute& current_attribute()
		{
			return current_tag_token().m_attributes.current();
		}

		comment_token& current_comment_token()
		{
			return m_comment_token;
		}

		DOCTYPE_token& current_DOCTYPE_token()
		{
			return m_DOCTYPE_token;
		}


		// ----------------------------------------------------------------------------------------
		// スタック
		//
		// 12.2.4.2 The stack of open elements
		// https://html.spec.whatwg.org/multipage/parsing.html#the-stack-of-open-elements
		// ----------------------------------------------------------------------------------------

		/*! @brief カレント・ノードがHTML名前空間に属するか調べる
		*/
		bool in_html_namespace() const
		{
			this_type const* P = static_cast<this_type const*>(this);
			return P->namespace_uri_name(P->adjusted_current_node().m_it) == ns_name::HTML;
		}

		/*! @brief 属性の重複を削る
		
		12.2.5.33 Attribute name state 
		*/
		void unify_attribute()
		{
			token_attribute_list& al = current_tag_token().m_attributes;
			auto it1 = al.begin();
			auto it2 = std::prev(al.end(), 1);
			while (it1 != it2) if (it1++->m_name == al.current().m_name) al.current().m_omitted = true;
		}
		
		/*
		bool empty_stack_of_open_elements() const
		{
			return m_open_element_stack.empty();
		}
		*/
		// トークンの発送-------------------------------------------------------

		template <typename Token>
		void emit_token(Token& token)
		{
			this_type* p = static_cast<this_type*>(this);

			if constexpr (std::is_base_of_v<tag_token, Token>)
			{
				assert(m_current_tag_token_id == 2 || m_current_tag_token_id == 3);

				tag_token& t = static_cast<tag_token&>(token);
				auto tag_it = tag_atom_tbl.find(t.m_tag_name);
				t.m_tag_name_id = (tag_it == tag_atom_tbl.end()) ? static_cast<tag_name>(0) : tag_it->second;

				auto it1 = t.begin();
				auto it2 = t.end();
				while (it1 != it2)
				{
					auto attr_it = attribute_atom_tbl.find(it1->m_name);
					it1->m_name_id = (attr_it == attribute_atom_tbl.end()) ? static_cast<attribute_name>(0) : attr_it->second;
				}

				if (m_current_tag_token_id == 2)
				{
					m_last_start_tag_name = m_start_tag_token.m_tag_name;
					p->on_emit_token(m_start_tag_token);
				}
				else if (m_current_tag_token_id == 3) p->on_emit_token(m_end_tag_token);
			}
			else static_cast<this_type*>(this)->on_emit_token(token);
		}

		void emit_token(char32_t cp)
		{
			m_character_token.m_data = cp;
			static_cast<this_type*>(this)->on_emit_token(m_character_token);
		}

		void emit_token(end_of_file_token)
		{
			static_cast<this_type*>(this)->on_emit_token(m_end_of_file_token);
		}

		// 状態の変更 ----------------------------------------------------------

		void change_state(state_type st) { m_state = st; }

		void return_state(state_type st) { m_return_state = st; }

		state_type return_state() const { return m_return_state; }

		// 文字の再消費 --------------------------------------------------------

		void reconsume(state_type st)
		{
			change_state(st);
			base_type::reconsume();
			on_emit_code_point();
		}

		// 
		bool consumed_as_part_of_attribute()
		{
			if (m_return_state == attribute_value_double_quoted_state
				|| m_return_state == attribute_value_single_quoted_state
				|| m_return_state == attribute_value_unquoted_state) return true;

			return false;
		}

		void flush_code_points_consumed_as_character_reference()
		{
			if (consumed_as_part_of_attribute()) for (char32_t cp : m_temporary_buffer) current_attribute().m_value.push_back(cp);
			else for (char32_t cp : m_temporary_buffer) emit_token(cp);
		}

		// コールバック --------------------------------------------------------

		void on_emit_code_point()
		{
			(this->*m_state)();
		}

		// 状態関数 -----------------------------------------------------------

		/*! 12.2.5.1 Data state */
		void on_data_state()
		{
			if (eof())
			{
				emit_token(end_of_file_token());
				return;
			}
			
			char32_t cp = consume();
			switch (cp)
			{
			case U'&':
				return_state(data_state);
				change_state(character_reference_state);
				break;
			case U'<':
				change_state(tag_open_state);
				break;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				emit_token(cp);
				break;
			default:
				emit_token(cp);
				break;
			}
		}

		/*! 12.2.5.2 RCDATA state */
		void on_RCDATA_state()
		{
			if (eof())
			{
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();
			switch (cp)
			{
			case U'&':
				return_state(RCDATA_state);
				change_state(character_reference_state);
				break;
			case U'<':
				change_state(RCDATA_less_than_sign_state);
				break;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				emit_token(U'\xFFFD');
				break;
			default:
				emit_token(cp);
				break;
			}
		}

		/*! 12.2.5.3 RAWTEXT state */
		void on_RAWTEXT_state()
		{
			if (eof())
			{
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();
			switch (cp)
			{
			case U'<':
				change_state(RAWTEXT_less_than_sign_state);
				break;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				emit_token(U'\xFFFD');
				break;
			default:
				emit_token(cp);
				break;
			}
		}

		/*! 12.2.5.4 Script data state */
		void on_script_data_state()
		{
			if (eof())
			{
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();
			switch (cp)
			{
			case U'<':
				change_state(script_data_less_than_sign_state);
				break;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				emit_token(U'\xFFFD');
				break;
			default:
				emit_token(cp);
				break;
			}
		}

		/*! 12.2.5.5 PLAINTEXT state */
		void on_PLAINTEXT_state()
		{
			if (eof())
			{
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			if (cp == U'\x0')
			{
				report_error(error_name::unexpected_null_character);
				emit_token(U'\xFFFD');
				return;
			}

			emit_token(cp);
		}

		/*! 12.2.5.6 Tag open state */
		void on_tag_open_state()
		{
			if (eof())
			{
				report_error(error_name::eof_before_tag_name);
				emit_token(U'<');
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();
			switch (cp)
			{
			case U'!':
				change_state(markup_declaration_open_state);
				return;
			case U'/':
				change_state(end_tag_open_state);
				return;
			case U'?':
				report_error(error_name::unexpected_question_mark_instead_of_tag_name);
				create_comment_token();
				reconsume(bogus_comment_state);
				return;
			}

			if (is_ascii_alpha(cp))
			{
				create_start_tag_token();
				reconsume(tag_name_state);
				return;
			}

			report_error(error_name::invalid_first_character_of_tag_name);
			emit_token(U'<');
			reconsume(data_state);
		}

		/*! 12.2.5.7 End tag open state */
		void on_end_tag_open_state()
		{
			if (eof())
			{
				report_error(error_name::eof_before_tag_name);
				emit_token(U'<');
				emit_token(U'/');
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			if (is_ascii_alpha(cp))
			{
				create_end_tag_token();
				reconsume(tag_name_state);
				return;
			}

			if (cp == U'>')
			{
				report_error(error_name::missing_end_tag_name);
				change_state(data_state);
				return;
			}

			report_error(error_name::invalid_first_character_of_tag_name);
			create_comment_token();
			reconsume(bogus_comment_state);
		}

		/*! 12.2.5.8 Tag name state */
		void on_tag_name_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_tag);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				change_state(before_attribute_name_state);
				return;
			case U'/':
				change_state(self_closing_start_tag_state);
				return;
			case U'>':
				change_state(data_state);
				emit_token(current_tag_token());
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				current_tag_token().m_tag_name.push_back(U'\xFFFD');
				return;
			}

			if (is_ascii_upper_alpha(cp))
			{
				current_tag_token().m_tag_name.push_back(cp + 0x20);
				return;
			}

			current_tag_token().m_tag_name.push_back(cp);	
		}

		/*! 12.2.5.9 RCDATA less-than sign state */
		void on_RCDATA_less_than_sign_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				if (cp == U'/')
				{
					m_temporary_buffer.clear();
					change_state(RCDATA_end_tag_open_state);
					return;
				}
			}

			emit_token(U'<');
			reconsume(RCDATA_state);
		}

		/*! 12.2.5.10 RCDATA end tag open state */
		void on_RCDATA_end_tag_open_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				if (is_ascii_alpha(cp))
				{
					create_end_tag_token();
					reconsume(RCDATA_end_tag_name_state);
					return;
				}
			}

			emit_token(U'<');
			emit_token(U'/');
			reconsume(RCDATA_state);
		}

		/*! 12.2.5.11 RCDATA end tag name state */
		void on_RCDATA_end_tag_name_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				switch (cp)
				{
				case U'\x9':  // TAB
				case U'\xA':  // LF
				case U'\xC':  // FF
				case U'\x20': // SPACE
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(before_attribute_name_state);
					return;
				case U'/':
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(self_closing_start_tag_state);
					return;
				case U'>':
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(data_state);
					emit_token(current_tag_token());
					return;
				}

				if (is_ascii_upper_alpha(cp))
				{
					current_tag_token().m_tag_name.push_back(cp + 0x20);
					m_temporary_buffer.push_back(cp);
					return;
				}

				if (is_ascii_lower_alpha(cp))
				{
					current_tag_token().m_tag_name.push_back(cp);
					m_temporary_buffer.push_back(cp);
					return;
				}
			}

		AnythingElse:
			emit_token(U'<');
			emit_token(U'/');
			for (char32_t c : m_temporary_buffer) emit_token(c);
			reconsume(RCDATA_state);
		}

		/*! 12.2.5.12 RAWTEXT less-than sign state */
		void on_RAWTEXT_less_than_sign_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				if (cp == U'/')
				{
					m_temporary_buffer.clear();
					change_state(RAWTEXT_end_tag_open_state);
					return;
				}
			}

			emit_token(U'<');
			reconsume(RAWTEXT_state);
		}

		/*! 12.2.5.13 RAWTEXT end tag open state */
		void on_RAWTEXT_end_tag_open_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				if (is_ascii_alpha(cp))
				{
					create_end_tag_token();
					reconsume(RAWTEXT_end_tag_name_state);
					return;
				}
			}

			emit_token(U'<');
			emit_token(U'/');
			reconsume(RAWTEXT_state);
		}

		/*! 12.2.5.14 RAWTEXT end tag name state */
		void on_RAWTEXT_end_tag_name_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				switch (cp)
				{
				case U'\x9':  // TAB
				case U'\xA':  // LF
				case U'\xC':  // FF
				case U'\x20': // SPACE
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(before_attribute_name_state);
					return;
				case U'/':
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(self_closing_start_tag_state);
					return;
				case U'>':
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(data_state);
					emit_token(current_tag_token());
					return;
				}

				if (is_ascii_upper_alpha(cp))
				{
					current_tag_token().m_tag_name.push_back(cp + 0x20);
					m_temporary_buffer.push_back(cp);
					return;
				}

				if (is_ascii_lower_alpha(cp))
				{
					current_tag_token().m_tag_name.push_back(cp);
					m_temporary_buffer.push_back(cp);
					return;
				}
			}

		AnythingElse:
			emit_token(U'<');
			emit_token(U'/');
			for (char32_t c : m_temporary_buffer) emit_token(c);
			reconsume(RAWTEXT_state);
		}

		/*! 12.2.5.15 Script data less-than sign state */
		void on_script_data_less_than_sign_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				switch (cp)
				{
				case U'/':
					m_temporary_buffer.clear();
					change_state(script_data_end_tag_open_state);
					return;
				case U'!':
					change_state(script_data_escape_start_state);
					emit_token(U'<');
					emit_token(U'!');
					return;
				}
			}

			emit_token(U'<');
			reconsume(script_data_state);
		}

		/*! 12.2.5.16 Script data end tag open state */
		void on_script_data_end_tag_open_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				if (is_ascii_alpha(cp))
				{
					create_end_tag_token();
					reconsume(script_data_end_tag_name_state);
					return;
				}
			}

			emit_token(U'<');
			emit_token(U'/');
			reconsume(script_data_state);
		}

		/*! 12.2.5.17 Script data end tag name state */
		void on_script_data_end_tag_name_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				switch (cp)
				{
				case U'\x9':  // TAB
				case U'\xA':  // LF
				case U'\xC':  // FF
				case U'\x20': // SPACE
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(before_attribute_name_state);
					return;
				case U'/':
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(self_closing_start_tag_state);
					return;
				case U'>':
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(data_state);
					emit_token(current_tag_token());
					return;
				}

				if (is_ascii_upper_alpha(cp))
				{
					current_tag_token().m_tag_name.push_back(cp + 0x20);
					m_temporary_buffer.push_back(cp);
					return;
				}

				if (is_ascii_lower_alpha(cp))
				{
					current_tag_token().m_tag_name.push_back(cp);
					m_temporary_buffer.push_back(cp);
					return;
				}
			}

		AnythingElse:
			emit_token(U'<');
			emit_token(U'/');
			for (char32_t c : m_temporary_buffer) emit_token(c);
			reconsume(script_data_state);
		}

		/*! 12.2.5.18 Script data escape start state */
		void on_script_data_escape_start_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				if (cp == U'-')
				{
					change_state(script_data_escape_start_dash_state);
					emit_token(U'-');
					return;
				}
			}

			reconsume(script_data_state);
		}

		/*! 12.2.5.19 Script data escape start dash state */
		void on_script_data_escape_start_dash_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				if (cp == U'-')
				{
					change_state(script_data_escaped_dash_dash_state);
					emit_token(U'-');
					return;
				}
			}

			reconsume(script_data_state);
		}

		/*! 12.2.5.20 Script data escaped state */
		void on_script_data_escaped_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_script_html_comment_like_text);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();
			switch (cp)
			{
			case U'-':
				change_state(script_data_escaped_dash_state);
				emit_token(U'-');
				break;
			case U'<':
				change_state(script_data_escaped_less_than_sign_state);
				break;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				emit_token(U'\xFFFD');
				break;
			default:
				emit_token(cp);
				break;
			}
		}

		/*! 12.2.5.21 Script data escaped dash state */
		void on_script_data_escaped_dash_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_script_html_comment_like_text);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();
			switch (cp)
			{
			case U'-':
				change_state(script_data_escaped_dash_dash_state);
				emit_token(U'-');
				break;
			case U'<':
				change_state(script_data_escaped_less_than_sign_state);
				break;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				change_state(script_data_escaped_state);
				emit_token(U'\xFFFD');
				break;
			default:
				change_state(script_data_escaped_state);
				emit_token(cp);
				break;
			}
		}

		/*! 12.2.5.22 Script data escaped dash dash state */
		void on_script_data_escaped_dash_dash_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_script_html_comment_like_text);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();
			switch (cp)
			{
			case U'-':
				emit_token(U'-');
				break;
			case U'<':
				change_state(script_data_escaped_less_than_sign_state);
				break;
			case U'>':
				change_state(script_data_state);
				emit_token(U'>');
				break;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				change_state(script_data_escaped_state);
				emit_token(U'\xFFFD');
				break;
			default:
				change_state(script_data_escaped_state);
				emit_token(cp);
				break;
			}
		}

		/*! 12.2.5.23 Script data escaped less-than sign state */
		void on_script_data_escaped_less_than_sign_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				if (cp == U'/')
				{
					m_temporary_buffer.clear();
					change_state(script_data_escaped_end_tag_open_state);
					return;
				}

				if (is_ascii_alpha(cp))
				{
					m_temporary_buffer.clear();
					emit_token(U'<');
					reconsume(script_data_double_escape_start_state);
					return;
				}
			}

			emit_token(U'<');
			reconsume(script_data_escaped_state);
		}

		/*! 12.2.5.24 Script data escaped end tag open state */
		void on_script_data_escaped_end_tag_open_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				if (is_ascii_alpha(cp))
				{
					create_end_tag_token();
					reconsume(script_data_escaped_end_tag_name_state);
					return;
				}
			}

			emit_token(U'<');
			emit_token(U'/');
			reconsume(script_data_escaped_state);
		}

		/*! 12.2.5.25 Script data escaped end tag name state */
		void on_script_data_escaped_end_tag_name_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				switch (cp)
				{
				case U'\x9':  // TAB
				case U'\xA':  // LF
				case U'\xC':  // FF
				case U'\x20': // SPACE
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(before_attribute_name_state);
					return;
				case U'/':
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(self_closing_start_tag_state);
					return;
				case U'>':
					if (!is_appropriate_end_tag_token(m_end_tag_token)) goto AnythingElse;
					change_state(data_state);
					emit_token(current_tag_token());
					return;
				}

				if (is_ascii_upper_alpha(cp))
				{
					current_tag_token().m_tag_name.push_back(cp + 0x20);
					m_temporary_buffer.push_back(cp);
					return;
				}

				if (is_ascii_lower_alpha(cp))
				{
					current_tag_token().m_tag_name.push_back(cp);
					m_temporary_buffer.push_back(cp);
					return;
				}
			}

		AnythingElse:
			emit_token(U'<');
			emit_token(U'/');
			for (char32_t c : m_temporary_buffer) emit_token(c);
			reconsume(script_data_escaped_state);
		}

		/*! 12.2.5.26 Script data double escape start state */
		void on_script_data_double_escape_start_state()
		{
			if (!eof())
			{
				char32_t cp = consume();
				switch (cp)
				{
				case U'\x9':  // TAB
				case U'\xA':  // LF
				case U'\xC':  // FF
				case U'\x20': // SPACE
				case U'/':
				case U'>':
					if (m_temporary_buffer == U"script") change_state(script_data_double_escaped_state);
					else change_state(before_attribute_name_state);
					emit_token(cp);
					return;
				}

				if (is_ascii_upper_alpha(cp))
				{
					m_temporary_buffer.push_back(cp + 0x20);
					emit_token(cp);
					return;
				}

				if (is_ascii_lower_alpha(cp))
				{
					m_temporary_buffer.push_back(cp);
					emit_token(cp);
					return;
				}
			}

			reconsume(script_data_escaped_state);
		}

		/*! 12.2.5.27 Script data double escaped state */
		void on_script_data_double_escaped_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_script_html_comment_like_text);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();
			switch (cp)
			{
			case U'-':
				change_state(script_data_double_escaped_dash_state);
				emit_token(U'-');
				return;
			case U'<':
				change_state(script_data_double_escaped_less_than_sign_state);
				emit_token(U'<');
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				emit_token(U'\xFFFD');
				return;
			}

			emit_token(cp);
		}

		/*! 12.2.5.28 Script data double escaped dash state */
		void on_script_data_double_escaped_dash_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_script_html_comment_like_text);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'-':
				change_state(script_data_double_escaped_dash_dash_state);
				emit_token(U'-');
				return;
			case U'<':
				change_state(script_data_double_escaped_less_than_sign_state);
				emit_token(U'<');
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				change_state(script_data_double_escaped_state);
				emit_token(U'\xFFFD');
				return;
			}

			change_state(script_data_double_escaped_state);
			emit_token(cp);
		}

		/*! 12.2.5.29 Script data double escaped dash dash state */
		void on_script_data_double_escaped_dash_dash_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_script_html_comment_like_text);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'-':
				emit_token(U'-');
				return;
			case U'<':
				change_state(script_data_double_escaped_less_than_sign_state);
				emit_token(U'<');
				return;
			case U'>':
				change_state(script_data_state);
				emit_token(U'>');
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				change_state(script_data_double_escaped_state);
				emit_token(U'\xFFFD');
				return;
			}

			change_state(script_data_double_escaped_state);
			emit_token(cp);
		}

		/*! 12.2.5.30 Script data double escaped less-than sign state */
		void on_script_data_double_escaped_less_than_sign_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				if (cp == U'/')
				{
					m_temporary_buffer.clear();
					change_state(script_data_double_escape_end_state);
					emit_token(U'/');
					return;
				}
			}

			reconsume(script_data_double_escaped_state);
		}

		/*! 12.2.5.31 Script data double escape end state */
		void on_script_data_double_escape_end_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				switch (cp)
				{
				case U'\x9':  // TAB
				case U'\xA':  // LF
				case U'\xC':  // FF
				case U'\x20': // SPACE
				case U'/':
				case U'>':
					if (m_temporary_buffer == U"script") change_state(script_data_escaped_state);
					else change_state(script_data_double_escaped_state);
					emit_token(cp);
					return;
				}

				if (is_ascii_upper_alpha(cp))
				{
					m_temporary_buffer.push_back(cp + 0x20);
					emit_token(cp);
					return;
				}

				if (is_ascii_lower_alpha(cp))
				{
					m_temporary_buffer.push_back(cp);
					emit_token(cp);
					return;
				}
			}

			reconsume(script_data_double_escaped_state);
		}

		/*! 12.2.5.32 Before attribute name state */
		void on_before_attribute_name_state()
		{
			if (eof())
			{
				reconsume(after_attribute_name_state);
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				return;
			case U'/':
			case U'>':
				reconsume(after_attribute_name_state);
				return;
			case U'=':
				report_error(error_name::unexpected_equals_sign_before_attribute_name);
				create_attribute();
				change_state(attribute_name_state);
				return;
			}

			create_attribute();
			reconsume(attribute_name_state);
		}

		/*! 12.2.5.33 Attribute name state */
		void on_attribute_name_state()
		{
			if (eof())
			{
				reconsume(after_attribute_name_state);
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
			case U'/':
			case U'>':
				unify_attribute();
				reconsume(after_attribute_name_state);
				return;
			case U'=':
				unify_attribute();
				change_state(before_attribute_value_state);
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				current_attribute().m_name.push_back(U'\xFFFD');
				return;
			case U'"':
			case U'\'':
			case U'<':
				report_error(error_name::unexpected_character_in_attribute_name);
				goto AnythingElse;
			}

			if (is_ascii_upper_alpha(cp))
			{
				current_attribute().m_name.push_back(cp + 0x20);
				return;
			}

		AnythingElse:
			current_attribute().m_name.push_back(cp);
		}

		/*! 12.2.5.34 After attribute name state */
		void on_after_attribute_name_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_tag);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				return;
			case U'/':
				change_state(self_closing_start_tag_state);
				return;
			case U'=':
				change_state(before_attribute_value_state);
				return;
			case U'>':
				change_state(data_state);
				emit_token(current_tag_token());
				return;
			}

			create_attribute();
			reconsume(attribute_name_state);
		}

		/*! 12.2.5.35 Before attribute value state */
		void on_before_attribute_value_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				switch (cp)
				{
				case U'\x9':  // TAB
				case U'\xA':  // LF
				case U'\xC':  // FF
				case U'\x20': // SPACE
					return;
				case U'"':
					change_state(attribute_value_double_quoted_state);
					return;
				case U'\'':
					change_state(attribute_value_single_quoted_state);
					return;
				case U'>':
					report_error(error_name::missing_attribute_value);
					change_state(data_state);
					emit_token(current_tag_token());
					return;
				}
			}

			reconsume(attribute_value_unquoted_state);
		}

		/*! 12.2.5.36 Attribute value (double-quoted) state */
		void on_attribute_value_double_quoted_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_tag);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'"':
				change_state(after_attribute_value_quoted_state);
				return;
			case U'&':
				return_state(attribute_value_double_quoted_state);
				change_state(character_reference_state);
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				current_attribute().m_value.push_back(U'\xFFFD');
				return;
			}

			current_attribute().m_value.push_back(cp);
		}

		/*! 12.2.5.37 Attribute value (single-quoted) state */
		void on_attribute_value_single_quoted_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_tag);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\'':
				change_state(after_attribute_value_quoted_state);
				return;
			case U'&':
				return_state(attribute_value_single_quoted_state);
				change_state(character_reference_state);
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				current_attribute().m_value.push_back(U'\xFFFD');
				return;
			}

			current_attribute().m_value.push_back(cp);
		}

		/*! 12.2.5.38 Attribute value (unquoted) state */
		void on_attribute_value_unquoted_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_tag);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				change_state(before_attribute_name_state);
				return;
			case U'&':
				return_state(attribute_value_unquoted_state);
				change_state(character_reference_state);
				return;
			case U'>':
				change_state(data_state);
				emit_token(current_tag_token());
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				current_attribute().m_value.push_back(U'\xFFFD');
				return;
			case U'"':
			case U'\'':
			case U'<':
			case U'=':
			case U'`':
				report_error(error_name::unexpected_character_in_unquoted_attribute_value);
				goto AnythingElse;
			}

		AnythingElse:
			current_attribute().m_value.push_back(cp);
		}

		/*! 12.2.5.39 After attribute value (quoted) state */
		void on_after_attribute_value_quoted_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_tag);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				change_state(before_attribute_name_state);
				return;
			case U'/':
				change_state(self_closing_start_tag_state);
				return;
			case U'>':
				change_state(data_state);
				emit_token(current_tag_token());
				return;
			}

			report_error(error_name::missing_whitespace_between_attributes);
			reconsume(before_attribute_name_state);
		}

		/*! 12.2.5.40 Self-closing start tag state */
		void on_self_closing_start_tag_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_tag);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();
			if (cp == U'>')
			{
				current_tag_token().m_self_closing_flag = true;
				change_state(data_state);
				emit_token(current_tag_token());
				return;
			}

			report_error(error_name::unexpected_solidus_in_tag);
			reconsume(before_attribute_name_state);
		}

		/*! 12.2.5.41 Bogus comment state */
		void on_bogus_comment_state()
		{
			if (eof())
			{
				emit_token(current_comment_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'>':
				change_state(data_state);
				emit_token(current_comment_token());
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				current_comment_token().m_data.push_back(U'\xFFFD');
				return;
			}

			current_comment_token().m_data.push_back(cp);
		}

		/*! 12.2.5.42 Markup declaration open state */
		void on_markup_declaration_open_state()
		{
			std::size_t constexpr n = std::max({ std::size(U"--") - 1, std::size(U"doctype") - 1, std::size(U"[CDATA[") - 1 });
			if (!fill(n)) return;

			if (match(U"--", false, false))
			{
				consume(std::size(U"--") - 1);
				create_comment_token();
				change_state(comment_start_state);
				goto Flush;
			}
			
			if (match(U"doctype", false, true))
			{
				consume(std::size(U"doctype") - 1);
				change_state(DOCTYPE_state);
				goto Flush;
			}
			
			if (match(U"[CDATA[", false, false))
			{
				consume(std::size(U"[CDATA[") - 1);

				this_type const* P = static_cast<this_type const*>(this);

				if (!P->m_open_element_stack.empty())
				{
					if (in_html_namespace())
					{
						change_state(CDATA_section_state);
						goto Flush;
					}
				}

				report_error(error_name::cdata_in_html_content);
				create_comment_token(U"[CDATA[");
				change_state(bogus_comment_state);
				goto Flush;
			}

			report_error(error_name::incorrectly_opened_comment);
			create_comment_token();
			change_state(bogus_comment_state);

		Flush:
			flush_code_point();
		}

		/*! 12.2.5.43 Comment start state */
		void on_comment_start_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				switch (cp)
				{
				case U'-':
					change_state(comment_start_dash_state);
					return;
				case U'>':
					report_error(error_name::abrupt_closing_of_empty_comment);
					change_state(data_state);
					emit_token(current_comment_token());
					return;
				}
			}

			reconsume(comment_state);
		}

		/*! 12.2.5.44 Comment start dash state */
		void on_comment_start_dash_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_comment);
				emit_token(current_comment_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'-':
				change_state(comment_end_state);
				return;
			case U'>':
				report_error(error_name::abrupt_closing_of_empty_comment);
				change_state(data_state);
				emit_token(current_comment_token());
				return;
			}

			current_comment_token().m_data.push_back(U'-');
			reconsume(comment_state);
		}

		/*! 12.2.5.45 Comment state */
		void on_comment_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_comment);
				emit_token(current_comment_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'<':
				current_comment_token().m_data.push_back(cp);
				change_state(comment_less_than_sign_state);
				return;
			case U'-':
				change_state(comment_end_dash_state);
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				emit_token(U'\xFFFD');
				return;
			}

			current_comment_token().m_data.push_back(cp);
		}

		/*! 12.2.5.46 Comment less-than sign state */
		void on_comment_less_than_sign_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				switch (cp)
				{
				case U'!':
					current_comment_token().m_data.push_back(cp);
					change_state(comment_less_than_sign_bang_state);
					return;
				case U'<':
					current_comment_token().m_data.push_back(cp);
					return;
				}
			}

			reconsume(comment_state);
		}

		/*! 12.2.5.47 Comment less-than sign bang state */
		void on_comment_less_than_sign_bang_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				if (cp == U'-')
				{
					change_state(comment_less_than_sign_bang_dash_state);
					return;
				}
			}

			reconsume(comment_state);
		}

		/*! 12.2.5.48 Comment less-than sign bang dash state */
		void on_comment_less_than_sign_bang_dash_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				if (cp == U'-')
				{
					change_state(comment_less_than_sign_bang_dash_dash_state);
					return;
				}
			}

			reconsume(comment_end_dash_state);
		}

		/*! 12.2.5.49 Comment less-than sign bang dash dash state */
		void on_comment_less_than_sign_bang_dash_dash_state()
		{
			if (eof())
			{
				reconsume(comment_end_state);
				return;
			}

			char32_t cp = consume();

			if (cp == U'>')
			{
				reconsume(comment_end_state);
				return;
			}

			report_error(error_name::nested_comment);
			reconsume(comment_end_state);
		}

		/*! 12.2.5.50 Comment end dash state */
		void on_comment_end_dash_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_comment);
				emit_token(current_comment_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			if (cp == U'-')
			{
				change_state(comment_end_state);
				return;
			}

			current_comment_token().m_data.push_back(U'-');
			reconsume(comment_state);
		}

		/*! 12.2.5.51 Comment end state */
		void on_comment_end_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_comment);
				emit_token(current_comment_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'>':
				change_state(data_state);
				emit_token(current_comment_token());
				return;
			case U'!':
				change_state(comment_end_bang_state);
				return;
			case U'-':
				current_comment_token().m_data.push_back(U'-');
				return;
			}

			current_comment_token().m_data.push_back(U'-');
			current_comment_token().m_data.push_back(U'-');
			reconsume(comment_state);
		}

		/*! 12.2.5.52 Comment end bang state */
		void on_comment_end_bang_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_comment);
				emit_token(current_comment_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'-':
				current_comment_token().m_data.push_back(U'-');
				current_comment_token().m_data.push_back(U'-');
				current_comment_token().m_data.push_back(U'!');
				change_state(comment_end_dash_state);
				return;
			case U'>':
				report_error(error_name::incorrectly_closed_comment);
				change_state(data_state);
				emit_token(current_comment_token());
				return;
			}

			current_comment_token().m_data.push_back(U'-');
			current_comment_token().m_data.push_back(U'-');
			current_comment_token().m_data.push_back(U'!');
			reconsume(comment_state);
		}

		/*! 12.2.5.53 DOCTYPE state */
		void on_DOCTYPE_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				DOCTYPE_token& d = create_DOCTYPE_token();
				d.m_force_quirks_flag = true;
				emit_token(d);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				change_state(before_DOCTYPE_name_state);
				return;
			case U'>':
				reconsume(before_DOCTYPE_name_state);
				return;
			}

			report_error(error_name::missing_whitespace_before_doctype_name);
			reconsume(before_DOCTYPE_name_state);
		}

		/*! 12.2.5.54 Before DOCTYPE name state */
		void on_before_DOCTYPE_name_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				DOCTYPE_token& d = create_DOCTYPE_token();
				d.m_force_quirks_flag = true;
				emit_token(d);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				create_DOCTYPE_token();
				current_DOCTYPE_token().m_name = U'\xFFFD';
				change_state(DOCTYPE_name_state);
				return;
			case U'>':
				report_error(error_name::missing_doctype_name);
				create_DOCTYPE_token();
				current_DOCTYPE_token().m_force_quirks_flag = true;
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			}

			if (is_ascii_upper_alpha(cp))
			{
				create_DOCTYPE_token();
				current_DOCTYPE_token().m_name = cp + 0x20;
				change_state(DOCTYPE_name_state);
				return;
			}

			DOCTYPE_token& d = create_DOCTYPE_token();
			d.m_name = cp;
			change_state(DOCTYPE_name_state);
		}

		/*! 12.2.5.55 DOCTYPE name state */
		void on_DOCTYPE_name_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				change_state(after_DOCTYPE_name_state);
				return;
			case U'>':
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				current_DOCTYPE_token().m_name.push_back(U'\xFFFD');
				return;
			}

			if (is_ascii_upper_alpha(cp))
			{
				current_DOCTYPE_token().m_name.push_back(cp + 0x20);
				return;
			}

			current_DOCTYPE_token().m_name.push_back(cp);
		}

		/*! 12.2.5.56 After DOCTYPE name state */
		void on_after_DOCTYPE_name_state()
		{
			std::size_t constexpr n = std::max(std::size(U"public") - 1, std::size(U"system") - 1);
			if (!fill(n)) return;

			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				goto Flush;
			case U'>':
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				goto Flush;
			}

			if (match(U"public", true, true))
			{
				consume(std::size(U"public") - 2);
				change_state(after_DOCTYPE_public_keyword_state);
				goto Flush;
			}

			if (match(U"system", true, true))
			{
				consume(std::size(U"system") - 2);
				change_state(after_DOCTYPE_system_keyword_state);
				goto Flush;
			}

			report_error(error_name::invalid_character_sequence_after_doctype_name);
			current_DOCTYPE_token().m_force_quirks_flag = true;
			reconsume(bogus_DOCTYPE_state);

		Flush:
			flush_code_point();
		}

		/*! 12.2.5.57 After DOCTYPE public keyword state */
		void on_after_DOCTYPE_public_keyword_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				change_state(before_DOCTYPE_public_identifier_state);
				return;
			case U'"':
				report_error(error_name::missing_whitespace_after_doctype_public_keyword);
				current_DOCTYPE_token().m_public_identifier.clear();
				change_state(DOCTYPE_public_identifier_double_quoted_state);
				return;
			case U'\'':
				report_error(error_name::missing_whitespace_after_doctype_public_keyword);
				current_DOCTYPE_token().m_public_identifier.clear();
				change_state(DOCTYPE_public_identifier_single_quoted_state);
				return;
			case U'>':
				report_error(error_name::missing_doctype_public_identifier);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			}

			report_error(error_name::missing_quote_before_doctype_public_identifier);
			current_DOCTYPE_token().m_force_quirks_flag = true;
			reconsume(bogus_DOCTYPE_state);
		}

		/*! 12.2.5.58 Before DOCTYPE public identifier state */
		void on_before_DOCTYPE_public_identifier_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				return;
			case U'"':
				current_DOCTYPE_token().m_public_identifier.clear();
				change_state(DOCTYPE_public_identifier_double_quoted_state);
				return;
			case U'\'':
				current_DOCTYPE_token().m_public_identifier.clear();
				change_state(DOCTYPE_public_identifier_single_quoted_state);
				return;
			case U'>':
				report_error(error_name::missing_doctype_public_identifier);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			}

			report_error(error_name::missing_quote_before_doctype_public_identifier);
			current_DOCTYPE_token().m_force_quirks_flag = true;
			reconsume(bogus_DOCTYPE_state);
		}

		/*! 12.2.5.59 DOCTYPE public identifier (double-quoted) state */
		void on_DOCTYPE_public_identifier_double_quoted_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'"':
				change_state(after_DOCTYPE_public_identifier_state);
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				current_DOCTYPE_token().m_public_identifier.push_back(U'\xFFFD');
				return;
			case U'>':
				report_error(error_name::abrupt_doctype_public_identifier);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			}

			current_DOCTYPE_token().m_public_identifier.push_back(cp);
		}

		/*! 12.2.5.60 DOCTYPE public identifier (single-quoted) state */
		void on_DOCTYPE_public_identifier_single_quoted_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\'':
				change_state(after_DOCTYPE_public_identifier_state);
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				current_DOCTYPE_token().m_public_identifier.push_back(U'\xFFFD');
				return;
			case U'>':
				report_error(error_name::abrupt_doctype_public_identifier);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			}

			current_DOCTYPE_token().m_public_identifier.push_back(cp);
		}

		/*! 12.2.5.61 After DOCTYPE public identifier state */
		void on_after_DOCTYPE_public_identifier_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				change_state(between_DOCTYPE_public_and_system_identifiers_state);
				return;
			case U'>':
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			case U'"':
				report_error(error_name::missing_whitespace_between_doctype_public_and_system_identifiers);
				current_DOCTYPE_token().m_system_identifier.clear();
				change_state(DOCTYPE_system_identifier_double_quoted_state);
				return;
			case U'\'':
				report_error(error_name::missing_whitespace_between_doctype_public_and_system_identifiers);
				current_DOCTYPE_token().m_system_identifier.clear();
				change_state(DOCTYPE_system_identifier_single_quoted_state);
				return;
			}

			report_error(error_name::missing_quote_before_doctype_system_identifier);
			current_DOCTYPE_token().m_force_quirks_flag = true;
			reconsume(bogus_DOCTYPE_state);
		}

		/*! 12.2.5.62 Between DOCTYPE public and system identifiers state */
		void on_between_DOCTYPE_public_and_system_identifiers_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				return;
			case U'>':
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			case U'"':
				current_DOCTYPE_token().m_system_identifier.clear();
				change_state(DOCTYPE_system_identifier_double_quoted_state);
				return;
			case U'\'':
				current_DOCTYPE_token().m_system_identifier.clear();
				change_state(DOCTYPE_system_identifier_single_quoted_state);
				return;
			}

			report_error(error_name::missing_quote_before_doctype_system_identifier);
			current_DOCTYPE_token().m_force_quirks_flag = true;
			reconsume(bogus_DOCTYPE_state);
		}

		/*! 12.2.5.63 After DOCTYPE system keyword state */
		void on_after_DOCTYPE_system_keyword_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				change_state(before_DOCTYPE_system_identifier_state);
				return;
			case U'"':
				report_error(error_name::missing_whitespace_after_doctype_system_keyword);
				current_DOCTYPE_token().m_system_identifier.clear();
				change_state(DOCTYPE_system_identifier_double_quoted_state);
				return;
			case U'\'':
				report_error(error_name::missing_whitespace_after_doctype_system_keyword);
				current_DOCTYPE_token().m_system_identifier.clear();
				change_state(DOCTYPE_system_identifier_single_quoted_state);
				return;
			case U'>':
				report_error(error_name::missing_doctype_system_identifier);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			}

			report_error(error_name::missing_quote_before_doctype_system_identifier);
			current_DOCTYPE_token().m_force_quirks_flag = true;
			reconsume(bogus_DOCTYPE_state);
		}

		/*! 12.2.5.64 Before DOCTYPE system identifier state */
		void on_before_DOCTYPE_system_identifier_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				return;
			case U'"':
				current_DOCTYPE_token().m_system_identifier.clear();
				change_state(DOCTYPE_system_identifier_double_quoted_state);
				return;
			case U'\'':
				current_DOCTYPE_token().m_system_identifier.clear();
				change_state(DOCTYPE_system_identifier_single_quoted_state);
				return;
			case U'>':
				report_error(error_name::missing_doctype_system_identifier);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			}

			report_error(error_name::missing_quote_before_doctype_system_identifier);
			current_DOCTYPE_token().m_force_quirks_flag = true;
			reconsume(bogus_DOCTYPE_state);
		}

		/*! 12.2.5.65 DOCTYPE system identifier (double-quoted) state */
		void on_DOCTYPE_system_identifier_double_quoted_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'"':
				change_state(after_DOCTYPE_system_identifier_state);
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				current_DOCTYPE_token().m_system_identifier.push_back(U'\xFFFD');
				return;
			case U'>':
				report_error(error_name::abrupt_doctype_system_identifier);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			}

			current_DOCTYPE_token().m_system_identifier.push_back(cp);
		}

		/*! 12.2.5.66 DOCTYPE system identifier (single-quoted) state */
		void on_DOCTYPE_system_identifier_single_quoted_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\'':
				change_state(after_DOCTYPE_system_identifier_state);
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				current_DOCTYPE_token().m_system_identifier.push_back(U'\xFFFD');
				return;
			case U'>':
				report_error(error_name::abrupt_doctype_system_identifier);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			}

			current_DOCTYPE_token().m_system_identifier.push_back(cp);
		}

		/*! 12.2.5.67 After DOCTYPE system identifier state */
		void on_after_DOCTYPE_system_identifier_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_doctype);
				current_DOCTYPE_token().m_force_quirks_flag = true;
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'\x9':  // TAB
			case U'\xA':  // LF
			case U'\xC':  // FF
			case U'\x20': // SPACE
				return;
			case U'>':
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			}

			report_error(error_name::unexpected_character_after_doctype_system_identifier);
			reconsume(bogus_DOCTYPE_state);
		}

		/*! 12.2.5.68 Bogus DOCTYPE state */
		void on_bogus_DOCTYPE_state()
		{
			if (eof())
			{
				emit_token(current_DOCTYPE_token());
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();

			switch (cp)
			{
			case U'>':
				change_state(data_state);
				emit_token(current_DOCTYPE_token());
				return;
			case U'\x0':
				report_error(error_name::unexpected_null_character);
				return;
			}
		}

		/*! 12.2.5.69 CDATA section state */
		void on_CDATA_section_state()
		{
			if (eof())
			{
				report_error(error_name::eof_in_cdata);
				emit_token(end_of_file_token());
				return;
			}

			char32_t cp = consume();
			if (cp == U']')
			{
				change_state(CDATA_section_bracket_state);
				return;
			}

			emit_token(cp);
		}

		/*! 12.2.5.70 CDATA section bracket state */
		void on_CDATA_section_bracket_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				if (cp == U']')
				{
					change_state(CDATA_section_end_state);
					return;
				}
			}

			emit_token(U']');
			reconsume(CDATA_section_state);
		}

		/*! 12.2.5.71 CDATA section end state */
		void on_CDATA_section_end_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				switch (cp)
				{
				case U']':
					emit_token(U']');
					return;
				case U'>':
					change_state(data_state);
					return;
				}
			}

			emit_token(U']');
			emit_token(U']');
			reconsume(CDATA_section_state);
		}

		/*! 12.2.5.72 Character reference state */
		void on_character_reference_state()
		{
			m_temporary_buffer.clear();
			m_temporary_buffer.push_back(U'&');

			if (!eof())
			{
				char32_t cp = consume();

				if (is_ascii_alphanumeric(cp))
				{
					reconsume(named_character_reference_state);
					return;
				}

				if (cp == U'#')
				{
					m_temporary_buffer.push_back(cp);
					change_state(numeric_character_reference_state);
					return;
				}
			}

			flush_code_points_consumed_as_character_reference();
			reconsume(return_state());
		}

		/*! 12.2.5.73 Named character reference state */
		void on_named_character_reference_state()
		{
			std::uint32_t idx, len;
			auto r = match_named_character_reference(idx, len);
			
			if (r == match_result::partial) return; // partial

			if (r == match_result::succeed) // matched
			{
				char32_t tail = *std::next(begin(), len - 1);
				char32_t cp = 0;
				consume(len);
				if(!eof()) cp = next_input_character();
				if (!(consumed_as_part_of_attribute() && tail != U';' && (cp == U'=' || is_ascii_alphanumeric(cp))))
				{
					if (tail != U';') report_error(error_name::missing_semicolon_after_character_reference);
					
					m_temporary_buffer.clear();
					std::array<char32_t, 2> a = named_character_reference(idx);
					m_temporary_buffer.push_back(a[0]);
					if(a[1] != 0) m_temporary_buffer.push_back(a[1]);
				}

				flush_code_points_consumed_as_character_reference();
				change_state(return_state());

				flush_code_point();
				return;
			}

			flush_code_points_consumed_as_character_reference();
			change_state(ambiguous_ampersand_state);

			flush_code_point();
		}

		/*! 12.2.5.74 Ambiguous ampersand state */
		void on_ambiguous_ampersand_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				if (is_ascii_alphanumeric(cp))
				{
					if (consumed_as_part_of_attribute()) current_attribute().m_value.push_back(cp);
					else emit_token(cp);
					return;
				}

				if (cp == U';')
				{
					report_error(error_name::unknown_named_character_reference);
					reconsume(return_state());
					return;
				}
			}

			reconsume(return_state());
		}

		/*! 12.2.5.75 Numeric character reference state */
		void on_numeric_character_reference_state()
		{
			m_character_reference_code = 0;

			if (!eof())
			{
				char32_t cp = consume();

				switch (cp)
				{
				case U'x':
				case U'X':
					m_temporary_buffer.push_back(cp);
					change_state(hexadecimal_character_reference_start_state);
					return;
				}
			}

			reconsume(decimal_character_reference_start_state);
		}

		/*! 12.2.5.76 Hexadecimal character reference start state */
		void on_hexadecimal_character_reference_start_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				if (is_ascii_hex_digit(cp))
				{
					reconsume(hexadecimal_character_reference_state);
					return;
				}
			}

			report_error(error_name::absence_of_digits_in_numeric_character_reference);
			flush_code_points_consumed_as_character_reference();
			reconsume(return_state());
		}

		/*! 12.2.5.77 Decimal character reference start state */
		void on_decimal_character_reference_start_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				if (is_ascii_digit(cp))
				{
					reconsume(decimal_character_reference_state);
					return;
				}
			}

			report_error(error_name::absence_of_digits_in_numeric_character_reference);
			flush_code_points_consumed_as_character_reference();
			reconsume(return_state());
		}

		/*! 12.2.5.78 Hexadecimal character reference state */
		void on_hexadecimal_character_reference_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				if (is_ascii_hex_digit(cp))
				{
					char32_t c = cp;
					
					if (is_ascii_digit(cp)) c -= 0x30;
					else if (is_ascii_upper_hex_digit(cp)) c -= 0x37;
					else c -= 0x57;

					m_character_reference_code = (m_character_reference_code * 16) + c;

					return;
				}

				if (cp == U';')
				{
					change_state(numeric_character_reference_end_state);
					return;
				}
			}

			report_error(error_name::missing_semicolon_after_character_reference);
			reconsume(numeric_character_reference_end_state);
		}

		/*! 12.2.5.79 Decimal character reference state */
		void on_decimal_character_reference_state()
		{
			if (!eof())
			{
				char32_t cp = consume();

				if (is_ascii_digit(cp))
				{
					m_character_reference_code = (m_character_reference_code * 10) + (cp - 0x30);
					return;
				}

				if (cp == U';')
				{
					change_state(numeric_character_reference_end_state);
					return;
				}
			}

			report_error(error_name::missing_semicolon_after_character_reference);
			reconsume(numeric_character_reference_end_state);
		}

		/*! 12.2.5.80 Numeric character reference end state */
		void on_numeric_character_reference_end_state()
		{
			char32_t c = m_character_reference_code;

			if (c == 0x0)
			{
				report_error(error_name::null_character_reference);
				m_character_reference_code = U'\xFFFD';
			}

			if(0x10FFFF < c)
			{
				report_error(error_name::character_reference_outside_unicode_range);
				m_character_reference_code = U'\xFFFD';
			}

			if (is_surrogate(c))
			{
				report_error(error_name::surrogate_character_reference);
				m_character_reference_code = U'\xFFFD';
			}

			if (is_noncharacter(c))
			{
				report_error(error_name::noncharacter_character_reference);
			}

			if (c == 0xD || (is_control(c) && !is_ascii_white_space(c)))
			{
				report_error(error_name::control_character_reference);
			}

			auto it = character_reference_code_tbl.find(c);
			if (it != character_reference_code_tbl.end()) m_character_reference_code = it->second;
			
			m_temporary_buffer.assign(1, m_character_reference_code);
			flush_code_points_consumed_as_character_reference();
			change_state(return_state());
		}

		// 状態番号 -----------------------------------------------------------
		
		static state_type constexpr data_state                                          = &tokenizer::on_data_state;
		static state_type constexpr RCDATA_state                                        = &tokenizer::on_RCDATA_state;
		static state_type constexpr RAWTEXT_state                                       = &tokenizer::on_RAWTEXT_state;
		static state_type constexpr script_data_state                                   = &tokenizer::on_script_data_state;
		static state_type constexpr PLAINTEXT_state                                     = &tokenizer::on_PLAINTEXT_state;
		static state_type constexpr tag_open_state                                      = &tokenizer::on_tag_open_state;
		static state_type constexpr end_tag_open_state                                  = &tokenizer::on_end_tag_open_state;
		static state_type constexpr tag_name_state                                      = &tokenizer::on_tag_name_state;
		static state_type constexpr RCDATA_less_than_sign_state                         = &tokenizer::on_RCDATA_less_than_sign_state;
		static state_type constexpr RCDATA_end_tag_open_state                           = &tokenizer::on_RCDATA_end_tag_open_state;
		static state_type constexpr RCDATA_end_tag_name_state                           = &tokenizer::on_RCDATA_end_tag_name_state;
		static state_type constexpr RAWTEXT_less_than_sign_state                        = &tokenizer::on_RAWTEXT_less_than_sign_state;
		static state_type constexpr RAWTEXT_end_tag_open_state                          = &tokenizer::on_RAWTEXT_end_tag_open_state;
		static state_type constexpr RAWTEXT_end_tag_name_state                          = &tokenizer::on_RAWTEXT_end_tag_name_state;
		static state_type constexpr script_data_less_than_sign_state                    = &tokenizer::on_script_data_less_than_sign_state;
		static state_type constexpr script_data_end_tag_open_state                      = &tokenizer::on_script_data_end_tag_open_state;
		static state_type constexpr script_data_end_tag_name_state                      = &tokenizer::on_script_data_end_tag_name_state;
		static state_type constexpr script_data_escape_start_state                      = &tokenizer::on_script_data_escape_start_state;
		static state_type constexpr script_data_escape_start_dash_state                 = &tokenizer::on_script_data_escape_start_dash_state;
		static state_type constexpr script_data_escaped_state                           = &tokenizer::on_script_data_escaped_state;
		static state_type constexpr script_data_escaped_dash_state                      = &tokenizer::on_script_data_escaped_dash_state;
		static state_type constexpr script_data_escaped_dash_dash_state                 = &tokenizer::on_script_data_escaped_dash_dash_state;
		static state_type constexpr script_data_escaped_less_than_sign_state            = &tokenizer::on_script_data_escaped_less_than_sign_state;
		static state_type constexpr script_data_escaped_end_tag_open_state              = &tokenizer::on_script_data_escaped_end_tag_open_state;
		static state_type constexpr script_data_escaped_end_tag_name_state              = &tokenizer::on_script_data_escaped_end_tag_name_state;
		static state_type constexpr script_data_double_escape_start_state               = &tokenizer::on_script_data_double_escape_start_state;
		static state_type constexpr script_data_double_escaped_state                    = &tokenizer::on_script_data_double_escaped_state;
		static state_type constexpr script_data_double_escaped_dash_state               = &tokenizer::on_script_data_double_escaped_dash_state;
		static state_type constexpr script_data_double_escaped_dash_dash_state          = &tokenizer::on_script_data_double_escaped_dash_dash_state;
		static state_type constexpr script_data_double_escaped_less_than_sign_state     = &tokenizer::on_script_data_double_escaped_less_than_sign_state;
		static state_type constexpr script_data_double_escape_end_state                 = &tokenizer::on_script_data_double_escape_end_state;
		static state_type constexpr before_attribute_name_state                         = &tokenizer::on_before_attribute_name_state;
		static state_type constexpr attribute_name_state                                = &tokenizer::on_attribute_name_state;
		static state_type constexpr after_attribute_name_state                          = &tokenizer::on_after_attribute_name_state;
		static state_type constexpr before_attribute_value_state                        = &tokenizer::on_before_attribute_value_state;
		static state_type constexpr attribute_value_double_quoted_state                 = &tokenizer::on_attribute_value_double_quoted_state;
		static state_type constexpr attribute_value_single_quoted_state                 = &tokenizer::on_attribute_value_single_quoted_state;
		static state_type constexpr attribute_value_unquoted_state                      = &tokenizer::on_attribute_value_unquoted_state;
		static state_type constexpr after_attribute_value_quoted_state                  = &tokenizer::on_after_attribute_value_quoted_state;
		static state_type constexpr self_closing_start_tag_state                        = &tokenizer::on_self_closing_start_tag_state;
		static state_type constexpr bogus_comment_state                                 = &tokenizer::on_bogus_comment_state;
		static state_type constexpr markup_declaration_open_state                       = &tokenizer::on_markup_declaration_open_state;
		static state_type constexpr comment_start_state                                 = &tokenizer::on_comment_start_state;
		static state_type constexpr comment_start_dash_state                            = &tokenizer::on_comment_start_dash_state;
		static state_type constexpr comment_state                                       = &tokenizer::on_comment_state;
		static state_type constexpr comment_less_than_sign_state                        = &tokenizer::on_comment_less_than_sign_state;
		static state_type constexpr comment_less_than_sign_bang_state                   = &tokenizer::on_comment_less_than_sign_bang_state;
		static state_type constexpr comment_less_than_sign_bang_dash_state              = &tokenizer::on_comment_less_than_sign_bang_dash_state;
		static state_type constexpr comment_less_than_sign_bang_dash_dash_state         = &tokenizer::on_comment_less_than_sign_bang_dash_dash_state;
		static state_type constexpr comment_end_dash_state                              = &tokenizer::on_comment_end_dash_state;
		static state_type constexpr comment_end_state                                   = &tokenizer::on_comment_end_state;
		static state_type constexpr comment_end_bang_state                              = &tokenizer::on_comment_end_bang_state;
		static state_type constexpr DOCTYPE_state                                       = &tokenizer::on_DOCTYPE_state;
		static state_type constexpr before_DOCTYPE_name_state                           = &tokenizer::on_before_DOCTYPE_name_state;
		static state_type constexpr DOCTYPE_name_state                                  = &tokenizer::on_DOCTYPE_name_state;
		static state_type constexpr after_DOCTYPE_name_state                            = &tokenizer::on_after_DOCTYPE_name_state;
		static state_type constexpr after_DOCTYPE_public_keyword_state                  = &tokenizer::on_after_DOCTYPE_public_keyword_state;
		static state_type constexpr before_DOCTYPE_public_identifier_state              = &tokenizer::on_before_DOCTYPE_public_identifier_state;
		static state_type constexpr DOCTYPE_public_identifier_double_quoted_state       = &tokenizer::on_DOCTYPE_public_identifier_double_quoted_state;
		static state_type constexpr DOCTYPE_public_identifier_single_quoted_state       = &tokenizer::on_DOCTYPE_public_identifier_single_quoted_state;
		static state_type constexpr after_DOCTYPE_public_identifier_state               = &tokenizer::on_after_DOCTYPE_public_identifier_state;
		static state_type constexpr between_DOCTYPE_public_and_system_identifiers_state = &tokenizer::on_between_DOCTYPE_public_and_system_identifiers_state;
		static state_type constexpr after_DOCTYPE_system_keyword_state                  = &tokenizer::on_after_DOCTYPE_system_keyword_state;
		static state_type constexpr before_DOCTYPE_system_identifier_state              = &tokenizer::on_before_DOCTYPE_system_identifier_state;
		static state_type constexpr DOCTYPE_system_identifier_double_quoted_state       = &tokenizer::on_DOCTYPE_system_identifier_double_quoted_state;
		static state_type constexpr DOCTYPE_system_identifier_single_quoted_state       = &tokenizer::on_DOCTYPE_system_identifier_single_quoted_state;
		static state_type constexpr after_DOCTYPE_system_identifier_state               = &tokenizer::on_after_DOCTYPE_system_identifier_state;
		static state_type constexpr bogus_DOCTYPE_state                                 = &tokenizer::on_bogus_DOCTYPE_state;
		static state_type constexpr CDATA_section_state                                 = &tokenizer::on_CDATA_section_state;
		static state_type constexpr CDATA_section_bracket_state                         = &tokenizer::on_CDATA_section_bracket_state;
		static state_type constexpr CDATA_section_end_state                             = &tokenizer::on_CDATA_section_end_state;
		static state_type constexpr character_reference_state                           = &tokenizer::on_character_reference_state;
		static state_type constexpr named_character_reference_state                     = &tokenizer::on_named_character_reference_state;
		static state_type constexpr ambiguous_ampersand_state                           = &tokenizer::on_ambiguous_ampersand_state;
		static state_type constexpr numeric_character_reference_state                   = &tokenizer::on_numeric_character_reference_state;
		static state_type constexpr hexadecimal_character_reference_start_state         = &tokenizer::on_hexadecimal_character_reference_start_state;
		static state_type constexpr decimal_character_reference_start_state             = &tokenizer::on_decimal_character_reference_start_state;
		static state_type constexpr hexadecimal_character_reference_state               = &tokenizer::on_hexadecimal_character_reference_state;
		static state_type constexpr decimal_character_reference_state                   = &tokenizer::on_decimal_character_reference_state;
		static state_type constexpr numeric_character_reference_end_state               = &tokenizer::on_numeric_character_reference_end_state;
	};
}
