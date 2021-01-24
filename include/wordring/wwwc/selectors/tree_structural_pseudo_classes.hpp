#pragma once

// ------------------------------------------------------------------------------------------------
// 13. Tree-Structural pseudo-classes
//
// https://drafts.csswg.org/selectors-4/#structural-pseudos
// https://triple-underscore.github.io/selectors4-ja.html#structural-pseudos
// ------------------------------------------------------------------------------------------------

#include <wordring/wwwc/selectors/pseudo_class_defs.hpp>
#include <wordring/wwwc/css_defs.hpp>

#include <wordring/html/html_defs.hpp>

namespace wordring::wwwc::css
{
	// --------------------------------------------------------------------------------------------
	// 13.1. :root pseudo-class
	//
	// https://drafts.csswg.org/selectors-4/#the-root-pseudo
	// https://triple-underscore.github.io/selectors4-ja.html#the-empty-pseudo
	// --------------------------------------------------------------------------------------------

	class root_pseudo_class
	{
		using document_type_name = wordring::html::document_type_name;
		using document_mode_name = wordring::html::document_mode_name;

	public:
		template <typename NodePointer>
		static bool match(NodePointer const& np, match_context<NodePointer> const& ctx)
		{
			using traits = typename wordring::html::node_traits<NodePointer>;
			return traits::is_root(np);
		}
	};
}
