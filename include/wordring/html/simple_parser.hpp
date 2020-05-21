#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/html/html_defs.hpp>
#include <wordring/html/simple_node.hpp>
#include <wordring/html/simple_policy.hpp>

#include <wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp>

#include <wordring/encoding/encoding.hpp>

#include <cassert>
#include <iterator>
#include <variant>

namespace wordring::html
{
	/*! @class simple_parser_base simple_parser.hpp wordring/html/simple_parser.hpp
	
	@brief simple_node 用の HTML5 パーサー

	@tparam T                 CRTP に基づく派生クラス
	@tparam String            文字列型
	@tparam Container         Tree コンテナ
	@tparam IsFragmentsParser HTML フラグメント・パーサーの場合 true


	*/
	template <typename T, typename String, typename Container, bool IsFragmentsParser = false>
	class simple_parser_base : public wordring::whatwg::html::parsing::tree_construction_dispatcher<T, simple_policy<String, Container, IsFragmentsParser>>
	{
	public:
		using policy = simple_policy<String, Container, IsFragmentsParser>;

		using base_type = wordring::whatwg::html::parsing::tree_construction_dispatcher<T, policy>;
		using this_type = T;

		using string_type  = typename policy::string_type;
		using container    = typename policy::container_type;
		using node_pointer = typename container::iterator;
		using node_type    = typename policy::node_type;

		using document_type               = typename policy::document_type;
		using document_type_type          = typename policy::document_type_type;
		using document_fragment_type      = typename policy::document_fragment_type;
		using element_type                = typename policy::element_type;
		using text_type                   = typename policy::text_type;
		using processing_instruction_type = typename policy::processing_instruction_type;
		using comment_type                = typename policy::comment_type;

		using attribute_type    = typename policy::attribute_type;
		using attribute_pointer = typename policy::attribute_pointer;

		using namespace_uri_type = basic_html_atom<string_type, ns_name>;
		using lacal_name_type    = basic_html_atom<string_type, tag_name>;

		using base_type::m_stack;

		using base_type::report_error;
		using base_type::eof;

		using typename base_type::mode_name;

	public:
		simple_parser_base()
			//: m_script_nesting_level(0)
			//, m_parser_pause_flag(false)
		{
			m_document  = m_c.insert(m_c.end(), document_type());
			m_temporary = m_c.insert(m_c.end(), node_type());
		}

		// ----------------------------------------------------------------------------------------
		// 文書
		// ----------------------------------------------------------------------------------------
		
		/*! @brief 文書ノードを返す

		パーサ構築時に文書ノードが挿入される。
		*/
		node_pointer document() { return m_document; }

		/*! @brief 文書が IFRAME ソース文書か調べる

		https://html.spec.whatwg.org/multipage/iframe-embed-object.html#an-iframe-srcdoc-document
		*/
		bool is_iframe_srcdoc_document() const { return false; }

		void set_document_type(document_type_name type)
		{
			std::get_if<document_type>(std::addressof(*document()))->document_type(type);
		}

		document_mode_name get_document_mode() const
		{
			return std::get_if<document_type>(std::addressof(*m_document))->document_mode();
		}

		void set_document_mode(document_mode_name mode)
		{
			std::get_if<document_type>(std::addressof(*document()))->document_mode(mode);
		}

		/*! 文書の準備度を設定する
		
		https://html.spec.whatwg.org/multipage/dom.html#current-document-readiness
		*/
		void set_current_document_readiness(std::u32string const& rediness)
		{
		}
		
		// ----------------------------------------------------------------------------------------
		// 文書型
		// ----------------------------------------------------------------------------------------

		document_type_type create_document_type(std::u32string const& name, std::u32string const& public_id, std::u32string const& system_id)
		{
			document_type_type doctype;

			doctype.name(encoding_cast<string_type>(name));
			doctype.public_id(encoding_cast<string_type>(public_id));
			doctype.system_id(encoding_cast<string_type>(system_id));

			return doctype;
		}

		node_pointer insert_document_type(node_pointer pos, document_type_type&& doctype)
		{
			return m_c.insert(pos, std::move(doctype));
		}

		// ----------------------------------------------------------------------------------------
		// 要素
		// ----------------------------------------------------------------------------------------

		/*! @brief NULLで初期化された空のポインタを作成する
		*/
		node_pointer create_pointer() const { return node_pointer(); }

		/*! @brief 親要素を返す

		根に親は無い。
		この場合、create_pointer() が返すものと同一のポインタを返す。
		*/
		node_pointer parent(node_pointer it) { return it.parent(); }

		/*! @brief 最初の子を返す
		*/
		node_pointer begin(node_pointer it) { return it.begin(); }

		/*! @brief 最後の子の次を返す
		*/
		node_pointer end(node_pointer it) { return it.end(); }

		/*! @brief 前の兄弟を返す
		*/
		node_pointer prev(node_pointer it) { return std::prev(it); }

		/*! @brief 次の兄弟を返す
		*/
		node_pointer next(node_pointer it) { return std::next(it); }

		/* @brief 要素を作成する

		- https://dom.spec.whatwg.org/#concept-create-element に対応する要素作成関数
		- https://triple-underscore.github.io/DOM4-ja.html#concept-create-element
		*/
		node_pointer create_element(node_pointer doc, std::u32string name, namespace_uri_type ns, std::u32string prefix)
		{
			element_type el(ns, encoding_cast<string_type>(prefix), encoding_cast<string_type>(name));
			return m_c.insert(m_temporary.end(), std::move(el));
		}

		node_pointer create_element(node_pointer doc, tag_name name, namespace_uri_type ns, std::u32string prefix)
		{
			element_type el(ns, encoding_cast<string_type>(prefix), name);

			return m_c.insert(m_temporary.end(), std::move(el));
		}

		node_pointer create_element(node_pointer doc, tag_name name, ns_name ns = ns_name::HTML)
		{
			element_type el(ns, string_type(), name);

			return m_c.insert(m_temporary.end(), std::move(el));
		}

		node_pointer insert_element(node_pointer pos, node_pointer it)
		{
			return m_c.move(pos, it);
		}

		/*! @brief ノードを削除する
		*/
		void erase_element(node_pointer it)
		{
			m_c.erase(it);
		}

		/*! @brief ノードを移動する
		*/
		void move_element(node_pointer pos, node_pointer it)
		{
			m_c.move(pos, it);
		}

		/*! @brief 要素へオーナー文書を設定する

		パーサーは呼び出すが、simple node側は実装していないので、何もしない。
		*/
		void set_document(node_pointer it, node_pointer doc) {}

		/*! @brief script 要素の "non-blocking" flag を設定する

		パーサーは呼び出すが、simple node側は実装していないので、何もしない。
		*/
		void set_non_blocking_flag(node_pointer it, bool b) {}

		/*! @brief script 要素の "already started" flag を設定する

		パーサーは呼び出すが、simple node側は実装していないので、何もしない。
		*/
		void set_already_started_flag(node_pointer it, bool b) {}

		/*! @brief 要素の名前空間を返す
		*/
		ns_name get_namespace_uri_id(node_pointer it) const
		{
			return std::get_if<element_type>(std::addressof(*it))->namespace_uri_id();
		}

		/*! @brief 要素のローカル名を返す
		*/
		tag_name get_local_name_id(node_pointer it) const
		{
			return std::get_if<element_type>(std::addressof(*it))->local_name_id();
		}

		std::u32string get_local_name(node_pointer it) const
		{
			return encoding_cast<std::u32string>(std::get_if<element_type>(std::addressof(*it))->local_name());
		}

		/*! @brief 二つの要素が同じシグネチャを持つか調べる

		push_active_formatting_element_list() から呼び出される。
		*/
		bool equals(node_pointer lhs, node_pointer rhs) const
		{
			return *lhs == *rhs;
		}

		// ----------------------------------------------------------------------------------------
		// 属性
		// ----------------------------------------------------------------------------------------

		attribute_type create_attribute(node_pointer el, ns_name ns, std::u32string const& prefix, std::u32string const& local_name)
		{
			return attribute_type(ns, encoding_cast<string_type>(prefix), encoding_cast<string_type>(local_name));
		}

		/*! @brief 要素へ属性を付加する
		*/
		void append_attribute(node_pointer it, attribute_type&& attr)
		{
			push_back(*it, std::move(attr));
		}

		void append_attribute(node_pointer it, ns_name ns, std::u32string const& prefix, std::u32string const& name, std::u32string const& value)
		{
		}

		bool contains(node_pointer it, ns_name ns, std::u32string const& prefix, std::u32string const& name)
		{
			return wordring::html::find(*it, ns, encoding_cast<string_type>(prefix), encoding_cast<string_type>(name)) != wordring::html::end(*it);
		}

		// ----------------------------------------------------------------------------------------
		// テキスト
		// ----------------------------------------------------------------------------------------

		text_type create_text(char32_t cp)
		{
			text_type text;
			to_string(cp, std::back_inserter(text));
			return text;
		}

		void append_text(node_pointer it, char32_t cp)
		{
			to_string(cp, std::back_inserter(*std::get_if<text_type>(std::addressof(*it))));
		}

		node_pointer insert_text(node_pointer pos, text_type&& text)
		{
			return m_c.insert(pos, std::move(text));
		}

		bool is_text(node_pointer it) const { return std::holds_alternative<text_type>(*it); }

		// ----------------------------------------------------------------------------------------
		// コメント
		// ----------------------------------------------------------------------------------------

		comment_type create_comment(std::u32string data)
		{
			return comment_type(encoding_cast<string_type>(data));
		}

		node_pointer insert_comment(node_pointer pos, comment_type&& comment)
		{
			return m_c.insert(pos, std::move(comment));
		}

		void on_report_error(parsing::error_name e) {}

	protected:

		//std::uint32_t m_script_nesting_level;
		//bool m_parser_pause_flag;

		container    m_c;
		node_pointer m_document;
		node_pointer m_temporary;
	};

	template <typename String, typename Container, bool IsFragmentsParser = false>
	class simple_parser : public simple_parser_base<simple_parser<String, Container, IsFragmentsParser>, String, Container, IsFragmentsParser>
	{
	};
}
