#pragma once

#include <wordring/html/html_defs.hpp>

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace wordring::wwwc::css
{
	/*! @brief CSS の構文解析に使われるコンテキスト
	* 
	* 構文規則を表現する複数のクラスを横断して情報を引き継ぐ必要が有るため、コンテキスト・クラスを作った。
	* 例えば、 @namespace によって定義される名前空間は、後続の構文規則内で使われる。
	* CSS は CFG でないため、構文解析中に意味解析を必要とする場合がある。
	* 
	* 名前空間のようなノードとの照合に使われるフィールドは照合時に引数の match_context へコピーする必要が有る。
	* 
	* 構文解析の時点で文書形式に依存させないために、 match_context と別のクラスを設けた。
	* つまり、一つのセレクタで HTML と XHTML の両方に対応できることを明確に示す。
	*/
	struct parse_context
	{
		/*! @brief 名前空間接頭辞 : 名前空間 URI のマップ
		*/
		std::map<std::u32string, std::u32string> m_namespace_uris;
	};

	/*! @brief CSS セレクタとノードの照合に使われるコンテキスト
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

		/*! @brief 名前空間接頭辞 : 名前空間 URI のマップ
		* 
		* 既定の名前空間は空（U""）の接頭辞で表現する。
		*/
		std::map<std::u32string, std::u32string> m_namespace_uris;

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
	};

}
