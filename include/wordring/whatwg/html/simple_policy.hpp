#pragma once

//#include <wordring/whatwg/html/parsing/token.hpp>
#include <wordring/whatwg/html/simple_node.hpp>

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

		using document_type               = simple_document<string_type>;
		using document_type_type          = simple_document_type<string_type>;
		using document_fragment_type      = simple_document_fragment<string_type>;
		using element_type                = simple_element<string_type>;
		using text_type                   = simple_text<string_type>;
		using processing_instruction_type = simple_processing_instruction<string_type>;
		using comment_type                = simple_comment<string_type>;

		using attribute_type    = typename element_type::attribute_type;
		using attribute_pointer = typename element_type::const_iterator;
	};
}
