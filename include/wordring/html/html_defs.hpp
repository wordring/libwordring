#pragma once

#include <wordring/whatwg/html/html_defs.hpp>
#include <wordring/whatwg/html/parsing/parser_defs.hpp>

namespace wordring::html
{
	using ns_name        = wordring::whatwg::html::ns_name;
	using tag_name       = wordring::whatwg::html::tag_name;
	using attribute_name = wordring::whatwg::html::attribute_name;

	using document_type_name = wordring::whatwg::html::document_type_name;
	using document_mode_name = wordring::whatwg::html::document_mode_name;
}

namespace wordring::html::parsing
{
	using error_name               = wordring::whatwg::html::parsing::error_name;
	using encoding_confidence_name = wordring::whatwg::html::parsing::encoding_confidence_name;
}
