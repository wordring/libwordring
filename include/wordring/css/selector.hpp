#pragma once

#include <wordring/whatwg/dom/infrastructure.hpp>

#include <wordring/encoding/encoding_defs.hpp>
#include <wordring/html/html_defs.hpp>

#include <iterator>
#include <string>
#include <vector>

namespace wordring::css
{
	/*! @brief セレクタと合致するノードをすべて列挙する
	*
	* @tparam NodePointer    ノードへのポインタあるいはイテレータ
	* @tparam String         UTF-8 UTF-16 UTF-32 といったユニコード文字列
	* @tparam OutputIterator 出力イテレータ
	*
	* @param [in]  np       起点となるノードへのポインタあるいはイテレータ
	* @param [in]  selector セレクタ文字列
	* @param [out] out      合致するノードを書き込む出力イテレータ
	*
	* 起点となるノード自身はセレクタと一致しません。
	* 
	* セレクタ文字列は UTF-8 UTF-16 UTF-32 といったユニコード文字列であれば十分であり、
	* HTML ノードの文字コードと一致させる必要はありません。
	* 
	* 出力イテレータは、std::vector<NodePointer> といったコンテナへの std::back_inserter
	* が一般的です。
	* 
	* @par 例
	* @code
	* 	namespace css  = wordring::css;
	* 	namespace html = wordring::html;
	*
	* 	std::string_view sv = u8"<p>text1</p><p>text2</p>";
	* 	html::u8simple_tree doc = html::make_document<html::u8simple_tree>(sv.begin(), sv.end());
	*
	* 	std::vector<html::u8simple_tree::const_iterator> v;
	* 	css::query_selector_all(doc.begin(), u"p", std::back_inserter(v));
	* 
	* 	std::u32string s;
	* 	html::to_string(v[0], std::back_inserter(s)); // s == U"text1"
	* @endcode
	* 
	* @sa https://triple-underscore.github.io/DOM4-ja.html#dom-parentnode-queryselectorall
	* @sa https://dom.spec.whatwg.org/#dom-parentnode-queryselectorall
	*/
	template <typename NodePointer, typename String, typename OutputIterator>
	inline void query_selector_all(NodePointer np, String const& selector, OutputIterator out)
	{
		std::u32string src = wordring::encoding_cast<std::u32string>(selector);
		wordring::whatwg::dom::scope_match(np, src, out);
	}

	/*! @brief セレクタと合致する最初のノードを返す
	*
	* @tparam NodePointer    ノードへのポインタあるいはイテレータ
	* @tparam String         UTF-8 UTF-16 UTF-32 といったユニコード文字列
	*
	* @param [in]  np       起点となるノードへのポインタあるいはイテレータ
	* @param [in]  selector セレクタ文字列
	*
	* 起点となるノード自身はセレクタと一致しません。
	*
	* セレクタ文字列は UTF-8 UTF-16 UTF-32 といったユニコード文字列であれば十分であり、
	* HTML ノードの文字コードと一致させる必要はありません。
	*
	* @par 例
	* @code
	* 	namespace css = wordring::css;
	*	namespace html = wordring::html;
	*
	*	std::u32string_view sv = U"<p>text1</p><p>text2</p>";
	*	html::u8simple_tree tree = html::make_document<html::u8simple_tree>(sv.begin(), sv.end());
	*
	*	html::u8simple_tree::const_iterator it = css::query_selector(tree.begin(), u"p");
	*	std::u16string s;
	*	html::to_string(it, std::back_inserter(s)); // s == u"text1"
	* @endcode
	*
	* @sa https://triple-underscore.github.io/DOM4-ja.html#dom-parentnode-queryselector
	* @sa https://dom.spec.whatwg.org/#dom-parentnode-queryselector
	*/
	template <typename NodePointer, typename String>
	inline NodePointer query_selector(NodePointer np, String const& selector)
	{
		std::vector<NodePointer> v;
		query_selector_all(np, selector, std::back_inserter(v));
		return v.front();
	}
}
