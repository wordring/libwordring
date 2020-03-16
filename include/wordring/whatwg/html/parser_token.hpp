#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <cstdint>
#include <string>
#include <vector>

namespace wordring::whatwg::html::detail
{
	struct DOCTYPE_token
	{
		std::u32string name;
		std::u32string public_identifier;
		std::u32string system_identifier;
		bool force_quirks_flag;
	};

	struct start_tag_token
	{
		std::u32string tag_name;
		bool self_closing_flag;
		std::vector<std::u32string> attributes;
	};

	struct end_tag_token
	{
		std::u32string tag_name;
		bool self_closing_flag;
		std::vector<std::u32string> attributes;
	};

	struct comment_token
	{
		std::u32string data;
	};

	struct character_token
	{
		char32_t data;
	};

	struct end_of_file_token
	{
	};
}
