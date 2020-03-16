#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html


namespace wordring::whatwg::html::detail
{
	template <typename Tokenizer>
	class tree_constructor
	{
		using tokenizer_type = Tokenizer;

	protected:
		tree_constructor(tokenizer_type& tokenizer)
			: m_tokenizer(tokenizer)
		{
		}

	protected:
		tokenizer_type& m_tokenizer;

	protected:

		template <typename Token>
		void on_initial_insertion_mode(Token&& token)
		{
		}
		void on_before_html_insertion_mode()
		{
		}
		void on_before_head_insertion_mode()
		{
		}
		void on_in_head_insertion_mode()
		{
		}
		void on_in_head_noscript_insertion_mode()
		{
		}
		void on_after_head_insertion_mode()
		{
		}
		void on_in_body_insertion_mode()
		{
		}
		void on_text_insertion_mode()
		{
		}
		void on_in_table_insertion_mode()
		{
		}
		void on_in_table_text_insertion_mode()
		{
		}
		void on_in_caption_insertion_mode()
		{
		}
		void on_in_column_group_insertion_mode()
		{
		}
		void on_in_table_body_insertion_mode()
		{
		}
		void on_in_row_insertion_mode()
		{
		}
		void on_in_cell_insertion_mode()
		{
		}
		void on_in_select_insertion_mode()
		{
		}
		void on_in_select_in_table_insertion_mode()
		{
		}
		void on_in_template_insertion_mode()
		{
		}
		void on_after_body_insertion_mode()
		{
		}
		void on_in_frameset_insertion_mode()
		{
		}
		void on_after_frameset_insertion_mode()
		{
		}
		void on_after_after_body_insertion_mode()
		{
		}
		void on_after_after_frameset_insertion_mode()
		{
		}
	};

	class tree_construction_dispatcher
	{

	};
}
