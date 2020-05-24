#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/html/html_defs.hpp>

#include <wordring/html/simple_adapter.hpp>
#include <wordring/html/simple_node.hpp>

#include <wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp>

#include <wordring/encoding/encoding.hpp>

#include <cassert>
#include <iterator>
#include <variant>

namespace wordring::html
{
	/*! @class simple_parser_base simple_parser.hpp wordring/html/simple_parser.hpp
	
	@brief simple_node 用の HTML5 パーサー

	@tparam T                 CRTP に基づく派生クラス
	@tparam String            文字列型
	@tparam Container         Tree コンテナ
	@tparam IsFragmentsParser HTML フラグメント・パーサーの場合 true


	*/
	template <typename T, typename String, typename Container>
	class simple_parser_base : public wordring::whatwg::html::parsing::tree_construction_dispatcher<T, simple_adapter<String, Container>>
	{
	public:
		using adapter = simple_adapter<String, Container>;

		using base_type = wordring::whatwg::html::parsing::tree_construction_dispatcher<T, adapter>;
		using this_type = T;

		using string_type  = typename adapter::string_type;
		using container    = typename adapter::container_type;
		using node_type    = typename adapter::node_type;
		using node_pointer = typename container::iterator;

		using document_type               = typename adapter::document_type;
		using document_type_type          = typename adapter::document_type_type;
		using document_fragment_type      = typename adapter::document_fragment_type;
		using element_type                = typename adapter::element_type;
		using text_type                   = typename adapter::text_type;
		using processing_instruction_type = typename adapter::processing_instruction_type;
		using comment_type                = typename adapter::comment_type;

		using attribute_type    = typename adapter::attribute_type;
		using attribute_pointer = typename adapter::attribute_pointer;

		using namespace_uri_type = basic_html_atom<string_type, ns_name>;
		using lacal_name_type    = basic_html_atom<string_type, tag_name>;

		//using base_type::m_stack;

		//using base_type::report_error;
		//using base_type::eof;

		//using typename base_type::mode_name;

	public:
		simple_parser_base()
			//: m_script_nesting_level(0)
			//, m_parser_pause_flag(false)
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

		- https://dom.spec.whatwg.org/#concept-create-element に対応する要素作成関数
		- https://triple-underscore.github.io/DOM4-ja.html#concept-create-element
		*/
		node_pointer create_element(node_pointer doc, std::u32string name, namespace_uri_type ns, std::u32string prefix)
		{
			element_type el(ns, encoding_cast<string_type>(prefix), encoding_cast<string_type>(name));
			return m_c.insert(m_temporary.end(), std::move(el));
		}

		node_pointer create_element(node_pointer doc, tag_name name, namespace_uri_type ns, std::u32string prefix)
		{
			element_type el(ns, encoding_cast<string_type>(prefix), name);

			return m_c.insert(m_temporary.end(), std::move(el));
		}

		node_pointer create_element(node_pointer doc, tag_name name, ns_name ns = ns_name::HTML)
		{
			element_type el(ns, string_type(), name);

			return m_c.insert(m_temporary.end(), std::move(el));
		}

		node_pointer insert_element(node_pointer pos, node_pointer it)
		{
			return m_c.move(pos, it);
		}

		/*! @brief ノードを削除する
		*/
		void erase_element(node_pointer it)
		{
			m_c.erase(it);
		}

		/*! @brief ノードを移動する
		*/
		void move_element(node_pointer pos, node_pointer it)
		{
			m_c.move(pos, it);
		}

		// ----------------------------------------------------------------------------------------
		// 属性
		// ----------------------------------------------------------------------------------------

		attribute_type create_attribute(node_pointer el, ns_name ns, std::u32string const& prefix, std::u32string const& local_name)
		{
			return attribute_type(ns, encoding_cast<string_type>(prefix), encoding_cast<string_type>(local_name));
		}

		/*! @brief 要素へ属性を付加する
		*/
		void append_attribute(node_pointer it, attribute_type&& attr)
		{
			push_back(*it, std::move(attr));
		}

		void append_attribute(node_pointer it, ns_name ns, std::u32string const& prefix, std::u32string const& name, std::u32string const& value)
		{
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

		void on_report_error(parsing::error_name e) {}

	protected:

		//std::uint32_t m_script_nesting_level;
		//bool m_parser_pause_flag;

		container    m_c;
		node_pointer m_document;
		node_pointer m_temporary;
	};

	template <typename String, typename Container>
	class simple_parser : public simple_parser_base<simple_parser<String, Container>, String, Container>
	{
	};
}
