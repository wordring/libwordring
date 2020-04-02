﻿#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <cstdint>

namespace wordring::whatwg::html::parsing
{
	// 12.2.2 Parse errors ----------------------------------------------------

	enum class error : std::uint32_t
	{
		abrupt_closing_of_empty_comment,
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
	
	enum encoding_confidence : std::uint32_t
	{
		tentative,
		certain,
		irrelevant,
	};

	/*
	// 12.2.5 Tokenization ----------------------------------------------------

	enum tokenizer_state : std::uint32_t
	{
		data_state,
		RCDATA_state,
		RAWTEXT_state,
		script_data_state,
		PLAINTEXT_state,
		tag_open_state,
		end_tag_open_state,
		tag_name_state,
		RCDATA_less_than_sign_state,
		RCDATA_end_tag_open_state,
		RCDATA_end_tag_name_state,
		RAWTEXT_less_than_sign_state,
		RAWTEXT_end_tag_open_state,
		RAWTEXT_end_tag_name_state,
		script_data_less_than_sign_state,
		script_data_end_tag_open_state,
		script_data_end_tag_name_state,
		script_data_escape_start_state,
		script_data_escape_start_dash_state,
		script_data_escaped_state,
		script_data_escaped_dash_state,
		script_data_escaped_dash_dash_state,
		script_data_escaped_less_than_sign_state,
		script_data_escaped_end_tag_open_state,
		script_data_escaped_end_tag_name_state,
		script_data_double_escape_start_state,
		script_data_double_escaped_state,
		script_data_double_escaped_dash_state,
		script_data_double_escaped_dash_dash_state,
		script_data_double_escaped_less_than_sign_state,
		script_data_double_escape_end_state,
		before_attribute_name_state,
		attribute_name_state,
		after_attribute_name_state,
		before_attribute_value_state,
		attribute_value_double_quoted_state,
		attribute_value_single_quoted_state,
		attribute_value_unquoted_state,
		after_attribute_value_quoted_state,
		self_closing_start_tag_state,
		bogus_comment_state,
		markup_declaration_open_state,
		comment_start_state,
		comment_start_dash_state,
		comment_state,
		comment_less_than_sign_state,
		comment_less_than_sign_bang_state,
		comment_less_than_sign_bang_dash_state,
		comment_less_than_sign_bang_dash_dash_state,
		comment_end_dash_state,
		comment_end_state,
		comment_end_bang_state,
		DOCTYPE_state,
		before_DOCTYPE_name_state,
		DOCTYPE_name_state,
		after_DOCTYPE_name_state,
		after_DOCTYPE_public_keyword_state,
		before_DOCTYPE_public_identifier_state,
		DOCTYPE_public_identifier_double_quoted_state,
		DOCTYPE_public_identifier_single_quoted_state,
		after_DOCTYPE_public_identifier_state,
		between_DOCTYPE_public_and_system_identifiers_state,
		after_DOCTYPE_system_keyword_state,
		before_DOCTYPE_system_identifier_state,
		DOCTYPE_system_identifier_double_quoted_state,
		DOCTYPE_system_identifier_single_quoted_state,
		after_DOCTYPE_system_identifier_state,
		bogus_DOCTYPE_state,
		CDATA_section_state,
		CDATA_section_bracket_state,
		CDATA_section_end_state,
		character_reference_state,
		named_character_reference_state,
		ambiguous_ampersand_state,
		numeric_character_reference_state,
		hexadecimal_character_reference_start_state,
		decimal_character_reference_start_state,
		hexadecimal_character_reference_state,
		decimal_character_reference_state,
		numeric_character_reference_end_state,
	};

	// 12.2.6.4 The rules for parsing tokens in HTML content ------------------

	enum insertion_mode : std::uint32_t
	{
		initial_insertion_mode,
		before_html_insertion_mode,
		before_head_insertion_mode,
		in_head_insertion_mode,
		in_head_noscript_insertion_mode,
		after_head_insertion_mode,
		in_body_insertion_mode,
		text_insertion_mode,
		in_table_insertion_mode,
		in_table_text_insertion_mode,
		in_caption_insertion_mode,
		in_column_group_insertion_mode,
		in_table_body_insertion_mode,
		in_row_insertion_mode,
		in_cell_insertion_mode,
		in_select_insertion_mode,
		in_select_in_table_insertion_mode,
		in_template_insertion_mode,
		after_body_insertion_mode,
		in_frameset_insertion_mode,
		after_frameset_insertion_mode,
		after_after_body_insertion_mode,
		after_after_frameset_insertion_mode,
	};
	*/
}
