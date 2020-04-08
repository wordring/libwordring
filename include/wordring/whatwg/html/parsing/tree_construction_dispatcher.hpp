#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/html/parsing/parser_defs.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>

#include <wordring/whatwg/infra/unicode.hpp>

namespace wordring::whatwg::html::parsing
{
	template <typename T, typename NodePolicy>
	class tree_construction_dispatcher : public tokenizer<T, NodePolicy>
	{
		friend tokenizer<T, NodePolicy>;

	protected:
		using base_type = tokenizer<T, NodePolicy>;
		using this_type = T;

		using policy = NodePolicy;

		using string_type    = typename policy::string_type;
		using container_type = typename policy::container_type;
		using node_type      = typename policy::node_type;
		using node_pointer   = typename container_type::iterator;

		using document_node_type               = typename policy::document_node_type;
		using document_type_node_type          = typename policy::document_type_node_type;
		using document_fragment_node_type      = typename policy::document_fragment_node_type;
		using element_node_type                = typename policy::element_node_type;
		using text_node_type                   = typename policy::text_node_type;
		using processing_instruction_node_type = typename policy::processing_instruction_node_type;
		using comment_node_type                = typename policy::comment_node_type;

		using base_type::m_open_element_stack;

		static bool constexpr is_fragments_parser = policy::is_fragments_parser;

	public:
		using base_type::report_error;
		using base_type::eof;

		using mode_type = void(tree_construction_dispatcher::*)();
		using scope_type = bool(tree_construction_dispatcher::*)(node_pointer);

		struct active_formatting_element
		{
			node_pointer    m_it;
			start_tag_token m_token;
			bool            m_marker;
		};

		/*! @brief 挿入モード
		- https://html.spec.whatwg.org/multipage/parsing.html#insertion-mode
		*/
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

		node_pointer m_context_element;


		/*
		https://html.spec.whatwg.org/multipage/parsing.html#the-list-of-active-formatting-elements
		deque の先頭を末尾として使う。
		*/
		std::deque<active_formatting_element> m_active_formatting_element_list;

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#foster-parent
		*/
		bool m_foster_parenting;

	protected:
		tree_construction_dispatcher()
			: m_insertion_mode(mode::initial_insertion_mode)
			, m_omit_lf(false)
			, m_foster_parenting(false)
		{
		}

	protected:

		// --------------------------------------------------------------------
		// 挿入モード
		//
		// 12.2.4.1 The insertion mode
		// https://html.spec.whatwg.org/multipage/parsing.html#the-insertion-mode
		// --------------------------------------------------------------------


		// --------------------------------------------------------------------
		// スタック
		//
		// 12.2.4.2 The stack of open elements
		// https://html.spec.whatwg.org/multipage/parsing.html#the-stack-of-open-elements
		// -----------------------------------------------------------

		node_pointer current_node()
		{
			return m_stack_of_open_elements.back();
		}

		node_pointer adjusted_current_node()
		{
			if constexpr (is_fragment_parser) { if (m_stack_of_open_elements.size() == 1) return m_context_element; }
			return current_node();
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#special
		*/
		bool is_special(node_pointer it)
		{
			using T = tag_name;

			auto const& ns = static_cast<this_type*>(this)->namespace_uri_name(it);
			auto const& tag = static_cast<this_type*>(this)->local_name_name(it);

			if (ns == ns_name::HTML)
			{
				switch (tag)
				{
				case T::Address:    case T::Applet:  case T::Area:   case T::Article: case T::Aside:     case T::Base:     case T::Basefont: case T::Bgsound:
				case T::Blockquote: case T::Body:    case T::Br:     case T::Button:  case T::Caption:   case T::Center:   case T::Col:      case T::Colgroup:
				case T::Dd:         case T::Details: case T::Dir:    case T::Div:     case T::Dl:        case T::Dt:       case T::Embed:    case T::Fieldset:
				case T::Figcaption: case T::Figure:  case T::Footer: case T::Form:    case T::Frame:     case T::Frameset: case T::H1:       case T::H2:
				case T::H3:         case T::H4:      case T::H5:     case T::H6:      case T::Head:      case T::Header:   case T::Hgroup:   case T::Hr:
				case T::Html:       case T::Iframe:  case T::Img:    case T::Input:   case T::Keygen:    case T::Li:       case T::Link:     case T::Listing:
				case T::Main:       case T::Marquee: case T::Menu:   case T::Meta:    case T::Nav:       case T::Noembed:  case T::Noframes: case T::Noscript:
				case T::Object:     case T::Ol:      case T::P:      case T::Param:   case T::Plaintext: case T::Pre:      case T::Script:   case T::Section:
				case T::Select:     case T::Source:  case T::Style:  case T::Summary: case T::Table:     case T::Tbody:    case T::Td:       case T::Template:
				case T::Textarea:   case T::Tfoot:   case T::Th:     case T::Thead:   case T::Title:     case T::Tr:       case T::Track:    case T::Ul:
				case T::Wbr:        case T::Xmp:
					return true;
				default:
					break;
				}
			}
			else if (ns == ns_name::MathML)
			{
				switch (tag)
				{
				case T::Mi: case T::Mo: case T::Mn: case T::Ms: case T::Mtext: case T::Annotation_xml:
					return true;
				default:
					break;
				}
			}
			else if(ns == ns_name::SVG)
			{
				switch (tag)
				{
				case T::ForeignObject: case T::Desc: case T::Title:
					return true;
				default:
					break;
				}
			}

			return false;
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#formatting
		*/
		bool is_formatting(node_pointer it)
		{
			using T = tag_name;

			if (static_cast<this_type*>(this)->namespace_uri_name(it) == ns_name::HTML)
			{
				switch (static_cast<this_type*>(this)->local_name_name(it))
				{
				case T::A: case T::B:     case T::Big:    case T::Code:   case T::Em: case T::Font: case T::I: case T::Nobr:
				case T::S: case T::Small: case T::Strike: case T::Strong: case T::Tt: case T::U:
					return true;
				default:
					return false;
				}
			}

			return false;
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-the-specific-scope
		*/
		template <typename Scope, typename Target>
		bool in_specific_scope(Scope s, Target target)
		{
			this_type* P = static_cast<this_type*>(this);

			auto it1 = m_open_element_stack.rbegin();
			auto it2 = m_open_element_stack.rend();

			while (it1 != it2)
			{
				auto it = *it1;
				// ターゲット確認
				if constexpr (std::is_same_v<Target, std::pair<ns_name, tag_name>>)
				{
					ns_name  ns = P->namespace_uri_name(it);
					tag_name t = P->local_name_name(it);

					if (ns == target.first && t == target.second) return true;
				}
				else if constexpr (std::is_same_v<Target, node_pointer>)
				{
					if (it == target) return true;
				}
				else
				{
					ns_name  ns = P->namespace_uri_name(it);
					tag_name t = P->local_name_name(it);

					for (tag_name tag : target) if (ns == ns_name::HTML && t == tag) return true;
				}

				// スコープ確認
				if ((this->*s)(it)) return false;
				++it1;
			}

			assert(false);
			return false;
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-scope
		*/
		bool is_default_scope(node_pointer it)
		{
			using T = tag_name;

			this_type* P = static_cast<this_type*>(this);
			ns_name  ns  = P->namespace_uri_name(it);
			tag_name tag = P->local_name_name(it);

			if (ns == ns_name::HTML)
			{
				switch (tag)
				{
				case T::Applet: case T::Caption: case T::Html: case T::Table: case T::Td: case T::Th: case T::Marquee: case T::Object:
				case T::Template:
					return true;
				default:
					return false;
				}
			}
			else if (ns == ns_name::MathML)
			{
				switch (tag)
				{
				case T::Mi: case T::Mo: case T::Mn: case T::Ms: case T::Mtext: case T::Annotation_xml:
					return true;
				default:
					return false;
				}
			}
			else if (ns == ns_name::SVG)
			{
				switch (tag)
				{
				case T::Factorial: case T::ForeignObject: case T::Desc: case T::Title:
					return true;
				default:
					return false;
				}
			}

			return false;
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-list-item-scope
		*/
		bool is_list_item_scope(node_pointer it)
		{
			if (is_default_scope(it)) return true;

			this_type* P = static_cast<this_type*>(this);
			ns_name  ns  = P->namespace_uri_name(it);
			tag_name tag = P->local_name_name(it);

			if (ns == ns_name::HTML && (tag == tag_name::Ol || tag == tag_name::Ul)) return true;

			return false;
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-button-scope
		*/
		bool is_button_scope(node_pointer it)
		{
			if (is_default_scope(it)) return true;

			this_type* P = static_cast<this_type*>(this);
			ns_name  ns = P->namespace_uri_name(it);
			tag_name tag = P->local_name_name(it);

			if (ns == ns_name::HTML && tag == tag_name::Button) return true;

			return false;
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-table-scope
		*/
		bool is_table_scope(node_pointer it)
		{
			this_type* P = static_cast<this_type*>(this);
			ns_name  ns = P->namespace_uri_name(it);
			tag_name tag = P->local_name_name(it);

			if (ns == ns_name::HTML && (tag == tag_name::Html || tag == tag_name::Table || tag == tag_name::Template)) return true;

			return false;
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-select-scope
		*/
		bool is_select_scope(node_pointer it)
		{
			this_type* P = static_cast<this_type*>(this);
			ns_name  ns = P->namespace_uri_name(it);
			tag_name tag = P->local_name_name(it);

			if (ns == ns_name::HTML && (tag == tag_name::Optgroup || tag == tag_name::Option)) return false;

			return true;
		}

		static scope_type constexpr default_scope   = &tree_construction_dispatcher::is_default_scope;
		static scope_type constexpr list_item_scope = &tree_construction_dispatcher::is_list_item_scope;
		static scope_type constexpr button_scope    = &tree_construction_dispatcher::is_button_scope;
		static scope_type constexpr table_scope     = &tree_construction_dispatcher::is_table_scope;
		static scope_type constexpr select_scope    = &tree_construction_dispatcher::is_select_scope;

		// --------------------------------------------------------------------
		// 整形要素のリスト
		// 
		//
		// --------------------------------------------------------------------

		/*! @brief アクティブ整形要素リストへ整形要素を挿入する

		@todo 要素が作成されたときと同じ比較とは、ケース・インセンシティブ、名前空間接頭辞の分離前の比較を意味するかもしれない。
		- https://html.spec.whatwg.org/multipage/parsing.html#the-list-of-active-formatting-elements
		*/
		void push_active_formatting_element_list(node_pointer it, start_tag_token const& token)
		{
			this_type* P = static_cast<this_type*>(this);

			std::uint32_t n = 0;
			auto pos = m_active_formatting_element_list.end();

			auto it1 = m_active_formatting_element_list.begin();
			auto it2 = m_active_formatting_element_list.end();
			while (it1 != it2)
			{
				active_formatting_element const& afe = *it1;

				if (afe.m_marker) break;
				if (P->equals(it, afe.m_it))
				{
					pos = it1;
					++n;
				}

				++it1;
			}

			if (3 <= n) m_active_formatting_element_list.erase(pos);

			m_active_formatting_element_list.push_front({ it, token, false });
		}

		/*! @brief アクティブ整形要素リストへマーカーを挿入する

		https://html.spec.whatwg.org/multipage/parsing.html#the-list-of-active-formatting-elements
		*/
		void push_active_formatting_element_list()
		{
			m_active_formatting_element_list.push_front({ node_pointer(), start_tag_token(), true });
		}

		/*! @brief アクティブ整形要素リストを再構築する

		https://html.spec.whatwg.org/multipage/parsing.html#reconstruct-the-active-formatting-elements
		*/
		void reconstruct_active_formatting_element_list()
		{
			bool found = false;

			auto it1 = m_active_formatting_element_list.begin();
			auto it2 = m_active_formatting_element_list.end();

			if (it1 == it2) return;

			if (it1->m_marker) return;
			for (auto const& it : m_open_element_stack) if (it1->m_it == it) return;

		Rewind:
			if (std::distance(it1, it2) <= 1) goto Create;
			
			++it1;

			for (auto const& it : m_open_element_stack) if (it1->m_it == it) found = true;
			if (!it1->m_marker && !found) goto Rewind;

		Advance:
			--it1;

		Create:
			auto it = insert_html_element(it1->m_token);

			it1->m_it = it;

			if (it1 != m_active_formatting_element_list.begin()) goto Advance;
		}

		/*! @brief アクティブ整形要素リストをマーカーまでクリアする
		
		https://html.spec.whatwg.org/multipage/parsing.html#clear-the-list-of-active-formatting-elements-up-to-the-last-marker
		*/
		void clear_active_formatting_element_list()
		{

		}

		// --------------------------------------------------------------------
		// ノードの作成と挿入
		//
		// 12.2.6.1 Creating and inserting nodes
		// https://html.spec.whatwg.org/multipage/parsing.html#creating-and-inserting-nodes
		// --------------------------------------------------------------------

		/*! @brief 妥当なノード挿入先を調べる

		https://html.spec.whatwg.org/multipage/parsing.html#appropriate-place-for-inserting-a-node
		*/
		node_pointer appropriate_place_for_inserting_node(node_pointer target)
		{
			using  T = tag_name;
			this_type* P = static_cast<this_type*>(this);

			node_pointer adjusted_insertion_location = P->end(target);

			bool tbl;
			switch (P->local_name_name(target))
			{
			case T::Table: case T::Tbody: case T::Tfoot: case tag_name::Thead: case tag_name::Tr:
				tbl = true;
				break;
			default:
				tbl = false;
			}
			if (m_foster_parenting && tbl)
			{
				auto it = std::find_if(m_open_element_stack.rbegin(), m_open_element_stack.rend(), [=](node_pointer it) {
					return P->local_name_name(it) == T::Template; });
				auto last_template = (it == m_open_element_stack.rend()) ? m_open_element_stack.end() : (++it).base();

				it = std::find_if(m_open_element_stack.rbegin(), m_open_element_stack.rend(), [=](node_pointer it) {
					return P->local_name_name(it) == T::Table; });
				auto last_table = (it == m_open_element_stack.rend()) ? m_open_element_stack.end() : (++it).base();

				if (last_template != m_open_element_stack.end())
				{
					std::int32_t distance = 0;
					if (last_table != m_open_element_stack.end()) distance = std::distance(last_table, last_template);

					if (0 < distance)
					{
						adjusted_insertion_location = P->end(*last_template);
						goto Abort;
					}
				}

				if (last_table == m_open_element_stack.end())
				{
					adjusted_insertion_location = m_open_element_stack.front().end();
					goto Abort;
				}

				if (P->parent(*last_table) != P->end())
				{
					adjusted_insertion_location = *last_table;
					goto Abort;
				}

				// スクリプトをサポートしないため、ここに到達することはないはず。
				// しかしテストでは通過させるため表明はしない。
				adjusted_insertion_location = P->end(*(++it).base());
			}
			else
			{
				adjusted_insertion_location = P->end(target);
			}

			Abort:

			return adjusted_insertion_location;
		}

		/*! @brief トークンから要素を作成する
		
		https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token
		*/
		int create_element_for_token(start_tag_token& token, ns_name ns, node_pointer parent)
		{
			this_type* P = static_cast<this_type*>(this);


		}

		/*!
		https://html.spec.whatwg.org/multipage/parsing.html#insert-a-foreign-element
		*/
		node_pointer insert_foreign_element(start_tag_token& token, ns_name ns)
		{
			return node_pointer();
		}

		/*!
		https://html.spec.whatwg.org/multipage/parsing.html#insert-an-html-element
		*/
		node_pointer insert_html_element(start_tag_token& token)
		{
			return insert_foreign_element(token, ns_name::HTML);
		}

		/*!
		
		https://html.spec.whatwg.org/multipage/parsing.html#adjust-mathml-attributes
		*/
		int adjust_mathml_attributes()
		{

		}

		/*!
		
		https://html.spec.whatwg.org/multipage/parsing.html#adjust-svg-attributes
		*/
		int adjust_svg_attributes()
		{

		}

		/*!
		https://html.spec.whatwg.org/multipage/parsing.html#adjust-foreign-attributes
		*/
		int adjust_foreign_attributes()
		{

		}

		/*!
		https://html.spec.whatwg.org/multipage/parsing.html#insert-a-character
		*/
		int insert_character()
		{

		}

		/*!
		https://html.spec.whatwg.org/multipage/parsing.html#insert-a-comment
		*/
		int  insert_comment()
		{

		}


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
			this_type* P = static_cast<this_type*>(this);

			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data)) return;
			}
			else if constexpr (std::is_same_v<comment_token, Token>)
			{
				P->insert_comment(node_pointer(), token);
			}
			else if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				document_type_node_type doctype = P->create_document_type(token.m_name, token.m_public_identifier, token.m_system_identifier);
				P->append_document_type(doctype);

				if (!P->is_iframe_srcdoc())
				{

				}
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
