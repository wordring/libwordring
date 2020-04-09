#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/html/parsing/parser_defs.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>

#include <wordring/whatwg/html/dom_defs.hpp>

#include <wordring/whatwg/infra/unicode.hpp>

#include <algorithm>
#include <deque>
#include <string>
#include <string_view>

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
		enum class mode_name : std::uint32_t
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
		mode_name m_insertion_mode;
		bool m_omit_lf;

		node_pointer m_context_element;


		// ----------------------------------------------------------------------------------------
		// 整形要素のリスト
		//
		// 12.2.4.3 The list of active formatting elements
		// https://html.spec.whatwg.org/multipage/parsing.html#the-list-of-active-formatting-elements
		// ----------------------------------------------------------------------------------------

		std::deque<active_formatting_element> m_active_formatting_element_list;

		// ----------------------------------------------------------------------------------------
		// 要素ポインタ
		//
		// 12.2.4.4 The element pointers
		// https://html.spec.whatwg.org/multipage/parsing.html#the-element-pointers
		// ----------------------------------------------------------------------------------------

		node_pointer m_head_element_pointer;
		node_pointer m_form_element_pointer;


		/*
		https://html.spec.whatwg.org/multipage/parsing.html#foster-parent
		*/
		bool m_foster_parenting;

	protected:
		tree_construction_dispatcher()
			: m_insertion_mode(mode_name::initial_insertion_mode)
			, m_omit_lf(false)
			, m_foster_parenting(false)
		{
		}

	protected:

		// ----------------------------------------------------------------------------------------
		// 挿入モード
		//
		// 12.2.4.1 The insertion mode
		// https://html.spec.whatwg.org/multipage/parsing.html#the-insertion-mode
		// ----------------------------------------------------------------------------------------

		void insertion_mode(mode_name mode)
		{
			m_insertion_mode = mode;
		}

		// --------------------------------------------------------------------
		// スタック
		//
		// 12.2.4.2 The stack of open elements
		// https://html.spec.whatwg.org/multipage/parsing.html#the-stack-of-open-elements
		// -----------------------------------------------------------

		node_pointer current_node()
		{
			return m_open_element_stack.back();
		}

		node_pointer adjusted_current_node()
		{
			if constexpr (is_fragments_parser) { if (m_open_element_stack.size() == 1) return m_context_element; }
			return current_node();
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#special
		*/
		bool is_special(node_pointer it)
		{
			auto const& ns = static_cast<this_type*>(this)->namespace_uri_name(it);
			auto const& tag = static_cast<this_type*>(this)->local_name_name(it);

			if (ns == ns_name::HTML)
			{
				switch (tag)
				{
				case tag_name::Address:    case tag_name::Applet:  case tag_name::Area:   case tag_name::Article: case tag_name::Aside:     case tag_name::Base:     case tag_name::Basefont: case tag_name::Bgsound:
				case tag_name::Blockquote: case tag_name::Body:    case tag_name::Br:     case tag_name::Button:  case tag_name::Caption:   case tag_name::Center:   case tag_name::Col:      case tag_name::Colgroup:
				case tag_name::Dd:         case tag_name::Details: case tag_name::Dir:    case tag_name::Div:     case tag_name::Dl:        case tag_name::Dt:       case tag_name::Embed:    case tag_name::Fieldset:
				case tag_name::Figcaption: case tag_name::Figure:  case tag_name::Footer: case tag_name::Form:    case tag_name::Frame:     case tag_name::Frameset: case tag_name::H1:       case tag_name::H2:
				case tag_name::H3:         case tag_name::H4:      case tag_name::H5:     case tag_name::H6:      case tag_name::Head:      case tag_name::Header:   case tag_name::Hgroup:   case tag_name::Hr:
				case tag_name::Html:       case tag_name::Iframe:  case tag_name::Img:    case tag_name::Input:   case tag_name::Keygen:    case tag_name::Li:       case tag_name::Link:     case tag_name::Listing:
				case tag_name::Main:       case tag_name::Marquee: case tag_name::Menu:   case tag_name::Meta:    case tag_name::Nav:       case tag_name::Noembed:  case tag_name::Noframes: case tag_name::Noscript:
				case tag_name::Object:     case tag_name::Ol:      case tag_name::P:      case tag_name::Param:   case tag_name::Plaintext: case tag_name::Pre:      case tag_name::Script:   case tag_name::Section:
				case tag_name::Select:     case tag_name::Source:  case tag_name::Style:  case tag_name::Summary: case tag_name::Table:     case tag_name::Tbody:    case tag_name::Td:       case tag_name::Template:
				case tag_name::Textarea:   case tag_name::Tfoot:   case tag_name::Th:     case tag_name::Thead:   case tag_name::Title:     case tag_name::Tr:       case tag_name::Track:    case tag_name::Ul:
				case tag_name::Wbr:        case tag_name::Xmp:
					return true;
				default:
					break;
				}
			}
			else if (ns == ns_name::MathML)
			{
				switch (tag)
				{
				case tag_name::Mi: case tag_name::Mo: case tag_name::Mn: case tag_name::Ms: case tag_name::Mtext: case tag_name::Annotation_xml:
					return true;
				default:
					break;
				}
			}
			else if(ns == ns_name::SVG)
			{
				switch (tag)
				{
				case tag_name::ForeignObject: case tag_name::Desc: case tag_name::Title:
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
			if (static_cast<this_type*>(this)->namespace_uri_name(it) == ns_name::HTML)
			{
				switch (static_cast<this_type*>(this)->local_name_name(it))
				{
				case tag_name::A: case tag_name::B:     case tag_name::Big:    case tag_name::Code:   case tag_name::Em: case tag_name::Font: case tag_name::I: case tag_name::Nobr:
				case tag_name::S: case tag_name::Small: case tag_name::Strike: case tag_name::Strong: case tag_name::Tt: case tag_name::U:
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
			this_type* P = static_cast<this_type*>(this);
			ns_name  ns  = P->namespace_uri_name(it);
			tag_name tag = P->local_name_name(it);

			if (ns == ns_name::HTML)
			{
				switch (tag)
				{
				case tag_name::Applet:   case tag_name::Caption: case tag_name::Html: case tag_name::Table: case tag_name::Td: case tag_name::Th: case tag_name::Marquee: case tag_name::Object:
				case tag_name::Template:
					return true;
				default:
					return false;
				}
			}
			else if (ns == ns_name::MathML)
			{
				switch (tag)
				{
				case tag_name::Mi: case tag_name::Mo: case tag_name::Mn: case tag_name::Ms: case tag_name::Mtext: case tag_name::Annotation_xml:
					return true;
				default:
					return false;
				}
			}
			else if (ns == ns_name::SVG)
			{
				switch (tag)
				{
				case tag_name::Factorial: case tag_name::ForeignObject: case tag_name::Desc: case tag_name::Title:
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

		// ----------------------------------------------------------------------------------------
		// 整形要素のリスト
		// 
		// 12.2.4.3 The list of active formatting elements
		// https://html.spec.whatwg.org/multipage/parsing.html#the-list-of-active-formatting-elements
		// ----------------------------------------------------------------------------------------

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


		// ----------------------------------------------------------------------------------------
		// ノードの作成と挿入
		//
		// 12.2.6.1 Creating and inserting nodes
		// https://html.spec.whatwg.org/multipage/parsing.html#creating-and-inserting-nodes
		// ----------------------------------------------------------------------------------------

		/*! @brief 妥当なノード挿入先を調べる

		https://html.spec.whatwg.org/multipage/parsing.html#appropriate-place-for-inserting-a-node
		*/
		node_pointer appropriate_place_for_inserting_node(node_pointer target)
		{
			this_type* P = static_cast<this_type*>(this);

			node_pointer adjusted_insertion_location = P->end(target);

			bool tbl;
			switch (P->local_name_name(target))
			{
			case tag_name::Table: case tag_name::Tbody: case tag_name::Tfoot: case tag_name::Thead: case tag_name::Tr:
				tbl = true;
				break;
			default:
				tbl = false;
			}
			if (m_foster_parenting && tbl)
			{
				auto it = std::find_if(m_open_element_stack.rbegin(), m_open_element_stack.rend(), [=](node_pointer it) {
					return P->local_name_name(it) == tag_name::Template; });
				auto last_template = (it == m_open_element_stack.rend()) ? m_open_element_stack.end() : (++it).base();

				it = std::find_if(m_open_element_stack.rbegin(), m_open_element_stack.rend(), [=](node_pointer it) {
					return P->local_name_name(it) == tag_name::Table; });
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

				if (P->parent(*last_table) != P->document())
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
		element_node_type create_element_for_token(start_tag_token& token, ns_name ns, node_pointer parent)
		{
			this_type* P = static_cast<this_type*>(this);

			element_node_type el = P->create_element(token.m_tag_name, ns, U"");
			

			return el;
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
		template <typename Token>
		void adjust_mathml_attributes(Token& token)
		{
			if constexpr (std::is_base_of_v<tag_token, Token>)
			{
				for (auto& attr : token.m_attributes)
				{
					if (attr.m_name == U"definitionurl") attr.m_name = U"definitionURL";
				}
			}
		}

		/*!	
		https://html.spec.whatwg.org/multipage/parsing.html#adjust-svg-attributes
		*/
		template <typename Token>
		void adjust_svg_attributes(Token& token)
		{
			if constexpr (std::is_base_of_v<tag_token, Token>)
			{
				for (auto& attr : token.m_attributes)
				{
					auto it = svg_attributes_conversion_tbl.find(attr.m_name);
					if(it != svg_attributes_conversion_tbl.end()) attr.m_name = it->second;
				}
			}
		}

		/*!
		https://html.spec.whatwg.org/multipage/parsing.html#adjust-foreign-attributes
		*/
		template <typename Token>
		void adjust_foreign_attributes(Token& token)
		{
			if constexpr (std::is_base_of_v<tag_token, Token>)
			{
				for (auto& attr : token.m_attributes)
				{
					auto it = foreign_attributes_conversion_tbl.find(attr.m_name);
					if (it != foreign_attributes_conversion_tbl.end())
					{
						attr.m_prefix     = it->second.m_prefix;
						attr.m_local_name = it->second.m_local_name;
						attr.m_namespace  = it->second.m_namespace;
					}
				}
			}
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
		void insert_comment(comment_token const& token, node_pointer pos)
		{
			this_type* P = static_cast<this_type*>(this);

			comment_node_type comment = P->create_comment(token.m_data);

			P->insert(pos, std::move(comment));
		}

		void insert_comment(comment_token const& token)
		{
			insert_comment(token, appropriate_place_for_inserting_node(current_node()));
		}
		// 

		template <typename Element>
		bool is_MathML_text_integration_point(Element c)
		{

		}



		template <typename Token>
		void process_token(mode_name mode, Token& token)
		{
			switch (mode)
			{
			case mode_name::initial_insertion_mode:              return on_initial_insertion_mode(token);
			case mode_name::before_html_insertion_mode:          return on_before_html_insertion_mode(token);
			case mode_name::before_head_insertion_mode:          return on_before_head_insertion_mode(token);
			case mode_name::in_head_insertion_mode:              return on_in_head_insertion_mode(token);
			case mode_name::in_head_noscript_insertion_mode:     return on_in_head_noscript_insertion_mode(token);
			case mode_name::after_head_insertion_mode:           return on_after_head_insertion_mode(token);
			case mode_name::in_body_insertion_mode:              return on_in_body_insertion_mode(token);
			case mode_name::text_insertion_mode:                 return on_text_insertion_mode(token);
			case mode_name::in_table_insertion_mode:             return on_in_table_insertion_mode(token);
			case mode_name::in_table_text_insertion_mode:        return on_in_table_text_insertion_mode(token);
			case mode_name::in_caption_insertion_mode:           return on_in_caption_insertion_mode(token);
			case mode_name::in_column_group_insertion_mode:      return on_in_column_group_insertion_mode(token);
			case mode_name::in_table_body_insertion_mode:        return on_in_table_body_insertion_mode(token);
			case mode_name::in_row_insertion_mode:               return on_in_row_insertion_mode(token);
			case mode_name::in_cell_insertion_mode:              return on_in_cell_insertion_mode(token);
			case mode_name::in_select_insertion_mode:            return on_in_select_insertion_mode(token);
			case mode_name::in_select_in_table_insertion_mode:   return on_in_select_in_table_insertion_mode(token);
			case mode_name::in_template_insertion_mode:          return on_in_template_insertion_mode(token);
			case mode_name::after_body_insertion_mode:           return on_after_body_insertion_mode(token);
			case mode_name::in_frameset_insertion_mode:          return on_in_frameset_insertion_mode(token);
			case mode_name::after_frameset_insertion_mode:       return on_after_frameset_insertion_mode(token);
			case mode_name::after_after_body_insertion_mode:     return on_after_after_body_insertion_mode(token);
			case mode_name::after_after_frameset_insertion_mode: return on_after_after_frameset_insertion_mode(token);
			}
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
			case mode_name::initial_insertion_mode:              return on_initial_insertion_mode(token);
			case mode_name::before_html_insertion_mode:          return on_before_html_insertion_mode(token);
			case mode_name::before_head_insertion_mode:          return on_before_head_insertion_mode(token);
			case mode_name::in_head_insertion_mode:              return on_in_head_insertion_mode(token);
			case mode_name::in_head_noscript_insertion_mode:     return on_in_head_noscript_insertion_mode(token);
			case mode_name::after_head_insertion_mode:           return on_after_head_insertion_mode(token);
			case mode_name::in_body_insertion_mode:              return on_in_body_insertion_mode(token);
			case mode_name::text_insertion_mode:                 return on_text_insertion_mode(token);
			case mode_name::in_table_insertion_mode:             return on_in_table_insertion_mode(token);
			case mode_name::in_table_text_insertion_mode:        return on_in_table_text_insertion_mode(token);
			case mode_name::in_caption_insertion_mode:           return on_in_caption_insertion_mode(token);
			case mode_name::in_column_group_insertion_mode:      return on_in_column_group_insertion_mode(token);
			case mode_name::in_table_body_insertion_mode:        return on_in_table_body_insertion_mode(token);
			case mode_name::in_row_insertion_mode:               return on_in_row_insertion_mode(token);
			case mode_name::in_cell_insertion_mode:              return on_in_cell_insertion_mode(token);
			case mode_name::in_select_insertion_mode:            return on_in_select_insertion_mode(token);
			case mode_name::in_select_in_table_insertion_mode:   return on_in_select_in_table_insertion_mode(token);
			case mode_name::in_template_insertion_mode:          return on_in_template_insertion_mode(token);
			case mode_name::after_body_insertion_mode:           return on_after_body_insertion_mode(token);
			case mode_name::in_frameset_insertion_mode:          return on_in_frameset_insertion_mode(token);
			case mode_name::after_frameset_insertion_mode:       return on_after_frameset_insertion_mode(token);
			case mode_name::after_after_body_insertion_mode:     return on_after_after_body_insertion_mode(token);
			case mode_name::after_after_frameset_insertion_mode: return on_after_after_frameset_insertion_mode(token);
			}
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.1 The "initial" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_initial_insertion_mode(Token& token)
		{
			this_type* P = static_cast<this_type*>(this);

			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(static_cast<character_token&>(token).m_data)) return;
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token, P->document().end());
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				document_type_node_type doctype = P->create_document_type(
					token.m_name, token.m_public_identifier, token.m_system_identifier);
				P->insert(P->end(P->document()), doctype);

				if (!P->is_iframe_srcdoc_document())
				{
					if (in_quirks_condition(token)) P->document_mode(document_mode_name::quirks);
					else if (in_limited_quirks_condition(token)) P->document_mode(document_mode_name::limited_quirks);
				}

				insertion_mode(mode_name::before_html_insertion_mode);

				return;
			}

			if (!P->is_iframe_srcdoc_document())
			{
				P->report_error();
				P->document_mode(document_mode_name::quirks);
			}

			insertion_mode(mode_name::before_html_insertion_mode);
			reprocess_token(token);
		}

		template <typename Token>
		bool in_quirks_condition(Token const& token) const
		{
			if (token.m_force_quirks_flag) return true;
			if (token.m_name != U"html") return true;
			if (token.m_public_identifier == U"-//w3o//dtd w3 html strict 3.0//en//"
				|| token.m_public_identifier == U"-/w3c/dtd html 4.0 transitional/en"
				|| token.m_public_identifier == U"html") return true;
			if (token.m_system_identifier == U"http://www.ibm.com/data/dtd/v11/ibmxhtml1-transitional.dtd") return true;

			auto it1 = token.m_public_identifier.begin();
			auto it2 = token.m_public_identifier.end();
			if (quirks_mode_tbl.lookup(it1, it2).first) return true;

			if (token.m_system_identifier.empty())
			{
				std::u32string_view constexpr sv1(U"-//w3c//dtd html 4.01 frameset//");
				if (sv1.size() <= token.m_public_identifier.size()
					&& std::equal(sv1.begin(), sv1.end(), token.m_public_identifier.begin())) return true;

				std::u32string_view constexpr sv2(U"-//w3c//dtd html 4.01 transitional//");
				if (sv2.size() <= token.m_public_identifier.size()
					&& std::equal(sv2.begin(), sv2.end(), token.m_public_identifier.begin())) return true;
			}

			return false;
		}

		template <typename Token>
		bool in_limited_quirks_condition(Token const& token) const
		{
			std::u32string_view constexpr sv1(U"-//w3c//dtd xhtml 1.0 frameset//");
			if (sv1.size() <= token.m_public_identifier.size()
				&& std::equal(sv1.begin(), sv1.end(), token.m_public_identifier.begin())) return true;

			std::u32string_view constexpr sv2(U"-//w3c//dtd xhtml 1.0 transitional//");
			if (sv2.size() <= token.m_public_identifier.size()
				&& std::equal(sv2.begin(), sv2.end(), token.m_public_identifier.begin())) return true;

			if (!token.m_system_identifier.empty())
			{
				std::u32string_view constexpr sv1(U"-//w3c//dtd html 4.01 frameset//");
				if (sv1.size() <= token.m_public_identifier.size()
					&& std::equal(sv1.begin(), sv1.end(), token.m_public_identifier.begin())) return true;

				std::u32string_view constexpr sv2(U"-//w3c//dtd html 4.01 transitional//");
				if (sv2.size() <= token.m_public_identifier.size()
					&& std::equal(sv2.begin(), sv2.end(), token.m_public_identifier.begin())) return true;
			}

			return false;
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.2 The "before html" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#the-before-html-insertion-mode
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_before_html_insertion_mode(Token& token)
		{
			this_type* P = static_cast<this_type*>(this);

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				report_error();
				return;
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token, P->document().end());
				return;
			}

			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data)) return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					element_node_type el = create_element_for_token(token, ns_name::HTML, P->document());
					node_pointer it = P->insert(P->document().end(), el);
					m_open_element_stack.push_back(it);
					insertion_mode(mode_name::before_head_insertion_mode);

					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Head: case tag_name::Body: case tag_name::Html: case tag_name::Br:
					goto AnythingElse;
				default:
					report_error();

					return;
				}
			}

			goto AnythingElse;
		AnythingElse:
			element_node_type el = P->create_element(tag_name::Html);
			node_pointer it = P->insert(P->document().end(), el);
			P->set_document(it, P->document());
			m_open_element_stack.push_back(it);
			insertion_mode(mode_name::before_head_insertion_mode);
			reprocess_token(token);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.3 The "before head" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#the-before-head-insertion-mode
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_before_head_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(static_cast<character_token&>(token).m_data)) return;
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token);
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					process_token(mode_name::in_body_insertion_mode, token);
					return;
				}

				if (token.m_tag_name_id == tag_name::Head)
				{
					node_pointer it = insert_html_element(token);
					m_head_element_pointer = it;
					insertion_mode(mode_name::in_head_insertion_mode);

					return;
				}
			}
			
			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Head: case tag_name::Body: case tag_name::Html: case tag_name::Br:
					goto AnythingElse;
				default:
					break;
				}

				report_error();
				return;
			}

			goto AnythingElse;
		AnythingElse:
			start_tag_token tkn(tag_name::Head);
			node_pointer it = insert_html_element(tkn);
			m_head_element_pointer = it;
			insertion_mode(mode_name::in_head_insertion_mode);
			reprocess_token(token);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.4 The "in head" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_head_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data))
				{

					return;
				}
			}

		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_head_noscript_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_after_head_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_body_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_text_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_table_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_table_text_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_caption_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_column_group_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_table_body_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_row_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_cell_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_select_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_select_in_table_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_template_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_after_body_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_frameset_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_after_frameset_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_after_after_body_insertion_mode(Token& token)
		{
		}

		// ----------------------------------------------------------------------------------------
		// 
		//
		// 
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_after_after_frameset_insertion_mode(Token& token)
		{
		}
	};

}
