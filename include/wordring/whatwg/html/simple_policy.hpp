#pragma once

#include <wordring/whatwg/html/parsing/token.hpp>

namespace wordring::whatwg::html::simple
{
	template <typename String, typename Container, bool IsFragmentsParser = false>
	class tree_construction_policy
	{
	public:
		static bool constexpr is_fragments_parser = IsFragmentsParser;

		using string_type    = String;
		using container_type = Container;

		using node_type    = basic_node<String>;
		using node_pointer = typename container_type::iterator;

		using document_node_type               = basic_document<string_type>;
		using document_type_node_type          = basic_document_type<string_type>;
		using document_fragment_node_type      = basic_document_fragment<string_type>;
		using element_node_type                = basic_element<string_type>;
		using text_node_type                   = basic_text<string_type>;
		using processing_instruction_node_type = basic_processing_instruction<string_type>;
		using comment_node_type                = basic_comment<string_type>;

		using attribute_type = basic_attr<string_type>;
		//using attribute_pointer = 

		/*
		using DOCTYPE_token     = wordring::whatwg::html::parsing::DOCTYPE_token;
		using tag_token         = wordring::whatwg::html::parsing::tag_token;
		using start_tag_token   = wordring::whatwg::html::parsing::start_tag_token;
		using end_tag_token     = wordring::whatwg::html::parsing::end_tag_token;
		using comment_token     = wordring::whatwg::html::parsing::comment_token;
		using character_token   = wordring::whatwg::html::parsing::character_token;
		using end_of_file_token = wordring::whatwg::html::parsing::end_of_file_token;
		*/

	};
}
