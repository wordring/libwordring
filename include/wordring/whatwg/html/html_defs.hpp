#pragma once

#include <wordring/whatwg/html/parsing/atom_defs.hpp>
#include <wordring/whatwg/html/parsing/parser_defs.hpp>

#include <wordring/whatwg/encoding/encoding_defs.hpp>

#include <cstdint>

namespace wordring::whatwg::html
{
	using ns_name        = parsing::ns_name;
	using tag_name       = parsing::tag_name;
	using attribute_name = parsing::attribute_name;

	// --------------------------------------------------------------------------------------------
	// 4.5. Interface Document
	//
	// https://dom.spec.whatwg.org/#interface-document
	// --------------------------------------------------------------------------------------------

	enum class document_type_name : std::uint32_t
	{
		Xml = 1,
		Html,
	};

	enum class document_mode_name : std::uint32_t
	{
		NoQuirks = 1,  /*!< 標準モード */
		Quirks,        /*!< 互換モード*/
		LimitedQuirks, /*!< 限定互換モード*/
	};

	using encoding_name = wordring::whatwg::encoding::encoding_name;
	using encoding_confidence_name = parsing::encoding_confidence_name;
}
