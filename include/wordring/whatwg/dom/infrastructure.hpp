#pragma once

// ------------------------------------------------------------------------------------------------
// 1. Infrastructure
//
// @sa https://dom.spec.whatwg.org/#infrastructure
// @sa https://triple-underscore.github.io/DOM4-ja.html#infrastructure
// ------------------------------------------------------------------------------------------------

#include <wordring/wwwc/selectors/api.hpp>

#include <wordring/wwwc/css_defs.hpp>

#include <wordring/html/html_defs.hpp>

#include <cassert>

namespace wordring::whatwg::dom
{
	
	// --------------------------------------------------------------------------------------------
	// 1.3. Selectors
	//
	// @sa https://dom.spec.whatwg.org/#selectors
	// @sa https://triple-underscore.github.io/DOM4-ja.html#selectors
	// --------------------------------------------------------------------------------------------

	/*! @brief スコープ下でセレクタを照合する
	* 
	* @param [in] ノードを指すポインタあるいはイテレータ
	* @param [in] セレクタ文字列
	* @param [out] マッチしたノードを出力するイテレータ
	* 
	* @sa https://dom.spec.whatwg.org/#scope-match-a-selectors-string
	* @sa https://triple-underscore.github.io/DOM4-ja.html#scope-match-a-selectors-string
	* 
	* @internal
	* <hr>
	* 
	* 自身の子孫を querySelector() で特定できる。
	* html:
	* @code
	*	<p>text</p>
	* @endcode
	* js:
	* @codevar
	* 	var html = document.querySelector("html");
	*	console.log(html.querySelector("p"));
	* @edcode
	* result:
	* @code
	*	<p></p>
	* @endcode
	* 
	* 一方、呼び出したノード自身は querySelector() で特定できない。
	* html:
	* @code
	*	<p>text</p>
	* @endcode
	* js:
	* @codevar
	* 	var html = document.querySelector("html");
	*	console.log(html.querySelector("html"));
	* @edcode
	* result:
	* @code
	*	null
	* @endcode
	* 
	* オブジェクトの根は親が NULL の場合、自身、他の場合、親の根。
	* @sa https://triple-underscore.github.io/DOM4-ja.html#concept-tree-root
	* 
	*/
	template <typename NodePointer, typename OutputIterator>
	inline void scope_match(NodePointer np, std::u32string const& selector, OutputIterator out)
	{
		using namespace wordring::wwwc::css;
		using namespace wordring::html;
		using traits = node_traits<NodePointer>;

		// 根を探索
		NodePointer root = traits::pointer();
		if (traits::is_document(np))
		{
			root = np;
		}
		else
		{
			for (NodePointer p = np; p != traits::pointer(); p = traits::parent(p))
			{
				if (traits::is_root(p))
				{
					root = p;
					break;
				}
			}
		}

		// セレクタを構文解析
		parse_context pctx;
		selector_list sl = parse_selector(selector, pctx);

		// 照合
		match_selector(sl, pctx, root, traits::next(root), out, np);
	}

}
