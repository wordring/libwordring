#pragma once

#include <wordring/wwwc/css_syntax/tokenization.hpp>

#include <any>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace wordring::wwwc::css
{
	// --------------------------------------------------------------------------------------------
	// 5. Parsing
	//
	// https://drafts.csswg.org/css-syntax-3/#parsing
	// https://triple-underscore.github.io/css-syntax-ja.html#parsing
	// --------------------------------------------------------------------------------------------

	/*! @brief <b>CSS成分値</b>を表現する AST ノード
	*/
	using component_value = std::any;

	/*! @brief <b>関数</b>を表現する AST ノード
	*/
	class function
	{
	public:

	protected:
		std::u32string m_name;
		std::vector<component_value> m_value;
	};

	/*! @brief <b>単純ブロック</b>を表現する AST ノード
	*/
	class simple_block
	{
	public:

	protected:
		css_token m_associated_token;
		std::vector<component_value> m_value;
	};

	/*! @brief <b>\@規則</b>を表現する AST ノード
	*/
	class at_rule
	{
	public:

	protected:
		std::u32string m_name;
		std::vector<component_value> m_prelude;
		std::optional<simple_block> m_block;
	};

	/*! @brief <b>修飾規則</b>を表現する AST ノード
	*/
	class qualified_rule
	{
	public:

	protected:
		std::vector<component_value> m_prelude;
		simple_block m_block;
	};

	/*! @brief <b>宣言</b>を表現する AST ノード
	*/
	class declaration
	{
	public:
		declaration() : m_important_flag(false) {}

	protected:
		std::u32string m_name;
		std::vector<component_value> m_value;
		bool m_important_flag;
	};

	// --------------------------------------------------------------------------------------------
	// 5.3. Parser Entry Points
	//
	// https://drafts.csswg.org/css-syntax-3/#parser-entry-points
	// https://triple-underscore.github.io/css-syntax-ja.html#parser-entry-points
	// --------------------------------------------------------------------------------------------

	/* @brief 入力をトークン列へ正規化する
	*/
	template <typename BidirectionalIterator, typename OutputIterator, typename ErrorHandler = std::nullptr_t>
	inline void normalize_into_token_stream(BidirectionalIterator first, BidirectionalIterator last, OutputIterator out, ErrorHandler handler = nullptr)
	{
		if constexpr (std::is_same_v<char32_t, decltype(*first)>) tokenize(first, last, out, handler);
		else
		{
			while (first != last) *out++ = *first++;
		}
	}

	// --------------------------------------------------------------------------------------------
	// 
	//
	// 
	// 
	// --------------------------------------------------------------------------------------------
}
