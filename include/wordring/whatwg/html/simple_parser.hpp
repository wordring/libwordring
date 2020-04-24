#pragma once

// https://html.spec.whatwg.org/multipage/parsing.html
// https://triple-underscore.github.io/HTML-parsing-ja.html

#include <wordring/whatwg/html/simple_policy.hpp>

//#include <wordring/whatwg/encoding/encoding.hpp>

#include <wordring/whatwg/html/parsing/token.hpp>
//#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
//#include <wordring/whatwg/html/parsing/input_stream.hpp>
#include <wordring/whatwg/html/parsing/parser_defs.hpp>
//#include <wordring/whatwg/html/parsing/tokenization.hpp>
#include <wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp>

#include <cassert>
#include <iterator>
#include <variant>

namespace wordring::whatwg::html::parsing
{

}

namespace wordring::whatwg::html::simple
{
	namespace parsing = wordring::whatwg::html::parsing;

	template <typename T, typename String, typename Container, bool IsFragmentsParser = false>
	class parser : public parsing::tree_construction_dispatcher<T, tree_construction_policy<String, Container, IsFragmentsParser>>
	{
	public:
		using policy = tree_construction_policy<String, Container, IsFragmentsParser>;

		using base_type = parsing::tree_construction_dispatcher<T, policy>;
		using this_type = T;

		using string_type    = typename policy::string_type;
		using container_type = typename policy::container_type;
		using node_type      = typename policy::node_type;
		using node_pointer   = typename container_type::iterator;

		using document_node_type               = typename policy::document_node_type;
		using document_type_node_type          = typename policy::document_type_node_type;
		using document_fragment_node_type      = typename policy::document_fragment_node_type;
		using element_node_type                = typename policy::element_node_type;
		using text_node_type                   = typename policy::text_node_type;
		using processing_instruction_node_type = typename policy::processing_instruction_node_type;
		using comment_node_type                = typename policy::comment_node_type;

		using attribute_type    = typename policy::attribute_type;
		using attribute_pointer = typename policy::attribute_pointer;

		using namespace_uri_type = basic_html_atom<string_type, ns_name>;
		using lacal_name_type    = basic_html_atom<string_type, tag_name>;

		using base_type::m_open_element_stack;

		using base_type::report_error;
		using base_type::eof;

	public:
		parser()
			: m_script_nesting_level(0)
			, m_parser_pause_flag(false)
		{
			m_c.insert(m_c.end(), document_node_type());
		}

		// ----------------------------------------------------------------------------------------
		// ノード
		// ----------------------------------------------------------------------------------------

		//bool is_document(node_pointer it) const { return std::holds_alternative<document_node_type>(*it); }

		//bool is_document_type(node_pointer it) const { return std::holds_alternative<document_type_node_type>(*it); }

		//bool is_document_fragment(node_pointer it) const { return std::holds_alternative<document_fragment_node_type>(*it); }

		//bool is_element(node_pointer it) const { return std::holds_alternative<element_node_type>(*it); }

		bool is_text(node_pointer it) const { return std::holds_alternative<text_node_type>(*it); }

		//bool is_processing_instruction(node_pointer it) const { return std::holds_alternative<processing_instruction_node_type>(*it); }

		//bool is_comment(node_pointer it) const { return std::holds_alternative<comment_node_type>(*it); }

		//document_node_type* to_document(node_pointer it) { return std::get_if<document_node_type>(std::addressof(*it)); }

		//document_type_node_type const* to_document_type(node_pointer it) const { return std::get_if<document_type_node_type>(std::addressof(*it)); }

		//document_fragment_node_type const* to_document_fragment(node_pointer it) const { return std::get_if<document_fragment_node_type>(std::addressof(*it)); }

		//element_node_type const* to_element(node_pointer it) const { return std::get_if<element_node_type>(std::addressof(*it)); }

		//text_node_type* to_text(node_pointer it) { return std::get_if<text_node_type>(std::addressof(*it)); }

		//processing_instruction_node_type const* to_processing_instruction(node_pointer it) const { return std::get_if<processing_instruction_node_type>(std::addressof(*it)); }

		//comment_node_type const* to_comment(node_pointer it) const { return std::get_if<comment_node_type>(std::addressof(*it)); }

		/*! @brief ノードを挿入する
		*/
		template <typename Node>
		node_pointer insert(node_pointer pos, Node node)
		{
			return m_c.insert(pos, std::forward<Node>(node));
		}

		// ----------------------------------------------------------------------------------------
		// 文書
		// ----------------------------------------------------------------------------------------

		/*! @brief 文書ノードを返す

		パーサ構築時に文書ノードが挿入される。
		*/
		node_pointer document() { return m_c.begin(); }

		/*! @brief 文書が IFRAME ソース文書か調べる

		https://html.spec.whatwg.org/multipage/iframe-embed-object.html#an-iframe-srcdoc-document
		*/
		bool is_iframe_srcdoc_document() const { return false; }

		void set_document_type(document_type_name type)
		{
			std::get_if<document_node_type>(std::addressof(*document()))->document_type(type);
		}

		void set_document_mode(document_mode_name mode)
		{
			std::get_if<document_node_type>(std::addressof(*document()))->document_mode(mode);
		}

		// ----------------------------------------------------------------------------------------
		// 文書型
		// ----------------------------------------------------------------------------------------

		document_type_node_type create_document_type(std::u32string const& name, std::u32string const& public_id, std::u32string const& system_id)
		{
			document_type_node_type doctype;

			doctype.name(encoding_cast<string_type>(name));
			doctype.public_id(encoding_cast<string_type>(public_id));
			doctype.system_id(encoding_cast<string_type>(system_id));

			return doctype;
		}

		// ----------------------------------------------------------------------------------------
		// 要素
		// ----------------------------------------------------------------------------------------

		/*! @brief 親要素を返す
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
		element_node_type create_element(node_pointer doc, std::u32string local_name, namespace_uri_type ns, std::u32string prefix)
		{
			element_node_type el;

			el.namespace_uri(ns);
			el.namespace_prefix(encoding_cast<string_type>(prefix));
			el.local_name(encoding_cast<string_type>(local_name));

			return el;
		}

		element_node_type create_element(node_pointer doc, tag_name local_name_id, namespace_uri_type ns, std::u32string prefix)
		{
			element_node_type el;

			el.namespace_uri(ns);
			el.namespace_prefix(encoding_cast<string_type>(prefix));
			el.local_name_id(local_name_id);

			return el;
		}

		element_node_type create_element(node_pointer doc, tag_name name, ns_name ns = ns_name::HTML)
		{
			element_node_type el;

			el.namespace_uri_id(ns);
			el.local_name_id(name);

			return el;
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
		ns_name namespace_uri_name(node_pointer it) const
		{
			return std::get_if<element_node_type>(std::addressof(*it))->namespace_uri_id();
		}

		/*! @brief 要素のローカル名を返す
		*/
		tag_name local_name_name(node_pointer it) const
		{
			return std::get_if<element_node_type>(std::addressof(*it))->local_name_id();
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

		attribute_pointer abegin(node_pointer it)
		{
		}

		attribute_type create_attribute(element_node_type& el, std::u32string local_name, ns_name ns, std::u32string prefix)
		{
			attribute_type a;

			a.namespace_uri_id(ns);
			a.local_name(encoding_cast<string_type>(local_name));
			a.prefix(encoding_cast<string_type>(prefix));

			return a;
		}

		attribute_type create_attribute(element_node_type& el, attribute_name local_name, ns_name ns, std::u32string prefix)
		{
			attribute_type a;

			a.namespace_uri_id(ns);
			a.local_name_id(local_name);
			a.prefix(encoding_cast<string_type>(prefix));

			return a;
		}

		/*! @brief 要素へ属性を付加する
		*/
		void append_attribute(element_node_type& el, attribute_type&& attr)
		{
			el.push_back(std::move(attr));
		}

		attribute_pointer find_attribute(node_pointer it, attribute_name name)
		{
			return std::get_if<element_node_type>(std::addressof(*it))->find(name);
		}

		//attribute_pointer find_attribute(node_pointer it) const
		//{
		//	//to_element(it)->
		//}

		// ----------------------------------------------------------------------------------------
		// テキスト
		// ----------------------------------------------------------------------------------------
		
		text_node_type create_text(char32_t cp)
		{
			text_node_type text;
			to_string(cp, std::back_inserter(text));
			return text;
		}

		void append_text(node_pointer it, char32_t cp)
		{
			to_string(cp, std::back_inserter(*std::get_if<text_node_type>(std::addressof(*it))));
		}

		// ----------------------------------------------------------------------------------------
		// コメント
		// ----------------------------------------------------------------------------------------

		comment_node_type create_comment(std::u32string data)
		{
			return comment_node_type(encoding_cast<string_type>(data));
		}

		// ノード作成 ----------------------------------------------------------


		document_fragment_node_type create_document_fragment(){}


		processing_instruction_node_type create_processing_instruction() {}



		// 


		/*void insert_comment(node_pointer pos, comment_token& comment)
		{
			m_c.insert(pos, comment_node_type(comment.m_data));
		}*/

		/*! @brief 与えられたノードがHTML名前空間に属するか調べる
		*/
		/*bool in_html_namespace(node_pointer it) const
		{
			if (std::holds_alternative<element_node_type>(*it))
			{
				element_node_type const& elm = std::get<element_node_type>(*it);
				return elm.namespace_uri_atom() == ns_name::HTML;
			}

			return false;
		}*/


		//void change_encoding(wordring::whatwg::encoding::name name) {}

		void on_report_error(parsing::error_name e) {}

	protected:

		//encoding_confidence m_confidence;

		std::uint32_t m_script_nesting_level;
		bool m_parser_pause_flag;

		container_type m_c;
	};
}
