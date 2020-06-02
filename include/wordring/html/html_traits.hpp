#pragma once

namespace wordring::html
{
	/*! @class node_traits html_traits.hpp wordring/html/html_traits.hpp

	@brief HTMLノードへの操作を仲介するアダプタ

	自由なHTMLノード実装を許容するため、特定の実装に依存しないパーサーなどは、このクラスを通してノードを操作する。
	ノード実装へのポインタ、あるいはイテレータに基づいて呼び出せるよう、各ノード実装はこのクラスの
	テンプレート特殊化を用意しなければならない。

	@sa simple_node_traits
	*/
	template <typename NodePointer>
	struct node_traits {};
}
