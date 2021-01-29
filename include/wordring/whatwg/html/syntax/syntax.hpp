#pragma once

#include <wordring/html/html_defs.hpp>

namespace wordring::whatwg::html
{
	template <typename NodePointer>
	inline bool is_void_element(NodePointer it)
	{
		using namespace wordring::html;
		using traits = node_traits<NodePointer>;

		if (traits::is_element(it) && traits::get_namespace_name(it) == ns_name::HTML)
		{
			switch (traits::get_local_name_name(it))
			{
			case tag_name::Area:  case tag_name::Base: case tag_name::Br:    case tag_name::Col:
			case tag_name::Embed: case tag_name::Hr:   case tag_name::Img:   case tag_name::Input:
			case tag_name::Link:  case tag_name::Meta: case tag_name::Param: case tag_name::Source:
			case tag_name::Track: case tag_name::Wbr:
				return true;
			default:
				break;
			}
		}

		return false;
	}
}