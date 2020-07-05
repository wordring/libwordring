#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <cstdint>

namespace wordring::whatwg::html::parsing
{
	// 12.2.2 Parse errors ----------------------------------------------------

	enum class error_name : std::uint32_t
	{
		abrupt_closing_of_empty_comment = 1,
		abrupt_doctype_public_identifier,
		abrupt_doctype_system_identifier,
		absence_of_digits_in_numeric_character_reference,
		cdata_in_html_content,
		character_reference_outside_unicode_range,
		control_character_in_input_stream,
		control_character_reference,
		end_tag_with_attributes,
		duplicate_attribute,
		end_tag_with_trailing_solidus,
		eof_before_tag_name,
		eof_in_cdata,
		eof_in_comment,
		eof_in_doctype,
		eof_in_script_html_comment_like_text,
		eof_in_tag,
		incorrectly_closed_comment,
		incorrectly_opened_comment,
		invalid_character_sequence_after_doctype_name,
		invalid_first_character_of_tag_name,
		missing_attribute_value,
		missing_doctype_name,
		missing_doctype_public_identifier,
		missing_doctype_system_identifier,
		missing_end_tag_name,
		missing_quote_before_doctype_public_identifier,
		missing_quote_before_doctype_system_identifier,
		missing_semicolon_after_character_reference,
		missing_whitespace_after_doctype_public_keyword,
		missing_whitespace_after_doctype_system_keyword,
		missing_whitespace_before_doctype_name,
		missing_whitespace_between_attributes,
		missing_whitespace_between_doctype_public_and_system_identifiers,
		nested_comment,
		noncharacter_character_reference,
		noncharacter_in_input_stream,
		non_void_html_element_start_tag_with_trailing_solidus,
		null_character_reference,
		surrogate_character_reference,
		surrogate_in_input_stream,
		unexpected_character_after_doctype_system_identifier,
		unexpected_character_in_attribute_name,
		unexpected_character_in_unquoted_attribute_value,
		unexpected_equals_sign_before_attribute_name,
		unexpected_null_character,
		unexpected_question_mark_instead_of_tag_name,
		unexpected_solidus_in_tag,
		unknown_named_character_reference,
	};

	// 12.2.3 The input byte stream -------------------------------------------
	
	enum class encoding_confidence_name : std::uint32_t
	{
		tentative = 1, // 暫定
		certain,       // 確定
		irrelevant,    // 無関係
	};
}
