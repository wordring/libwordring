#pragma once

#include <wordring/html/simple_node.hpp>
#include <wordring/html/simple_parser.hpp>

#include <wordring/html/html_defs.hpp>

#include <wordring/encoding/encoding_defs.hpp>
#include <wordring/whatwg/html/parsing/serializing.hpp>

#include <wordring/tag_tree/tag_tree.hpp>
#include <wordring/compatibility.hpp>

namespace wordring::html
{
	using wordring::whatwg::html::to_string;

	using u8simple_node  = simple_node<std::u8string>;
	using u16simple_node = simple_node<std::u16string>;
	using u32simple_node = simple_node<std::u32string>;

	using u8simple_tree  = tag_tree<u8simple_node>;
	using u16simple_tree = tag_tree<u16simple_node>;
	using u32simple_tree = tag_tree<u32simple_node>;

	template <typename T> struct is_simple_tree : std::false_type {};

	template <> struct is_simple_tree<u8simple_tree> : public std::true_type {};
	template <> struct is_simple_tree<u16simple_tree> : public std::true_type {};
	template <> struct is_simple_tree<u32simple_tree> : public std::true_type {};

	template <typename T> constexpr bool is_simple_tree_v = is_simple_tree<T>::value;

	template <typename String>
	using simple_tag_tree_iterator = typename wordring::detail::tag_tree_iterator<simple_node<String>>;

	template <typename String>
	using const_simple_tag_tree_iterator = typename wordring::detail::tag_tree_iterator<simple_node<String> const>;

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

	//template <typename String>
	//using const_simple_node_tree_iterator = typename wordring::tree<simple_node<String>>::const_iterator;

	template <typename Container, typename ForwardIterator, typename std::enable_if_t<is_simple_tree_v<Container>, std::nullptr_t> = nullptr>
	inline Container make_document(
		ForwardIterator first,
		ForwardIterator last,
		encoding_name enc = encoding_name::UTF_8,
		encoding_confidence_name confidence = encoding_confidence_name::tentative,
		bool fragments_parser = false)
	{
		auto p = basic_simple_parser<Container, ForwardIterator>(confidence, enc, fragments_parser);
		p.parse(first, last);
		return p.get();
	}
}
