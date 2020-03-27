#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <cstdint>
#include <string>
#include <vector>

namespace wordring::whatwg::html::parsing
{
	struct DOCTYPE_token
	{
		std::u32string name;
		std::u32string public_identifier;
		std::u32string system_identifier;
		bool force_quirks_flag;
	};

	struct tag_token
	{
		std::u32string tag_name;
		std::uint32_t tag_name_id;
		bool self_closing_flag;
		std::vector<std::u32string> attributes;

		void clear()
		{
			tag_name.clear();
			tag_name_id = 0;
			self_closing_flag = false;
			attributes.clear();
		}
	};

	struct start_tag_token : tag_token
	{
	};

	struct end_tag_token : tag_token
	{
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
