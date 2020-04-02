#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/html/parsing/parser_defs.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>

namespace wordring::whatwg::html::parsing
{
	template <typename T, typename NodePolicy>
	class tree_construction_dispatcher : public tokenizer<T, NodePolicy>
	{
		friend tokenizer<T, NodePolicy>;

	protected:
		using base_type = tokenizer<T, NodePolicy>;
		using this_type = T;

		using node_policy = NodePolicy;

		 

	public:
		using base_type::report_error;
		using base_type::eof;
		
		using mode_type = void(tree_construction_dispatcher::*)();

		enum class mode : std::uint32_t
		{
			initial_insertion_mode,
			before_html_insertion_mode,
			before_head_insertion_mode,
			in_head_insertion_mode,
			in_head_noscript_insertion_mode,
			after_head_insertion_mode,
			in_body_insertion_mode,
			text_insertion_mode,
			in_table_insertion_mode,
			in_table_text_insertion_mode,
			in_caption_insertion_mode,
			in_column_group_insertion_mode,
			in_table_body_insertion_mode,
			in_row_insertion_mode,
			in_cell_insertion_mode,
			in_select_insertion_mode,
			in_select_in_table_insertion_mode,
			in_template_insertion_mode,
			after_body_insertion_mode,
			in_frameset_insertion_mode,
			after_frameset_insertion_mode,
			after_after_body_insertion_mode,
			after_after_frameset_insertion_mode,
		};

	protected:
		mode m_insertion_mode;
		bool m_omit_lf;

	protected:
		tree_construction_dispatcher()
			: m_insertion_mode(mode::initial_insertion_mode)
			, m_omit_lf(false)
		{
		}

	protected:

		// スタック -----------------------------------------------------------

		// 

		template <typename Element>
		bool is_MathML_text_integration_point(Element c)
		{

		}

		template <typename Token>
		void reprocess_token(Token& token)
		{
			on_emit_token(token);
		}

		template <typename Token>
		void on_emit_token(Token& token)
		{
			switch (m_insertion_mode)
			{
			case mode::initial_insertion_mode:              return on_initial_insertion_mode(token);
			case mode::before_html_insertion_mode:          return on_before_html_insertion_mode(token);
			case mode::before_head_insertion_mode:          return on_before_head_insertion_mode(token);
			case mode::in_head_insertion_mode:              return on_in_head_insertion_mode(token);
			case mode::in_head_noscript_insertion_mode:     return on_in_head_noscript_insertion_mode(token);
			case mode::after_head_insertion_mode:           return on_after_head_insertion_mode(token);
			case mode::in_body_insertion_mode:              return on_in_body_insertion_mode(token);
			case mode::text_insertion_mode:                 return on_text_insertion_mode(token);
			case mode::in_table_insertion_mode:             return on_in_table_insertion_mode(token);
			case mode::in_table_text_insertion_mode:        return on_in_table_text_insertion_mode(token);
			case mode::in_caption_insertion_mode:           return on_in_caption_insertion_mode(token);
			case mode::in_column_group_insertion_mode:      return on_in_column_group_insertion_mode(token);
			case mode::in_table_body_insertion_mode:        return on_in_table_body_insertion_mode(token);
			case mode::in_row_insertion_mode:               return on_in_row_insertion_mode(token);
			case mode::in_cell_insertion_mode:              return on_in_cell_insertion_mode(token);
			case mode::in_select_insertion_mode:            return on_in_select_insertion_mode(token);
			case mode::in_select_in_table_insertion_mode:   return on_in_select_in_table_insertion_mode(token);
			case mode::in_template_insertion_mode:          return on_in_template_insertion_mode(token);
			case mode::after_body_insertion_mode:           return on_after_body_insertion_mode(token);
			case mode::in_frameset_insertion_mode:          return on_in_frameset_insertion_mode(token);
			case mode::after_frameset_insertion_mode:       return on_after_frameset_insertion_mode(token);
			case mode::after_after_body_insertion_mode:     return on_after_after_body_insertion_mode(token);
			case mode::after_after_frameset_insertion_mode: return on_after_after_frameset_insertion_mode(token);
			}
		}

		template <typename Token>
		void on_initial_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data)) return;
			}
			else if constexpr (std::is_same_v<comment_token, Token>)
			{

			}
		}

		template <typename Token>
		void on_before_html_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_before_head_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_head_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_head_noscript_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_after_head_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_body_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_text_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_table_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_table_text_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_caption_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_column_group_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_table_body_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_row_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_cell_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_select_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_select_in_table_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_template_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_after_body_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_in_frameset_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_after_frameset_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_after_after_body_insertion_mode(Token& token)
		{
		}

		template <typename Token>
		void on_after_after_frameset_insertion_mode(Token& token)
		{
		}
	};

}
