#pragma once

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

#include <iostream>

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

		// ノード作成 ----------------------------------------------------------

		element_node_type create_element(string_type local, string_type ns, string_type prefix)
		{

		}

		/*! @brief 与えられたノードがHTML名前空間に属するか調べる
		*/
		bool in_html_namespace(node_pointer it) const
		{
			node_type const& p = *it;

			if (std::holds_alternative<element_node_type>(p))
			{
				element_node_type const& elm = std::get<element_node_type>(p);
				return elm == parsing::namespaces::HTML;
			}

			return false;
		}

		void insert_comment(node_pointer pos, comment_token& comment)
		{
			c.insert(pos, comment);
		}


		//void change_encoding(wordring::whatwg::encoding::name name) {}

		void on_report_error(parsing::error e) {}

	protected:

		//encoding_confidence m_confidence;

		std::uint32_t m_script_nesting_level;
		bool m_parser_pause_flag;

		container_type m_container_type;
	};
}
