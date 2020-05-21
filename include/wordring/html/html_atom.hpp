#pragma once

#include <wordring/whatwg/html/html_atom.hpp>

namespace wordring::html
{
	template <typename String, typename Name>
	using basic_html_atom = wordring::whatwg::html::basic_html_atom<String, Name>;
}