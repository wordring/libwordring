#pragma once

#include <wordring/html/simple_node.hpp>

#include <wordring/encoding/encoding.hpp>

namespace wordring::html
{
	/*! @class simple_adapter simple_adapter.hpp wordring/html/simple_adapter.hpp
	*/
	template <typename String, typename Container>
	class simple_adapter
	{
	public:
		using string_type    = String;
		using container_type = Container;

		using node_type    = simple_node<String>;
		using node_pointer = typename container_type::iterator;

		using element_type                = simple_element<string_type>;                // 1.
		using text_type                   = simple_text<string_type>;                   // 3.
		using processing_instruction_type = simple_processing_instruction<string_type>; // 7.
		using comment_type                = simple_comment<string_type>;                // 8.
		using document_type               = simple_document<string_type>;               // 9.
		using document_type_type          = simple_document_type<string_type>;          // 10.
		using document_fragment_type      = simple_document_fragment<string_type>;      // 11.

		using attribute_type    = typename element_type::attribute_type;
		using attribute_pointer = typename element_type::const_iterator;

	public:
		// ----------------------------------------------------------------------------------------
		// Node
		// ----------------------------------------------------------------------------------------

		/*! @brief NULLで初期化された空のポインタを作成する
		*/
		static node_pointer pointer() { return node_pointer(); }

		/*! @brief 親要素を返す

		根に親は無い。
		この場合、create_pointer() が返すものと同一のポインタを返す。
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

		// ----------------------------------------------------------------------------------------
		// Element
		// ----------------------------------------------------------------------------------------

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

		/*! @brief 二つの要素が同じシグネチャを持つか調べる

		push_active_formatting_element_list() から呼び出される。
		*/
		static bool equals(node_pointer lhs, node_pointer rhs)
		{
			return *lhs == *rhs;
		}

		/*! @brief script 要素の "non-blocking" flag を設定する

		パーサーは呼び出すが、simple node側は実装していないので、何もしない。
		*/
		static void set_non_blocking_flag(node_pointer it, bool b) {}

		/*! @brief script 要素の "already started" flag を設定する

		パーサーは呼び出すが、simple node側は実装していないので、何もしない。
		*/
		static void set_already_started_flag(node_pointer it, bool b) {}

		// ----------------------------------------------------------------------------------------
		// Text
		// ----------------------------------------------------------------------------------------

		static bool is_text(node_pointer it) { return std::holds_alternative<text_type>(*it); }

		static text_type create_text(char32_t cp)
		{
			text_type text;
			to_string(cp, std::back_inserter(text));
			return text;
		}

		static void append_text(node_pointer it, char32_t cp)
		{
			to_string(cp, std::back_inserter(wordring::html::data(*it)));
		}

		// ----------------------------------------------------------------------------------------
		// Comment
		// ----------------------------------------------------------------------------------------

		static comment_type create_comment(string_type data)
		{
			return comment_type(data);
		}

		// ----------------------------------------------------------------------------------------
		// Document
		// ----------------------------------------------------------------------------------------

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

		static document_type_type create_document_type(
			string_type const& name, string_type const& public_id, string_type const& system_id)
		{
			return document_type_type(name, public_id, system_id);
		}

	};
}
