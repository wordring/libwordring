#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/html/html_defs.hpp>

#include <wordring/html/simple_node.hpp>
#include <wordring/html/simple_traits.hpp>

#include <wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp>

#include <wordring/encoding/encoding.hpp>

#include <cassert>
#include <utility>

namespace wordring::html
{
	/*! @class simple_parser_base simple_parser.hpp wordring/html/simple_parser.hpp
	
	@brief simple_node 用の HTML5 パーサー

	@tparam T         CRTP に基づく派生クラス
	@tparam Container Tree コンテナ

	*/
	template <typename T, typename Container>
	class simple_parser_base : public wordring::whatwg::html::parsing::tree_construction_dispatcher<T, node_traits<typename Container::iterator>>
	{
	public:
		using traits = node_traits<typename Container::iterator>;

		using base_type = wordring::whatwg::html::parsing::tree_construction_dispatcher<T, traits>;
		using this_type = T;

		using container = Container;

		using string_type  = typename traits::string_type;
		using node_type    = typename traits::node_type;
		using node_pointer = typename traits::node_pointer;

		using document_type               = typename traits::document_type;
		using document_type_type          = typename traits::document_type_type;
		using document_fragment_type      = typename traits::document_fragment_type;
		using element_type                = typename traits::element_type;
		using text_type                   = typename traits::text_type;
		using processing_instruction_type = typename traits::processing_instruction_type;
		using comment_type                = typename traits::comment_type;

		using attribute_type    = typename traits::attribute_type;
		using attribute_pointer = typename traits::attribute_pointer;

		using namespace_uri_type = basic_html_atom<string_type, ns_name>;
		using lacal_name_type    = basic_html_atom<string_type, tag_name>;

	public:
		simple_parser_base()
		{
			m_document  = m_c.insert(m_c.end(), document_type());
			m_temporary = m_c.insert(m_c.end(), node_type());
		}

		// ----------------------------------------------------------------------------------------
		// 文書
		// ----------------------------------------------------------------------------------------
		
		/*! @brief 文書ノードを返す

		パーサ構築時に文書ノードが挿入される。
		*/
		node_pointer get_document() { return m_document; }
		
		// ----------------------------------------------------------------------------------------
		// 文書型
		// ----------------------------------------------------------------------------------------

		node_pointer insert_document_type(node_pointer pos, document_type_type&& doctype)
		{
			return m_c.insert(pos, std::move(doctype));
		}

		// ----------------------------------------------------------------------------------------
		// 要素
		// ----------------------------------------------------------------------------------------

		/* @brief 要素を作成する

		@sa https://dom.spec.whatwg.org/#concept-create-element
		@sa https://triple-underscore.github.io/DOM4-ja.html#concept-create-element
		*/
		node_pointer create_element(node_pointer doc, std::u32string name, ns_name ns)
		{
			return m_c.insert(m_temporary.end(), element_type(ns, string_type(), encoding_cast<string_type>(name)));
		}

		node_pointer create_element(node_pointer doc, tag_name name, ns_name ns)
		{	
			return m_c.insert(m_temporary.end(), element_type(ns, string_type(), name));
		}

		node_pointer insert_element(node_pointer pos, node_pointer it)
		{
			return m_c.move(pos, it);
		}

		node_pointer get_node_document(node_pointer it)
		{
			return get_document();
		}

		/*! @brief ノードを削除する
		*/
		void erase_element(node_pointer it)
		{
			m_c.erase(it);
		}

		/*! @brief ノードを移動する
		*/
		void move_node(node_pointer pos, node_pointer it)
		{
			m_c.move(pos, it);
		}

		// ----------------------------------------------------------------------------------------
		// 属性
		// ----------------------------------------------------------------------------------------

		void append_attribute(node_pointer it, ns_name ns, std::u32string const& prefix, std::u32string const& name, std::u32string const& value)
		{
			push_back(*it, attribute_type(ns, encoding_cast<string_type>(prefix), encoding_cast<string_type>(name), encoding_cast<string_type>(value)));
		}

		bool contains(node_pointer it, ns_name ns, std::u32string const& prefix, std::u32string const& name)
		{
			return wordring::html::find(*it, ns, encoding_cast<string_type>(prefix), encoding_cast<string_type>(name)) != wordring::html::end(*it);
		}

		// ----------------------------------------------------------------------------------------
		// テキスト
		// ----------------------------------------------------------------------------------------

		node_pointer insert_text(node_pointer pos, text_type&& text)
		{
			return m_c.insert(pos, std::move(text));
		}

		// ----------------------------------------------------------------------------------------
		// コメント
		// ----------------------------------------------------------------------------------------

		node_pointer insert_comment(node_pointer pos, comment_type&& comment)
		{
			return m_c.insert(pos, std::move(comment));
		}

		// ----------------------------------------------------------------------------------------
		// 解析エラー
		//
		// 12.2.2 Parse errors
		// https://html.spec.whatwg.org/multipage/parsing.html#parse-errors
		// ----------------------------------------------------------------------------------------

		/*! @brief エラーが起きた時に呼び出されるコールバック
		*/
		void on_report_error(error_name e) {}

		// ----------------------------------------------------------------------------------------
		// 符号化法
		//
		// 12.2.3.3 Character encodings
		// https://html.spec.whatwg.org/multipage/parsing.html#character-encodings
		// ----------------------------------------------------------------------------------------

		/*! @brief 文字エンコーディングを変更する
		*/
		void on_change_encoding(encoding_name name)
		{
		}

	protected:
		container    m_c;
		node_pointer m_document;
		node_pointer m_temporary;
	};

	template <typename Container>
	class simple_parser : public simple_parser_base<simple_parser<Container>, Container>
	{
	};
}
