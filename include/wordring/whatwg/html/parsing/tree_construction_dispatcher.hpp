#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/html/parsing/parser_defs.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>

#include <wordring/whatwg/html/html_defs.hpp>
#include <wordring/whatwg/html/url.hpp>

#include <wordring/whatwg/infra/infra.hpp>
#include <wordring/whatwg/infra/unicode.hpp>

#include <wordring/whatwg/encoding/encoding.hpp>

#include <algorithm>
#include <deque>
#include <list>
#include <string>
#include <string_view>

namespace wordring::whatwg::html::parsing
{
	/*! @class tree_construction_dispatcher tree_construction_dispatcher.hpp wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp
	
	@brief HTML5 パーサーの木構築段階

	@tparam T          CRTP に基づく派生クラス
	@tparam NodeTraits ノードの特性

	派生クラスはいくつかのメンバを持たなければならない。

	@sa wordring::html::simple_parser_base

	NodeTraits はノードに対する操作を行うアダプタとなる。

	@sa wordring::html::simple_traits
	*/
	template <typename T, typename NodeTraits>
	class tree_construction_dispatcher : public tokenizer<T, NodeTraits>
	{
		friend tokenizer<T, NodeTraits>;

	protected:
		using base_type = tokenizer<T, NodeTraits>;
		using this_type = T;

		using traits = NodeTraits;

		using string_type  = typename traits::string_type;
		using node_pointer = typename traits::node_pointer;

	public:
		// ----------------------------------------------------------------------------------------
		// 入力バイトストリーム
		//
		// 12.2.3 The input byte stream
		// https://html.spec.whatwg.org/multipage/parsing.html#the-input-byte-stream
		// ----------------------------------------------------------------------------------------

		encoding_confidence_name m_encoding_confidence;
		encoding_name m_encoding_name;

		// ----------------------------------------------------------------------------------------
		// 挿入モード
		//
		// 12.2.4.1 The insertion mode
		// https://html.spec.whatwg.org/multipage/parsing.html#the-insertion-mode
		// ----------------------------------------------------------------------------------------

		using mode_type  = void(tree_construction_dispatcher::*)();
		using scope_type = bool(tree_construction_dispatcher::*)(node_pointer) const;

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

		mode_name m_insertion_mode;
		mode_name m_original_insertion_mode;
		std::deque<mode_name> m_template_insertion_mode_stack;

		// ----------------------------------------------------------------------------------------
		// スタック
		//
		// 12.2.4.2 The stack of open elements
		// https://html.spec.whatwg.org/multipage/parsing.html#the-stack-of-open-elements
		// ----------------------------------------------------------------------------------------

		struct stack_entry
		{
			start_tag_token m_token;
			node_pointer    m_it;
		};

		std::deque<stack_entry> m_stack;

		// ----------------------------------------------------------------------------------------
		// アクティブ整形要素のリスト
		//
		// 12.2.4.3 The list of active formatting elements
		// https://html.spec.whatwg.org/multipage/parsing.html#the-list-of-active-formatting-elements
		// ----------------------------------------------------------------------------------------

		struct active_formatting_element
		{
			start_tag_token m_token;
			node_pointer    m_it;
			bool            m_marker;
		};

		/*! @brief アクティブ整形要素のリスト
		*/
		std::list<active_formatting_element> m_list;

		// ----------------------------------------------------------------------------------------
		// 要素ポインタ
		//
		// 12.2.4.4 The element pointers
		// https://html.spec.whatwg.org/multipage/parsing.html#the-element-pointers
		// ----------------------------------------------------------------------------------------

		node_pointer m_head_element_pointer;
		node_pointer m_form_element_pointer;

		// ----------------------------------------------------------------------------------------
		// 他のパーサ状態フラグ
		//
		// 12.2.4.5 Other parsing state flags
		// https://html.spec.whatwg.org/multipage/parsing.html#other-parsing-state-flags
		// ----------------------------------------------------------------------------------------

		bool m_scripting_flag;
		bool m_frameset_ok_flag;

		// ----------------------------------------------------------------------------------------
		// ノードの作成と挿入
		//
		// 12.2.6.1 Creating and inserting nodes
		// https://html.spec.whatwg.org/multipage/parsing.html#creating-and-inserting-nodes
		// ----------------------------------------------------------------------------------------

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#foster-parent
		*/
		bool m_foster_parenting;

		// ----------------------------------------------------------------------------------------
		// HTML フラグメント
		//
		// 12.4 Parsing HTML fragments
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-html-fragments
		// ----------------------------------------------------------------------------------------

		bool m_fragments_parser;

		stack_entry m_context_entry;

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.7 The "in body" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inbody
		// ----------------------------------------------------------------------------------------

		bool m_omit_lf;

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.9 The "in table" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intable
		// ----------------------------------------------------------------------------------------

		std::vector<character_token> m_pending_table_character_tokens;

	public:
		tree_construction_dispatcher(encoding_confidence_name confidence, encoding_name enc, bool fragments_parser = false)
			: m_encoding_confidence(confidence)
			, m_encoding_name(enc)
			, m_insertion_mode(mode_name::initial_insertion_mode)
			, m_original_insertion_mode(static_cast<mode_name>(0))
			, m_head_element_pointer(traits::pointer())
			, m_form_element_pointer(traits::pointer())
			, m_scripting_flag(false)
			, m_frameset_ok_flag(true)
			, m_foster_parenting(false)
			, m_fragments_parser(fragments_parser)
			, m_omit_lf(false)
		{
		}

		/*! @brief 初期状態に戻し、パーサーを再利用可能とする
		*/
		void clear(encoding_confidence_name confidence, encoding_name enc)
		{
			base_type::clear();

			m_encoding_confidence = confidence;
			m_encoding_name       = enc;

			m_insertion_mode          = mode_name::initial_insertion_mode;
			m_original_insertion_mode = static_cast<mode_name>(0);
			m_template_insertion_mode_stack.clear();

			m_stack.clear();

			m_list.clear();

			m_head_element_pointer = traits::pointer();
			m_form_element_pointer = traits::pointer();

			m_scripting_flag   = false;
			m_frameset_ok_flag = true;

			m_foster_parenting = false;

			m_context_entry = stack_entry();

			m_omit_lf = false;

			m_pending_table_character_tokens.clear();
		}

		/*! 要素が指定のHTML要素であることを調べる

		@param [in] it  要素を指すポインタあるいはイテレータ
		@param [in] tag タグ名

		@return it が tag を名前として持つHTML要素である場合 true 、そうでない場合、 false を返す

		このメンバは規格に無い。
		*/
		bool is_html_element_of(node_pointer it, tag_name tag) const
		{
			return traits::get_namespace_name(it) == ns_name::HTML && traits::get_local_name_name(it) == tag;
		}

		/*! 要素が指定のHTML要素であることを調べる

		@param [in] it  要素を指すポインタあるいはイテレータ
		@param [in] tag タグ名

		@return it が tag を名前として持つHTML要素である場合 true 、そうでない場合、 false を返す

		このメンバは規格に無い。
		*/
		bool is_html_element_of(node_pointer it, std::u32string const& tag) const
		{
			this_type const* P = static_cast<this_type const*>(this);

			return traits::get_namespace_name(it) == ns_name::HTML && encoding_cast<std::u32string>(traits::get_local_name(it)) == tag;
		}

		/*! 要素が指定の要素であることを調べる

		@param [in] it  要素を指すポインタあるいはイテレータ
		@param [in] ns  名前空間名
		@param [in] tag タグ名

		@return it が ns 名前空間の tag を名前として持つ要素である場合 true 、そうでない場合、 false を返す

		このメンバは規格に無い。
		*/
		bool is_element_of(node_pointer it, ns_name ns, tag_name tag) const
		{
			return traits::get_namespace_name(it) == ns && traits::get_local_name_name(it) == tag;
		}


		// ----------------------------------------------------------------------------------------
		// 符号化法
		//
		// 12.2.3.3 Character encodings
		// https://html.spec.whatwg.org/multipage/parsing.html#character-encodings
		// ----------------------------------------------------------------------------------------

		/*! @brief 文字エンコーディングを変更する
		*/
		void change_encoding(encoding_name name)
		{
			this_type* P = static_cast<this_type*>(this);

			// 1.
			if (m_encoding_name == encoding_name::UTF_16BE || m_encoding_name == encoding_name::UTF_16LE)
			{
				m_encoding_confidence = encoding_confidence_name::certain;
				return;
			}
			// 2.
			if (name == encoding_name::UTF_16BE || name == encoding_name::UTF_16LE) name = encoding_name::UTF_8;
			// 3.
			if (name == encoding_name::x_user_defined) name = encoding_name::windows_1252;
			// 4.
			if (name == m_encoding_name)
			{
				m_encoding_confidence = encoding_confidence_name::certain;
				return;
			}
			// 6.
			m_encoding_confidence = encoding_confidence_name::certain;

			P->on_change_encoding(name);
		}

		// ----------------------------------------------------------------------------------------
		// 挿入モード
		//
		// 12.2.4.1 The insertion mode
		// https://html.spec.whatwg.org/multipage/parsing.html#the-insertion-mode
		// ----------------------------------------------------------------------------------------

		/*! @brief 挿入モードを設定する
		*/
		void insertion_mode(mode_name mode)
		{
			m_insertion_mode = mode;
		}

		mode_name current_template_insertion_mode() const
		{
			return m_template_insertion_mode_stack.back();
		}

		/*! @brief 挿入モードを適切に設定し直す

		- https://triple-underscore.github.io/HTML-parsing-ja.html#reset-the-insertion-mode-appropriately
		- https://html.spec.whatwg.org/multipage/parsing.html#reset-the-insertion-mode-appropriately
		*/
		void reset_insertion_mode_appropriately()
		{
			assert(!m_stack.empty());

			bool last = false;
			auto it = --m_stack.end();
			node_pointer node = it->m_it;

		Loop1:
			if (it == m_stack.begin())
			{
				last = true;
				if (m_fragments_parser) node = m_context_entry.m_it;
			}

			// 4.
			if (is_html_element_of(node, tag_name::Select))
			{
				node_pointer ancestor;

				if (last) goto Done;
				ancestor = node;
			Loop2:
				if (it == m_stack.begin()) goto Done;
				
				--it;
				ancestor = it->m_it;

				if (is_html_element_of(ancestor, tag_name::Template)) goto Done;

				if (is_html_element_of(ancestor, tag_name::Table))
				{
					insertion_mode(mode_name::in_select_in_table_insertion_mode);
					return;
				}

				goto Loop2;
			Done:
				insertion_mode(mode_name::in_select_insertion_mode);
				return;
			}

			// 5.
			if((is_html_element_of(node, tag_name::Td) || is_html_element_of(node, tag_name::Th))
				&& last == false) return insertion_mode(mode_name::in_cell_insertion_mode);
			// 6.
			if (is_html_element_of(node, tag_name::Tr)) return insertion_mode(mode_name::in_row_insertion_mode);
			// 7.
			if (is_html_element_of(node, tag_name::Tbody)
			 || is_html_element_of(node, tag_name::Thead)
			 || is_html_element_of(node, tag_name::Tfoot)) return insertion_mode(mode_name::in_table_body_insertion_mode);
			// 8.
			if (is_html_element_of(node, tag_name::Caption)) return insertion_mode(mode_name::in_caption_insertion_mode);
			// 9.
			if (is_html_element_of(node, tag_name::Colgroup)) return insertion_mode(mode_name::in_column_group_insertion_mode);
			// 10.
			if (is_html_element_of(node, tag_name::Table)) return insertion_mode(mode_name::in_table_insertion_mode);
			// 11.
			if (is_html_element_of(node, tag_name::Template)) return insertion_mode(current_template_insertion_mode());
			// 12.
			if (is_html_element_of(node, tag_name::Head) && last == false) return insertion_mode(mode_name::in_head_insertion_mode);
			// 13.
			if (is_html_element_of(node, tag_name::Body)) return insertion_mode(mode_name::in_body_insertion_mode);
			// 14.
			if (is_html_element_of(node, tag_name::Frameset)) return insertion_mode(mode_name::in_frameset_insertion_mode);
			// 15.
			if (is_html_element_of(node, tag_name::Html))
			{
				if (m_head_element_pointer == traits::pointer()) return insertion_mode(mode_name::before_head_insertion_mode);
				return insertion_mode(mode_name::after_head_insertion_mode);
			}
			// 16.
			if (last == true) return insertion_mode(mode_name::in_body_insertion_mode);
			// 17.
			--it;
			node = it->m_it;
			// 18.
			goto Loop1;
		}

		// ----------------------------------------------------------------------------------------
		// スタック
		//
		// 12.2.4.2 The stack of open elements
		// https://html.spec.whatwg.org/multipage/parsing.html#the-stack-of-open-elements
		// ----------------------------------------------------------------------------------------

		/*! @brief スタックに指定されたタグが有るか調べる

		@param [in] ns  名前空間名
		@param [in] tag タグ名

		@return スタック内に名前空間とタグ名が一致する要素が存在する場合 true 、そうでない場合 false を返す

		このメンバは規格に無い。
		*/
		bool contains(ns_name ns, tag_name tag) const
		{
			for (stack_entry const& se : m_stack)
			{
				if (is_element_of(se.m_it, ns, tag)) return true;
			}

			return false;
		}

		/*! @brief スタックに指定されたタグが有るか調べる

		@param [in] tag タグ名

		@return スタック内にタグ名が一致するHTML要素が存在する場合 true 、そうでない場合 false を返す

		このメンバは規格に無い。
		*/
		bool contains(tag_name name) const
		{
			for (stack_entry const& se : m_stack)
			{
				if (is_html_element_of(se.m_it, name)) return true;
			}

			return false;
		}

		/*! @brief スタックに指定されたポインタが有るか調べる

		@param [in] it 要素を指すポインタあるいはイテレータ

		@return スタック内に一致するポインタが存在する場合 true 、そうでない場合 false を返す

		このメンバは規格に無い。
		*/
		bool contains(node_pointer it) const
		{
			for (stack_entry const& se : m_stack) { if (se.m_it == it) return true; }
			return false;
		}

		/*! @brief 現在のノードを返す
		
		- https://html.spec.whatwg.org/multipage/parsing.html#current-node
		- https://triple-underscore.github.io/HTML-parsing-ja.html#current-node
		*/
		stack_entry& current_node() { return m_stack.back(); }

		stack_entry const& current_node() const { return m_stack.back(); }

		/*! @brief 現在の調整済みノードを返す
		
		- https://html.spec.whatwg.org/multipage/parsing.html#adjusted-current-node
		- https://triple-underscore.github.io/HTML-parsing-ja.html#adjusted-current-node
		*/
		stack_entry& adjusted_current_node()
		{
			if (m_fragments_parser) { if (m_stack.size() == 1) return m_context_entry; }
			return current_node();
		}

		stack_entry const& adjusted_current_node() const
		{
			if (m_fragments_parser) { if (m_stack.size() == 1) return m_context_entry; }
			return current_node();
		}

		/*! @brief 特別な要素か調べる

		@param [in] it 要素を指すポインタあるいはイテレータ

		@return it が特別な要素の場合 true を返す

		- https://html.spec.whatwg.org/multipage/parsing.html#special
		- https://triple-underscore.github.io/HTML-parsing-ja.html#special
		*/
		bool is_special(node_pointer it) const
		{
			ns_name  ns  = traits::get_namespace_name(it);
			tag_name tag = traits::get_local_name_name(it);

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

		/*! @brief 整形要素か調べる

		@param [in] it 要素を指すポインタあるいはイテレータ

		@return it が整形要素の場合 true を返す

		- https://html.spec.whatwg.org/multipage/parsing.html#formatting
		- https://triple-underscore.github.io/HTML-parsing-ja.html#formatting
		*/
		bool is_formatting(node_pointer it) const
		{
			if (traits::get_namespace_name(it) == ns_name::HTML)
			{
				switch (traits::get_local_name_name(it))
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

		/*! @brief 要素が特定の視野内（スコープ内）にあるか調べる

		@param [in] s スコープ
		@param [in] target ターゲット条件

		@return 視野内にある場合 true を返す

		スコープに指定する値は以下の通り。
		- default_scope
		- list_item_scope
		- button_scope
		- table_scope
		- select_scope

		ターゲット条件に指定する型は以下の通り。
		- std::pair<ns_name, tag_name>
			+ 名前空間とタグ名のペア
		- tag_name
			+ タグ名
		- std::u32string
			+ タグ名
		- std::array<tag_name, n>
			+ タグ名の配列
		- node_pointer
			+ 要素へのポインタあるいはイテレータ
		.

		- https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-the-specific-scope
		- https://triple-underscore.github.io/HTML-parsing-ja.html#has-an-element-in-the-specific-scope
		*/
		template <typename Scope, typename Target>
		bool in_specific_scope(Scope s, Target const& target) const
		{
			auto it1 = m_stack.rbegin();
			auto it2 = m_stack.rend();

			while (it1 != it2)
			{
				auto it = it1->m_it;

				// ターゲット確認
				if constexpr (std::is_same_v<Target, std::pair<ns_name, tag_name>>)
				{
					if (is_element_of(it, target.first, target.second)) return true;
				}
				else if constexpr (std::is_same_v<Target, node_pointer>)
				{
					if (it == target) return true;
				}
				else if constexpr (std::is_same_v<Target, tag_name>)
				{
					if (is_html_element_of(it, target)) return true;
				}
				else if constexpr (std::is_same_v<Target, std::u32string>)
				{
					if (is_html_element_of(it, target)) return true;
				}
				else // [h1-h6], [tbody, thead, tfoot] に対応するstd::array
				{
					for (tag_name tag : target) if (is_html_element_of(it, tag)) return true;
				}

				// スコープ確認
				if ((this->*s)(it)) return false;
				++it1;
			}

			assert(false);
			return false;
		}

		/*! @brief デフォルトのスコープ内の特定な要素か調べる

		@param [in] 要素を指すポインタあるいはイテレータ

		@return 要素が特定な要素の場合 true を返す

		- https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-scope
		- https://triple-underscore.github.io/HTML-parsing-ja.html#has-an-element-in-scope
		*/
		bool is_default_scope(node_pointer it) const
		{
			ns_name  ns  = traits::get_namespace_name(it);
			tag_name tag = traits::get_local_name_name(it);

			if (ns == ns_name::HTML)
			{
				switch (tag)
				{
				case tag_name::Applet: case tag_name::Caption: case tag_name::Html:   case tag_name::Table:    case tag_name::Td:
				case tag_name::Th:     case tag_name::Marquee: case tag_name::Object: case tag_name::Template:
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

		/*! @brief リスト・アイテムのスコープ内の特定な要素か調べる

		@param [in] 要素を指すポインタあるいはイテレータ

		@return 要素が特定な要素の場合 true を返す

		- https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-list-item-scope
		- https://triple-underscore.github.io/HTML-parsing-ja.html#has-an-element-in-list-item-scope
		*/
		bool is_list_item_scope(node_pointer it) const
		{
			if (is_default_scope(it)) return true;
			if (is_html_element_of(it, tag_name::Ol) || is_html_element_of(it, tag_name::Ul)) return true;

			return false;
		}

		/*! @brief ボタンのスコープ内の特定な要素か調べる

		@param [in] 要素を指すポインタあるいはイテレータ

		@return 要素が特定な要素の場合 true を返す

		- https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-button-scope
		- https://triple-underscore.github.io/HTML-parsing-ja.html#has-an-element-in-button-scope
		*/
		bool is_button_scope(node_pointer it) const
		{
			if (is_default_scope(it)) return true;
			if (is_html_element_of(it, tag_name::Button)) return true;

			return false;
		}

		/*! @brief テーブルのスコープ内の特定な要素か調べる

		@param [in] 要素を指すポインタあるいはイテレータ

		@return 要素が特定な要素の場合 true を返す

		- https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-table-scope
		- https://triple-underscore.github.io/HTML-parsing-ja.html#has-an-element-in-table-scope
		*/
		bool is_table_scope(node_pointer it) const
		{
			if (is_html_element_of(it, tag_name::Html)
			 || is_html_element_of(it, tag_name::Table)
			 || is_html_element_of(it, tag_name::Template)) return true;

			return false;
		}

		/*! @brief セレクトのスコープ内の特定な要素か調べる

		@param [in] 要素を指すポインタあるいはイテレータ

		@return 要素が特定な要素の場合 true を返す

		- https://html.spec.whatwg.org/multipage/parsing.html#has-an-element-in-select-scope
		- https://triple-underscore.github.io/HTML-parsing-ja.html#has-an-element-in-select-scope
		*/
		bool is_select_scope(node_pointer it) const
		{
			if (is_html_element_of(it, tag_name::Optgroup) || is_html_element_of(it, tag_name::Option)) return false;

			return true;
		}

		static scope_type constexpr default_scope   = &tree_construction_dispatcher::is_default_scope;
		static scope_type constexpr list_item_scope = &tree_construction_dispatcher::is_list_item_scope;
		static scope_type constexpr button_scope    = &tree_construction_dispatcher::is_button_scope;
		static scope_type constexpr table_scope     = &tree_construction_dispatcher::is_table_scope;
		static scope_type constexpr select_scope    = &tree_construction_dispatcher::is_select_scope;

		/*! @brief スタックから指定のタグが現れるまでPOPする

		@param [in] ns  名前空間
		@param [in] tag タグ名

		指定のタグ名を持つ項目自身もPOPされる。
		このメンバは規格に無い。

		- https://triple-underscore.github.io/HTML-parsing-ja.html#_pop-until
		*/
		void pop_until(ns_name ns, tag_name tag)
		{
			while (!m_stack.empty())
			{
				bool f = is_element_of(m_stack.back().m_it, ns, tag);
				m_stack.pop_back();
				if (f) break;
			}
		}

		/*! @brief スタックから指定のタグが現れるまでPOPする

		@param [in] ns         名前空間
		@param [in] conditions タグ名の配列

		タグ名の配列に指定する型は以下の通り。
		- std::array<tag_name, n>

		指定のタグ名を持つ項目自身もPOPされる。
		このメンバは規格に無い。

		- https://triple-underscore.github.io/HTML-parsing-ja.html#_pop-until
		*/
		template <typename T1>
		void pop_until(ns_name ns, T1 conditions)
		{
			auto it1 = std::begin(conditions);
			auto it2 = std::end(conditions);
			while (!m_stack.empty())
			{
				node_pointer it = m_stack.back().m_it;
				bool f = std::find_if(it1, it2, [this, ns, it](tag_name tag) { return is_element_of(it, ns, tag); }) != it2;
				m_stack.pop_back();
				if (f) break;
			}
		}

		/*! @brief スタックから指定の要素が現れるまでPOPする

		@param [in] it 要素を指すポインタあるいはイテレータ

		指定の要素を持つ項目自身もPOPされる。
		このメンバは規格に無い。

		- https://triple-underscore.github.io/HTML-parsing-ja.html#_pop-until
		*/
		void pop_until(node_pointer it)
		{
			while (!m_stack.empty())
			{
				node_pointer p = m_stack.back().m_it;
				m_stack.pop_back();
				if (p == it) break;
			}
		}

		/*! @brief スタックから指定の要素を持つ項目を検索する
		
		@param [in] it 要素を指すポインタあるいはイテレータ

		@return スタック内で指定の要素を持つ項目を指すイテレータ

		このメンバは規格に無い。
		*/
		auto find(node_pointer it)
		{
			auto it1 = m_stack.begin();
			auto it2 = m_stack.end();
			while (it1 != it2)
			{
				if (it1->m_it == it) break;
				++it1;
			}

			return it1;
		}

		/*! @brief スタックから指定の要素を持つ項目を除去する

		@param [in] it 要素を指すポインタあるいはイテレータ

		このメンバは規格に無い。
		*/
		void remove(node_pointer it)
		{
			auto it1 = std::find_if(m_stack.begin(), m_stack.end(), [it](stack_entry const& se) {
				return se.m_it == it; });
			assert(it1 != m_stack.end());
			m_stack.erase(it1);
		}

		// ----------------------------------------------------------------------------------------
		// 整形要素のリスト
		// 
		// 12.2.4.3 The list of active formatting elements
		// https://html.spec.whatwg.org/multipage/parsing.html#the-list-of-active-formatting-elements
		// ----------------------------------------------------------------------------------------

		/*! @brief 整形要素リストへ整形要素を挿入する

		@param [in] it 整形要素を指すポインタあるいはイテレータ
		@param [in] token 整形要素を作成するために使われた開始タグ・トークン

		- https://html.spec.whatwg.org/multipage/parsing.html#push-onto-the-list-of-active-formatting-elements
		- https://triple-underscore.github.io/HTML-parsing-ja.html#push-onto-the-list-of-active-formatting-elements

		@todo
			要素が作成されたときと同じ比較とは、ケース・インセンシティブ、名前空間接頭辞の分離前の比較を意味するかもしれない。
			あるいは、実際にトークンと比較するのかもしれない。
			この実装は、スクリプトをサポートしないため、トークンと要素の属性は違いが無い。
			したがって、要素同士を比較している。
		*/
		void push_formatting_element_list(node_pointer it, start_tag_token const& token)
		{
			std::uint32_t n = 0;
			auto pos = m_list.end();

			auto it1 = m_list.end();
			auto it2 = m_list.begin();
			while (it1 != it2)
			{
				--it1;
				if (it1->m_marker) break;
				if (traits::equals(it, it1->m_it))
				{
					pos = it1;
					++n;
				}
			}

			if (3 <= n) m_list.erase(pos);

			m_list.push_back({ token, it, false });
		}

		/*! @brief アクティブ整形要素リストへマーカーを挿入する

		- https://html.spec.whatwg.org/multipage/parsing.html#push-onto-the-list-of-active-formatting-elements
		- https://triple-underscore.github.io/HTML-parsing-ja.html#push-onto-the-list-of-active-formatting-elements
		*/
		void push_formatting_element_list()
		{
			m_list.push_back({ start_tag_token(), node_pointer(), true });
		}

		/*! @brief アクティブ整形要素リストを再構築する

		- https://html.spec.whatwg.org/multipage/parsing.html#reconstruct-the-active-formatting-elements
		- https://triple-underscore.github.io/HTML-parsing-ja.html#reconstruct-the-active-formatting-elements
		*/
		void reconstruct_formatting_element_list()
		{
			node_pointer new_element; // 8.

			// 1.
			if (m_list.empty()) return;
			// 2.
			if (m_list.back().m_marker || contains(m_list.back().m_it)) return;
			// 3.
			auto entry = --m_list.end();
			// 4.
		Rewind:
			if (entry == m_list.begin()) goto Create;
			// 5.
			--entry;
			// 6.
			if (!entry->m_marker && !contains(entry->m_it)) goto Rewind;
			// 7.
		Advance:
			++entry;
			//8.
		Create:
			new_element = insert_html_element(entry->m_token);
			// 9.
			entry->m_it = new_element;
			// 10.
			if (std::next(entry) != m_list.end()) goto Advance;
		}

		/*! @brief アクティブ整形要素リストをマーカーまでクリアする
		
		- https://html.spec.whatwg.org/multipage/parsing.html#clear-the-list-of-active-formatting-elements-up-to-the-last-marker
		- https://triple-underscore.github.io/HTML-parsing-ja.html#clear-the-list-of-active-formatting-elements-up-to-the-last-marker
		*/
		void clear_formatting_element_list()
		{
			while (!m_list.empty())
			{
				bool marker = m_list.back().m_marker;
				m_list.pop_back();
				if (marker) break;
			}
		}

		/*! @brief リストから指定の要素を持つ項目を検索する

		@param [in] it 要素を指すポインタあるいはイテレータ

		@return リスト内で指定の要素を持つ項目を指すイテレータ

		このメンバは規格に無い。
		*/
		auto find_from_list(node_pointer it)
		{
			auto it1 = m_list.begin();
			auto it2 = m_list.end();
			while (it1 != it2)
			{
				if (it1->m_it == it) break;
				++it1;
			}

			return it1;
		}

		/*! @brief 整形要素リストから指定の要素を持つ項目を削除する

		@param [in] it 要素を指すポインタあるいはイテレータ

		このメンバは規格に無い。
		*/
		void remove_from_list(node_pointer it)
		{
			auto it1 = find_from_list(it);
			if (it1 != m_list.end()) m_list.erase(it1);
		}

		// ----------------------------------------------------------------------------------------
		// ツリー構築
		//
		// 12.2.6 Tree construction
		// https://html.spec.whatwg.org/multipage/parsing.html#tree-construction
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void process_token(mode_name mode, Token& token)
		{
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (m_omit_lf && token.m_data == U'\xA')
				{
					m_omit_lf = false;
					return;
				}
			}
			m_omit_lf = false;

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				// https://html.spec.whatwg.org/multipage/parsing.html#tokenization

				if (!token.m_attributes.empty()) base_type::report_error(error_name::end_tag_with_attributes);
				if (token.m_self_closing_flag) base_type::report_error(error_name::end_tag_with_trailing_solidus);
			}

			if (m_stack.empty()) goto Html;
			else
			{
				stack_entry& entry = adjusted_current_node();

				if (traits::get_namespace_name(entry.m_it) == ns_name::HTML) goto Html;
				if constexpr (std::is_same_v<start_tag_token, Token>)
				{
					if (is_mathml_text_integration_point(entry)
						&& token.m_tag_name_id != tag_name::Mglyph
						&& token.m_tag_name_id != tag_name::Malignmark) goto Html;

					if (traits::get_local_name_name(entry.m_it) == tag_name::Annotation_xml
						&& token.m_tag_name_id == tag_name::Svg) goto Html;

					if (is_html_integration_point(entry)) goto Html;
				}
				if constexpr (std::is_same_v<character_token, Token>)
				{
					if (is_mathml_text_integration_point(entry)) goto Html;
					if (is_html_integration_point(entry)) goto Html;
				}
				if constexpr (std::is_same_v<end_of_file_token, Token>) goto Html;
			}

			on_foreign_content(token);
			goto Finish;

		Html:
			switch (mode)
			{
			case mode_name::initial_insertion_mode:
				on_initial_insertion_mode(token);
				break;
			case mode_name::before_html_insertion_mode:
				on_before_html_insertion_mode(token);
				break;
			case mode_name::before_head_insertion_mode:
				on_before_head_insertion_mode(token);
				break;
			case mode_name::in_head_insertion_mode:
				on_in_head_insertion_mode(token);
				break;
			case mode_name::in_head_noscript_insertion_mode:
				on_in_head_noscript_insertion_mode(token);
				break;
			case mode_name::after_head_insertion_mode:
				on_after_head_insertion_mode(token);
				break;
			case mode_name::in_body_insertion_mode:
				on_in_body_insertion_mode(token);
				break;
			case mode_name::text_insertion_mode:
				on_text_insertion_mode(token);
				break;
			case mode_name::in_table_insertion_mode:
				on_in_table_insertion_mode(token);
				break;
			case mode_name::in_table_text_insertion_mode:
				on_in_table_text_insertion_mode(token);
				break;
			case mode_name::in_caption_insertion_mode:
				on_in_caption_insertion_mode(token);
				break;
			case mode_name::in_column_group_insertion_mode:
				on_in_column_group_insertion_mode(token);
				break;
			case mode_name::in_table_body_insertion_mode:
				on_in_table_body_insertion_mode(token);
				break;
			case mode_name::in_row_insertion_mode:
				on_in_row_insertion_mode(token);
				break;
			case mode_name::in_cell_insertion_mode:
				on_in_cell_insertion_mode(token);
				break;
			case mode_name::in_select_insertion_mode:
				on_in_select_insertion_mode(token);
				break;
			case mode_name::in_select_in_table_insertion_mode:
				on_in_select_in_table_insertion_mode(token);
				break;
			case mode_name::in_template_insertion_mode:
				on_in_template_insertion_mode(token);
				break;
			case mode_name::after_body_insertion_mode:
				on_after_body_insertion_mode(token);
				break;
			case mode_name::in_frameset_insertion_mode:
				on_in_frameset_insertion_mode(token);
				break;
			case mode_name::after_frameset_insertion_mode:
				on_after_frameset_insertion_mode(token);
				break;
			case mode_name::after_after_body_insertion_mode:
				on_after_after_body_insertion_mode(token);
				break;
			case mode_name::after_after_frameset_insertion_mode:
				on_after_after_frameset_insertion_mode(token);
				break;
			}

		Finish:
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				// https://html.spec.whatwg.org/multipage/parsing.html#acknowledge-self-closing-flag

				if(token.m_self_closing_flag && !token.m_acknowledged_self_closing_flag)
					base_type::report_error(error_name::non_void_html_element_start_tag_with_trailing_solidus);
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
			process_token(m_insertion_mode, token);
		}

		bool is_mathml_text_integration_point(stack_entry const& entry) const
		{
			if (traits::get_namespace_name(entry.m_it) == ns_name::MathML)
			{
				switch (traits::get_local_name_name(entry.m_it))
				{
				case tag_name::Mi: case tag_name::Mo: case tag_name::Mn: case tag_name::Ms: case tag_name::Mtext:
					return true;
				default:
					break;
				}
			}

			return false;
		}

		bool is_html_integration_point(stack_entry const& entry) const
		{
			ns_name  ns  = traits::get_namespace_name(entry.m_it);
			tag_name tag = traits::get_local_name_name(entry.m_it);

			if (ns == ns_name::MathML && tag == tag_name::Annotation_xml)
			{
				auto it = entry.m_token.find(U"encoding");
				if (it != entry.m_token.end())
				{
					std::u32string s;
					to_ascii_lowercase(it->m_value.begin(), it->m_value.end(), std::back_inserter(s));
					if (s == U"text/html" || s == U"application/xhtml+xml") return true;
				}
			}
			else if (ns == ns_name::SVG)
			{
				if (tag == tag_name::ForeignObject
				 || tag == tag_name::Desc
				 || tag == tag_name::Title) return true;
			}

			return false;
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

			node_pointer adjusted_insertion_location = traits::end(target);

			bool f = traits::get_namespace_name(target) == ns_name::HTML;
			if (f)
			{
				switch (traits::get_local_name_name(target))
				{
				case tag_name::Table: case tag_name::Tbody: case tag_name::Tfoot: case tag_name::Thead: case tag_name::Tr:
					f = true;
					break;
				default:
					f = false;
				}
			}

			if (m_foster_parenting && f)
			{
				auto it = std::find_if(m_stack.rbegin(), m_stack.rend(), [this](stack_entry const& entry) {
					return is_html_element_of(entry.m_it, tag_name::Template); });
				auto last_template = (it == m_stack.rend()) ? m_stack.end() : (++it).base();

				it = std::find_if(m_stack.rbegin(), m_stack.rend(), [this](stack_entry const& entry) {
					return is_html_element_of(entry.m_it, tag_name::Table); });
				auto last_table = (it == m_stack.rend()) ? m_stack.end() : (++it).base();

				if (last_template != m_stack.end())
				{
					std::int32_t distance = 0;
					if (last_table != m_stack.end()) distance = static_cast<std::int32_t>(std::distance(last_table, last_template));

					if (0 < distance)
					{
						adjusted_insertion_location = traits::end(last_template->m_it);
						goto Abort;
					}
				}

				if (last_table == m_stack.end())
				{
					adjusted_insertion_location = m_stack.front().m_it.end();
					goto Abort;
				}

				if (traits::parent(last_table->m_it) != P->get_document())
				{
					adjusted_insertion_location = last_table->m_it;
					goto Abort;
				}

				// スクリプトをサポートしないため、ここに到達することはないはず。
				// しかしテストでは通過させるため表明はしない。
				adjusted_insertion_location = traits::end((++it).base()->m_it);
			}
			else
			{
				adjusted_insertion_location = traits::end(target);
			}

		Abort:
			return adjusted_insertion_location;
		}

		/*! @brief トークンから要素を作成する
		
		@sa https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token
		@sa https://triple-underscore.github.io/HTML-parsing-ja.html#create-an-element-for-the-token
		*/
		node_pointer create_element_for_token(start_tag_token& token, ns_name ns, node_pointer intended_parent)
		{
			//TODO: 未実装
			this_type* P = static_cast<this_type*>(this);

			node_pointer el;
			if (token.m_tag_name_id != static_cast<tag_name>(0))
			{
				el = P->create_element(P->get_node_document(intended_parent), token.m_tag_name_id, ns);
			}
			else
			{
				assert(!token.m_tag_name.empty());
				el = P->create_element(P->get_node_document(intended_parent), token.m_tag_name, ns);
			}

			for (token_attribute const& a : token)
			{
				if (a.m_omitted) continue;
				P->append_attribute(el, a.m_namespace, a.m_prefix, a.m_name, a.m_value);
			}

			return el;
		}

		/*!
		https://html.spec.whatwg.org/multipage/parsing.html#insert-a-foreign-element
		*/
		node_pointer insert_foreign_element(start_tag_token& token, ns_name ns)
		{
			this_type* P = static_cast<this_type*>(this);

			node_pointer adjusted_insertion_location = appropriate_place_for_inserting_node(current_node().m_it);
			node_pointer el = create_element_for_token(token, ns, traits::parent(adjusted_insertion_location));
			node_pointer it = traits::pointer();

			//TODO: 要素を挿入可能か検査する機構が必要

			it = P->insert_element(adjusted_insertion_location, el);

			m_stack.push_back({ token, it });

			return it;
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
				for (auto& a : token.m_attributes)
				{
					auto it = foreign_attributes_conversion_tbl.find(a.m_name);
					if (it != foreign_attributes_conversion_tbl.end())
					{
						a.m_namespace = it->second.m_namespace;
						a.m_prefix    = it->second.m_prefix;
						a.m_name      = it->second.m_local_name;
					}
				}
			}
		}

		/*!
		@sa https://html.spec.whatwg.org/multipage/parsing.html#insert-a-character
		@sa 
		*/
		void insert_character(char32_t cp)
		{
			this_type* P = static_cast<this_type*>(this);

			node_pointer it1 = appropriate_place_for_inserting_node(current_node().m_it);

			node_pointer parent = traits::parent(it1);
			if (parent == P->get_document()) return;

			if (it1 != traits::begin(parent))
			{
				node_pointer prev = traits::prev(it1);
				if (traits::is_text(prev))
				{
					traits::append_text(prev, cp);
					return;
				}
			}

			auto text = traits::create_text(cp);
			node_pointer it2 = P->insert_text(it1, std::move(text));
			traits::set_document(it2, P->get_document());
		}

		/*!
		https://html.spec.whatwg.org/multipage/parsing.html#insert-a-comment
		*/
		void insert_comment(comment_token const& token, node_pointer pos)
		{
			this_type* P = static_cast<this_type*>(this);

			auto comment = traits::create_comment(encoding_cast<string_type>(token.m_data));

			P->insert_comment(pos, std::move(comment));
		}

		void insert_comment(comment_token const& token)
		{
			insert_comment(token, appropriate_place_for_inserting_node(current_node().m_it));
		}

		// ----------------------------------------------------------------------------------------
		// テキストのみ含む要素の解析
		//
		// 12.2.6.2 Parsing elements that contain only text
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-elements-that-contain-only-text
		// ----------------------------------------------------------------------------------------

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#generic-raw-text-element-parsing-algorithm
		*/
		void parse_generic_raw_text_element(start_tag_token& token)
		{
			insert_html_element(token);
			base_type::change_state(base_type::RAWTEXT_state);
			m_original_insertion_mode = m_insertion_mode;
			insertion_mode(mode_name::text_insertion_mode);
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#generic-rcdata-element-parsing-algorithm
		*/
		void parse_generic_raw_rcdata_element(start_tag_token& token)
		{
			insert_html_element(token);
			base_type::change_state(base_type::RCDATA_state);
			m_original_insertion_mode = m_insertion_mode;
			insertion_mode(mode_name::text_insertion_mode);
		}

		// ----------------------------------------------------------------------------------------
		// 暗黙の終了タグを持つ要素を閉じる
		//
		// 12.2.6.3 Closing elements that have implied end tags
		// https://html.spec.whatwg.org/multipage/parsing.html#closing-elements-that-have-implied-end-tags
		// ----------------------------------------------------------------------------------------

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#generate-implied-end-tags
		*/
		template <typename TagName>
		void generate_implied_end_tags(TagName without)
		{
			while (!m_stack.empty())
			{
				auto it = m_stack.back().m_it;
				tag_name tag = traits::get_local_name_name(it);
				ns_name  ns  = traits::get_namespace_name(it);

				if constexpr (std::is_same_v<TagName, tag_name>)
				{
					if (tag == without) return;
				}
				if constexpr (std::is_same_v<TagName, std::pair<ns_name, tag_name>>)
				{
					if (ns == without.first && tag == without.second) return;
				}
				if constexpr (std::is_same_v<TagName, std::pair<ns_name, std::u32string>>)
				{
					if (ns == without.first && encoding_cast<std::u32string>(traits::get_local_name(it)) == without.second) return;
				}

				if (ns != ns_name::HTML) return;
				switch (tag)
				{
				case tag_name::Dd: case tag_name::Dt: case tag_name::Li: case tag_name::Optgroup: case tag_name::Option:
				case tag_name::P:  case tag_name::Rb: case tag_name::Rp: case tag_name::Rt:       case tag_name::Rtc:
					break;
				default:
					return;
				}

				m_stack.pop_back();
			}
		}

		void generate_implied_end_tags()
		{
			generate_implied_end_tags(0);
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#generate-all-implied-end-tags-thoroughly
		*/
		void generate_all_implied_end_tags_thoroughly()
		{
			while (!m_stack.empty())
			{
				auto it = m_stack.back().m_it;
				tag_name tag = traits::get_local_name_name(it);
				ns_name  ns  = traits::get_namespace_name(it);

				if (ns != ns_name::HTML) return;
				switch (tag)
				{
				case tag_name::Caption: case tag_name::Colgroup: case tag_name::Dd:    case tag_name::Dt: case tag_name::Li:    case tag_name::Optgroup:
				case tag_name::Option:  case tag_name::P:        case tag_name::Rb:    case tag_name::Rp: case tag_name::Rt:    case tag_name::Rtc:
				case tag_name::Tbody:   case tag_name::Td:       case tag_name::Tfoot: case tag_name::Th: case tag_name::Thead: case tag_name::Tr:
					break;
				default:
					return;
				}

				m_stack.pop_back();
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
				if (is_ascii_white_space(token.m_data)) return;
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token, P->get_document().end());
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				auto doctype = traits::create_document_type(
					encoding_cast<string_type>(token.m_name),
					encoding_cast<string_type>(token.m_public_identifier),
					encoding_cast<string_type>(token.m_system_identifier));
				P->insert_document_type(traits::end(P->get_document()), std::move(doctype));

				if (!traits::is_iframe_srcdoc_document(P->get_document()))
				{
					if (in_quirks_condition(token)) traits::set_document_mode(P->get_document(), document_mode_name::Quirks);
					else if (in_limited_quirks_condition(token)) traits::set_document_mode(P->get_document(), document_mode_name::LimitedQuirks);
				}

				insertion_mode(mode_name::before_html_insertion_mode);
				return;
			}

			if (!traits::is_iframe_srcdoc_document(P->get_document()))
			{
				P->report_error();
				traits::set_document_mode(P->get_document(), document_mode_name::Quirks);
			}

			insertion_mode(mode_name::before_html_insertion_mode);
			reprocess_token(token);
		}

		template <typename Token>
		bool in_quirks_condition(Token const& token) const
		{
			if (token.m_force_quirks_flag) return true;
			if (token.m_name != U"html") return true;

			std::u32string ipublic;
			to_ascii_lowercase(token.m_public_identifier.begin(), token.m_public_identifier.end(), std::back_inserter(ipublic));

			std::u32string isystem;
			to_ascii_lowercase(token.m_system_identifier.begin(), token.m_system_identifier.end(), std::back_inserter(isystem));

			if (ipublic == U"-//w3o//dtd w3 html strict 3.0//en//") return true;
			if (ipublic == U"-/w3c/dtd html 4.0 transitional/en") return true;
			if (ipublic == U"html") return true;

			if (isystem == U"http://www.ibm.com/data/dtd/v11/ibmxhtml1-transitional.dtd") return true;

			if (quirks_mode_tbl.lookup(ipublic.begin(), ipublic.end()).first) return true;

			if (token.m_system_identifier.empty())
			{
				std::u32string_view constexpr sv1(U"-//w3c//dtd html 4.01 frameset//");
				if (sv1.size() <= ipublic.size() && std::equal(sv1.begin(), sv1.end(), ipublic.begin())) return true;

				std::u32string_view constexpr sv2(U"-//w3c//dtd html 4.01 transitional//");
				if (sv2.size() <= ipublic.size() && std::equal(sv2.begin(), sv2.end(), ipublic.begin())) return true;
			}

			return false;
		}

		template <typename Token>
		bool in_limited_quirks_condition(Token const& token) const
		{
			if (istart_with(U"-//w3c//dtd xhtml 1.0 frameset//", token.m_public_identifier)) return true;
			if (istart_with(U"-//w3c//dtd xhtml 1.0 transitional//", token.m_public_identifier)) return true;

			if (!token.m_system_identifier.empty())
			{
				if (istart_with(U"-//w3c//dtd html 4.01 frameset//", token.m_public_identifier)) return true;
				if (istart_with(U"-//w3c//dtd html 4.01 transitional//", token.m_public_identifier)) return true;
			}

			return false;
		}

		bool istart_with(std::u32string_view s1, std::u32string const& s2) const
		{
			if (s2.size() < s1.size()) return false;

			auto it = s2.begin();
			for (char32_t c : s1)
			{
				char32_t cp = *it++;
				if (is_ascii_upper_alpha(cp)) cp += 0x20;
				if (cp != c) return false;
			}

			return true;
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
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token, P->get_document().end());
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
					node_pointer el = create_element_for_token(token, ns_name::HTML, P->get_document());
					node_pointer it = P->insert_element(P->get_document().end(), el);
					m_stack.push_back({ token, it });
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
					base_type::report_error();
					return;
				}
			}

			goto AnythingElse;
		AnythingElse:
			node_pointer el = P->create_element(P->get_document(), tag_name::Html, ns_name::HTML);
			node_pointer it = P->insert_element(P->get_document().end(), std::move(el));
			traits::set_document(it, P->get_document());
			m_stack.push_back({ start_tag_token(), it });
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
				if (is_ascii_white_space(token.m_data)) return ;
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token);
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					on_in_body_insertion_mode(token);
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
					base_type::report_error();
					return;
				}
			}

			goto AnythingElse;
		AnythingElse:
			start_tag_token t(tag_name::Head);
			node_pointer it = insert_html_element(t);
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
			this_type* P = static_cast<this_type*>(this);
			bool f;

			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data))
				{
					insert_character(token.m_data);
					return;
				}
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token);
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Html:
					on_in_body_insertion_mode(token);
					return;
				case tag_name::Base: case tag_name::Basefont: case tag_name::Bgsound: case tag_name::Link:
					insert_html_element(token);
					m_stack.pop_back();
					if (token.m_self_closing_flag == true) token.m_acknowledged_self_closing_flag = true;
					return;
				case tag_name::Meta:
				{
					insert_html_element(token);
					m_stack.pop_back();
					if (token.m_self_closing_flag == true) token.m_acknowledged_self_closing_flag = true;
					if (m_encoding_confidence != encoding_confidence_name::tentative) return;

					auto it = std::find_if(token.begin(), token.end(), [](token_attribute const& attr) { return attr.m_name == U"charset"; });
					if (it != token.end())
					{
						encoding_name en = get_encoding_name(it->m_value);
						if (en != static_cast<encoding_name>(0))
						{
							change_encoding(en);
							return;
						}
					}

					it = std::find_if(token.begin(), token.end(), [](token_attribute const& attr) { return attr.m_name == U"http-equiv"; });
					if (it != token.end())
					{
						auto sv = std::u32string_view(U"Content-Type");
						if (is_ascii_case_insensitive_match(it->m_value.begin(), it->m_value.end(), sv.begin(), sv.end()))
						{
							it = std::find_if(token.begin(), token.end(), [](token_attribute const& attr) { return attr.m_name == U"content"; });
							if (it != token.end())
							{
								encoding_name en = extract_character_encoding_from_meta_element(it->m_value);
								if (en != static_cast<encoding_name>(0)) change_encoding(en);
							}
						}
					}

					return;
				}
				case tag_name::Title:
					parse_generic_raw_rcdata_element(token);
					return;
				case tag_name::Noframes: case tag_name::Style:
					parse_generic_raw_text_element(token);
					return;
				case tag_name::Noscript:
					if (m_scripting_flag)
					{
						parse_generic_raw_text_element(token);
						return;
					}
					insert_html_element(token);
					insertion_mode(mode_name::in_head_noscript_insertion_mode);
					return;
				case tag_name::Script:
				{
					node_pointer adjusted_insertion_location = appropriate_place_for_inserting_node(current_node().m_it);
					node_pointer el = create_element_for_token(token, ns_name::HTML, traits::parent(adjusted_insertion_location));
					node_pointer it = P->insert_element(adjusted_insertion_location, el);
					traits::set_document(it, P->get_document());
					traits::set_non_blocking_flag(it, false);
					if (m_fragments_parser) traits::set_already_started_flag(it, true);

					//TODO: スクリプトの実行を防ぐを無条件に設定している
					traits::set_already_started_flag(it, true);
					m_stack.push_back({ token, it });
					base_type::change_state(base_type::script_data_state);
					m_original_insertion_mode = m_insertion_mode;
					insertion_mode(mode_name::text_insertion_mode);
					return;
				}
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Head:
					assert(is_html_element_of(current_node().m_it, tag_name::Head));
					m_stack.pop_back();
					insertion_mode(mode_name::after_head_insertion_mode);
					return;
				case tag_name::Body: case tag_name::Html: case tag_name::Br:
					goto AnythingElse;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Template)
				{
					insert_html_element(token);
					push_formatting_element_list();
					m_frameset_ok_flag = false;
					insertion_mode(mode_name::in_template_insertion_mode);
					m_template_insertion_mode_stack.push_back(mode_name::in_template_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Template)
				{
					if (!contains(ns_name::HTML, tag_name::Template))
					{
						base_type::report_error();
						return;
					}
					generate_all_implied_end_tags_thoroughly();
					if (!is_html_element_of(current_node().m_it, tag_name::Template)) base_type::report_error();
					pop_until(ns_name::HTML, tag_name::Template);
					clear_formatting_element_list();
					m_template_insertion_mode_stack.pop_back();
					reset_insertion_mode_appropriately();
					return;
				}
			}

			f = false;
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Head) f = true;
			}
			else if constexpr (std::is_same_v<end_tag_token, Token>) f = true;

			if(f)
			{
				base_type::report_error();
				return;
			}

			goto AnythingElse;
		AnythingElse:
			assert(is_html_element_of(current_node().m_it, tag_name::Head));
			m_stack.pop_back();
			insertion_mode(mode_name::after_head_insertion_mode);
			reprocess_token(token);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.5 The "in head noscript" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inheadnoscript
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_head_noscript_insertion_mode(Token& token)
		{
			bool f;

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					on_in_body_insertion_mode(token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Noscript)
				{
					assert(is_html_element_of(current_node().m_it, tag_name::Noscript));
					m_stack.pop_back();
					assert(is_html_element_of(current_node().m_it, tag_name::Head));
					insertion_mode(mode_name::in_head_insertion_mode);
					return;
				}
			}

			f = false;
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data)) f = true;
			}
			else if constexpr (std::is_same_v<comment_token, Token>) f = true;
			else if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Basefont: case tag_name::Bgsound: case tag_name::Link: case tag_name::Meta:
				case tag_name::Noframes: case tag_name::Style:
					f = true;
					break;
				default:
					break;
				}
			}

			if (f)
			{
				on_in_head_insertion_mode(token);
				return;
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Br) goto AnythingElse;
			}

			f = false;
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Head: case tag_name::Noscript:
					f = true;
					break;
				default:
					break;
				}
			}
			else if constexpr (std::is_same_v<end_tag_token, Token>) f = true;

			if (f)
			{
				base_type::report_error();
				return;
			}

			goto AnythingElse;
		AnythingElse:
			base_type::report_error();
			assert(is_html_element_of(current_node().m_it, tag_name::Noscript));
			m_stack.pop_back();
			assert(is_html_element_of(current_node().m_it, tag_name::Head));
			insertion_mode(mode_name::in_head_insertion_mode);
			reprocess_token(token);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.6 The "after head" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#the-after-head-insertion-mode
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_after_head_insertion_mode(Token& token)
		{
			bool f;

			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data))
				{
					insert_character(token.m_data);
					return;
				}
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token);
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Html:
					on_in_body_insertion_mode(token);
					return;
				case tag_name::Body:
					insert_html_element(token);
					m_frameset_ok_flag = false;
					insertion_mode(mode_name::in_body_insertion_mode);
					return;
				case tag_name::Frameset:
					insert_html_element(token);
					insertion_mode(mode_name::in_frameset_insertion_mode);
					return;
				case tag_name::Base:     case tag_name::Basefont: case tag_name::Bgsound: case tag_name::Link:
				case tag_name::Meta:     case tag_name::Noframes: case tag_name::Script:  case tag_name::Style:
				case tag_name::Template: case tag_name::Title:
					base_type::report_error();
					m_stack.push_back({ start_tag_token(), m_head_element_pointer });
					on_in_head_insertion_mode(token);
					m_stack.erase(std::remove_if(m_stack.begin(), m_stack.end(), [this](stack_entry const& entry) {
						return entry.m_it == m_head_element_pointer; }), m_stack.end());
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Template:
					on_in_head_insertion_mode(token);
					return;
				case tag_name::Body: case tag_name::Html: case tag_name::Br:
					goto AnythingElse;
				default:
					break;
				}
			}

			f = false;
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Head) f = true;
			}
			if constexpr (std::is_same_v<end_tag_token, Token>) f = true;

			if (f)
			{
				base_type::report_error();
				return;
			}

			goto AnythingElse;
		AnythingElse:
			start_tag_token t(tag_name::Body);
			insert_html_element(t);
			insertion_mode(mode_name::in_body_insertion_mode);
			reprocess_token(token);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.7 The "in body" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inbody
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_body_insertion_mode(Token& token)
		{
			this_type* P = static_cast<this_type*>(this);
			bool f;

			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (token.m_data == U'\0')
				{
					base_type::report_error();
					return;
				}

				if (is_ascii_white_space(token.m_data))
				{
					reconstruct_formatting_element_list();
					insert_character(token.m_data);
					return;
				}

				reconstruct_formatting_element_list();
				insert_character(token.m_data);
				m_frameset_ok_flag = false;
				return;
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token);
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					base_type::report_error();
					if (contains(ns_name::HTML, tag_name::Template)) return;
					node_pointer it = m_stack.front().m_it;
					for (token_attribute const& a : token)
					{
						if (a.m_omitted) continue;
						if (!P->contains(it, a.m_namespace, a.m_prefix, a.m_name))
						{
							P->append_attribute(it, a.m_namespace, a.m_prefix, a.m_name, a.m_value);
						}
					}
					return;
				}
			}

			f = false;
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Base:     case tag_name::Basefont: case tag_name::Bgsound: case tag_name::Link:     case tag_name::Meta:
				case tag_name::Noframes: case tag_name::Script:   case tag_name::Style:   case tag_name::Template: case tag_name::Title:
					f = true;
					break;
				default:
					break;
				}
			}
			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Template) f = true;
			}

			if (f)
			{
				on_in_head_insertion_mode(token);
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Body)
				{
					base_type::report_error();
					if (m_stack.size() == 1
						|| is_html_element_of(m_stack[1].m_it, tag_name::Body)
						|| contains(ns_name::HTML, tag_name::Template)) return;

					m_frameset_ok_flag = false;
					node_pointer it = m_stack[1].m_it;
					for (token_attribute const& a : token)
					{
						if (a.m_omitted) continue;
						if (!P->contains(it, a.m_namespace, a.m_prefix, a.m_name))
						{
							P->append_attribute(it, a.m_namespace, a.m_prefix, a.m_name, a.m_value);
						}
					}
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Frameset)
				{
					base_type::report_error();
					if (m_stack.size() == 1
					 || is_html_element_of(m_stack[1].m_it, tag_name::Body)) return;

					if (m_frameset_ok_flag == false) return;

					node_pointer it = traits::parent(m_stack[1].m_it);
					if (traits::parent(it) != P->get_document()) P->erase_element(it);
					while (!is_html_element_of(current_node().m_it, tag_name::Html)) m_stack.pop_back();
					insert_html_element(token);
					insertion_mode(mode_name::in_frameset_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<end_of_file_token, Token>)
			{
				if (!m_template_insertion_mode_stack.empty())
				{
					on_in_template_insertion_mode(token);
					return;
				}

				for (stack_entry const& se : m_stack)
				{
					switch (traits::get_local_name_name(se.m_it))
					{
					case tag_name::Dd:    case tag_name::Dt: case tag_name::Li:    case tag_name::Optgroup: case tag_name::Option: case tag_name::P:
					case tag_name::Rb:    case tag_name::Rp: case tag_name::Rt:    case tag_name::Rtc:      case tag_name::Tbody:  case tag_name::Td:
					case tag_name::Tfoot: case tag_name::Th: case tag_name::Thead: case tag_name::Tr:       case tag_name::Body:   case tag_name::Html:
						if(traits::get_namespace_name(se.m_it) == ns_name::HTML) continue;
					default:
						base_type::report_error();
						break;
					}
				}

				stop_parsing();
				return;
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Body)
				{
					if (!in_specific_scope(default_scope, tag_name::Body))
					{
						base_type::report_error();
						return;
					}

					for (stack_entry const& se : m_stack)
					{
						switch (traits::get_local_name_name(se.m_it))
						{
						case tag_name::Dd:    case tag_name::Dt: case tag_name::Li:    case tag_name::Optgroup: case tag_name::Option: case tag_name::P:
						case tag_name::Rb:    case tag_name::Rp: case tag_name::Rt:    case tag_name::Rtc:      case tag_name::Tbody:  case tag_name::Td:
						case tag_name::Tfoot: case tag_name::Th: case tag_name::Thead: case tag_name::Tr:       case tag_name::Body:   case tag_name::Html:
							if (traits::get_namespace_name(se.m_it) == ns_name::HTML) continue;
						default:
							base_type::report_error();
							break;
						}
					}

					insertion_mode(mode_name::after_body_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					if (!in_specific_scope(default_scope, tag_name::Body))
					{
						base_type::report_error();
						return;
					}

					for (stack_entry const& se : m_stack)
					{
						switch (traits::get_local_name_name(se.m_it))
						{
						case tag_name::Dd:    case tag_name::Dt: case tag_name::Li:    case tag_name::Optgroup: case tag_name::Option: case tag_name::P:
						case tag_name::Rb:    case tag_name::Rp: case tag_name::Rt:    case tag_name::Rtc:      case tag_name::Tbody:  case tag_name::Td:
						case tag_name::Tfoot: case tag_name::Th: case tag_name::Thead: case tag_name::Tr:       case tag_name::Body:   case tag_name::Html:
							if (traits::get_namespace_name(se.m_it) == ns_name::HTML) continue;
						default:
							base_type::report_error();
							break;
						}
					}

					insertion_mode(mode_name::after_body_insertion_mode);
					reprocess_token(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Address: case tag_name::Article: case tag_name::Aside:    case tag_name::Blockquote:
				case tag_name::Center:  case tag_name::Details: case tag_name::Dialog:   case tag_name::Dir:
				case tag_name::Div:     case tag_name::Dl:      case tag_name::Fieldset: case tag_name::Figcaption:
				case tag_name::Figure:  case tag_name::Footer:  case tag_name::Header:   case tag_name::Hgroup:
				case tag_name::Main:    case tag_name::Menu:    case tag_name::Nav:      case tag_name::Ol:
				case tag_name::P:       case tag_name::Section: case tag_name::Summary:  case tag_name::Ul:
					if (in_specific_scope(button_scope, tag_name::P)) close_p_element();
					insert_html_element(token);
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::H1: case tag_name::H2: case tag_name::H3: case tag_name::H4: case tag_name::H5: case tag_name::H6:
					if (in_specific_scope(button_scope, tag_name::P)) close_p_element();
					if (traits::get_namespace_name(current_node().m_it) == ns_name::HTML)
					{
						switch (traits::get_local_name_name(current_node().m_it))
						{
						case tag_name::H1: case tag_name::H2: case tag_name::H3: case tag_name::H4: case tag_name::H5: case tag_name::H6:
							if (traits::get_namespace_name(current_node().m_it) == ns_name::HTML)
							{
								base_type::report_error();
								m_stack.pop_back();
							}
						default:
							break;
						}
					}
					insert_html_element(token);
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Pre || token.m_tag_name_id == tag_name::Listing)
				{
					if (in_specific_scope(button_scope, tag_name::P)) close_p_element();
					insert_html_element(token);
					m_omit_lf = true;
					m_frameset_ok_flag = false;
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Form)
				{
					if (traits::pointer() != m_form_element_pointer && !contains(ns_name::HTML, tag_name::Template))
					{
						base_type::report_error();
						return;
					}
					if (in_specific_scope(button_scope, tag_name::P)) close_p_element();
					node_pointer it = insert_html_element(token);
					if (!contains(ns_name::HTML, tag_name::Template)) m_form_element_pointer = it;
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Li)
				{
					m_frameset_ok_flag = false;
					auto it1 = m_stack.end();
					auto it2 = m_stack.begin();
					while (it1 != it2)
					{
						--it1;
						node_pointer it = it1->m_it;
						if (is_html_element_of(it, tag_name::Li))
						{
							generate_implied_end_tags(tag_name::Li);
							if (is_html_element_of(current_node().m_it, tag_name::Li)) base_type::report_error();
							pop_until(ns_name::HTML, tag_name::Li);
							break;
						}
						if (is_special(it) &&
								(is_html_element_of(it, tag_name::Address)
							  || is_html_element_of(it, tag_name::Div)
							  || is_html_element_of(it, tag_name::P))) break;
						if (in_specific_scope(button_scope, tag_name::P)) close_p_element();
						insert_html_element(token);
						return;
					}
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Dd || token.m_tag_name_id == tag_name::Dt)
				{
					m_frameset_ok_flag = false;

					auto it1 = --m_stack.end(); // node

				Loop1:
					if (is_html_element_of(it1->m_it, tag_name::Dd))
					{
						generate_implied_end_tags(tag_name::Dd);
						if (is_html_element_of(current_node().m_it, tag_name::Dd)) base_type::report_error();
						pop_until(ns_name::HTML, tag_name::Dd);
						goto Done;
					}

					if (is_html_element_of(it1->m_it, tag_name::Dt))
					{
						generate_implied_end_tags(tag_name::Dt);
						if (is_html_element_of(current_node().m_it, tag_name::Dt)) base_type::report_error();
						pop_until(ns_name::HTML, tag_name::Dt);
						goto Done;
					}

					if (is_special(it1->m_it)
						&& !is_html_element_of(it1->m_it, tag_name::Address)
						&& !is_html_element_of(it1->m_it, tag_name::Div)
						&& !is_html_element_of(it1->m_it, tag_name::P)) goto Done;

					--it1;
					goto Loop1;

				Done:
					if (in_specific_scope(button_scope, tag_name::P)) close_p_element();
					insert_html_element(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Plaintext)
				{
					if (in_specific_scope(button_scope, tag_name::P)) close_p_element();
					insert_html_element(token);
					base_type::change_state(base_type::PLAINTEXT_state);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Button)
				{
					if (in_specific_scope(default_scope, tag_name::P))
					{
						base_type::report_error();
						generate_implied_end_tags();
						pop_until(ns_name::HTML, tag_name::Button);
					}
					reconstruct_formatting_element_list();
					insert_html_element(token);
					m_frameset_ok_flag = false;
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
					case tag_name::Address:    case tag_name::Article: case tag_name::Aside:   case tag_name::Blockquote:
					case tag_name::Button:     case tag_name::Center:  case tag_name::Details: case tag_name::Dialog:
					case tag_name::Dir:        case tag_name::Div:     case tag_name::Dl:      case tag_name::Fieldset:
					case tag_name::Figcaption: case tag_name::Figure:  case tag_name::Footer:  case tag_name::Header:
					case tag_name::Hgroup:     case tag_name::Listing: case tag_name::Main:    case tag_name::Menu:
					case tag_name::Nav:        case tag_name::Ol:      case tag_name::Pre:     case tag_name::Section:
					case tag_name::Summary:    case tag_name::Ul:
						if (!in_specific_scope(default_scope, std::make_pair(ns_name::HTML, token.m_tag_name_id)))
						{
							base_type::report_error();
							return;
						}
						generate_implied_end_tags();
						if (!is_html_element_of(current_node().m_it, token.m_tag_name_id)) base_type::report_error();
						pop_until(ns_name::HTML, token.m_tag_name_id);
						return;
					default:
						break;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Form)
				{
					if (!contains(ns_name::HTML, tag_name::Template))
					{
						node_pointer node = m_form_element_pointer;
						m_form_element_pointer = traits::pointer();
						if (node == traits::pointer() || !in_specific_scope(default_scope, node))
						{
							base_type::report_error();
							return;
						}
						generate_implied_end_tags();
						if (current_node().m_it != node) base_type::report_error();
						remove(node);
						return;
					}

					if (!in_specific_scope(default_scope, tag_name::Form))
					{
						base_type::report_error();
						return;
					}
					generate_implied_end_tags();
					if (!is_html_element_of(current_node().m_it, tag_name::Form)) base_type::report_error();
					pop_until(ns_name::HTML, tag_name::Form);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::P)
				{
					if (!in_specific_scope(button_scope, tag_name::P))
					{
						base_type::report_error();
						start_tag_token t(tag_name::P);
						insert_html_element(t);
					}
					close_p_element();
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Li)
				{
					if (!in_specific_scope(list_item_scope, tag_name::Li))
					{
						base_type::report_error();
						return;
					}
					generate_implied_end_tags(tag_name::Li);
					if (!is_html_element_of(current_node().m_it, tag_name::Li)) base_type::report_error();
					pop_until(ns_name::HTML, tag_name::Li);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Dd || token.m_tag_name_id == tag_name::Dt)
				{
					if (!in_specific_scope(default_scope, std::make_pair(ns_name::HTML, token.m_tag_name_id)))
					{
						base_type::report_error();
						return;
					}
					generate_implied_end_tags(std::make_pair(ns_name::HTML, token.m_tag_name_id));
					if (!is_html_element_of(current_node().m_it, token.m_tag_name_id)) base_type::report_error();
					pop_until(ns_name::HTML, token.m_tag_name_id);
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::H1: case tag_name::H2: case tag_name::H3: case tag_name::H4: case tag_name::H5: case tag_name::H6:
					if (!in_specific_scope(default_scope, std::array<tag_name, 6>({ tag_name::H1, tag_name::H2, tag_name::H3, tag_name::H4, tag_name::H5, tag_name::H6 })))
					{
						base_type::report_error();
						return;
					}
					generate_implied_end_tags();
					if (!is_html_element_of(current_node().m_it, token.m_tag_name_id)) base_type::report_error();
					pop_until(ns_name::HTML, std::array<tag_name, 6>({ tag_name::H1, tag_name::H2, tag_name::H3, tag_name::H4, tag_name::H5, tag_name::H6 }));
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::A)
				{
					node_pointer it = traits::pointer();

					auto it1 = m_list.end();
					auto it2 = m_list.begin();
					while (it1 != it2)
					{
						--it1;
						if (it1->m_marker) break;
						if (is_html_element_of(it1->m_it, tag_name::A))
						{
							it = it1->m_it;
							base_type::report_error();
							if (run_adoption_agency_algorithm(token)) goto AnyOtherEndTag;
							if (find_from_list(it) != m_list.end()) remove_from_list(it);
							if (find(it) != m_stack.end()) remove(it);
							break;
						}
					}

					reconstruct_formatting_element_list();
					it = insert_html_element(token);
					push_formatting_element_list(it, token);

					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
					case tag_name::B:  case tag_name::Big: case tag_name::Code:  case tag_name::Em:     case tag_name::Font:
					case tag_name::I:  case tag_name::S:   case tag_name::Small: case tag_name::Strike: case tag_name::Strong:
					case tag_name::Tt: case tag_name::U:
						reconstruct_formatting_element_list();
						push_formatting_element_list(insert_html_element(token), token);
						return;
					default:
						break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Nobr)
				{
					reconstruct_formatting_element_list();
					if (in_specific_scope(default_scope, tag_name::Nobr))
					{
						base_type::report_error();
						if (run_adoption_agency_algorithm(token)) goto AnyOtherEndTag;
						reconstruct_formatting_element_list();
					}
					push_formatting_element_list(insert_html_element(token), token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
					case tag_name::A:      case tag_name::B:      case tag_name::Big:  case tag_name::Code: case tag_name::Em:
					case tag_name::Font:   case tag_name::I:      case tag_name::Nobr: case tag_name::S:    case tag_name::Small:
					case tag_name::Strike: case tag_name::Strong: case tag_name::Tt:   case tag_name::U:
						if (run_adoption_agency_algorithm(token)) goto AnyOtherEndTag;
						return;
					default:
						break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Applet: case tag_name::Marquee: case tag_name::Object:
					reconstruct_formatting_element_list();
					insert_html_element(token);
					push_formatting_element_list();
					m_frameset_ok_flag = false;
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				node_pointer it = traits::pointer();

				switch (token.m_tag_name_id)
				{
				case tag_name::Applet: case tag_name::Marquee: case tag_name::Object:
					if (!in_specific_scope(default_scope, std::make_pair(ns_name::HTML, token.m_tag_name_id)))
					{
						base_type::report_error();
						return;
					}
					generate_implied_end_tags();
					it = current_node().m_it;
					if (traits::get_namespace_name(it) != ns_name::HTML
						|| traits::get_local_name_name(it) != token.m_tag_name_id) base_type::report_error();
					pop_until(ns_name::HTML, token.m_tag_name_id);
					clear_formatting_element_list();
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Table)
				{
					if (traits::get_document_mode(P->get_document()) != document_mode_name::Quirks
						&& in_specific_scope(button_scope, tag_name::P)) close_p_element();
					insert_html_element(token);
					m_frameset_ok_flag = false;
					insertion_mode(mode_name::in_table_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Br)
				{
					base_type::report_error();
					start_tag_token t(tag_name::Br);
					on_emit_token(t);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Area: case tag_name::Br: case tag_name::Embed: case tag_name::Img: case tag_name::Keygen:
				case tag_name::Wbr:
					reconstruct_formatting_element_list();
					insert_html_element(token);
					m_stack.pop_back();
					if (token.m_self_closing_flag) token.m_acknowledged_self_closing_flag = true;
					m_frameset_ok_flag = false;
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Input)
				{
					reconstruct_formatting_element_list();
					insert_html_element(token);
					m_stack.pop_back();
					if (token.m_self_closing_flag) token.m_acknowledged_self_closing_flag = true;
					auto it = token.find(U"type");
					std::u32string_view sv(U"hidden");
					if (it != token.end()
						&& is_ascii_case_insensitive_match(it->m_value.begin(), it->m_value.end(), sv.begin(), sv.end())) return;
					m_frameset_ok_flag = false;
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Param: case tag_name::Source: case tag_name::Track:
					insert_html_element(token);
					m_stack.pop_back();
					if (token.m_self_closing_flag) token.m_acknowledged_self_closing_flag = true;
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Hr)
				{
					if (in_specific_scope(button_scope, tag_name::P)) close_p_element();
					insert_html_element(token);
					m_stack.pop_back();
					if (token.m_self_closing_flag) token.m_acknowledged_self_closing_flag = true;
					m_frameset_ok_flag = false;
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Image)
				{
					base_type::report_error();
					token.m_tag_name_id = tag_name::Img;
					on_emit_token(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Textarea)
				{
					insert_html_element(token);
					m_omit_lf = true;
					base_type::change_state(base_type::RCDATA_state);
					m_original_insertion_mode = m_insertion_mode;
					m_frameset_ok_flag = false;
					insertion_mode(mode_name::text_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Xmp)
				{
					if (in_specific_scope(button_scope, tag_name::P)) close_p_element();
					reconstruct_formatting_element_list();
					m_frameset_ok_flag = false;
					parse_generic_raw_text_element(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Iframe)
				{
					m_frameset_ok_flag = false;
					parse_generic_raw_text_element(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Noembed)
				{
					parse_generic_raw_text_element(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Noscript)
				{
					if (!m_scripting_flag) goto AnyOtherStartTag;
					parse_generic_raw_text_element(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Select)
				{
					reconstruct_formatting_element_list();
					insert_html_element(token);
					m_frameset_ok_flag = false;
					switch (m_insertion_mode)
					{
					case mode_name::in_table_insertion_mode:
					case mode_name::in_caption_insertion_mode:
					case mode_name::in_table_body_insertion_mode:
					case mode_name::in_row_insertion_mode:
					case mode_name::in_cell_insertion_mode:
						insertion_mode(mode_name::in_select_in_table_insertion_mode);
						return;
					default:
						insertion_mode(mode_name::in_select_insertion_mode);
						return;
					}
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Optgroup: case tag_name::Option:
					if (is_html_element_of(current_node().m_it, tag_name::Option)) m_stack.pop_back();
					reconstruct_formatting_element_list();
					insert_html_element(token);
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Rb: case tag_name::Rtc:
					if (in_specific_scope(default_scope, tag_name::Ruby))
					{
						generate_implied_end_tags();
						if (!is_html_element_of(current_node().m_it, tag_name::Ruby)) base_type::report_error();
					}
					insert_html_element(token);
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Rp: case tag_name::Rt:
					if (in_specific_scope(default_scope, tag_name::Ruby))
					{
						generate_implied_end_tags(tag_name::Rtc);
						if (!is_html_element_of(current_node().m_it, tag_name::Rtc)
						 && !is_html_element_of(current_node().m_it, tag_name::Ruby)) base_type::report_error();
					}
					insert_html_element(token);
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Math)
				{
					reconstruct_formatting_element_list();
					adjust_mathml_attributes(token);
					adjust_foreign_attributes(token);
					insert_foreign_element(token, ns_name::MathML);
					if (token.m_self_closing_flag)
					{
						m_stack.pop_back();
						token.m_acknowledged_self_closing_flag = true;
					}
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Svg)
				{
					reconstruct_formatting_element_list();
					adjust_mathml_attributes(token);
					adjust_foreign_attributes(token);
					insert_foreign_element(token, ns_name::SVG);
					if (token.m_self_closing_flag)
					{
						m_stack.pop_back();
						token.m_acknowledged_self_closing_flag = true;
					}
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Caption: case tag_name::Col: case tag_name::Colgroup: case tag_name::Frame: case tag_name::Head:
				case tag_name::Tbody:   case tag_name::Td:  case tag_name::Tfoot:    case tag_name::Th:    case tag_name::Thead:
				case tag_name::Tr:
					base_type::report_error();
					return;
				default:
					break;
				}
			}

			goto AnyOtherStartTag;
		AnyOtherStartTag:
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				reconstruct_formatting_element_list();
				insert_html_element(token);
				return;
			}

			goto AnyOtherEndTag;
		AnyOtherEndTag:
			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				auto it1 = --m_stack.end();
				node_pointer node = it1->m_it;
			Loop2:
				if (traits::get_namespace_name(node) == ns_name::HTML
					&& encoding_cast<std::u32string>(traits::get_local_name(node)) == token.m_tag_name)
				{
					generate_implied_end_tags(std::make_pair(ns_name::HTML, token.m_tag_name));
					if (node != current_node().m_it) base_type::report_error();
					pop_until(node);
					return;
				}
				if (is_special(node))
				{
					base_type::report_error();
					return;
				}
				--it1;
				node = it1->m_it;
				goto Loop2;

				return;
			}
		}

		void close_p_element()
		{
			generate_implied_end_tags(tag_name::P);
			if (!is_html_element_of(current_node().m_it, tag_name::P)) base_type::report_error();
			pop_until(ns_name::HTML, tag_name::P);
		}

		/*! adoption agency algorithm を実行する

		@return その他の終了タグに飛ばす場合、true を返す。

		@internal
		メモに翻訳在り。
		*/
		template <typename Token>
		bool run_adoption_agency_algorithm(Token& token)
		{
			this_type* P = static_cast<this_type*>(this);

			if constexpr (std::is_base_of_v<tag_token, Token>)
			{
				std::uint32_t outer_loop_counter; // 3.
				node_pointer  formatting_element; // 5.
				node_pointer  furthest_block;     // 10.
				node_pointer  common_ancestor;    // 12.
				typename std::list<active_formatting_element>::iterator bookmark; // 13.
				node_pointer  node;      // 14.
				node_pointer  prev_node; // 14.
				node_pointer  last_node; // 14.
				std::uint32_t inner_loop_counter; // 14.1.

				// 1.
				std::u32string const& subject = token.m_tag_name;
				// 2.
				node_pointer it = current_node().m_it;
				if (traits::get_namespace_name(it) == ns_name::HTML
					&& encoding_cast<std::u32string>(traits::get_local_name(it)) == subject
					&& find_from_list(it) == m_list.end())
				{
					m_stack.pop_back();
					return false;
				}
				// 3.
				outer_loop_counter = 0;
				// 4.
			OuterLoop:
				if (8 <= outer_loop_counter) return false;
				// 5.
				++outer_loop_counter;
				// 6.
				formatting_element = traits::pointer();
				{
					auto it1 = m_list.end();
					auto it2 = m_list.begin();
					while (it1 != it2)
					{
						--it1;
						if (it1->m_marker) break;
						if (encoding_cast<std::u32string>(traits::get_local_name(it1->m_it)) == subject)
						{
							formatting_element = it1->m_it;
							break;
						}
					}
				}
				if (formatting_element == traits::pointer()) return true;
				// 7.
				if (!contains(formatting_element))
				{
					base_type::report_error();
					remove_from_list(formatting_element);
					return false;
				}
				// 8.
				if (!in_specific_scope(default_scope, formatting_element))
				{
					base_type::report_error();
					return false;
				}
				// 9.
				if (formatting_element != current_node().m_it) base_type::report_error();
				// 10.
				furthest_block = traits::pointer();
				{
					auto it1 = m_stack.begin();
					auto it2 = m_stack.end();
					while (it1 != it2) { if (it1++->m_it == formatting_element) break; }
					while (it1 != it2)
					{
						if (is_special(it1->m_it))
						{
							furthest_block = it1->m_it;
							break;
						}
						++it1;
					}
				}
				// 11.
				if (furthest_block == traits::pointer())
				{
					pop_until(formatting_element);
					remove_from_list(formatting_element);
					return false;
				}
				// 12.
				common_ancestor = std::prev(find(formatting_element))->m_it;
				// 13.
				bookmark = std::next(find_from_list(formatting_element));
				// 14
				node = furthest_block;
				last_node = furthest_block;
				// 14.1.
				inner_loop_counter = 0;
				// 14.2.
			InnerLoop:
				++inner_loop_counter;
				// 14.3.
				{
					auto it1 = find(node);
					if (it1 != m_stack.end()) node = (--it1)->m_it;
					else node = prev_node;
				}
				// 14.4.
				if (node == formatting_element) goto NextStep;
				// 14.5.
				if (3 < inner_loop_counter && find_from_list(node) != m_list.end())
				{
					remove_from_list(node);
				}
				// 14.6.
				if (find_from_list(node) == m_list.end())
				{
					auto it1 = find(node);
					assert(it1 != m_stack.end());
					prev_node = (--it1)->m_it;
					remove(node);
					goto InnerLoop;
				}
				// 14.7.
				{
					auto it1 = find(node);
					auto it2 = find_from_list(node);
					node = create_element_for_token(it1->m_token, ns_name::HTML, common_ancestor);
					it1->m_it = node;
					it2->m_it = node;
				}
				// 14.8.
				if (last_node == furthest_block) bookmark = ++find_from_list(node);
				// 14.9.
				P->move_node(traits::end(node), last_node);
				// 14.10.
				last_node = node;
				// 14.11.
				goto InnerLoop;
			NextStep:
				// 15.
				P->insert_element(appropriate_place_for_inserting_node(common_ancestor), last_node);
				// 16.
				{
					start_tag_token t = find(formatting_element)->m_token;
					node_pointer el = create_element_for_token(t, ns_name::HTML, furthest_block);
				// 17.
					auto it1 = furthest_block.begin();
					auto it2 = furthest_block.end();
					while (it1 != it2)
					{
						auto it = it1++;
						P->move_node(traits::end(el), it);
					}
				// 18.
					P->insert_element(traits::end(furthest_block), el);
				// 19.
					remove_from_list(formatting_element);
					m_list.insert(bookmark, { t, el, false });
				//20.
					remove(formatting_element);
					m_stack.insert(std::next(find(furthest_block)), { t, el });
				}
				
				// 21.
				goto OuterLoop;
			}
			else assert(false);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.8 The "text" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incdata
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_text_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<character_token, Token>)
			{
				insert_character(token.m_data);
				return;
			}

			if constexpr (std::is_same_v<end_of_file_token, Token>)
			{
				base_type::report_error();
				node_pointer it = current_node().m_it;
				if (is_html_element_of(it, tag_name::Script)) traits::set_already_started_flag(it, true);
				m_stack.pop_back();
				insertion_mode(m_original_insertion_mode);
				reprocess_token(token);
				return;
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Script)
				{
					m_stack.pop_back();
					insertion_mode(m_original_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				m_stack.pop_back();
				insertion_mode(m_original_insertion_mode);
				return;
			}
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.9 The "in table" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intable
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_table_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<character_token, Token>)
			{
				node_pointer it = current_node().m_it;
				switch (traits::get_local_name_name(it))
				{
				case tag_name::Table: case tag_name::Tbody: case tag_name::Tfoot: case tag_name::Thead: case tag_name::Tr:
					if (traits::get_namespace_name(it) != ns_name::HTML) break;

					m_pending_table_character_tokens.clear();
					m_original_insertion_mode = m_insertion_mode;
					insertion_mode(mode_name::in_table_text_insertion_mode);
					reprocess_token(token);
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token);
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Caption)
				{
					clear_stack_back_to_table_context();
					push_formatting_element_list();
					insert_html_element(token);
					insertion_mode(mode_name::in_caption_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Colgroup)
				{
					clear_stack_back_to_table_context();
					insert_html_element(token);
					insertion_mode(mode_name::in_column_group_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Col)
				{
					clear_stack_back_to_table_context();
					start_tag_token t(tag_name::Colgroup);
					insert_html_element(t);
					insertion_mode(mode_name::in_column_group_insertion_mode);
					reprocess_token(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Tbody: case tag_name::Tfoot: case tag_name::Thead:
					clear_stack_back_to_table_context();
					insert_html_element(token);
					insertion_mode(mode_name::in_table_body_insertion_mode);
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Td: case tag_name::Th: case tag_name::Tr:
					clear_stack_back_to_table_context();
					{
						start_tag_token t(tag_name::Tbody);
						insert_html_element(t);
					}
					insertion_mode(mode_name::in_table_body_insertion_mode);
					reprocess_token(token);
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Table)
				{
					base_type::report_error();
					if (!in_specific_scope(table_scope, tag_name::Table)) return;
					pop_until(ns_name::HTML, tag_name::Table);
					reset_insertion_mode_appropriately();
					reprocess_token(token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Table)
				{
					if (!in_specific_scope(table_scope, tag_name::Table))
					{
						base_type::report_error();
						return;
					}
					pop_until(ns_name::HTML, tag_name::Table);
					reset_insertion_mode_appropriately();
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Body:  case tag_name::Caption: case tag_name::Col:   case tag_name::Colgroup: case tag_name::Html:
				case tag_name::Tbody: case tag_name::Td:      case tag_name::Tfoot: case tag_name::Th:       case tag_name::Thead:
				case tag_name::Tr:
					base_type::report_error();
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_base_of_v<tag_token, Token>)
			{
				tag_name tn = token.m_tag_name_id;
				if ((std::is_same_v<start_tag_token, Token> && (tn == tag_name::Style || tn == tag_name::Script || tn == tag_name::Template))
					|| (std::is_same_v<start_tag_token, Token> && tn == tag_name::Template))
				{
					on_in_head_insertion_mode(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Input)
				{
					auto it = token.find(U"type");
					std::u32string_view sv(U"hidden");
					if (it == token.end()
						|| !is_ascii_case_insensitive_match(it->m_value.begin(), it->m_value.end(), sv.begin(), sv.end())) goto AnythingElse;
					base_type::report_error();
					insert_html_element(token);
					m_stack.pop_back();
					if (token.m_self_closing_flag) token.m_acknowledged_self_closing_flag = true;
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Form)
				{
					base_type::report_error();
					if (contains(ns_name::HTML, tag_name::Template)
						|| m_form_element_pointer != traits::pointer()) return;
					m_form_element_pointer = insert_html_element(token);
					m_stack.pop_back();
					return;
				}
			}

			if constexpr (std::is_same_v<end_of_file_token, Token>)
			{
				on_in_body_insertion_mode(token);
				return;
			}

			goto AnythingElse;
		AnythingElse:
			base_type::report_error();
			m_foster_parenting = true;
			on_in_body_insertion_mode(token);
			m_foster_parenting = false;
		}

		/*
		https://html.spec.whatwg.org/multipage/parsing.html#clear-the-stack-back-to-a-table-context
		*/
		void clear_stack_back_to_table_context()
		{
			while (!m_stack.empty())
			{
				node_pointer it = current_node().m_it;
				if (traits::get_namespace_name(it) != ns_name::HTML) return;
				switch (traits::get_local_name_name(it))
				{
				case tag_name::Table: case tag_name::Template: case tag_name::Html:
					return;
				default:
					m_stack.pop_back();
				}
			}
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.10 The "in table text" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intabletext
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_table_text_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (token.m_data == U'\0')
				{
					base_type::report_error();
					return;
				}
			}

			if constexpr (std::is_same_v<character_token, Token>)
			{
				m_pending_table_character_tokens.push_back(token);
				return;
			}

			bool f = false;
			for (character_token& t : m_pending_table_character_tokens)
			{
				if (!is_ascii_white_space(t.m_data))
				{
					f = true;
					break;
				}
			}
			if (f)
			{
				base_type::report_error();
				for (character_token& t : m_pending_table_character_tokens)
				{
					base_type::report_error();
					m_foster_parenting = true;
					on_in_body_insertion_mode(t);
					m_foster_parenting = false;
				}
			}
			else
			{
				for (character_token& t : m_pending_table_character_tokens) insert_character(t.m_data);
			}
			insertion_mode(m_original_insertion_mode);
			reprocess_token(token);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.11 The "in caption" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incaption
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_caption_insertion_mode(Token& token)
		{
			bool f;

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Caption)
				{
					if (!in_specific_scope(table_scope, tag_name::Caption))
					{
						base_type::report_error();
						return;
					}
					generate_implied_end_tags();
					if (is_html_element_of(current_node().m_it, tag_name::Caption)) base_type::report_error();
					pop_until(ns_name::HTML, tag_name::Caption);
					clear_formatting_element_list();
					insertion_mode(mode_name::in_table_insertion_mode);
					return;
				}
			}

			f = false;
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Caption: case tag_name::Col: case tag_name::Colgroup: case tag_name::Tbody: case tag_name::Td:
				case tag_name::Tfoot:   case tag_name::Th:  case tag_name::Thead:    case tag_name::Tr:
					f = true;
				default:
					break;
				}
			}
			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Table) f = true;
			}
			if (f)
			{
				if (!in_specific_scope(table_scope, tag_name::Caption))
				{
					base_type::report_error();
					return;
				}
				generate_implied_end_tags();
				if (is_html_element_of(current_node().m_it, tag_name::Caption)) base_type::report_error();
				pop_until(ns_name::HTML, tag_name::Caption);
				clear_formatting_element_list();
				insertion_mode(mode_name::in_table_insertion_mode);
				reprocess_token(token);
				return;
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Body: case tag_name::Col:   case tag_name::Colgroup: case tag_name::Html:  case tag_name::Tbody:
				case tag_name::Td:   case tag_name::Tfoot: case tag_name::Th:       case tag_name::Thead:
					base_type::report_error();
					return;
				default:
					break;
				}
			}

			on_in_body_insertion_mode(token);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.12 The "in column group
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-incolgroup
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_column_group_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data))
				{
					insert_character(token.m_data);
					return;
				}
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token);
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					on_in_body_insertion_mode(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Col)
				{
					insert_html_element(token);
					m_stack.pop_back();
					if (token.m_self_closing_flag) token.m_acknowledged_self_closing_flag = true;
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Colgroup)
				{
					if (!is_html_element_of(current_node().m_it, tag_name::Colgroup))
					{
						base_type::report_error();
						return;
					}
					m_stack.pop_back();
					insertion_mode(mode_name::in_table_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Col)
				{
					return;
				}
			}

			if constexpr (std::is_base_of_v<tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Template)
				{
					on_in_head_insertion_mode(token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_of_file_token, Token>)
			{
				on_in_body_insertion_mode(token);
				return;
			}

			if (!is_html_element_of(current_node().m_it, tag_name::Colgroup))
			{
				base_type::report_error();
				return;
			}
			m_stack.pop_back();
			insertion_mode(mode_name::in_table_insertion_mode);
			reprocess_token(token);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.13 The "in table body" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intbody
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_table_body_insertion_mode(Token& token)
		{
			bool f;

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Tr)
				{
					clear_stack_back_to_table_body_context();
					insert_html_element(token);
					insertion_mode(mode_name::in_row_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Th || token.m_tag_name_id == tag_name::Td)
				{
					base_type::report_error();
					clear_stack_back_to_table_body_context();
					start_tag_token t(tag_name::Tr);
					insert_html_element(t);
					insertion_mode(mode_name::in_row_insertion_mode);
					reprocess_token(token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Tbody: case tag_name::Tfoot: case tag_name::Thead:
					if (!in_specific_scope(table_scope, token.m_tag_name_id))
					{
						base_type::report_error();
						return;
					}
					clear_stack_back_to_table_body_context();
					m_stack.pop_back();
					insertion_mode(mode_name::in_table_insertion_mode);
					return;
				default:
					break;
				}
			}

			f = false;
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Caption: case tag_name::Col: case tag_name::Colgroup: case tag_name::Tbody: case tag_name::Tfoot:
				case tag_name::Thead:
					f = true;
				default:
					break;
				}
			}
			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Table) f = true;
			}
			if(f)
			{
				if (!in_specific_scope(table_scope, std::array<tag_name, 3>({ tag_name::Tbody, tag_name::Thead, tag_name::Tfoot })))
				{
					base_type::report_error();
					return;
				}
				clear_stack_back_to_table_body_context();
				m_stack.pop_back();
				insertion_mode(mode_name::in_table_insertion_mode);
				reprocess_token(token);
				return;
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Body: case tag_name::Caption: case tag_name::Col: case tag_name::Colgroup: case tag_name::Html:
				case tag_name::Td:   case tag_name::Th:      case tag_name::Tr:
					base_type::report_error();
					return;
				default:
					break;
				}
			}

			on_in_table_insertion_mode(token);
		}

		void clear_stack_back_to_table_body_context()
		{
			while (true)
			{
				node_pointer it = current_node().m_it;
				switch (traits::get_local_name_name(it))
				{
				case tag_name::Tbody: case tag_name::Tfoot: case tag_name::Thead: case tag_name::Template: case tag_name::Html:
					if (traits::get_namespace_name(it) == ns_name::HTML) return;
				default:
					break;
				}
				m_stack.pop_back();
			}
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.14 The "in row
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intr
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_row_insertion_mode(Token& token)
		{
			bool f;

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Th || token.m_tag_name_id == tag_name::Td)
				{
					clear_stack_back_to_table_row_context();
					insert_html_element(token);
					insertion_mode(mode_name::in_cell_insertion_mode);
					push_formatting_element_list();
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Tr)
				{
					if (!in_specific_scope(table_scope, tag_name::Tr))
					{
						base_type::report_error();
						return;
					}
					clear_stack_back_to_table_row_context();
					assert(is_html_element_of(current_node().m_it, tag_name::Tr));
					m_stack.pop_back();
					insertion_mode(mode_name::in_table_body_insertion_mode);
					return;
				}
			}

			f = false;
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Caption: case tag_name::Col: case tag_name::Colgroup: case tag_name::Tbody: case tag_name::Tfoot:
				case tag_name::Thead:   case tag_name::Tr:
					f = true;
				default:
					break;
				}
			}
			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Table) f = true;
			}
			if (f)
			{
				if (!in_specific_scope(table_scope, tag_name::Tr))
				{
					base_type::report_error();
					return;
				}
				clear_stack_back_to_table_row_context();
				assert(is_html_element_of(current_node().m_it, tag_name::Tr));
				m_stack.pop_back();
				insertion_mode(mode_name::in_table_body_insertion_mode);
				reprocess_token(token);
				return;
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Tbody
				 || token.m_tag_name_id == tag_name::Tfoot
				 || token.m_tag_name_id == tag_name::Thead)
				{
					if (!in_specific_scope(table_scope, token.m_tag_name_id))
					{
						base_type::report_error();
						return;
					}
					if (!in_specific_scope(table_scope, tag_name::Tr)) base_type::report_error();
					clear_stack_back_to_table_row_context();
					assert(is_html_element_of(current_node().m_it, tag_name::Tr));
					m_stack.pop_back();
					insertion_mode(mode_name::in_table_body_insertion_mode);
					reprocess_token(token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
					case tag_name::Body: case tag_name::Caption: case tag_name::Col: case tag_name::Colgroup:
					case tag_name::Html: case tag_name::Td:      case tag_name::Th:
						base_type::report_error();
						return;
					default:
						break;
				}
			}

			on_in_table_insertion_mode(token);
		}

		void clear_stack_back_to_table_row_context()
		{
			while (true)
			{
				node_pointer it = current_node().m_it;
				if (is_html_element_of(it, tag_name::Tr)
				 || is_html_element_of(it, tag_name::Table)
				 || is_html_element_of(it, tag_name::Html)) return;

				m_stack.pop_back();
			}
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.15 The "in cell
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intd
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_cell_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Td || token.m_tag_name_id == tag_name::Th)
				{
					if (!in_specific_scope(table_scope, token.m_tag_name_id))
					{
						base_type::report_error();
						return;
					}
					generate_implied_end_tags();
					if (!is_html_element_of(current_node().m_it, token.m_tag_name_id)) base_type::report_error();
					pop_until(ns_name::HTML, token.m_tag_name_id);
					clear_formatting_element_list();
					insertion_mode(mode_name::in_row_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Caption: case tag_name::Col: case tag_name::Colgroup: case tag_name::Tbody: case tag_name::Td:
				case tag_name::Tfoot:   case tag_name::Th:  case tag_name::Thead:    case tag_name::Tr:
					if (!in_specific_scope(table_scope, std::array<tag_name, 2>({ tag_name::Td, tag_name::Th })))
					{
						base_type::report_error();
						return;
					}
					close_cell();
					reprocess_token(token);
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Body: case tag_name::Caption: case tag_name::Col: case tag_name::Colgroup: case tag_name::Html:
					base_type::report_error();
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Table: case tag_name::Tbody: case tag_name::Tfoot: case tag_name::Thead: case tag_name::Tr:
					if (!in_specific_scope(table_scope, token.m_tag_name_id))
					{
						base_type::report_error();
						return;
					}
					close_cell();
					reprocess_token(token);
					return;
				default:
					break;
				}
			}

			on_in_body_insertion_mode(token);
		}

		void close_cell()
		{
			generate_implied_end_tags();
			if (!is_html_element_of(current_node().m_it, tag_name::Td)
			 || !is_html_element_of(current_node().m_it, tag_name::Th)) base_type::report_error();
			pop_until(ns_name::HTML, std::array<tag_name, 2>({ tag_name::Td, tag_name::Th }));
			clear_formatting_element_list();
			insertion_mode(mode_name::in_row_insertion_mode);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.16 The "in select
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselect
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_select_insertion_mode(Token& token)
		{
			bool f;

			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (token.m_data == U'\0')
				{
					base_type::report_error();
					return;
				}
			}

			if constexpr (std::is_same_v<character_token, Token>)
			{
				insert_character(token.m_data);
				return;
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token);
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					on_in_body_insertion_mode(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Option)
				{
					if (is_html_element_of(current_node().m_it, tag_name::Option)) m_stack.pop_back();
					insert_html_element(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Optgroup)
				{
					if (is_html_element_of(current_node().m_it, tag_name::Option)) m_stack.pop_back();
					if (is_html_element_of(current_node().m_it, tag_name::Optgroup)) m_stack.pop_back();
					insert_html_element(token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Optgroup)
				{
					if (is_html_element_of(current_node().m_it, tag_name::Option)
					 && is_html_element_of(std::prev(m_stack.end(), 2)->m_it, tag_name::Optgroup)) m_stack.pop_back();
					if (is_html_element_of(current_node().m_it, tag_name::Optgroup)) m_stack.pop_back();
					else base_type::report_error();
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Option)
				{
					if (is_html_element_of(current_node().m_it, tag_name::Option)) m_stack.pop_back();
					else base_type::report_error();
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Select)
				{
					if (!in_specific_scope(select_scope, tag_name::Select))
					{
						base_type::report_error();
						return;
					}
					pop_until(ns_name::HTML, tag_name::Select);
					reset_insertion_mode_appropriately();
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Select)
				{
					base_type::report_error();
					if (!in_specific_scope(select_scope, tag_name::Select)) return;
					pop_until(ns_name::HTML, tag_name::Select);
					reset_insertion_mode_appropriately();
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Input: case tag_name::Keygen: case tag_name::Textarea:
					base_type::report_error();
					if (!in_specific_scope(select_scope, tag_name::Select)) return;
					pop_until(ns_name::HTML, tag_name::Select);
					reset_insertion_mode_appropriately();
					reprocess_token(token);
					return;
				default:
					break;
				}
			}

			f = false;
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Script || token.m_tag_name_id == tag_name::Template) f = true;
			}
			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Template) f = true;
			}
			if (f)
			{
				on_in_head_insertion_mode(token);
				return;
			}

			if constexpr (std::is_same_v<end_of_file_token, Token>)
			{
				on_in_body_insertion_mode(token);
				return;
			}

			base_type::report_error();
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.17 The "in select in table
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inselectintable
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_select_in_table_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Caption: case tag_name::Table: case tag_name::Tbody: case tag_name::Tfoot: case tag_name::Thead:
				case tag_name::Tr:      case tag_name::Td:    case tag_name::Th:
					base_type::report_error();
					pop_until(ns_name::HTML, tag_name::Select);
					reset_insertion_mode_appropriately();
					reprocess_token(token);
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Caption: case tag_name::Table: case tag_name::Tbody: case tag_name::Tfoot: case tag_name::Thead:
				case tag_name::Tr:      case tag_name::Td:    case tag_name::Th:
					base_type::report_error();
					if (!in_specific_scope(select_scope, token.m_tag_name_id)) return;
					pop_until(ns_name::HTML, tag_name::Select);
					reset_insertion_mode_appropriately();
					reprocess_token(token);
					return;
				default:
					break;
				}
			}

			on_in_select_insertion_mode(token);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.18 The "in template
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-intemplate
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_template_insertion_mode(Token& token)
		{
			bool f;

			if constexpr (std::is_same_v<character_token, Token>
					   || std::is_same_v<comment_token, Token>
					   || std::is_same_v<DOCTYPE_token, Token>)
			{
				on_in_body_insertion_mode(token);
				return;
			}

			f = false;
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Base:     case tag_name::Basefont: case tag_name::Bgsound: case tag_name::Link:
				case tag_name::Meta:     case tag_name::Noframes: case tag_name::Script:  case tag_name::Style:
				case tag_name::Template: case tag_name::Title:
					f = true;
				default:
					break;
				}
			}
			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Template) f = true;
			}
			if (f)
			{
				on_in_head_insertion_mode(token);
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
				case tag_name::Caption: case tag_name::Colgroup: case tag_name::Tbody: case tag_name::Tfoot:
				case tag_name::Thead:
					m_template_insertion_mode_stack.pop_back();
					m_template_insertion_mode_stack.push_back(mode_name::in_table_insertion_mode);
					insertion_mode(mode_name::in_table_insertion_mode);
					reprocess_token(token);
					return;
				default:
					break;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Col)
				{
					m_template_insertion_mode_stack.pop_back();
					m_template_insertion_mode_stack.push_back(mode_name::in_column_group_insertion_mode);
					insertion_mode(mode_name::in_column_group_insertion_mode);
					reprocess_token(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Tr)
				{
					m_template_insertion_mode_stack.pop_back();
					m_template_insertion_mode_stack.push_back(mode_name::in_table_body_insertion_mode);
					insertion_mode(mode_name::in_table_body_insertion_mode);
					reprocess_token(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Td || token.m_tag_name_id == tag_name::Th)
				{
					m_template_insertion_mode_stack.pop_back();
					m_template_insertion_mode_stack.push_back(mode_name::in_row_insertion_mode);
					insertion_mode(mode_name::in_row_insertion_mode);
					reprocess_token(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				m_template_insertion_mode_stack.pop_back();
				m_template_insertion_mode_stack.push_back(mode_name::in_body_insertion_mode);
				insertion_mode(mode_name::in_body_insertion_mode);
				reprocess_token(token);
				return;
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<end_of_file_token, Token>)
			{
				if (!contains(tag_name::Template))
				{
					stop_parsing();
					return;
				}
				base_type::report_error();
				pop_until(ns_name::HTML, tag_name::Template);
				clear_formatting_element_list();
				m_template_insertion_mode_stack.pop_back();
				reset_insertion_mode_appropriately();
				reprocess_token(token);
				return;
			}

			assert(false);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.19 The "after body" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-afterbody
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_after_body_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data))
				{
					on_in_body_insertion_mode(token);
					return;
				}
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token, traits::end(m_stack.front().m_it));
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					on_in_body_insertion_mode(token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					if (m_fragments_parser)
					{
						base_type::report_error();
						return;
					}
					else
					{
						insertion_mode(mode_name::after_after_body_insertion_mode);
						return;
					}
				}
			}

			if constexpr (std::is_same_v<end_of_file_token, Token>)
			{
				stop_parsing();
				return;
			}

			base_type::report_error();
			insertion_mode(mode_name::in_body_insertion_mode);
			reprocess_token(token);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.20 The "in frameset" insertion mode
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inframeset
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_in_frameset_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data))
				{
					insert_character(token.m_data);
					return;
				}
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token);
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					on_in_body_insertion_mode(token);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Frameset)
				{
					insert_html_element(token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Frameset)
				{
					if (is_html_element_of(current_node().m_it, tag_name::Html))
					{
						base_type::report_error();
						return;
					}
					m_stack.pop_back();
					if (!m_fragments_parser && !is_html_element_of(current_node().m_it, tag_name::Frameset))
					{
						insertion_mode(mode_name::after_frameset_insertion_mode);
					}
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Frame)
				{
					insert_html_element(token);
					m_stack.pop_back();
					if (token.m_self_closing_flag) token.m_acknowledged_self_closing_flag = true;
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Noframes)
				{
					on_in_head_insertion_mode(token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_of_file_token, Token>)
			{
				if (!is_html_element_of(current_node().m_it, tag_name::Html)) base_type::report_error();
				stop_parsing();
				return;
			}

			base_type::report_error();
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.21 The "after frameset
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-afterframeset
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_after_frameset_insertion_mode(Token& token)
		{
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data))
				{
					insert_character(token.m_data);
					return;
				}
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token);
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					on_in_body_insertion_mode(token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html)
				{
					insertion_mode(mode_name::after_after_frameset_insertion_mode);
					return;
				}
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Noframes)
				{
					on_in_head_insertion_mode(token);
					return;
				}
			}

			if constexpr (std::is_same_v<end_of_file_token, Token>)
			{
				stop_parsing();
				return;
			}

			base_type::report_error();
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.22 The "after after body
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-body-insertion-mode
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_after_after_body_insertion_mode(Token& token)
		{
			this_type* P = static_cast<this_type*>(this);
			bool f;

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token, traits::end(P->get_document()));
				return;
			}

			f = false;
			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				f = true;
			}
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data)) f = true;
			}
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html) f = true;
			}
			if (f)
			{
				on_in_body_insertion_mode(token);
				return;
			}

			if constexpr (std::is_same_v<end_of_file_token, Token>)
			{
				stop_parsing();
				return;
			}

			base_type::report_error();
			insertion_mode(mode_name::in_body_insertion_mode);
			reprocess_token(token);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.4.23 The "after after frameset
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-frameset-insertion-mode
		// ----------------------------------------------------------------------------------------

		template <typename Token>
		void on_after_after_frameset_insertion_mode(Token& token)
		{
			this_type* P = static_cast<this_type*>(this);
			bool f;

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token, traits::end(P->get_document()));
				return;
			}

			f = false;
			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				f = true;
			}
			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data)) f = true;
			}
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Html) f = true;
			}
			if (f)
			{
				on_in_body_insertion_mode(token);
				return;
			}

			if constexpr (std::is_same_v<end_of_file_token, Token>)
			{
				stop_parsing();
				return;
			}

			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Noframes)
				{
					on_in_head_insertion_mode(token);
					return;
				}
			}

			base_type::report_error();
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.6.5 The rules for parsing tokens in foreign content
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inforeign
		// ----------------------------------------------------------------------------------------
		
		template <typename Token>
		void on_foreign_content(Token& token)
		{
			bool f;

			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (token.m_data == U'\U00000000')
				{
					base_type::report_error();
					insert_character(U'\U0000FFFD');
					return;
				}
			}

			if constexpr (std::is_same_v<character_token, Token>)
			{
				if (is_ascii_white_space(token.m_data))
				{
					insert_character(token.m_data);
					return;
				}
			}

			if constexpr (std::is_same_v<character_token, Token>)
			{
				insert_character(token.m_data);
				m_frameset_ok_flag = false;
				return;
			}

			if constexpr (std::is_same_v<comment_token, Token>)
			{
				insert_comment(token);
				return;
			}

			if constexpr (std::is_same_v<DOCTYPE_token, Token>)
			{
				base_type::report_error();
				return;
			}

			f = false;
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				switch (token.m_tag_name_id)
				{
					case tag_name::B:       case tag_name::Big:    case tag_name::Blockquote: case tag_name::Body:
					case tag_name::Br:      case tag_name::Center: case tag_name::Code:       case tag_name::Dd:
					case tag_name::Div:     case tag_name::Dl:     case tag_name::Dt:         case tag_name::Em:
					case tag_name::Embed:   case tag_name::H1:     case tag_name::H2:         case tag_name::H3:
					case tag_name::H4:      case tag_name::H5:     case tag_name::H6:         case tag_name::Head:
					case tag_name::Hr:      case tag_name::I:      case tag_name::Img:        case tag_name::Li:
					case tag_name::Listing: case tag_name::Menu:   case tag_name::Meta:       case tag_name::Nobr:
					case tag_name::Ol:      case tag_name::P:      case tag_name::Pre:        case tag_name::Ruby:
					case tag_name::S:       case tag_name::Small:  case tag_name::Span:       case tag_name::Strong:
					case tag_name::Strike:  case tag_name::Sub:    case tag_name::Sup:        case tag_name::Table:
					case tag_name::Tt:      case tag_name::U:      case tag_name::Ul:         case tag_name::Var:
						f = true;
						break;
					default:
						break;
				}
				if (!f && token.m_tag_name_id == tag_name::Font)
				{
					if (token.find(U"Color") != token.end()
					 || token.find(U"Face") != token.end()
					 || token.find(U"Size") != token.end()) f = true;
				}
			}
			if (f)
			{
				base_type::report_error();
				if(m_fragments_parser) goto AnyOtherStartTag;
				m_stack.pop_back();
				while (true)
				{
					m_stack.pop_back();
					if (is_mathml_text_integration_point(current_node())
					 || is_html_integration_point(current_node())
					 || traits::get_namespace_name(current_node().m_it) == ns_name::HTML) break;
				}
				reprocess_token(token);
				return;
			}

			goto AnyOtherStartTag;
		AnyOtherStartTag:
			if constexpr (std::is_same_v<start_tag_token, Token>)
			{
				ns_name ns = traits::get_namespace_name(adjusted_current_node().m_it);

				if (ns == ns_name::MathML) adjust_mathml_attributes(token);
				else if (ns == ns_name::SVG)
				{
					auto it1 = svg_elements_conversion_tbl.find(token.m_tag_name);
					if (it1 != svg_elements_conversion_tbl.end()) token.m_tag_name = it1->second;

					adjust_svg_attributes(token);
				}
				adjust_foreign_attributes(token);
				insert_foreign_element(token, ns);
				if (token.m_self_closing_flag)
				{
					if (token.m_tag_name_id == tag_name::Script
					 && is_element_of(current_node().m_it, ns_name::SVG, tag_name::Script))
					{
						token.m_acknowledged_self_closing_flag = true;
						m_stack.pop_back();
						return;
					}
					else
					{
						m_stack.pop_back();
						token.m_acknowledged_self_closing_flag = true;
						return;
					}
				}
				return;
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				if (token.m_tag_name_id == tag_name::Script
				 && is_element_of(current_node().m_it, ns_name::SVG, tag_name::Script))
				{
					m_stack.pop_back();
					return;
				}
			}

			if constexpr (std::is_same_v<end_tag_token, Token>)
			{
				// 1.
				auto node = --m_stack.end();
				// 2.
				auto tmp = traits::get_local_name(node->m_it);
				std::u32string tag;
				to_ascii_lowercase(tmp.begin(), tmp.end(), std::back_inserter(tag));
				if (tag != token.m_tag_name) base_type::report_error();
				// 3.
			Loop:
				if (node->m_it == m_stack.front().m_it) return;
				// 4.
				tmp = traits::get_local_name(node->m_it);
				tag.clear();
				to_ascii_lowercase(tmp.begin(), tmp.end(), std::back_inserter(tag));
				if (tag == token.m_tag_name)
				{
					pop_until(node->m_it);
					return;
				}
				// 5.
				--node;
				// 6.
				if (traits::get_namespace_name(node->m_it) != ns_name::HTML) goto Loop;
				// 7.
				switch (m_insertion_mode)
				{
				case mode_name::initial_insertion_mode:
					on_initial_insertion_mode(token);
					return;
				case mode_name::before_html_insertion_mode:
					on_before_html_insertion_mode(token);
					return;
				case mode_name::before_head_insertion_mode:
					on_before_head_insertion_mode(token);
					return;
				case mode_name::in_head_insertion_mode:
					on_in_head_insertion_mode(token);
					return;
				case mode_name::in_head_noscript_insertion_mode:
					on_in_head_noscript_insertion_mode(token);
					return;
				case mode_name::after_head_insertion_mode:
					on_after_head_insertion_mode(token);
					return;
				case mode_name::in_body_insertion_mode:
					on_in_body_insertion_mode(token);
					return;
				case mode_name::text_insertion_mode:
					on_text_insertion_mode(token);
					return;
				case mode_name::in_table_insertion_mode:
					on_in_table_insertion_mode(token);
					return;
				case mode_name::in_table_text_insertion_mode:
					on_in_table_text_insertion_mode(token);
					return;
				case mode_name::in_caption_insertion_mode:
					on_in_caption_insertion_mode(token);
					return;
				case mode_name::in_column_group_insertion_mode:
					on_in_column_group_insertion_mode(token);
					return;
				case mode_name::in_table_body_insertion_mode:
					on_in_table_body_insertion_mode(token);
					return;
				case mode_name::in_row_insertion_mode:
					on_in_row_insertion_mode(token);
					return;
				case mode_name::in_cell_insertion_mode:
					on_in_cell_insertion_mode(token);
					return;
				case mode_name::in_select_insertion_mode:
					on_in_select_insertion_mode(token);
					return;
				case mode_name::in_select_in_table_insertion_mode:
					on_in_select_in_table_insertion_mode(token);
					return;
				case mode_name::in_template_insertion_mode:
					on_in_template_insertion_mode(token);
					return;
				case mode_name::after_body_insertion_mode:
					on_after_body_insertion_mode(token);
					return;
				case mode_name::in_frameset_insertion_mode:
					on_in_frameset_insertion_mode(token);
					return;
				case mode_name::after_frameset_insertion_mode:
					on_after_frameset_insertion_mode(token);
					return;
				case mode_name::after_after_body_insertion_mode:
					on_after_after_body_insertion_mode(token);
					return;
				case mode_name::after_after_frameset_insertion_mode:
					on_after_after_frameset_insertion_mode(token);
					return;
				}
			}

			assert(false);
		}

		// ----------------------------------------------------------------------------------------
		// 12.2.7 The end
		//
		// https://html.spec.whatwg.org/multipage/parsing.html#the-end
		// ----------------------------------------------------------------------------------------

		/*! @brief 構文解析を停止する

		スクリプトに対応しないため、ほとんど何も実装しない。

		https://html.spec.whatwg.org/multipage/parsing.html#stop-parsing
		*/
		void stop_parsing()
		{
			traits::set_document_ready_state(encoding_cast<string_type>(U"interactive"));
			m_stack.clear();
			traits::set_document_ready_state(encoding_cast<string_type>(U"complete"));
		}

		/*! @brief 構文解析を中止する

		https://html.spec.whatwg.org/multipage/parsing.html#abort-a-parser
		*/
		void abort_parser()
		{
			base_type::m_c.clear();

			traits::set_document_ready_state(encoding_cast<string_type>(U"interactive"));
			m_stack.clear();
			traits::set_document_ready_state(encoding_cast<string_type>(U"complete"));
		}
	};
}
