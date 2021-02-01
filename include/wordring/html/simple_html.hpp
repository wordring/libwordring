#pragma once

#include <wordring/html/simple_node.hpp>
#include <wordring/html/simple_parser.hpp>

#include <wordring/html/html_defs.hpp>

#include <wordring/encoding/encoding_defs.hpp>
#include <wordring/whatwg/html/parsing/serializing.hpp>

#include <wordring/tag_tree/tag_tree.hpp>
#include <wordring/compatibility.hpp>

#include <iterator>
#include <type_traits>

namespace wordring::html
{
	using wordring::whatwg::html::to_string;

	/*! @brief 文字列としてstd::u8string を使用する HTML ノード */
	using u8simple_node  = simple_node<std::u8string>;

	/*! @brief 文字列としてstd::u16string を使用する HTML ノード */
	using u16simple_node = simple_node<std::u16string>;

	/*! @brief 文字列としてstd::u32string を使用する HTML ノード */
	using u32simple_node = simple_node<std::u32string>;

	/*! @brief 文字列としてstd::u8string を使用する HTML 木コンテナ */
	using u8simple_tree  = tag_tree<u8simple_node>;

	/*! @brief 文字列としてstd::u16string を使用する HTML 木コンテナ */
	using u16simple_tree = tag_tree<u16simple_node>;

	/*! @brief 文字列としてstd::u32string を使用する HTML 木コンテナ */
	using u32simple_tree = tag_tree<u32simple_node>;

	template <typename T> struct is_simple_tree : std::false_type {};

	template <> struct is_simple_tree<u8simple_tree> : public std::true_type {};
	template <> struct is_simple_tree<u16simple_tree> : public std::true_type {};
	template <> struct is_simple_tree<u32simple_tree> : public std::true_type {};

	template <typename T> constexpr bool is_simple_tree_v = is_simple_tree<T>::value;

	template <typename String>
	using const_simple_tag_tree_iterator = typename wordring::detail::const_tag_tree_iterator<simple_node<String>>;

	template <typename String>
	using simple_tag_tree_iterator = typename wordring::detail::tag_tree_iterator<simple_node<String>>;

	template<>
	struct node_traits<simple_tag_tree_iterator<std::u8string>> : public simple_node_traits<simple_tag_tree_iterator<std::u8string>> {};

	template<>
	struct node_traits<const_simple_tag_tree_iterator<std::u8string>> : public simple_node_traits<const_simple_tag_tree_iterator<std::u8string>> {};

	template<>
	struct node_traits<simple_tag_tree_iterator<std::u16string>> : public simple_node_traits<simple_tag_tree_iterator<std::u16string>> {};

	template<>
	struct node_traits<const_simple_tag_tree_iterator<std::u16string>> : public simple_node_traits<const_simple_tag_tree_iterator<std::u16string>> {};

	template<>
	struct node_traits<simple_tag_tree_iterator<std::u32string>> : public simple_node_traits<simple_tag_tree_iterator<std::u32string>> {};

	template<>
	struct node_traits<const_simple_tag_tree_iterator<std::u32string>> : public simple_node_traits<const_simple_tag_tree_iterator<std::u32string>> {};

	/*! @brief 文字列から HTML 文書を作成する便利関数
	*
	* @tparam Container HTML  文書を格納する木コンテナ
	* @tparam ForwardIterator 文字列へのイテレータ
	* 
	* 木コンテナは、 wordring::tree と wordring::tag_tree でテストされています。
	* 文書内にエンコーディングの指定を発見した場合、入力文字列を最初から読み直します。
	* 
	* イテレータの逆参照が返す文字は char16_t 、 char32_t 、 char を想定します。
	* char の場合のみ、WHATWG HTML5 規格そのままに、妥当な文字エンコーディングへのデコードを試みます。
	* 
	* @param [in] first HTML       ソース文字列の最初を指すイテレータ
	* @param [in] first HTML       ソース文字列の終端を指すイテレータ
	* @param [in] confidence       エンコーディングの確かさ
	* @param [in] enc              エンコーディング名
	* @param [in] fragments_parser フラグメント・パーサーを構築する場合、 true を設定します
	*/
	template <typename Container, typename ForwardIterator, typename std::enable_if_t<is_simple_tree_v<Container>, std::nullptr_t> = nullptr>
	inline Container make_document(
		ForwardIterator          first,
		ForwardIterator          last,
		encoding_name            enc = encoding_name::UTF_8,
		encoding_confidence_name confidence = encoding_confidence_name::tentative,
		bool                     fragments_parser = false)
	{
		static_assert(std::is_base_of_v<std::forward_iterator_tag, typename std::iterator_traits<ForwardIterator>::iterator_category>);

		auto p = basic_simple_parser<Container, ForwardIterator>(confidence, enc, fragments_parser);
		p.parse(first, last);
		return p.get();
	}
}
