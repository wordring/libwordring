#pragma once

#include <wordring/whatwg/url/api.hpp>

#include <wordring/compatibility.hpp>

#include <string>

namespace wordring
{
	using u8url = wordring::whatwg::basic_url<std::u8string>;
	using u16url = wordring::whatwg::basic_url<std::u16string>;
	using u32url = wordring::whatwg::basic_url<std::u32string>;

	using wordring:whatwg::to_string;
}
