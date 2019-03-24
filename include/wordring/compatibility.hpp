#pragma once

#include <limits>

#ifdef __cpp_lib_char8_t
using char_utf8_type = char8_t;
#else
using char_utf8_type = char;
#endif
