#pragma once

// ------------------------------------------------------------------------------------------------
// 18. API Hooks
//
// https://drafts.csswg.org/selectors-4/#api-hooks
// https://triple-underscore.github.io/selectors4-ja.html#api-hooks
// ------------------------------------------------------------------------------------------------

#include <wordring/wwwc/selectors/grammar.hpp>

#include <algorithm>
#include <string>

namespace wordring::wwwc::css
{

	// --------------------------------------------------------------------------------------------
	// 18.1. Parse A Selector
	//
	// https://drafts.csswg.org/selectors-4/#parse-selector
	// https://triple-underscore.github.io/selectors4-ja.html#parse-a-selector
	// --------------------------------------------------------------------------------------------

	/*! @brief セレクタとして構文解析する
	*
	* @sa https://drafts.csswg.org/selectors-4/#parse-a-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#parse-a-selector
	*/
	inline selector_list parse_selector(std::u32string const& src)
	{
		parse_grammar(std::u32string(src), [&](std::vector<syntax_primitive> const& v)->bool
		{
			return true;
		});

		selector_list::consume(syntax_primitive_stream());
	}

	// --------------------------------------------------------------------------------------------
	// 
	//
	// 
	// 
	// --------------------------------------------------------------------------------------------
}
