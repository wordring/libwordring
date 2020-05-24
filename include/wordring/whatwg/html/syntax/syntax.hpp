#pragma once

#include <wordring/whatwg/html/html_defs.hpp>

namespace wordring::whatwg::html
{
	static bool is_void_element(ns_name ns, tag_name::tag_name tag)
	{
		if (ns == ns_name::HTML)
		{
			switch (tag)
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