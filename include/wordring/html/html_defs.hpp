#pragma once

#include <wordring/whatwg/html/html_defs.hpp>
#include <wordring/whatwg/html/parsing/parser_defs.hpp>

namespace wordring::html
{
	using ns_name        = wordring::whatwg::html::ns_name;
	using tag_name       = wordring::whatwg::html::tag_name;
	using attribute_name = wordring::whatwg::html::attribute_name;

	using document_type_name = wordring::whatwg::html::document_type_name;
	using document_mode_name = wordring::whatwg::html::document_mode_name;

	using error_name = wordring::whatwg::html::parsing::error_name;
	using encoding_confidence_name = wordring::whatwg::html::parsing::encoding_confidence_name;

	/*! @class node_traits html_defs.hpp wordring/html/html_defs.hpp

	@brief HTMLノードへの操作を仲介するアダプタ

	自由なHTMLノード実装を許容するため、特定の実装に依存しないパーサーなどは、このクラスを通してノードを操作する。
	ノード実装へのポインタ、あるいはイテレータに基づいて呼び出せるよう、各ノード実装はこのクラスの
	テンプレート特殊化を用意しなければならない。

	@sa simple_node_traits
	*/
	template <typename NodePointer>
	struct node_traits {};
}
