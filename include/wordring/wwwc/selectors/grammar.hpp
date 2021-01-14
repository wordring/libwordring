#pragma once

// ------------------------------------------------------------------------------------------------
// 17. Grammar
//
// https://drafts.csswg.org/selectors-4/#grammar
// https://triple-underscore.github.io/selectors4-ja.html#grammar
// ------------------------------------------------------------------------------------------------

#include <wordring/wwwc/css_syntax/parsing.hpp>

#include <algorithm>
#include <optional>
#include <string>
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

	class selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

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
		const_iterator m_first;
		const_iterator m_last;
	};

	/*! @brief <combinator>

	@code
		<combinator> = '>' | '+' | '~' | [ '|' '|' ]
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-combinator
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-combinator
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

	@code
		<ns-prefix> = [ <ident-token> | '*' ]? '|'
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-ns-prefix
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-ns-prefix
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
		ns_prefix(const_iterator first, const_iterator last, std::u32string const& ns);

		ns_prefix& operator =(ns_prefix const&) = default;
		ns_prefix& operator =(ns_prefix&&) = default;

		/*! @brief 名前空間を返す
		
		@return * あるいは名前空間名
		
		*/
		std::u32string const& nspace() const;

	private:
		std::u32string m_ns;
	};

	/*! @brief <wq-name>

	@code
		<wq-name> = <ns-prefix>? <ident-token>
	@endcode

	@par 実装
	- [attr|=val] → attr| が <ns_prefix> と誤認されるため、後続が「IdentToken あるいは *」か先読みする必要が有る。

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-wq-name
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-wq-name
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

		/*! @brief 名前空間名を返す
		*/
		std::u32string nspace() const;

		/*! @brief 名前を返す
		*/
		std::u32string const& name() const;

	private:
		std::optional<ns_prefix> m_ns;
		std::u32string m_name;
	};
	
	/*! @brief <id-selector>

	@code
		<id-selector> = <hash-token>
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-id-selector
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-id-selector
	*/
	class id_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static id_selector consume(syntax_primitive_stream in);

	public:
		id_selector() = default;
		id_selector(const_iterator first, const_iterator last, std::u32string const& value);

		/*! @brief 属性値を返す
		*/
		std::u32string const& value() const;

		template <typename Node>
		bool match(Node const& node) const
		{
			return true;
		}

	private:
		std::u32string m_value;
	};

	/*! @brief <class-selector>

	@code
		<class-selector> = '.' <ident-token>
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-class-selector
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-class-selector
	*/
	class class_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static class_selector consume(syntax_primitive_stream in);

	public:
		class_selector() = default;
		class_selector(const_iterator first, const_iterator last, std::u32string const& name);

		/*! @brief クラス名を返す
		*/
		std::u32string const& name() const;

	private:
		std::u32string m_name;
	};

	/*! @brief <attr-matcher>
	
	@code
		<attr-matcher> = [ '~' | '|' | '^' | '$' | '*' ]? '='
	@endcode

	@sa selector_list

	@sa https://drafts.csswg.org/selectors-4/#typedef-attr-matcher
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-attr-matcher
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
	
	@code
		<attr-modifier> = i | s
	@endcode
	
	@sa selector_list

	@sa https://drafts.csswg.org/selectors-4/#typedef-attr-modifier
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-attr-modifier
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

	@code
		<pseudo-class-selector> = ':' <ident-token> | ':' <function-token> <any-value> ')'
	@endcode

	@sa selector_list

	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-pseudo-class-selector
	@sa https://drafts.csswg.org/selectors-4/#typedef-pseudo-class-selector
	*/
	class pseudo_class_selector: public selector_grammar
	{
	public:
		using value_type = std::variant<std::monostate, ident_token, function>;
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static pseudo_class_selector consume(syntax_primitive_stream in);

	public:
		pseudo_class_selector() = default;

		pseudo_class_selector(const_iterator first, const_iterator last);
		pseudo_class_selector(const_iterator first, const_iterator last, ident_token const& id);
		pseudo_class_selector(const_iterator first, const_iterator last, function const& fn);

		syntax_primitive_name type() const;

		template <typename T>
		T const& get() const { return std::get<T>(m_value); }

	protected:
		value_type m_value;
	};

	/*! @brief <pseudo-element-selector>
	
	@code
		<pseudo-element-selector> = ':' <pseudo-class-selector>
	@endcode

	@sa selector_list
	
	@sa https://drafts.csswg.org/selectors-4/#typedef-pseudo-element-selector
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-pseudo-element-selector
	*/
	class pseudo_element_selector : public pseudo_class_selector
	{
	public:
		using value_type = pseudo_class_selector::value_type;
		using const_iterator = syntax_primitive_stream::const_iterator;

	public:
		static pseudo_element_selector consume(syntax_primitive_stream s);

	private:
		explicit pseudo_element_selector(pseudo_class_selector&& s);
		pseudo_element_selector(const_iterator first, const_iterator last);
		pseudo_element_selector(const_iterator first, const_iterator last, ident_token const& id);
	};

	/*! @brief <attribute-selector>

	@code
		<attribute-selector> = '[' <wq-name> ']' |
			'[' <wq-name> <attr-matcher> [ <string-token> | <ident-token> ] <attr-modifier>? ']'
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-attribute-selector
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-attribute-selector
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

		std::u32string nspace() const;
		std::u32string const& name() const;
		char32_t matcher() const;
		std::u32string const& value() const;
		char32_t modifier() const;

	private:
		wq_name        m_name;
		char32_t       m_matcher;
		std::u32string m_value;
		char32_t       m_modifier;
	};

	/*! @brief <subclass-selector>

	@code
		<subclass-selector> = <id-selector> | <class-selector> |
			<attribute-selector> | <pseudo-class-selector>
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-subclass-selector
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-subclass-selector
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

	private:
		value_type m_value;
	};

	/*! @brief <type-selector>
	
	タグ名にマッチするセレクタ。

	@code
		<type-selector> = <wq-name> | <ns-prefix>? '*'
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-type-selector
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-type-selector
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

	private:
		value_type m_value;
	};

	/*! @brief <simple-selector>

	@code
		<simple-selector> = <type-selector> | <subclass-selector>
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-simple-selector
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-simple-selector
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

	private:
		value_type m_value;
	};

	/*! @brief <compound-selector>

	@code
		<compound-selector> = [ <type-selector>? <subclass-selector>*
								[ <pseudo-element-selector> <pseudo-class-selector>* ]* ]!
	@endcode

	Compound セレクタは、すべての単体セレクタとマッチするとき、全体とマッチする。
	したがって、順番のような単体セレクタ同士の関係性が無いので、構造を保存せず、単体セレクタのリストとして値を保持することとした。

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-compound-selector
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-compound-selector
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

	private:
		std::vector<value_type> m_value;
	};

	/*! @brief <complex-selector>

	@code
		<complex-selector> = <compound-selector> [ <combinator>? <compound-selector> ]*
	@endcode

	m_value において、 compound_selector 間に combinator が無い場合は、子孫を表す空白文字があるものとみなす。

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-complex-selector
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-complex-selector
	*/
	class complex_selector: public selector_grammar
	{
	public:
		using const_iterator = syntax_primitive_stream::const_iterator;

		using value_type = std::variant<compound_selector, combinator>;

	public:
		static complex_selector consume(syntax_primitive_stream in);

	public:
		complex_selector() = default;
		complex_selector(const_iterator first, const_iterator last, std::vector<value_type>&& value);

		std::vector<value_type> const& value() const;

	private:
		std::vector<value_type> m_value;
	};

	/*! @brief <relative-selector>
	
	Combinator から始まることも出来る Complex セレクタという位置づけ。

	先頭に空白文字があった場合の取り扱いが規格上あいまいである。
	しかし、先頭に Combinator が無い場合、子孫結合子（空白文字）がある扱いと規格で決まっている。
	マッチング時においては、先頭に Combinator が無ければ子孫結合子があるものとして取り扱う。

	@code
		<relative-selector> = <combinator>? <complex-selector>
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-relative-selector
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-relative-selector
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

	private:
		std::vector<value_type> m_value;
	};

	/*! @brief <relative-selector-list>

	@code
		<relative-selector-list> = <relative-selector>#
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-relative-selector-list
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-relative-selector-list
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

	@code
		<simple-selector-list> = <simple-selector>#
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-simple-selector-list
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-simple-selector-list
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

	@code
		<compound-selector-list> = <compound-selector>#
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-compound-selector-list
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-compound-selector-list
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

	@code
		<complex-selector-list> = <complex-selector>#
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-complex-selector-list
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-complex-selector-list
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

	@code
		<selector-list> = <complex-selector-list>
	@endcode

	@sa selector_list
	@sa https://drafts.csswg.org/selectors-4/#typedef-selector-list
	@sa https://triple-underscore.github.io/selectors4-ja.html#typedef-selector-list
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
