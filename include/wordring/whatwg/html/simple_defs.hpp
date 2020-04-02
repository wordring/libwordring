#pragma once

#include <wordring/whatwg/html/parsing/atom_defs.hpp>
#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/token.hpp>

namespace wordring::whatwg::html::simple
{
	using namespaces = wordring::whatwg::html::parsing::namespaces;

	using DOCTYPE_token     = wordring::whatwg::html::parsing::DOCTYPE_token;
	using tag_token         = wordring::whatwg::html::parsing::tag_token;
	using start_tag_token   = wordring::whatwg::html::parsing::start_tag_token;
	using end_tag_token     = wordring::whatwg::html::parsing::end_tag_token;
	using comment_token     = wordring::whatwg::html::parsing::comment_token;
	using character_token   = wordring::whatwg::html::parsing::character_token;
	using end_of_file_token = wordring::whatwg::html::parsing::end_of_file_token;
};