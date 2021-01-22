#pragma once

#include <wordring/html/html_defs.hpp>

#include <optional>
#include <vector>

namespace wordring::wwwc::css
{
	/*! @brief セレクタとノードの照合に使われるcontext
	*/
	template <typename NodePointer>
	struct match_context
	{
		using document_type_name = wordring::html::document_type_name;
		using document_mode_name = wordring::html::document_mode_name;

		using node_pointer = NodePointer;
		using traits = typename wordring::html::node_traits<node_pointer>;

		/*! @brief 文書型
		* 
		* XML、あるいはHTML。
		* 属性値等の文字照合で大文字小文字を区別するか決定するために使われる。
		*/
		document_type_name m_type_name = static_cast<document_type_name>(0);

		/*! @brief 文書モード
		*
		* NoQuirks 、 Quirks あるいは LimitedQuirks。
		* 属性値等の文字照合で大文字小文字を区別するか決定するために使われる。
		*/
		document_mode_name m_mode_name = static_cast<document_mode_name>(0);

		/*! @brief 名前空間をサポートするかを示すフラグ
		* 
		* スタイルシートのセレクタとしてマッチする場合、名前空間をサポートする必要が有る。
		* querySelector() としてマッチする場合、名前空間のサポートは無い。
		*
		* wordring_cpp は、まだスタイルシートを実装していないので、常に false 。
		*/
		bool m_namespace_enabled = false;

		/*! @brief root 要素
		*
		* HTML 要素のような実際の root 要素の場合と、DocumentFragment のような仮の root の場合がある。
		* 
		* 指定されない場合、 :root と等価の要素となる。
		* 
		* @sa https://triple-underscore.github.io/selectors4-ja.html#scoping-root
		*/
		node_pointer m_scoping_root = traits::pointer();

		/*! @brief scope 要素
		*
		* このフィールドが空の場合において、 m_scoping_root が存在する場合それ、存在しないない場合 :root と等価の要素となる。
		* @sa https://triple-underscore.github.io/selectors4-ja.html#scope-element
		*/
		std::vector<node_pointer> m_scope_elements;

		/*! @brief 既定の名前空間 URI
		*
		* CSS スタイルシートは以下の形式で規定の名前空間を指定できる。
		* @code
		* 	@namespace "http://wordring.net/foo";
		* @endcode
		*
		* 既定の名前空間は、型セレクタのマッチに使われる。
		*/
		std::u32string m_default_namespace_uri;

		/*! @brief 名前空間接頭辞から名前空間 URI を検索する
		*
		* @param [in] prefix 名前空間接頭辞
		*
		* @return 名前空間 URI
		*
		* スタイルシートは @namespace で名前空間を設定できる。
		* この関数は、名前空間接頭辞を引数に指定して、スタイルシートから名前空間 URI を検索する。
		*
		* @internal
		* <hr>
		* 現在、スタイルシートの実装が無いため、URI を返せない。
		* querySelector() は名前空間をサポートしないので、スタイルシートが実装されるまで、この関数の実装も必要とされない。
		*/
		std::u32string const* get_namespace(std::u32string const& prefix) const { return nullptr; }
	};

}
