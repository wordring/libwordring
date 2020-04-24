#pragma once

#include <wordring/whatwg/html/parsing/token.hpp>

namespace wordring::whatwg::html
{
	template <typename String, typename Container, bool IsFragmentsParser = false>
	class simple_policy
	{
	public:
		static bool constexpr is_fragments_parser = IsFragmentsParser;

		using string_type    = String;
		using container_type = Container;

		using node_type    = simple_node<String>;
		using node_pointer = typename container_type::iterator;

		using document_node_type               = simple_document<string_type>;
		using document_type_node_type          = simple_document_type<string_type>;
		using document_fragment_node_type      = simple_document_fragment<string_type>;
		using element_node_type                = simple_element<string_type>;
		using text_node_type                   = simple_text<string_type>;
		using processing_instruction_node_type = simple_processing_instruction<string_type>;
		using comment_node_type                = simple_comment<string_type>;

		using attribute_type    = typename element_node_type::attribute_type;
		using attribute_pointer = typename element_node_type::iterator;
	};
}
