﻿#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/html/simple_policy.hpp>

//#include <wordring/whatwg/encoding/encoding.hpp>

#include <wordring/whatwg/html/parsing/token.hpp>
//#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
//#include <wordring/whatwg/html/parsing/input_stream.hpp>
#include <wordring/whatwg/html/parsing/parser_defs.hpp>
//#include <wordring/whatwg/html/parsing/tokenization.hpp>
#include <wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp>

#include <variant>

namespace wordring::whatwg::html::parsing
{

}

namespace wordring::whatwg::html::simple
{
	namespace parsing = wordring::whatwg::html::parsing;

	template <typename T, typename String, typename Container, bool IsFragmentsParser = false>
	class parser : public parsing::tree_construction_dispatcher<T, tree_construction_policy<String, Container, IsFragmentsParser>>
	{
	public:
		using policy = tree_construction_policy<String, Container, IsFragmentsParser>;

		using base_type = parsing::tree_construction_dispatcher<T, policy>;
		using this_type = T;

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

		using base_type::report_error;
		using base_type::eof;

	public:
		parser()
			: m_script_nesting_level(0)
			, m_parser_pause_flag(false)
		{
		}

		// ノード型判定 --------------------------------------------------------

		bool is_document(node_pointer it) const { return std::holds_alternative<document_node_type>(*it); }

		bool is_document_type(node_pointer it) const { return std::holds_alternative<document_type_node_type>(*it); }

		bool is_document_fragment(node_pointer it) const { return std::holds_alternative<document_fragment_node_type>(*it); }

		bool is_element(node_pointer it) const { return std::holds_alternative<element_node_type>(*it); }

		bool is_text(node_pointer it) const { return std::holds_alternative<text_node_type>(*it); }

		bool is_processing_instruction(node_pointer it) const { return std::holds_alternative<processing_instruction_node_type>(*it); }

		bool is_comment(node_pointer it) const { return std::holds_alternative<comment_node_type>(*it); }

		// ノード取得 ----------------------------------------------------------

		document_node_type const* to_document(node_pointer it) const { return std::get_if<document_node_type>(std::addressof(*it)); }

		document_type_node_type const* to_document_type(node_pointer it) const { return std::get_if<document_type_node_type>(std::addressof(*it)); }

		document_fragment_node_type const* to_document_fragment(node_pointer it) const { return std::get_if<document_fragment_node_type>(std::addressof(*it)); }

		element_node_type const* to_element(node_pointer it) const { return std::get_if<element_node_type>(std::addressof(*it)); }

		text_node_type const* to_text(node_pointer it) const { return std::get_if<text_node_type>(std::addressof(*it)); }

		processing_instruction_node_type const* to_processing_instruction(node_pointer it) const { return std::get_if<processing_instruction_node_type>(std::addressof(*it)); }

		comment_node_type const* to_comment(node_pointer it) const { return std::get_if<comment_node_type>(std::addressof(*it)); }

		// 文書 ---------------------------------------------------------------

		/*! @brief 最初の子を返す
		*/
		node_pointer begin() { return m_c.begin(); }

		/*! @brief 最後の子の次を返す
		*/
		node_pointer end() { return m_c.end(); }

		bool is_iframe_srcdoc() const { return false; }

		// 要素 ---------------------------------------------------------------

		/*! @brief 親要素を返す
		*/
		node_pointer parent(node_pointer it) { return it.parent(); }

		/*! @brief 最初の子を返す
		*/
		node_pointer begin(node_pointer it) { return it.begin(); }

		/*! @brief 最後の子の次を返す
		*/
		node_pointer end(node_pointer it) { return it.end(); }

		/*!
		*/
		template <typename T>
		node_pointer insert(node_pointer pos, T node)
		{
			return m_c.insert(pos, std::forward<T>(node));
		}

		/*! @brief 二つの要素が同じシグネチャを持つか調べる

		push_active_formatting_element_list() から呼び出される。
		*/
		bool equals(node_pointer lhs, node_pointer rhs) const
		{
			return *lhs == *rhs;
		}

		ns_name namespace_uri_name(node_pointer it) const { return to_element(it)->namespace_uri_atom(); }

		tag_name local_name_name(node_pointer it) { return to_element(it)->local_name_atom(); }

		// ノード作成 ----------------------------------------------------------

		document_node_type create_document()
		{
		}

		document_type_node_type create_document_type(std::u32string const& name, std::u32string const& public_id, std::u32string const& system_id)
		{
			document_type_node_type doctype;

			doctype.name(encoding_cast<string_type>(name));
			doctype.public_id(encoding_cast<string_type>(public_id));
			doctype.system_id(encoding_cast<string_type>(system_id));

			return doctype;
		}

		document_fragment_node_type create_document_fragment(){}

		/* @brief 要素を作成する

		- https://dom.spec.whatwg.org/#concept-create-element に対応する要素作成関数
		- https://triple-underscore.github.io/DOM4-ja.html#concept-create-element


		*/
		element_node_type create_element(string_type local, string_type ns, string_type prefix)
		{
			element_node_type el;

			
			return el;
		}

		text_node_type create_text() {}

		processing_instruction_node_type create_processing_instruction() {}

		comment_node_type create_comment() {}


		// 

		void append_document_type(document_type_node_type const& doctype)
		{

		}

		void insert_comment(node_pointer pos, comment_token& comment)
		{
			m_c.insert(pos, comment_node_type(comment.m_data));
		}

		/*! @brief 与えられたノードがHTML名前空間に属するか調べる
		*/
		/*bool in_html_namespace(node_pointer it) const
		{
			if (std::holds_alternative<element_node_type>(*it))
			{
				element_node_type const& elm = std::get<element_node_type>(*it);
				return elm.namespace_uri_atom() == ns_name::HTML;
			}

			return false;
		}*/


		//void change_encoding(wordring::whatwg::encoding::name name) {}

		void on_report_error(parsing::error e) {}

	protected:

		//encoding_confidence m_confidence;

		std::uint32_t m_script_nesting_level;
		bool m_parser_pause_flag;

		container_type m_c;
	};
}