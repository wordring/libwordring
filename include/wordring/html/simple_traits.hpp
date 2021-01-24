#pragma once

#include <wordring/html/simple_node.hpp>

#include <wordring/encoding/encoding.hpp>

#include <wordring/compatibility.hpp>
#include <wordring/tree/tree.hpp>

#include <iterator>
#include <string>
#include <variant>

namespace wordring::html
{
	/*! @class simple_node_traits simple_traits.hpp wordring/html/simple_traits.hpp

	@brief node_traits のテンプレート特殊化

	@sa node_traits
	*/
	template <typename NodeIterator>
	struct simple_node_traits
	{
		using node_pointer = NodeIterator;
		using node_type    = typename node_pointer::value_type;

		using element_type                = std::variant_alternative_t<1, node_type>;  // 1.
		using text_type                   = std::variant_alternative_t<3, node_type>;  // 3.
		using processing_instruction_type = std::variant_alternative_t<7, node_type>;  // 7.
		using comment_type                = std::variant_alternative_t<8, node_type>;  // 8.
		using document_type               = std::variant_alternative_t<9, node_type>;  // 9.
		using document_type_type          = std::variant_alternative_t<10, node_type>; // 10.
		using document_fragment_type      = std::variant_alternative_t<11, node_type>; // 11.

		using attribute_type    = typename element_type::attribute_type;
		using attribute_pointer = typename element_type::const_iterator;

		using string_type = typename element_type::string_type;

		// ----------------------------------------------------------------------------------------
		// Node
		// ----------------------------------------------------------------------------------------

		/*! @brief NULLで初期化された空のポインタを作成する
		*/
		static node_pointer pointer() { return node_pointer(); }

		/*! @brief 親要素を返す

		根に親は無い。
		この場合、pointer() が返すものと同一のポインタを返す。
		*/
		static node_pointer parent(node_pointer it) { return it.parent(); }

		/*! @brief 最初の子を返す
		*/
		static node_pointer begin(node_pointer it) { return it.begin(); }

		/*! @brief 最後の子の次を返す
		*/
		static node_pointer end(node_pointer it) { return it.end(); }

		/*! @brief 前の兄弟を返す
		*/
		static node_pointer prev(node_pointer it) { return --it; }

		/*! @brief 次の兄弟を返す
		*/
		static node_pointer next(node_pointer it) { return ++it; }

		static auto data(node_pointer it)
		{
			return wordring::html::data(*it);
		}

		static auto target(node_pointer it)
		{
			return wordring::html::target(*it);
		}

		static auto name(node_pointer it)
		{
			return wordring::html::name(*it);
		}

		// ----------------------------------------------------------------------------------------
		// Element
		// ----------------------------------------------------------------------------------------

		static bool is_element(node_pointer it) { return std::holds_alternative<element_type>(*it); }

		/*! @brief 要素へオーナー文書を設定する

		パーサーは呼び出すが、simple node側は実装していないので、何もしない。
		*/
		static void set_document(node_pointer it, node_pointer doc) {}

		/*! @brief 要素の名前空間を返す
		*/
		static ns_name get_namespace_id(node_pointer it)
		{
			return wordring::html::get_namespace_id(*it);
		}

		static string_type get_namespace(node_pointer it)
		{
			return wordring::html::get_namespace(*it);
		}

		/*! @brief 要素のローカル名を返す
		*/
		static tag_name get_local_name_id(node_pointer it)
		{
			return wordring::html::get_local_name_id(*it);
		}

		static string_type get_local_name(node_pointer it)
		{
			return wordring::html::get_local_name(*it);
		}

		static string_type get_qualified_name(node_pointer it)
		{
			return wordring::html::get_qualified_name(*it);
		}

		static bool is_html_element_of(node_pointer it, tag_name tag)
		{
			return get_namespace_id(it) == ns_name::HTML && get_local_name_id(it) == tag;
		}

		/*! @brief 二つの要素が同じシグネチャを持つか調べる

		push_active_formatting_element_list() から呼び出される。
		*/
		static bool equals(node_pointer lhs, node_pointer rhs)
		{
			return *lhs == *rhs;
		}

		/*! @brief 要素ノードから最初の属性を指すイテレータを取得する
		*/
		static auto abegin(node_pointer it) { return wordring::html::begin(*it); }

		static auto aend(node_pointer it) { return wordring::html::end(*it); }

		/*! @brief 属性を検索する
		*/
		/*
		static auto find(node_pointer it, attribute_name name)
		{
			return wordring::html::find(*it, name);
		}
		*/

		/*! @brief script 要素の "non-blocking" flag を設定する

		パーサーは呼び出すが、simple node側は実装していないので、何もしない。
		*/
		static void set_non_blocking_flag(node_pointer it, bool b) {}

		/*! @brief script 要素の "already started" flag を設定する

		パーサーは呼び出すが、simple node側は実装していないので、何もしない。
		*/
		static void set_already_started_flag(node_pointer it, bool b) {}

		// ----------------------------------------------------------------------------------------
		// Attr
		// ----------------------------------------------------------------------------------------

		static ns_name get_namespace_id(attribute_type const& attr)
		{
			return wordring::html::get_namespace_id(attr);
		}

		static string_type get_namespace(attribute_type const& attr)
		{
			return wordring::html::get_namespace(attr);
		}

		static attribute_name get_local_name_id(attribute_type const& attr)
		{
			return wordring::html::get_local_name_id(attr);
		}

		static string_type get_local_name(attribute_type const& attr)
		{
			return wordring::html::get_local_name(attr);
		}

		static string_type get_qualified_name(attribute_type const& attr)
		{
			return attr.qualified_name();
		}

		static auto value(attribute_type const& attr) { return wordring::html::value(attr); }

		// ----------------------------------------------------------------------------------------
		// Text
		// ----------------------------------------------------------------------------------------

		static bool is_text(node_pointer it) { return std::holds_alternative<text_type>(*it); }

		static text_type create_text(char32_t cp)
		{
			text_type text;
			wordring::to_string(cp, std::back_inserter(text));
			return text;
		}

		static void append_text(node_pointer it, char32_t cp)
		{
			wordring::to_string(cp, std::back_inserter(wordring::html::data(*it)));
		}
		
		// ----------------------------------------------------------------------------------------
		// ProcessingInstruction
		// ----------------------------------------------------------------------------------------

		static bool is_processing_instruction(node_pointer it) { return std::holds_alternative<processing_instruction_type>(*it); }

		// ----------------------------------------------------------------------------------------
		// Comment
		// ----------------------------------------------------------------------------------------

		static bool is_comment(node_pointer it) { return std::holds_alternative<comment_type>(*it); }

		static comment_type create_comment(string_type data)
		{
			return comment_type(data);
		}

		// ----------------------------------------------------------------------------------------
		// Document
		// ----------------------------------------------------------------------------------------

		static bool is_document(node_pointer it) { return std::holds_alternative<document_type>(*it); }

		/*! @brief 文書ノードから文書形式を取得する
		*/
		static document_type_name get_document_type(node_pointer it)
		{
			return wordring::html::get_document_type(*it);
		}

		/*! @brief 文書ノードに文書形式を設定する

		@param [in] node 文書ノード
		@param [in] type 文書形式（ html あるいは xml ）
		*/
		static void set_document_type(node_pointer it, document_type_name type)
		{
			wordring::html::set_document_type(*it, type);
		}

		/*! @brief 文書ノードから文書形式を取得する
		*/
		static document_mode_name get_document_mode(node_pointer it)
		{
			return wordring::html::get_document_mode(*it);
		}

		/*! @brief 文書ノードに文書モードを設定する

		@param [in] node 文書ノード
		@param [in] mode 文書モード（ no_quirks、quirks、あるいは limited_quirks ）
		*/
		static void set_document_mode(node_pointer it, document_mode_name mode)
		{
			wordring::html::set_document_mode(*it, mode);
		}

		/*! @brief 文書が IFRAME ソース文書か調べる

		@param [in] it Document ノードを指すイテレータ

		@sa https://triple-underscore.github.io/HTML-embed-ja.html#an-iframe-srcdoc-document
		@sa https://html.spec.whatwg.org/multipage/iframe-embed-object.html#an-iframe-srcdoc-document
		*/
		static bool is_iframe_srcdoc_document(node_pointer it) { return false; }

		/*! 文書の準備度を設定する

		@sa https://html.spec.whatwg.org/multipage/dom.html#current-document-readiness
		@sa https://triple-underscore.github.io/HTML-dom-ja.html#current-document-readiness
		*/
		static void set_document_ready_state(string_type const& readiness) {}

		// ----------------------------------------------------------------------------------------
		// DocumentType
		// ----------------------------------------------------------------------------------------

		static bool is_document_type(node_pointer it) { return std::holds_alternative<document_type_type>(*it); }

		static document_type_type create_document_type(
			string_type const& name, string_type const& public_id, string_type const& system_id)
		{
			return document_type_type(name, public_id, system_id);
		}

		// ----------------------------------------------------------------------------------------
		// CSS Support
		// ----------------------------------------------------------------------------------------

		static bool is_root(node_pointer it)
		{
			return is_element(it) && is_html_element_of(it, tag_name::Html);
		}
	};

	template <typename String>
	using simple_node_tree_iterator = typename wordring::tree<simple_node<String>>::iterator;

	template<>
	struct node_traits<simple_node_tree_iterator<std::u8string>> : public simple_node_traits<simple_node_tree_iterator<std::u8string>> {};

	template<>
	struct node_traits<simple_node_tree_iterator<std::u16string>> : public simple_node_traits<simple_node_tree_iterator<std::u16string>> {};

	template<>
	struct node_traits<simple_node_tree_iterator<std::u32string>> : public simple_node_traits<simple_node_tree_iterator<std::u32string>> {};

	template <typename String>
	using const_simple_node_tree_iterator = typename wordring::tree<simple_node<String>>::const_iterator;

	template<>
	struct node_traits<const_simple_node_tree_iterator<std::u8string>> : public simple_node_traits<const_simple_node_tree_iterator<std::u8string>> {};

	template<>
	struct node_traits<const_simple_node_tree_iterator<std::u16string>> : public simple_node_traits<const_simple_node_tree_iterator<std::u16string>> {};

	template<>
	struct node_traits<const_simple_node_tree_iterator<std::u32string>> : public simple_node_traits<const_simple_node_tree_iterator<std::u32string>> {};


}
