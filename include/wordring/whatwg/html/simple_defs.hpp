#pragma once

#include <wordring/whatwg/html/parsing/atom_defs.hpp>
//#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/html_atom.hpp>
#include <wordring/whatwg/html/parsing/token.hpp>

namespace wordring::whatwg::html
{
	using ns_name        = wordring::whatwg::html::parsing::ns_name;
	using tag_name       = wordring::whatwg::html::parsing::tag_name;
	using attribute_name = wordring::whatwg::html::parsing::attribute_name;

	template <typename String, typename Atom>
	using basic_html_atom = wordring::whatwg::html::parsing::basic_html_atom<String, Atom>;
};