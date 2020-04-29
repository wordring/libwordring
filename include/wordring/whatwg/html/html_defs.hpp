#pragma once

#include <wordring/whatwg/html/parsing/atom_defs.hpp>

#include <cstdint>

namespace wordring::whatwg::html
{
	using ns_name        = parsing::ns_name;
	using tag_name       = parsing::tag_name;
	using attribute_name = parsing::attribute_name;

	// ------------------------------------------------------------------------
	// 
	//
	// 
	// ------------------------------------------------------------------------

	// ------------------------------------------------------------------------
	// 4.5. Interface Document
	//
	// https://dom.spec.whatwg.org/#interface-document
	// ------------------------------------------------------------------------

	enum class document_type_name : std::uint32_t
	{
		xml = 1,
		html,
	};

	enum class document_mode_name : std::uint32_t
	{
		no_quirks = 1,
		quirks,
		limited_quirks,
	};


	// ------------------------------------------------------------------------
	// 
	//
	// 
	// ------------------------------------------------------------------------

}
