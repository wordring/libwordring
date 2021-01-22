#pragma once

// ------------------------------------------------------------------------------------------------
// 17. Grammar
//
// https://drafts.csswg.org/selectors-4/#grammar
// https://triple-underscore.github.io/selectors4-ja.html#grammar
// ------------------------------------------------------------------------------------------------

#include <wordring/wwwc/selectors/pseudo_class_defs.hpp>
#include <wordring/wwwc/selectors/selectors_defs.hpp>
#include <wordring/wwwc/selectors/tree_structural_pseudo_classes.hpp>

#include <wordring/wwwc/css_syntax/parsing.hpp>

#include <wordring/encoding/encoding.hpp>
#include <wordring/html/html_defs.hpp>

#include <wordring/whatwg/infra/infra.hpp>

#include <algorithm>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace wordring::wwwc::css
{
	// --------------------------------------------------------------------------------------------
	// 17. Grammar
	//
	// https://drafts.csswg.org/selectors-4/#grammar
	// https://triple-underscore.github.io/selectors4-ja.html#grammar
	// --------------------------------------------------------------------------------------------

	/*! @brief セレクタに関する構文規則の基本クラス
	*
	* 構文規則共通のフィールドを切り出しただけで、特別な意味はない。
	*
	* 現在、セレクタ以外の CSS クラスを実装していないため、セレクタのための基本クラスとなっているが、
	* 他の CSS クラスを実装する場合、それらすべての基本クラスに変更する可能性がある。
	* その場合、名前が css_grammar に変わるかもしれない。
	*/
	class selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

		using document_type_name = wordring::html::document_type_name;
		using document_mode_name = wordring::html::document_mode_name;

	public:
		selector_grammar() = default;
		selector_grammar(selector_grammar const&) = default;
		selector_grammar(selector_grammar&&) = default;

		selector_grammar(const_iterator first, const_iterator last);

		selector_grammar& operator=(selector_grammar const&) = default;
		selector_grammar& operator=(selector_grammar&&) = default;

		operator bool() const;

		bool operator !() const;

		const_iterator begin() const;

		const_iterator end() const;

	protected:
		/*! @brief 構文解析時に部分セレクタが消費した入力の先頭
		*/
		const_iterator m_first;

		/*! @brief 構文解析時に部分セレクタが消費した入力の終端
		*/
		const_iterator m_last;

	};

	/*! @brief <combinator>
	*
	* @code
	* 	<combinator> = '>' | '+' | '~' | [ '|' '|' ]
	* @endcode
	*
	* @sa selector_list
	* @sa https://drafts.csswg.org/selectors-4/#typedef-combinator
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-combinator
	*/
	class combinator: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static combinator consume(syntax_primitive_stream in);

	public:
		combinator() = default;
		combinator(combinator const&) = default;
		combinator(const_iterator first, const_iterator last, char32_t c);

		char32_t type() const;

	private:
		char32_t m_c;
	};

	/*! @brief <ns-prefix>
	*
	* @code
	* 	<ns-prefix> = [ <ident-token> | '*' ]? '|'
	* @endcode
	*
	* @sa selector_list
	* @sa https://drafts.csswg.org/selectors-4/#typedef-ns-prefix
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-ns-prefix
	*/
	class ns_prefix: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static ns_prefix consume(syntax_primitive_stream in);

	public:
		ns_prefix() = default;
		ns_prefix(ns_prefix const&) = default;
		ns_prefix(ns_prefix&&) = default;
		ns_prefix(const_iterator first, const_iterator last, std::u32string const& prefix);

		ns_prefix& operator =(ns_prefix const&) = default;
		ns_prefix& operator =(ns_prefix&&) = default;

		/*! @brief 名前空間接頭辞を返す
		*
		* @return 「*」、名前空間接頭辞、空文字列のいずれか
		*
		* 「*」はすべての名前空間とマッチし、空文字列は名前空間に属さない要素とマッチする。
		*/
		std::u32string const& string() const;

	private:
		std::u32string m_ns_prefix;
	};

	/*! @brief <wq-name>
	*
	* @code
	* 	<wq-name> = <ns-prefix>? <ident-token>
	* @endcode
	*
	* @par 実装
	* - [attr|=val] → attr| が <ns_prefix> と誤認されるため、後続が「IdentToken あるいは *」か先読みする必要が有る。
	*
	* @sa selector_list
	* @sa https://drafts.csswg.org/selectors-4/#typedef-wq-name
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-wq-name
	*/
	class wq_name: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static wq_name consume(syntax_primitive_stream in);

	public:
		wq_name() = default;
		wq_name(const_iterator first, const_iterator last, std::optional<ns_prefix> ns, std::u32string const& name);

		/*! @brief 名前空間接頭辞を返す
		*/
		std::optional<ns_prefix> const& prefix() const;

		/*! @brief 名前を返す
		*/
		std::u32string const& name() const;

	private:
		std::optional<ns_prefix> m_ns_prefix;
		std::u32string m_name;
	};
	
	/*! @brief <id-selector>
	*
	* @code
	* 	<id-selector> = <hash-token>
	* @endcode
	*
	* @par 例
	* @code
	* 	#my-id
	* @endcode
	*
	* @sa selector_list
	* @sa https://drafts.csswg.org/selectors-4/#typedef-id-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-id-selector
	*/
	class id_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

		using document_type_name = wordring::html::document_type_name;
		using document_mode_name = wordring::html::document_mode_name;

	public:
		static id_selector consume(syntax_primitive_stream in);

	public:
		id_selector() = default;
		
		/*! @brief
		* 
		* @param [in] first このセレクタが消費した範囲の先頭
		* @param [in] last  このセレクタが消費した範囲の終端
		* @param [in] value 属性値
		*/
		id_selector(const_iterator first, const_iterator last, std::u32string const& value);

		/*! @brief 属性値を返す
		*/
		std::u32string const& value() const;

		/*! @brief ノードにマッチするか検査する
		* 
		* @param [in] np HTML ノードを指すイテレータあるいはポインタ
		* 
		* - HTML では属性名「 id 」をASCII大小無視で照合する。
		* - XML では属性名「 id 」は小文字。
		* - quirks モードでは ID 値を大小無視で照合する。
		* - それ以外のモードでは ID 値を大小区別して照合する。
		*
		* @sa https://triple-underscore.github.io/selectors4-ja.html#id-selector
		* @sa https://triple-underscore.github.io/selectors4-ja.html#case-sensitive
		*/
		template <typename NodePointer>
		bool match(NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			using traits = typename wordring::html::node_traits<NodePointer>;
			
			assert(!(ctx.m_type_name == document_type_name::Xml
				&& (ctx.m_mode_name == document_mode_name::LimitedQuirks || ctx.m_mode_name == document_mode_name::Quirks)));

			if (traits::is_element(np))
			{
				auto it1 = traits::abegin(np);
				auto it2 = traits::aend(np);
				while (it1 != it2)
				{
					std::u32string name = wordring::encoding_cast<std::u32string>(traits::get_qualified_name(*it1));
					if (ctx.m_type_name == document_type_name::Html)
					{
						std::u32string tmp;
						wordring::whatwg::to_ascii_lowercase(name.begin(), name.end(), std::back_inserter(tmp));
						name = std::move(tmp);
					}

					if (name == U"id")
					{
						std::u32string val = wordring::encoding_cast<std::u32string>(traits::value(*it1));
						if (ctx.m_mode_name == document_mode_name::Quirks)
						{
							std::u32string tmp;
							wordring::whatwg::to_ascii_lowercase(val.begin(), val.end(), std::back_inserter(tmp));
							val = std::move(tmp);
						}
						if (val == m_value) return true;
					}
					++it1;
				}
			}
			return false;
		}

	private:
		std::u32string m_value;
	};

	/*! @brief <class-selector>
	*
	* @code
	* 	<class-selector> = '.' <ident-token>
	* @endcode
	*
	* @par 例
	* @code
	* 	.my-class
	* @endcode
	*
	* @sa selector_list
	* @sa https://drafts.csswg.org/selectors-4/#typedef-class-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-class-selector
	*/
	class class_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

		using document_type_name = wordring::html::document_type_name;
		using document_mode_name = wordring::html::document_mode_name;

	public:
		static class_selector consume(syntax_primitive_stream in);

	public:
		class_selector() = default;
		class_selector(const_iterator first, const_iterator last, std::u32string const& name);

		/*! @brief クラス名を返す
		*/
		std::u32string const& name() const;

		/*! @brief ノードにマッチするか検査する
		* 
		* @sa id_selector::match()
		*
		* <hr>
		* @internal
		*
		* - HTML では属性名「 class 」をASCII大小無視で照合する。
		* - XML では属性名を大小区別して照合する。
		* - quirks モードでは class 名を大小無視で照合する。
		* - それ以外のモードでは class 名を大小区別して照合する。
		*
		* @sa https://triple-underscore.github.io/selectors4-ja.html#class-selector
		* @sa https://triple-underscore.github.io/selectors4-ja.html#case-sensitive
		*
		* - セレクタの空白文字は SPACE TAB LF CR FF。
		* - これは HTML5 と同じなので分割関数を借用する。
		*
		* @sa https://triple-underscore.github.io/selectors4-ja.html#whitespace
		*/
		template <typename NodePointer>
		bool match(NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			using traits = typename wordring::html::node_traits<NodePointer>;

			assert(!(ctx.m_type_name == document_type_name::Xml
				&& (ctx.m_mode_name == document_mode_name::LimitedQuirks || ctx.m_mode_name == document_mode_name::Quirks)));

			if (traits::is_element(np))
			{
				auto it1 = traits::abegin(np);
				auto it2 = traits::aend(np);
				while (it1 != it2)
				{
					std::u32string name = wordring::encoding_cast<std::u32string>(traits::get_qualified_name(*it1));
					if (ctx.m_type_name == document_type_name::Html)
					{
						std::u32string tmp;
						wordring::whatwg::to_ascii_lowercase(name.begin(), name.end(), std::back_inserter(tmp));
						name = std::move(tmp);
					}
					if (name != U"class")
					{
						++it1;
						continue;
					}

					std::u32string val = wordring::encoding_cast<std::u32string>(traits::value(*it1));
					if (ctx.m_mode_name == document_mode_name::Quirks)
					{
						std::u32string tmp;
						wordring::whatwg::to_ascii_lowercase(val.begin(), val.end(), std::back_inserter(tmp));
						val = std::move(tmp);
					}

					std::vector<std::u32string> v;
					wordring::whatwg::split_on_ascii_whitespace(val.begin(), val.end(), std::back_inserter(v));
					for (std::u32string const& s : v)
					{
						bool matched = (ctx.m_mode_name == document_mode_name::Quirks)
							? wordring::whatwg::is_ascii_case_insensitive_match(s.begin(), s.end(), m_name.begin(), m_name.end())
							: s == val;

						if (matched) return true;
					}
					++it1;
				}
			}
			return false;
		}

	private:
		std::u32string m_name;
	};

	/*! @brief <attr-matcher>
	* 
	* @code
	* 	<attr-matcher> = [ '~' | '|' | '^' | '$' | '*' ]? '='
	* @endcode
	*
	* @sa selector_list
	*
	* @sa https://drafts.csswg.org/selectors-4/#typedef-attr-matcher
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-attr-matcher
	*/
	class attr_matcher: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static attr_matcher consume(syntax_primitive_stream in);

	public:
		attr_matcher() = default;
		attr_matcher(const_iterator first, const_iterator last, char32_t c);

		char32_t prefix() const;

	private:
		char32_t m_prefix = U'\0';
	};

	/*! @brief <attr-modifier>
	* 
	* @code
	* 	<attr-modifier> = i | s
	* @endcode
	* 
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-attr-modifier
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-attr-modifier
	* 
	* @internal
	* <hr>
	* 
	* - 識別子「i」「s」自身は大文字小文字を区別しない。\n
	* @sa https://triple-underscore.github.io/selectors4-ja.html#attribute-case
	*/
	class attr_modifier: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static attr_modifier consume(syntax_primitive_stream in);

	public:
		attr_modifier() = default;
		attr_modifier(const_iterator first, const_iterator last, char32_t c);

		char32_t value() const;

	private:
		char32_t m_c;
	};

	/*! @brief <pseudo-class-selector>
	* 
	* @code
	* 	<pseudo-class-selector> = ':' <ident-token> | ':' <function-token> <any-value> ')'
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-pseudo-class-selector
	* @sa https://drafts.csswg.org/selectors-4/#typedef-pseudo-class-selector
	*/
	class pseudo_class_selector: public selector_grammar
	{
	public:
		using value_type = std::variant<std::monostate, ident_token, function>;
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:

		/*! @brief 疑似クラスを消費する
		* 
		* 現在、対応している疑似クラスは :root のみ。
		*
		* @internal
		* <hr>
		* 
		* - 他の規格から参照される場合に備えて、サポートしない疑似クラス名であっても、即座にエラーとはしない。
		* - pseudo_element_selector::consume() からも利用される。
		*/
		static pseudo_class_selector consume(syntax_primitive_stream in);

	public:
		pseudo_class_selector();
		pseudo_class_selector(const_iterator first, const_iterator last);
		pseudo_class_selector(const_iterator first, const_iterator last, ident_token const& id, pseudo_class_id_name name);
		pseudo_class_selector(const_iterator first, const_iterator last, function const& fn, pseudo_class_fn_name name);

		syntax_primitive_name type() const;

		template <typename T>
		T const& get() const { return std::get<T>(m_value); }

		/*! @brief ノードにマッチするか検査する
		*
		* @sa id_selector::match()
		*
		* @internal
		* <hr>
		* 
		* - 識別子形式・関数形式に関わらず、疑似クラス名は ASCII 大小無視で照合する。
		* @sa https://triple-underscore.github.io/selectors4-ja.html#pseudo-class
		*/
		template <typename NodePointer>
		bool match(NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			assert(!(ctx.m_type_name == document_type_name::Xml
				&& (ctx.m_mode_name == document_mode_name::LimitedQuirks || ctx.m_mode_name == document_mode_name::Quirks)));

			// 識別子だけの疑似クラスはこちら
			switch (m_pseudo_class_id_name)
			{
			case pseudo_class_id_name::Root: // :root
				return root_pseudo_class::match(np, ctx);
			default:
				break;
			}

			// 関数型の疑似クラスはこちら
			//switch (m_pseudo_class_fn_name)
			//{
			//default:
			//	break;
			//}

			return false;
		}

	protected:
		value_type m_value; //!< 疑似クラスを示す関数、あるいは識別子
		pseudo_class_id_name m_pseudo_class_id_name; //!< 識別子を示す列挙値、あるいは 0
		pseudo_class_fn_name m_pseudo_class_fn_name; //!< 関数を示す列挙値、あるいは 0
	};

	/*! @brief <pseudo-element-selector>
	* 
	* @code
	* 	<pseudo-element-selector> = ':' <pseudo-class-selector>
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-pseudo-element-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-pseudo-element-selector
	*/
	class pseudo_element_selector : public pseudo_class_selector
	{
	public:
		using value_type = pseudo_class_selector::value_type;
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		/*! @brief 疑似要素を消費する
		*
		* 現在、対応している疑似要素はない。
		*
		* @internal
		* <hr>
		*
		* - 他の規格から参照される場合に備えて、サポートしない疑似要素名であっても、即座にエラーとはしない。
		* - 疑似要素に疑似クラスが続く場合、ユーザー・アクション疑似クラス群以外は、エラーとなる。\n
		*   https://triple-underscore.github.io/selectors4-ja.html#pseudo-element-states \n
		*   疑似要素に疑似クラスが続くのは、 <compound-selector> 構文規則のみなので、そこでエラーを補足すると良い。
		*/
		static pseudo_element_selector consume(syntax_primitive_stream in);

	public:
		explicit pseudo_element_selector(pseudo_class_selector&& s);
		pseudo_element_selector(const_iterator first, const_iterator last);
		pseudo_element_selector(const_iterator first, const_iterator last, ident_token const& id, pseudo_element_id_name name);

		/*! @brief ノードにマッチするか検査する
		*
		* @sa id_selector::match()
		*
		* @internal
		* <hr>
		* 
		* - 識別子形式・関数形式に関わらず、疑似クラス名は ASCII 大小無視で照合する。
		* @sa https://triple-underscore.github.io/selectors4-ja.html#pseudo-class
		*/
		template <typename NodePointer>
		bool match(NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			assert(!(ctx.m_type_name == document_type_name::Xml
				&& (ctx.m_mode_name == document_mode_name::LimitedQuirks || ctx.m_mode_name == document_mode_name::Quirks)));

			return false;
		}

	private:
		pseudo_element_id_name m_pseudo_element_id_name;
	};

	/*! @brief <attribute-selector>
	*
	* @code
	* 	<attribute-selector> = '[' <wq-name> ']' |
	* 		'[' <wq-name> <attr-matcher> [ <string-token> | <ident-token> ] <attr-modifier>? ']'
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-attribute-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-attribute-selector
	*/
	class attribute_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static attribute_selector consume(syntax_primitive_stream in);

	public:
		attribute_selector() = default;
		attribute_selector(const_iterator first, const_iterator last, wq_name name, char32_t matcher, std::u32string const& value, char32_t modifier);

		std::optional<ns_prefix> const& prefix() const;
		std::u32string const& name() const;
		char32_t matcher() const;
		std::u32string const& value() const;
		char32_t modifier() const;

		/*! @brief ノードにマッチするか検査する
		* 
		* 名前空間をサポートしないcontextで、名前空間を指定したセレクタは無効なセレクタとなり何にもマッチしない。
		*
		* @sa id_selector::match()
		*
		* @internal
		* <hr>
		*
		* 同一要素内で属性名の重複は、HTML5はトークン化ステージで後ろの属性が捨てられ、XMLはエラーとなる。
		* したがって、属性名の重複は考慮しなくて良い。
		*
		* HTML 下では、要素名属性名はASCII大小無視で照合される。
		* XML 下では、要素名属性名はASCII大小区別される。
		* HTML5 パーサーはトークン化ステージですでに属性名を小文字化して格納する。
		*
		* 属性値はASCII大小区別される。
		* [課題] 属性名「 class 」のASCII大小をどう扱うか？
		*
		* @par 名前空間
		*
		* CSS 名前空間接頭辞は、ASCII大小区別する。
		*
		* @sa https://triple-underscore.github.io/css-namespaces-ja.html#namespace-prefix
		*
		* 属性には \@namespace で指定されたデフォルト（無名）の名前空間が適用されない。
		*
		* @sa https://triple-underscore.github.io/selectors4-ja.html#attrnmsp
		* @sa https://developer.mozilla.org/ja/docs/Web/CSS/\@namespace
		*
		* HTML5 は、 querySelector() に名前空間をサポートする予定はない。
		* @sa https://triple-underscore.github.io/DOM4-ja.html#selectors
		*
		* 宣言されていない名前空間接頭辞を伴う属性セレクタは無効なセレクタである。
		* @sa https://triple-underscore.github.io/selectors4-ja.html#attrnmsp
		*
		* 無効なセレクタは何にもマッチしない。
		* @sa https://triple-underscore.github.io/selectors4-ja.html#invalid-selector
		*
		* [*|attr] はすべての接頭辞、あるいは接頭辞を持たない属性にマッチする。
		* [|attr] あるいは [attr] は、名前空間に属さない属性のみにマッチする。
		* @sa https://triple-underscore.github.io/selectors4-ja.html#attrnmsp
		*
		* ノードの名前空間 URI はノードから得られる。
		* CSS 側の名前空間 URI は ctx から得る。
		*
		* <hr>
		* スタイルシートをサポートしていないので、名前空間をテストできていない。
		*/
		template <typename NodePointer>
		bool match(NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			using traits = typename wordring::html::node_traits<NodePointer>;
			using wordring::whatwg::is_ascii_case_insensitive_match;
			using wordring::whatwg::split_on_ascii_whitespace;
			using wordring::encoding_cast;
			using wordring::whatwg::to_ascii_lowercase;

			assert(!(ctx.m_type_name == document_type_name::Xml
				&& (ctx.m_mode_name == document_mode_name::LimitedQuirks || ctx.m_mode_name == document_mode_name::Quirks)));

			// 要素以外のノードに属性は無い
			if (!traits::is_element(np)) return false;

			// 属性名照合
			auto it1 = traits::abegin(np);
			auto it2 = traits::aend(np);
			while (it1 != it2)
			{
				// 名前空間接頭辞
				if (m_name.prefix())
				{
					if (ctx.m_namespace_enabled)
					{
						std::u32string const& prefix1 = m_name.prefix()->string();
						std::u32string uri2 = encoding_cast<std::u32string>(traits::get_namespace(*it1));

						if (prefix1 == U"*"); // 接頭辞を持たない属性を含めて全てにマッチ
						else if (prefix1 == U"" && !uri2.empty())
						{
							++it1;
							continue;
						}
						else
						{
							std::u32string const* uri1 = ctx.get_namespace(prefix1);
							if (uri1 == nullptr || *uri1 != uri2)
							{
								++it1;
								continue;
							}
						}
					}
					else
					{
						// 名前空間をサポートしないにもかかわらず、セレクタで名前空間が指定された。
						// TOTO: ワイルドカード、空の接頭辞も無効なセレクタとして何にもマッチしないことにしているが、
						// どうすべきか？
						++it1;
						continue;
					}
				}
				else
				{
					if (!traits::get_namespace(*it1).empty())
					{
						++it1;
						continue;
					}
				}

				// 属性名
				std::u32string const& n1 = m_name.name();
				std::u32string n2 = encoding_cast<std::u32string>(traits::get_local_name(*it1));
				// XML はASCII大小区別、HTMLは大小無視
				if (ctx.m_type_name == document_type_name::Xml)
				{
					if (n1 == n2) break;
				}
				else
				{
					if (is_ascii_case_insensitive_match(n1.begin(), n1.end(), n2.begin(), n2.end())) break;
				}
				
				++it1;
			}
			if (it1 == it2) return false;
		
			// 属性値照合
			switch (m_matcher)
			{
			case U'\0':// [attr]
				assert(m_value.empty());
				return true;
			case U'=': // [attr=val]
			{
				std::u32string s = encoding_cast<std::u32string>(traits::value(*it1));
				if (m_modifier == U'i') return is_ascii_case_insensitive_match(m_value.begin(), m_value.end(), s.begin(), s.end());
				else return m_value == s;
			}
			case U'~': // [attr~=val]
			{
				std::u32string tmp = encoding_cast<std::u32string>(traits::value(*it1));
				std::vector<std::u32string> v;
				split_on_ascii_whitespace(tmp.begin(), tmp.end(), std::back_inserter(v));
				for (std::u32string const& s : v)
				{
					if (m_modifier == U'i' && is_ascii_case_insensitive_match(m_value.begin(), m_value.end(), s.begin(), s.end())) return true;
					else if(m_value == s) return true;
				}
				break;
			}
			case U'|': // [attr|=val]
			{
				std::u32string s = encoding_cast<std::u32string>(traits::value(*it1));
				if (m_modifier == U'i')
				{
					if (m_value.size() < s.size()
						&& is_ascii_case_insensitive_match(m_value.begin(), m_value.end(), s.begin(), s.begin() + m_value.size())
						&& s[m_value.size()] == U'-') return true;
				}
				else
				{
					if (m_value.size() < s.size()
						&& std::equal(m_value.begin(), m_value.end(), s.begin(), s.begin() + m_value.size())
						&& s[m_value.size()] == U'-') return true;
				}
				break;
			}
			case U'^': // [attr^=val]
			{
				std::u32string s = encoding_cast<std::u32string>(traits::value(*it1));
				if (m_modifier == U'i')
				{
					if (m_value.size() <= s.size()
						&& is_ascii_case_insensitive_match(m_value.begin(), m_value.end(), s.begin(), s.begin() + m_value.size())) return true;
				}
				else
				{
					if (m_value.size() <= s.size()
						&& std::equal(m_value.begin(), m_value.end(), s.begin(), s.begin() + m_value.size())) return true;
				}
				break;
			}
			case U'$': // [attr$=val]
			{
				std::u32string s = encoding_cast<std::u32string>(traits::value(*it1));
				if (s.size() < m_value.size()) return false;
				if (m_modifier == U'i')
				{
					if (is_ascii_case_insensitive_match(m_value.rbegin(), m_value.rend(), s.rbegin(), s.rbegin() + m_value.size())) return true;
				}
				else
				{
					if (std::equal(m_value.rbegin(), m_value.rend(), s.rbegin(), s.rbegin() + m_value.size())) return true;
				}
				break;
			}
			case U'*': // [attr*=val]
			{
				std::u32string s1 = encoding_cast<std::u32string>(traits::value(*it1));
				std::u32string s2 = m_value;
				if (m_modifier == U'i')
				{
					std::u32string tmp;
					to_ascii_lowercase(s1.begin(), s1.end(), std::back_inserter(tmp));
					std::swap(s1, tmp);
					tmp.clear();
					to_ascii_lowercase(s2.begin(), s2.end(), std::back_inserter(tmp));
					s2 = std::move(tmp);
				}
				if (s1.find(s2) != std::u32string::npos) return true;
				break;
			}
			default:
				break;
			}
			return false;
		}

	private:
		/*! @brief 属性名*/
		wq_name        m_name;
		char32_t       m_matcher;
		std::u32string m_value;

		/*! @brief 照合オプション
		* 
		* 現在は、「 i 」大文字小文字を無視して照合と、「 s 」大文字小文字を区別して照合の二種類。
		* 構文解析時には、セレクタ文字列の通りに記録する。
		* ホスト言語に合わせて改変するようなことはしない。
		*/
		char32_t m_modifier;
	};

	/*! @brief <subclass-selector>
	*
	* @code
	* 	<subclass-selector> = <id-selector> | <class-selector> |
	* 		<attribute-selector> | <pseudo-class-selector>
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-subclass-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-subclass-selector
	*/
	class subclass_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

		using value_type = std::variant<id_selector, class_selector, attribute_selector, pseudo_class_selector>;

	public:
		static subclass_selector consume(syntax_primitive_stream in);
	public:
		subclass_selector() = default;
		subclass_selector(const_iterator first, const_iterator last, value_type value);

		value_type const& value() const;

		/*! @brief ノードにマッチするか検査する
		*
		* @sa id_selector::match()
		*/
		template <typename NodePointer>
		bool match(NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			assert(!(ctx.m_type_name == document_type_name::Xml
				&& (ctx.m_mode_name == document_mode_name::LimitedQuirks || ctx.m_mode_name == document_mode_name::Quirks)));

			return std::visit([&](auto const& x) { return x.match(np, ctx); }, m_value);
		}

	private:
		value_type m_value;
	};

	/*! @brief <type-selector>
	* 
	* タグ名にマッチするセレクタ。
	*
	* @code
	* 	<type-selector> = <wq-name> | <ns-prefix>? '*'
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-type-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-type-selector
	*/
	class type_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

		using value_type = std::variant<wq_name, std::optional<ns_prefix>>;

	public:
		static type_selector consume(syntax_primitive_stream in);

	public:
		type_selector() = default;
		type_selector(const_iterator first, const_iterator last, value_type value);

		value_type const& value() const;

		/*! @brief ノードにマッチするか検査する
		*
		* @sa id_selector::match()
		*
		* @internal
		* <hr>
		* スタイルシートをサポートしていないので、名前空間をテストできていない。
		*/
		template <typename NodePointer>
		bool match(NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			using traits = typename wordring::html::node_traits<NodePointer>;
			using wordring::whatwg::is_ascii_case_insensitive_match;
			using wordring::encoding_cast;

			assert(!(ctx.m_type_name == document_type_name::Xml
				&& (ctx.m_mode_name == document_mode_name::LimitedQuirks || ctx.m_mode_name == document_mode_name::Quirks)));

			// 要素以外のノードにタグ名は無い
			if (!traits::is_element(np)) return false;

			// 名前空間接頭辞
			std::optional<ns_prefix> const& opt = (m_value.index() == 0)
				? std::get_if<wq_name>(&m_value)->prefix()
				: **std::get_if<std::optional<ns_prefix>>(&m_value);
	
			if (opt)
			{
				if (ctx.m_namespace_enabled)
				{
					std::u32string const& prefix1 = opt->string();
					std::u32string uri2 = encoding_cast<std::u32string>(traits::get_namespace(np));

					if (prefix1 == U"*"); // 接頭辞を持たない要素を含めて全てにマッチ
					else if (prefix1 == U"" && !uri2.empty()) return false;
					else
					{
						std::u32string const* uri1 = ctx.get_namespace(prefix1);
						if (uri1 == nullptr || *uri1 != uri2) return false;
					}
				}
				else
				{
					// 名前空間をサポートしないにもかかわらず、セレクタで名前空間が指定された。
					// TODO: ワイルドカード、空の接頭辞も無効なセレクタとして何にもマッチしないことにしているが、
					// どうすべきか？
					return false;
				}
			}
			else
			{
				if (ctx.m_default_namespace_uri.empty());
				else if (ctx.m_namespace_enabled)
				{
					std::u32string const* uri1 = ctx.get_namespace(ctx.m_default_namespace_uri);
					std::u32string uri2 = encoding_cast<std::u32string>(traits::get_namespace(np));
					if (uri1 == nullptr || *uri1 != uri2) return false;
				}
			}

			// タグ名
			if (m_value.index() == 1) return true;

			std::u32string const& name1 = std::get_if<wq_name>(&m_value)->name();
			std::u32string name2 = encoding_cast<std::u32string>(traits::get_local_name(np));

			if (ctx.m_type_name == document_type_name::Html)
			{
				if (is_ascii_case_insensitive_match(name1.begin(), name1.end(), name2.begin(), name2.end())) return true;
			}
			else
			{
				if (name1 == name2) return true;
			}

			return false;
		}

	private:
		/*! wq_name あるいは std::optional の場合は * セレクタ
		*/
		value_type m_value;
	};

	/*! @brief <simple-selector>
	*
	* @code
	* 	<simple-selector> = <type-selector> | <subclass-selector>
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-simple-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-simple-selector
	*/
	class simple_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

		using value_type = std::variant<std::monostate, type_selector, subclass_selector>;

	public:
		static simple_selector consume(syntax_primitive_stream in);

	public:
		simple_selector() = default;
		simple_selector(const_iterator first, const_iterator last, value_type value);

		value_type const& value() const;

		/*! @brief ノードにマッチするか検査する
		*
		* @sa id_selector::match()
		*/
		template <typename NodePointer>
		bool match(NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			assert(!(ctx.m_type_name == document_type_name::Xml
				&& (ctx.m_mode_name == document_mode_name::LimitedQuirks || ctx.m_mode_name == document_mode_name::Quirks)));

			if (m_value.index() == 1) return std::get_if<type_selector>(&m_value)->match(np, ctx);
			else if (m_value.index() == 2) return std::get_if<subclass_selector>(&m_value)->match(np, ctx);

			return false;
		}

	private:
		value_type m_value;
	};

	/*! @brief <compound-selector>
	*
	* @code
	* 	<compound-selector> = [ <type-selector>? <subclass-selector>*
	* 							[ <pseudo-element-selector> <pseudo-class-selector>* ]* ]!
	* @endcode
	*
	* Compound セレクタは、すべての単体セレクタとマッチするとき、全体とマッチする。
	* したがって、順番のような単体セレクタ同士の関係性が無いので、構造を保存せず、単体セレクタのリストとして値を保持することとした。
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-compound-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-compound-selector
	*/
	class compound_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

		using value_type = std::variant<type_selector, subclass_selector, pseudo_element_selector, pseudo_class_selector>;

	public:
		static compound_selector consume(syntax_primitive_stream in);

	public:
		compound_selector() = default;
		compound_selector(const_iterator first, const_iterator last, std::vector<value_type>&& value);

		std::vector<value_type> const& value() const;

		/*! @brief ノードにマッチするか検査する
		*
		* @sa id_selector::match()
		*/
		template <typename NodePointer>
		bool match(NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			assert(!(ctx.m_type_name == document_type_name::Xml
				&& (ctx.m_mode_name == document_mode_name::LimitedQuirks || ctx.m_mode_name == document_mode_name::Quirks)));

			for (value_type const& val : m_value)
			{
				if (!std::visit([&](auto const& x) { return x.match(np, ctx); }, val)) return false;
			}

			return true;
		}

	private:
		std::vector<value_type> m_value;
	};

	/*! @brief <complex-selector>
	*
	* @code
	* 	<complex-selector> = <compound-selector> [ <combinator>? <compound-selector> ]*
	* @endcode
	*
	* m_value において、 compound_selector 間に combinator が無い場合は、子孫を表す空白文字があるものとみなす。
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-complex-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-complex-selector
	*/
	class complex_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

		using value_type = std::variant<compound_selector, combinator>;
		using reverse_iterator = std::vector<value_type>::const_reverse_iterator;
	public:
		static complex_selector consume(syntax_primitive_stream in);

	private:
		/*! @brief ノードにマッチするか検査する
		*
		* @internal
		* <hr>
		* - テキスト、コメントの子要素は無いので、子結合子「>」は直上のノードを要素と見做せるはず？
		* - 次・同胞結合子「+」は、要素以外のノードを無視する必要が有る。
		* @sa https://triple-underscore.github.io/selectors4-ja.html#match-a-complex-selector-against-an-element
		*
		* Scoping root を持つ場合、子孫である必要が有るのは判定しようとするノードのみで、
		* セレクタの他の部分はその制約を受けない。
		* @sa https://triple-underscore.github.io/selectors4-ja.html#relative-selector
		*/
		template <typename NodePointer>
		bool match(reverse_iterator it, NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			using traits = typename wordring::html::node_traits<NodePointer>;

			if (!std::get_if<compound_selector>(&*it)->match(np, ctx)) return false;
			++it;
			if (it == m_value.rend()) return true;

			char32_t c = U' ';
			if (it->index() == 1)
			{
				c = std::get_if<combinator>(&*it)->type();
				++it;
			}

			switch (c)
			{
			case U' ':
				for (auto p = traits::parent(np); p != traits::pointer(); p = traits::parent(p))
				{
					if (match(it, p, ctx)) return true;
				}
				break;
			case U'>':
			{
				auto p = traits::parent(np);
				if (p != traits::pointer() && match(it, p, ctx)) return true;
				break;
			}
			case U'+':
			{
				auto parent = traits::parent(np);
				if (parent == traits::pointer()) return false; // 根に兄弟は無いはず。
				auto first = traits::begin(parent);
				for(auto p = np; p != first;)
				{
					p = traits::prev(p);
					if (traits::is_element(p)) return match(it, p, ctx);
				}
				break;
			}
			case U'~':
			{
				auto parent = traits::parent(np);
				if (parent == traits::pointer()) return false; // 根に兄弟は無いはず。
				auto first = traits::begin(parent);
				for (auto p = np; p != first;)
				{
					p = traits::prev(p);
					if (traits::is_element(p) && match(it, p, ctx)) return true;
				}
				break;
			}
			default:
				break;
			}

			return false;
		}

	public:
		complex_selector() = default;
		complex_selector(const_iterator first, const_iterator last, std::vector<value_type>&& value);

		std::vector<value_type> const& value() const;

		/*! @brief ノードにマッチするか検査する
		*
		* @sa id_selector::match()
		*/
		template <typename NodePointer>
		bool match(NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			assert(!(ctx.m_type_name == document_type_name::Xml
				&& (ctx.m_mode_name == document_mode_name::LimitedQuirks || ctx.m_mode_name == document_mode_name::Quirks)));

			return match(m_value.rbegin(), np, ctx);
		}

	private:
		std::vector<value_type> m_value;
	};

	/*! @brief <relative-selector>
	*
	* Combinator から始まることも出来る Complex セレクタという位置づけ。
	*
	* 先頭に空白文字があった場合の取り扱いが規格上あいまいである。
	* しかし、先頭に Combinator が無い場合、子孫結合子（空白文字）がある扱いと規格で決まっている。
	* マッチング時においては、先頭に Combinator が無ければ子孫結合子があるものとして取り扱う。
	*
	* @code
	* 	<relative-selector> = <combinator>? <complex-selector>
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-relative-selector
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-relative-selector
	*/
	class relative_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

		using value_type = std::variant<compound_selector, combinator>;

	public:
		static relative_selector consume(syntax_primitive_stream in);

	public:
		relative_selector() = default;
		relative_selector(const_iterator first, const_iterator last, std::vector<value_type>&& value);

		std::vector<value_type> const& value() const;

		/*! @brief ノードにマッチするか検査する
		*
		* @sa id_selector::match()
		*/
		template <typename NodePointer>
		bool match(NodePointer const& np, match_context<NodePointer> const& ctx) const
		{
			using traits = typename wordring::html::node_traits<NodePointer>;
			using ns_name = wordring::html::ns_name;
			using wordring::whatwg::is_ascii_case_insensitive_match;
			using wordring::encoding_cast;

			assert(!(ctx.m_type_name == document_type_name::Xml
				&& (ctx.m_mode_name == document_mode_name::LimitedQuirks || ctx.m_mode_name == document_mode_name::Quirks)));

			return false;
		}

		/*! @brief 相対セレクタを絶対化する
		* 
		* @sa https://triple-underscore.github.io/selectors4-ja.html#absolutize
		*/
		template <typename NodePointer>
		void absolutize(match_context<NodePointer> const& ctx)
		{
			using traits = typename wordring::html::node_traits<NodePointer>;

			if (ctx.m_scope_elements.empty()
				&& (ctx.m_scoping_root == traits::pointer() || !traits::is_element(ctx.m_scoping_root)));
			else
			{
				;
			}
		}

	private:
		std::vector<value_type> m_value;
	};

	/*! @brief <relative-selector-list>
	*
	* @code
	* 	<relative-selector-list> = <relative-selector>#
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-relative-selector-list
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-relative-selector-list
	*/
	class relative_selector_list: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static relative_selector_list consume(syntax_primitive_stream in);

	public:
		relative_selector_list() = default;
		relative_selector_list(const_iterator first, const_iterator last, std::vector<relative_selector>&& value);

		std::vector<relative_selector> const& value() const;

	private:
		std::vector<relative_selector> m_value;
	};

	/*! @brief <simple-selector-list>
	*
	* @code
	* 	<simple-selector-list> = <simple-selector>#
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-simple-selector-list
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-simple-selector-list
	*/
	class simple_selector_list: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static simple_selector_list consume(syntax_primitive_stream in);

	public:
		simple_selector_list() = default;
		simple_selector_list(const_iterator first, const_iterator last, std::vector<simple_selector>&& value);

		std::vector<simple_selector> const& value() const;

	private:
		std::vector<simple_selector> m_value;
	};

	/*! @brief <compound-selector-list>
	*
	* @code
	* 	<compound-selector-list> = <compound-selector>#
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-compound-selector-list
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-compound-selector-list
	*/
	class compound_selector_list: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static compound_selector_list consume(syntax_primitive_stream in);

	public:
		compound_selector_list() = default;
		compound_selector_list(const_iterator first, const_iterator last, std::vector<compound_selector>&& value);

		std::vector<compound_selector> const& value() const;

	private:
		std::vector<compound_selector> m_value;
	};

	/*! @brief <complex-selector-list>
	*
	* @code
	* 	<complex-selector-list> = <complex-selector>#
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-complex-selector-list
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-complex-selector-list
	*/
	class complex_selector_list: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static complex_selector_list consume(syntax_primitive_stream in);

	public:
		complex_selector_list() = default;
		complex_selector_list(const_iterator first, const_iterator last, std::vector<complex_selector>&& value);

		std::vector<complex_selector> const& value() const;

	private:
		std::vector<complex_selector> m_value;
	};


	/*! @brief <selector-list>
	*
	* @code
	* 	<selector-list> = <complex-selector-list>
	* @endcode
	*
	* @sa selector_list
	* 
	* @sa https://drafts.csswg.org/selectors-4/#typedef-selector-list
	* @sa https://triple-underscore.github.io/selectors4-ja.html#typedef-selector-list
	*/
	class selector_list: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static selector_list consume(syntax_primitive_stream in);

	public:
		selector_list() = default;
		selector_list(const_iterator first, const_iterator last, complex_selector_list&& value);

		complex_selector_list const& value() const;

	private:
		complex_selector_list m_value;
	};


}
