#pragma once

#include <cstdint>
#include <string_view>

namespace wordring::wwwc::css
{
	enum class pseudo_class_id_name : std::uint32_t
	{
		Root = 1,
	};

	inline pseudo_class_id_name to_pseudo_class_id_name(std::u32string_view name)
	{
		if (name == U"root") return pseudo_class_id_name::Root;

		return static_cast<pseudo_class_id_name>(0);
	}

	enum class pseudo_class_fn_name : std::uint32_t
	{
	};

	inline pseudo_class_fn_name to_pseudo_class_fn_name(std::u32string_view name)
	{
		return static_cast<pseudo_class_fn_name>(0);
	}


	// --------------------------------------------------------------------------------------------
	// CSS Pseudo-Elements Module Level 4
	//
	// https://drafts.csswg.org/css-pseudo-4/
	// https://triple-underscore.github.io/css-pseudo-ja.html
	// --------------------------------------------------------------------------------------------

	/*! @brief 疑似要素を区別する列挙体
	*
	* @sa https://drafts.csswg.org/css-pseudo-4/
	* @sa https://triple-underscore.github.io/css-pseudo-ja.html
	*/
	enum class pseudo_element_id_name : std::uint32_t
	{
		FirstLine = 1,
		FirstLetter,
		Selection,
		TargetText,
		SpellingError,
		GrammarError,
		Before,
		After,
		Marker,
		Placeholder,
		FileSelectorButton,
	};

	inline pseudo_element_id_name to_pseudo_element_id_name(std::u32string_view name)
	{
		/*
		if      (name == U"firstLine")          return pseudo_element_id_name::FirstLine;
		else if (name == U"FirstLetter")        return pseudo_element_id_name::FirstLetter;
		else if (name == U"Selection")          return pseudo_element_id_name::Selection;
		else if (name == U"TargetText")         return pseudo_element_id_name::TargetText;
		else if (name == U"SpellingError")      return pseudo_element_id_name::SpellingError;
		else if (name == U"GrammarError")       return pseudo_element_id_name::GrammarError;
		else if (name == U"Before")             return pseudo_element_id_name::Before;
		else if (name == U"After")              return pseudo_element_id_name::After;
		else if (name == U"Marker")             return pseudo_element_id_name::Marker;
		else if (name == U"Placeholder")        return pseudo_element_id_name::Placeholder;
		else if (name == U"FileSelectorButton") return pseudo_element_id_name::FileSelectorButton;
		*/

		return static_cast<pseudo_element_id_name>(0);
	}
}
