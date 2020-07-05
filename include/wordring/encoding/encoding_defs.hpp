#pragma once

#include <wordring/whatwg/encoding/encoding.hpp>
#include <wordring/whatwg/encoding/encoding_defs.hpp>

namespace wordring
{
	using encoding_name = wordring::whatwg::encoding::name;

	using wordring::whatwg::to_string;
	using wordring::whatwg::encoding_cast;
	using wordring::whatwg::get_encoding_name;
}

namespace wordring::encoding
{
	using result_value    = wordring::whatwg::encoding::result_value;
	using result_finished = wordring::whatwg::encoding::result_finished;
	using result_continue = wordring::whatwg::encoding::result_continue;
	using result_error    = wordring::whatwg::encoding::result_error;

	using error_mode_name = wordring::whatwg::encoding::error_mode_name;
}