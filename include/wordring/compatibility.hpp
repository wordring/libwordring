#pragma once

#include <limits>
#include <string>
#include <string_view>

#ifndef __cpp_lib_char8_t
using char8_t = char;
namespace std
{
	using u8string = basic_string<char8_t>;
	using u8string_view = basic_string_view<char8_t>;
}
#endif
