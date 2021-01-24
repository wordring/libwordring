#pragma once

// ------------------------------------------------------------------------------------------------
// 18. API Hooks
//
// https://drafts.csswg.org/selectors-4/#api-hooks
// https://triple-underscore.github.io/selectors4-ja.html#api-hooks
// ------------------------------------------------------------------------------------------------

#include <wordring/wwwc/selectors/grammar.hpp>

#include <wordring/html/html_defs.hpp>
#include <wordring/tree/tree_iterator.hpp>

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
	* @param [in]     src ソース文字列
	* @param [in/out] ctx 構文解析コンテキスト
	* 
	* @sa https://drafts.csswg.org/selectors-4/#parse-a-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#parse-a-selector
	*/
	inline selector_list parse_selector(std::u32string const& src, parse_context& ctx)
	{
		selector_list sl = parse_grammar<selector_list>(std::u32string(src), ctx);
		return sl;
	}

	// --------------------------------------------------------------------------------------------
	// 18.2. Parse A Relative Selector
	//
	// @sa https://drafts.csswg.org/selectors-4/#parse-relative-selector
	// @sa https://triple-underscore.github.io/selectors4-ja.html#parse-relative-selector
	// --------------------------------------------------------------------------------------------

	/*! @brief 相対セレクタとして構文解析する
	* 
	* 未実装。
	*/
	inline relative_selector_list parse_relative_selector(std::u32string const& src, parse_context ctx)
	{
		relative_selector_list rsl = parse_grammar<relative_selector_list>(std::u32string(src), ctx);
		return rsl;
	}

	// --------------------------------------------------------------------------------------------
	// 18.3. Match a Selector Against an Element
	//
	// @sa https://drafts.csswg.org/selectors-4/#match-against-element
	// @sa https://triple-underscore.github.io/selectors4-ja.html#match-against-element
	//
	// 各構文クラスに内蔵。
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// 18.4. Match a Selector Against a Pseudo-element
	//
	// @sa https://drafts.csswg.org/selectors-4/#match-against-pseudo-element
	// @sa https://triple-underscore.github.io/selectors4-ja.html#match-against-pseudo-element
	//
	// 構文クラスに内蔵。
	// --------------------------------------------------------------------------------------------

	// --------------------------------------------------------------------------------------------
	// 18.5. Match a Selector Against a Tree
	//
	// @sa https://drafts.csswg.org/selectors-4/#match-against-tree
	// @sa https://triple-underscore.github.io/selectors4-ja.html#match-against-tree
	// --------------------------------------------------------------------------------------------

	/*! @brief 木とセレクタを照合する
	*
	* @param [in]  s                     構文クラス
	* @param [in]  pctx                  構文解析コンテキスト
	* @param [in]  first                 根要素リストの開始
	* @param [in]  last                  根要素リストの終端
	* @param [out] out                   セレクタに合致したノードを出力するイテレータ
	* @param [in]  scoping_root          スコープの根要素
	* @param [in]  scope_elements_first  :scope 疑似クラスと合致する要素集合の開始
	* @param [in]  scope_elements_last   :scope 疑似クラスと合致する要素集合の終端
	* @param [in]  pseudo_elements_first サポートする疑似要素リストの開始
	* @param [in]  pseudo_elements_last  サポートする疑似要素リストの終端
	* 
	* @return 照合が成功した場合 true を返す
	* 
	* サポートする疑似要素リストとは、 pseudo_element_id_name のリスト。
	* 省略時はすべての疑似要素。
	* ただし wordring_cpp は現在、一切の疑似要素に対応していない。
	* 
	* @sa https://drafts.csswg.org/selectors-4/#match-a-selector-against-a-tree
	* @sa https://triple-underscore.github.io/selectors4-ja.html#match-a-selector-against-a-tree
	* 
	*/
	template <typename Selector, typename NodePointer, typename OutputIterator, typename PseudoElementIterator = std::nullptr_t>
	inline void match_selector(
		Selector const& s,
		parse_context const& pctx,
		NodePointer first,
		NodePointer last,
		OutputIterator out,
		NodePointer scoping_root         = wordring::html::node_traits<NodePointer>::pointer(),
		NodePointer scope_elements_first = wordring::html::node_traits<NodePointer>::pointer(),
		NodePointer scope_elements_last  = wordring::html::node_traits<NodePointer>::pointer(),
		PseudoElementIterator pseudo_elements_first = nullptr,
		PseudoElementIterator pseudo_elements_last  = nullptr)
	{
		using namespace wordring::html;
		using traits = node_traits<NodePointer>;

		document_type_name type = static_cast<document_type_name>(0);
		document_mode_name mode = static_cast<document_mode_name>(0);

		if (scope_elements_first == traits::pointer())
		{
			if (scoping_root != traits::pointer())
			{
				scope_elements_first = scoping_root;
				scope_elements_last = traits::next(scoping_root);
			}
			else
			{
				scope_elements_first = first;
				scope_elements_last = last;
			}
		}

		for (auto p = first; p != traits::pointer(); p = traits::parent(p))
		{
			if (traits::is_document(p))
			{
				type = traits::get_document_type(p);
				mode = traits::get_document_mode(p);
				break;
			}
		}

		match_context<NodePointer> mctx;
		mctx.m_type_name      = type;
		mctx.m_mode_name      = mode;
		mctx.m_scoping_root   = scoping_root;
		mctx.m_scope_elements = { scope_elements_first, scope_elements_last };
		mctx.m_namespace_uris = pctx.m_namespace_uris;

		while (first != last)
		{
			wordring::tree_iterator<NodePointer> it1(first), it2;
			while (it1 != it2)
			{
				if (scoping_root != traits::pointer())
				{
					for (NodePointer p = it1.base(); p != scoping_root; p = traits::parent(p))
					{
						if (p == traits::pointer()) continue;
					}
				}
				// 要素と照合
				if(s.match(it1.base(), mctx)) *out++ = it1.base();
				// 疑似要素と照合
				{
					// TODO:
				}
				++it1;
			}
			++first;
		}
	}
}
