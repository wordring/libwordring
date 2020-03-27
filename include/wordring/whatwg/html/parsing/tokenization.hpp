#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/infra/infra.hpp>
#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/input_stream.hpp>
#include <wordring/whatwg/html/parsing/parser_defs.hpp>
#include <wordring/whatwg/html/parsing/token.hpp>

#include <type_traits>

namespace wordring::whatwg::html::parsing
{

	template <typename T>
	class tokenizer : public input_stream<T>
	{
		friend input_stream<T>;

	protected:
		using base_type = input_stream<T>;
		using this_type = T;


		using state_type = void(tokenizer::*)();

		using base_type::current_input_character;
		using base_type::next_input_character;
		using base_type::consume;

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

		tokenizer()
			: m_state(data_state)
			, m_return_state(nullptr)
			, m_current_tag_token_id(0)
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
			m_current_tag_token_id = 0;
			m_comment_token.data = data;
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

			if (m_last_start_tag_name.empty() || (m_last_start_tag_name != token.tag_name)) return false;
			return true;
		}

		// トークンの発送-------------------------------------------------------

		template <typename Token>
		void emit_token(Token& token)
		{
			this_type* p = static_cast<this_type*>(this);

			if constexpr (std::is_base_of_v<tag_token, Token>)
			{
				assert(m_current_tag_token_id == 2 || m_current_tag_token_id == 3);

				token.tag_name_id = atom_tbl.at(token.tag_name);
				
				if (m_current_tag_token_id == 2)
				{
					m_last_start_tag_name = m_start_tag_token.tag_name;
					p->on_emit_token(m_start_tag_token);
				}
				else if (m_current_tag_token_id == 3) p->on_emit_token(m_end_tag_token);
			}
			else static_cast<this_type*>(this)->on_emit_token(token);
		}

		void emit_token(char32_t cp)
		{
			m_character_token.data = cp;
			static_cast<this_type*>(this)->on_emit_token(m_character_token);
		}

		void emit_token(end_of_file_token)
		{
			static_cast<this_type*>(this)->on_emit_token(m_end_of_file_token);
		}

		// 状態の変更 ----------------------------------------------------------

		void change_state(state_type st) { m_state = st; }

		void return_state(state_type st) { m_return_state = st; }

		// 文字の再消費 --------------------------------------------------------

		void reconsume(state_type st)
		{
			change_state(st);
			base_type::reconsume();
			on_emit_code_point();
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
				report_error(error::unexpected_null_character);
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
				report_error(error::unexpected_null_character);
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
				report_error(error::unexpected_null_character);
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
				report_error(error::unexpected_null_character);
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
				report_error(error::unexpected_null_character);
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
				report_error(error::eof_before_tag_name);
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
				report_error(error::unexpected_question_mark_instead_of_tag_name);
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

			report_error(error::invalid_first_character_of_tag_name);
			emit_token(U'<');
			reconsume(data_state);
		}

		/*! 12.2.5.7 End tag open state */
		void on_end_tag_open_state()
		{
			if (eof())
			{
				report_error(error::eof_before_tag_name);
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
				report_error(error::missing_end_tag_name);
				change_state(data_state);
				return;
			}

			report_error(error::invalid_first_character_of_tag_name);
			create_comment_token();
			reconsume(bogus_comment_state);
		}

		/*! 12.2.5.8 Tag name state */
		void on_tag_name_state()
		{
			if (eof())
			{
				report_error(error::eof_in_tag);
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
				report_error(error::unexpected_null_character);
				current_tag_token().tag_name.push_back(U'\xFFFD');
				return;
			}

			if (is_ascii_upper_alpha(cp))
			{
				current_tag_token().tag_name.push_back(cp + 0x20);
				return;
			}

			current_tag_token().tag_name.push_back(cp);	
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
					current_tag_token().tag_name.push_back(cp + 0x20);
					m_temporary_buffer.push_back(cp);
					return;
				}

				if (is_ascii_lower_alpha(cp))
				{
					current_tag_token().tag_name.push_back(cp);
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
					current_tag_token().tag_name.push_back(cp + 0x20);
					m_temporary_buffer.push_back(cp);
					return;
				}

				if (is_ascii_lower_alpha(cp))
				{
					current_tag_token().tag_name.push_back(cp);
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
					current_tag_token().tag_name.push_back(cp + 0x20);
					m_temporary_buffer.push_back(cp);
					return;
				}

				if (is_ascii_lower_alpha(cp))
				{
					current_tag_token().tag_name.push_back(cp);
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
				report_error(error::eof_in_script_html_comment_like_text);
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
				report_error(error::unexpected_null_character);
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
				report_error(error::eof_in_script_html_comment_like_text);
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
				report_error(error::unexpected_null_character);
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
				report_error(error::eof_in_script_html_comment_like_text);
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
				report_error(error::unexpected_null_character);
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
				else if (is_ascii_alpha(cp))
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

		/*!  */
		void on_script_data_escaped_end_tag_name_state()
		{
		}

		/*!  */
		void on_script_data_double_escape_start_state()
		{
		}

		/*!  */
		void on_script_data_double_escaped_state()
		{
		}

		/*!  */
		void on_script_data_double_escaped_dash_state()
		{
		}

		/*!  */
		void on_script_data_double_escaped_dash_dash_state()
		{
		}

		/*!  */
		void on_script_data_double_escaped_less_than_sign_state()
		{
		}

		/*!  */
		void on_script_data_double_escape_end_state()
		{
		}

		/*!  */
		void on_before_attribute_name_state()
		{
		}

		/*!  */
		void on_attribute_name_state()
		{
		}

		/*!  */
		void on_after_attribute_name_state()
		{
		}

		/*!  */
		void on_before_attribute_value_state()
		{
		}

		/*!  */
		void on_attribute_value_double_quoted_state()
		{
		}

		/*!  */
		void on_attribute_value_single_quoted_state()
		{
		}

		/*!  */
		void on_attribute_value_unquoted_state()
		{
		}

		/*!  */
		void on_after_attribute_value_quoted_state()
		{
		}

		/*!  */
		void on_self_closing_start_tag_state()
		{
		}

		/*!  */
		void on_bogus_comment_state()
		{
		}

		/*!  */
		void on_markup_declaration_open_state()
		{
		}

		/*!  */
		void on_comment_start_state()
		{
		}

		/*!  */
		void on_comment_start_dash_state()
		{
		}

		/*!  */
		void on_comment_state()
		{
		}

		/*!  */
		void on_comment_less_than_sign_state()
		{
		}

		/*!  */
		void on_comment_less_than_sign_bang_state()
		{
		}

		/*!  */
		void on_comment_less_than_sign_bang_dash_state()
		{
		}

		/*!  */
		void on_comment_less_than_sign_bang_dash_dash_state()
		{
		}

		/*!  */
		void on_comment_end_dash_state()
		{
		}

		/*!  */
		void on_comment_end_state()
		{
		}

		/*!  */
		void on_comment_end_bang_state()
		{
		}

		/*!  */
		void on_DOCTYPE_state()
		{
		}

		/*!  */
		void on_before_DOCTYPE_name_state()
		{
		}

		/*!  */
		void on_DOCTYPE_name_state()
		{
		}

		/*!  */
		void on_after_DOCTYPE_name_state()
		{
		}

		/*!  */
		void on_after_DOCTYPE_public_keyword_state()
		{
		}

		/*!  */
		void on_before_DOCTYPE_public_identifier_state()
		{
		}

		/*!  */
		void on_DOCTYPE_public_identifier_double_quoted_state()
		{
		}

		/*!  */
		void on_DOCTYPE_public_identifier_single_quoted_state()
		{
		}

		/*!  */
		void on_after_DOCTYPE_public_identifier_state()
		{
		}

		/*!  */
		void on_between_DOCTYPE_public_and_system_identifiers_state()
		{
		}

		/*!  */
		void on_after_DOCTYPE_system_keyword_state()
		{
		}

		/*!  */
		void on_before_DOCTYPE_system_identifier_state()
		{
		}

		/*!  */
		void on_DOCTYPE_system_identifier_double_quoted_state()
		{
		}

		/*!  */
		void on_DOCTYPE_system_identifier_single_quoted_state()
		{
		}

		/*!  */
		void on_after_DOCTYPE_system_identifier_state()
		{
		}

		/*!  */
		void on_bogus_DOCTYPE_state()
		{
		}

		/*!  */
		void on_CDATA_section_state()
		{
		}

		/*!  */
		void on_CDATA_section_bracket_state()
		{
		}

		/*!  */
		void on_CDATA_section_end_state()
		{
		}

		/*!  */
		void on_character_reference_state()
		{
		}

		/*!  */
		void on_named_character_reference_state()
		{
		}

		/*!  */
		void on_ambiguous_ampersand_state()
		{
		}

		/*!  */
		void on_numeric_character_reference_state()
		{
		}

		/*!  */
		void on_hexadecimal_character_reference_start_state()
		{
		}

		/*!  */
		void on_decimal_character_reference_start_state()
		{
		}

		/*!  */
		void on_hexadecimal_character_reference_state()
		{
		}

		/*!  */
		void on_decimal_character_reference_state()
		{
		}

		/*!  */
		void on_numeric_character_reference_end_state()
		{
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
