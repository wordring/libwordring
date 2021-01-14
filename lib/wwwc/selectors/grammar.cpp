// wwwc/selctors/grammar.ccp

#include <wordring/wwwc/selectors/grammar.hpp>

#include <wordring/whatwg/infra/infra.hpp>

#include <array>
#include <string_view>

using namespace wordring::wwwc::css;
using namespace wordring::whatwg;


// ------------------------------------------------------------------------------------------------
// class selector_grammar
// ------------------------------------------------------------------------------------------------

selector_grammar::selector_grammar(const_iterator first, const_iterator last)
	: m_first(first)
	, m_last(last)
{
}

selector_grammar::operator bool() const
{
	return m_first != m_last;
}

bool selector_grammar::operator !() const
{
	return m_first == m_last;
}

selector_grammar::const_iterator selector_grammar::begin() const
{
	return m_first;
}

selector_grammar::const_iterator selector_grammar::end() const
{
	return m_last;
}

// ------------------------------------------------------------------------------------------------
// <combinator> = '>' | '+' | '~' | [ '|' '|' ]
// ------------------------------------------------------------------------------------------------

combinator combinator::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	syntax_primitive const& p1 = in.consume();
	if (p1.type() == syntax_primitive_name::DelimToken)
	{
		char32_t cp = p1.get<delim_token>().m_value;
		switch (cp)
		{
		case U'>':
		case U'+':
		case U'~':
			return combinator(it0, in.begin(), cp);
		default:
			break;
		}
		if (cp == U'|')
		{
			syntax_primitive const& p2 = in.consume();
			if (p2.type() == syntax_primitive_name::DelimToken && p2.get<delim_token>().m_value == U'|')
			{
				return combinator(it0, in.begin(), U'|');
			}
		}
	}
	return combinator(it0, it0, U'\0');
}

combinator::combinator(const_iterator first, const_iterator last, char32_t c)
	: selector_grammar(first, last)
	, m_c(c)
{
}

char32_t combinator::type() const
{
	return m_c;
}

// ------------------------------------------------------------------------------------------------
// <ns-prefix> = [ <ident-token> | '*' ]? '|'
// ------------------------------------------------------------------------------------------------

ns_prefix ns_prefix::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	std::u32string s;
	syntax_primitive const& p1 = in.current();
	switch (p1.type())
	{
	case syntax_primitive_name::IdentToken:
		s = p1.get<ident_token>().m_value;
		in.consume();
		break;
	case syntax_primitive_name::DelimToken:
		if (p1.get<delim_token>().m_value == U'*')
		{
			s.push_back(U'*');
			in.consume();
		}
		break;
	default:
		break;
	}

	syntax_primitive const& p2 = in.consume();
	if (p2.type() == syntax_primitive_name::DelimToken && p2.get<delim_token>().m_value == U'|')
	{
		return ns_prefix(it0, in.begin(), s);
	}

	return ns_prefix(it0, it0, U"");
}

ns_prefix::ns_prefix(const_iterator first, const_iterator last, std::u32string const& ns)
	: selector_grammar(first, last)
	, m_ns(ns)
{
}

std::u32string const& ns_prefix::nspace() const
{
	return m_ns;
}

// ------------------------------------------------------------------------------------------------
// <wq-name> = <ns-prefix>? <ident-token>
// ------------------------------------------------------------------------------------------------

wq_name wq_name::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	std::optional<ns_prefix> opt;
	ns_prefix ns = ns_prefix::consume(in);
	in.advance(ns.end());

	syntax_primitive const& p1 = in.current();
	if ((p1.type() == syntax_primitive_name::IdentToken)
	 || (p1.type() == syntax_primitive_name::DelimToken && p1.get<delim_token>().m_value == U'*'))
	{
		if (ns) opt = std::make_optional<ns_prefix>(std::move(ns));
	}
	else
	{
		in.advance(it0);
	}

	syntax_primitive const& p2 = in.consume();
	if (p2.type() == syntax_primitive_name::IdentToken)
	{
		return wq_name(it0, in.begin(), opt, p2.get<ident_token>().m_value);
	}

	return wq_name(it0, it0, std::make_optional<ns_prefix>(), U"");
}

wq_name::wq_name(const_iterator first, const_iterator last, std::optional<ns_prefix> ns, std::u32string const& name)
	: selector_grammar(first, last)
	, m_ns(ns)
	, m_name(name)
{
}

std::u32string wq_name::nspace() const
{
	if (m_ns) return m_ns->nspace();
	return U"";
}

std::u32string const& wq_name::name() const
{
	return m_name;
}

// ------------------------------------------------------------------------------------------------
// <id-selector> = <hash-token>
// ------------------------------------------------------------------------------------------------

id_selector id_selector::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	syntax_primitive const& p = in.consume();
	if (p.type() == syntax_primitive_name::HashToken)
	{
		return id_selector(it0, in.begin(), p.get<hash_token>().m_value);
	}

	return id_selector(it0, it0, U"");
}

id_selector::id_selector(const_iterator first, const_iterator last, std::u32string const& value)
	: selector_grammar(first, last)
	, m_value(value)
{
}

std::u32string const& id_selector::value() const
{
	return m_value;
}

// ------------------------------------------------------------------------------------------------
// <class-selector> = '.' <ident-token>
// ------------------------------------------------------------------------------------------------

class_selector class_selector::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	syntax_primitive const& p1 = in.consume();
	if (p1.type() == syntax_primitive_name::DelimToken && p1.get<delim_token>().m_value == U'.')
	{
		syntax_primitive const& p2 = in.consume();
		if (p2.type() == syntax_primitive_name::IdentToken)
		{
			return class_selector(it0, in.begin(), p2.get<ident_token>().m_value);
		}
	}

	return class_selector(it0, it0, U"");
}

class_selector::class_selector(const_iterator first, const_iterator last, std::u32string const& name)
	: selector_grammar(first, last)
	, m_name(name)
{
}

std::u32string const& class_selector::name() const
{
	return m_name;
}

// ------------------------------------------------------------------------------------------------
// <attr-matcher> = [ '~' | '|' | '^' | '$' | '*' ]? '='
// ------------------------------------------------------------------------------------------------

attr_matcher attr_matcher::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	syntax_primitive const& p1 = in.consume();
	if (p1.type() == syntax_primitive_name::DelimToken)
	{
		char32_t c1 = p1.get<delim_token>().m_value;
		switch (c1)
		{
		case U'=':
			return attr_matcher(it0, in.begin(), U'\0');
		case U'~':
		case U'|':
		case U'^':
		case U'$':
		case U'*':
			break;
		default:
			return attr_matcher(it0, it0, U'\0');
		}
		syntax_primitive const& p2 = in.consume();
		if (p2.type() == syntax_primitive_name::DelimToken && p2.get<delim_token>().m_value == U'=')
		{
			return attr_matcher(it0, in.begin(), c1);
		}
	}
	return attr_matcher(it0, it0, U'\0');
}

attr_matcher::attr_matcher(const_iterator first, const_iterator last, char32_t c)
	: selector_grammar(first, last)
	, m_prefix(c)
{
}

char32_t attr_matcher::prefix() const
{
	return m_prefix;
}

// ------------------------------------------------------------------------------------------------
// <attr-modifier> = i | s
// ------------------------------------------------------------------------------------------------

attr_modifier attr_modifier::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	syntax_primitive const& p = in.consume();
	if (p.type() == syntax_primitive_name::IdentToken)
	{
		std::u32string const& s = p.get<ident_token>().m_value;
		if (s == U"i" || s == U"I")
		{
			return attr_modifier(it0, in.begin(), U'i');
		}
		else if (s == U"s" || s == U"S")
		{
			return attr_modifier(it0, in.begin(), U's');
		}
	}
	return attr_modifier(it0, it0, U'\0');
}

attr_modifier::attr_modifier(const_iterator first, const_iterator last, char32_t c)
	: selector_grammar(first, last)
	, m_c(c)
{
}

char32_t attr_modifier::value() const
{
	return m_c;
}

// ------------------------------------------------------------------------------------------------
// <pseudo-class-selector>	= ':' <ident-token> | ':' <function-token> <any-value> ')'
// ------------------------------------------------------------------------------------------------

pseudo_class_selector pseudo_class_selector::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	if (in.consume().type() == syntax_primitive_name::ColonToken)
	{
		syntax_primitive const& p = in.consume();
		switch (p.type())
		{
		case syntax_primitive_name::IdentToken:
		{
			std::array<std::u32string_view, 4> a =
			{
				U"before",
				U"after",
				U"first-line",
				U"first-letter"
			};

			ident_token const& id = p.get<ident_token>();
			std::u32string const& sz = id.m_value;
			for (std::u32string_view const& sv : a)
			{
				if (is_ascii_case_insensitive_match(sv.begin(), sv.end(), sz.begin(), sz.end()))
				{
					return { it0, it0 };
				}
			}

			return { it0, in.begin(), id };
		}
		case syntax_primitive_name::Function:
			return { it0, in.begin(), p.get<function>() };
		default:
			break;
		}
	}
	return { it0, it0 };
}

pseudo_class_selector::pseudo_class_selector(const_iterator first, const_iterator last)
	: selector_grammar(first, last)
	, m_value()
{
}

pseudo_class_selector::pseudo_class_selector(const_iterator first, const_iterator last, ident_token const& id)
	: selector_grammar(first, last)
	, m_value(id)
{
}

pseudo_class_selector::pseudo_class_selector(const_iterator first, const_iterator last, function const& fn)
	: selector_grammar(first, last)
	, m_value(fn)
{
}

syntax_primitive_name pseudo_class_selector::type() const
{
	switch (m_value.index())
	{
	case 0:
		assert(false);
		break;
	case 1: return syntax_primitive_name::IdentToken;
	case 2: return syntax_primitive_name::Function;
	}
	return static_cast<syntax_primitive_name>(0);
}

// ------------------------------------------------------------------------------------------------
// <pseudo-element-selector> = ':' <pseudo-class-selector>
// ------------------------------------------------------------------------------------------------

pseudo_element_selector pseudo_element_selector::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	if (in.consume().type() == syntax_primitive_name::ColonToken)
	{
		pseudo_element_selector s(pseudo_class_selector::consume(in));
		if (s)
		{
			s.m_first = it0;
			return s;
		}

		if (in.current().type() == syntax_primitive_name::ColonToken) in.consume();

		syntax_primitive const& p = in.consume();
		if (p.type() == syntax_primitive_name::IdentToken)
		{
			std::array<std::u32string_view, 4> a =
			{
				U"before",
				U"after",
				U"first-line",
				U"first-letter"
			};

			ident_token const& id = p.get<ident_token>();
			std::u32string const& sz = id.m_value;
			for (std::u32string_view const& sv : a)
			{
				if (is_ascii_case_insensitive_match(sv.begin(), sv.end(), sz.begin(), sz.end()))
				{
					return { it0, in.begin(), id };
				}
			}
		}
	}

	return { it0, it0 };
}

pseudo_element_selector::pseudo_element_selector(pseudo_class_selector&& s)
	: pseudo_class_selector(std::move(s))
{
}

pseudo_element_selector::pseudo_element_selector(const_iterator first, const_iterator last)
	: pseudo_class_selector(first, last)
{
}

pseudo_element_selector::pseudo_element_selector(const_iterator first, const_iterator last, ident_token const& id)
	: pseudo_class_selector(first, last, id)
{
}

// ------------------------------------------------------------------------------------------------
// <attribute-selector> =
//		'[' <wq-name> ']' |
//		'[' <wq-name> <attr-matcher> [ <string-token> | <ident-token> ] <attr-modifier>? ']'
// ------------------------------------------------------------------------------------------------

attribute_selector attribute_selector::consume(syntax_primitive_stream in1)
{
	const_iterator it0 = in1.begin();

	syntax_primitive const& p1 = in1.consume();
	if (p1.type() == syntax_primitive_name::SimpleBlock)
	{
		simple_block const& sb = p1.get<simple_block>();
		if (sb.m_associated_token != U'[') return attribute_selector(it0, it0, wq_name(), U'\0', U"", U'\0');

		std::vector<syntax_primitive> v;
		for (component_value const& cv : sb.m_value) v.push_back(cv);
		syntax_primitive_stream in2(v);

		in2.skip_whitespace();
		wq_name name = wq_name::consume(in2);
		if (!name) return attribute_selector(it0, it0, wq_name(), U'\0', U"", U'\0');

		in2.advance(name.end());
		in2.skip_whitespace();
		attr_matcher matcher = attr_matcher::consume(in2);
		if (matcher)
		{
			std::u32string value;

			in2.advance(matcher.end());
			in2.skip_whitespace();
			syntax_primitive const& p2 = in2.consume();
			switch (p2.type())
			{
			case syntax_primitive_name::StringToken:
				value = p2.get<string_token>().m_value;
				break;
			case syntax_primitive_name::IdentToken:
				value = p2.get<ident_token>().m_value;
				break;
			default:
				return attribute_selector(it0, it0, wq_name(), U'\0', U"", U'\0');
				break;
			}

			char32_t modifier = U'\0';
			in2.skip_whitespace();
			attr_modifier m = attr_modifier::consume(in2);
			if (m)
			{
				in2.advance(m.end());
				modifier = m.value();
			}

			return attribute_selector(it0, in1.begin(), name, matcher.prefix(), value, modifier);
		}
		if(!in2) return attribute_selector(it0, in1.begin(), name, U'\0', U"", U'\0');
	}
	return attribute_selector(it0, it0, wq_name(), U'\0', U"", U'\0');
}

attribute_selector::attribute_selector(const_iterator first, const_iterator last, wq_name name, char32_t matcher, std::u32string const& value, char32_t modifier)
	: selector_grammar(first, last)
	, m_name(name)
	, m_matcher(matcher)
	, m_value(value)
	, m_modifier(modifier)
{
}

std::u32string attribute_selector::nspace() const
{
	return m_name.nspace();
}

std::u32string const& attribute_selector::name() const
{
	return m_name.name();
}

char32_t attribute_selector::matcher() const
{
	return m_matcher;
}

std::u32string const& attribute_selector::value() const
{
	return m_value;
}

char32_t attribute_selector::modifier() const
{
	return m_modifier;
}

// ------------------------------------------------------------------------------------------------
// <subclass-selector> = <id-selector> | <class-selector> | <attribute-selector> | <pseudo-class-selector>
// ------------------------------------------------------------------------------------------------

subclass_selector subclass_selector::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	id_selector id = id_selector::consume(in);
	if (id) return subclass_selector(it0, id.end(), id);

	class_selector cls = class_selector::consume(in);
	if(cls) return subclass_selector(it0, cls.end(), cls);

	attribute_selector attr = attribute_selector::consume(in);
	if(attr) return subclass_selector(it0, attr.end(), attr);

	pseudo_class_selector pcls = pseudo_class_selector::consume(in);
	if(pcls) return subclass_selector(it0, pcls.end(), pcls);

	return subclass_selector();
}

subclass_selector::subclass_selector(const_iterator first, const_iterator last, value_type value)
	: selector_grammar(first, last)
	, m_value(value)
{
}

subclass_selector::value_type const& subclass_selector::value() const
{
	return m_value;
}

// ------------------------------------------------------------------------------------------------
// <type-selector> = <wq-name> | <ns-prefix>? '*'
// ------------------------------------------------------------------------------------------------

type_selector type_selector::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	wq_name name = wq_name::consume(in);
	if (name) return type_selector(it0, name.end(), name);

	std::optional<ns_prefix> opt;
	ns_prefix ns = ns_prefix::consume(in);
	if (ns)
	{
		opt = std::make_optional<ns_prefix>(std::move(ns));
		in.advance(ns.end());
	}

	syntax_primitive const& p = in.consume();
	if (p.type() == syntax_primitive_name::DelimToken && p.get<delim_token>().m_value == U'*')
	{
		return type_selector(it0, in.begin(), opt);
	}

	return type_selector(it0, it0, opt);
}

type_selector::type_selector(const_iterator first, const_iterator last, value_type value)
	: selector_grammar(first, last)
	, m_value(value)
{
}

type_selector::value_type const& type_selector::value() const
{
	return m_value;
}

// ------------------------------------------------------------------------------------------------
// <simple-selector> = <type-selector> | <subclass-selector>
// ------------------------------------------------------------------------------------------------

simple_selector simple_selector::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	type_selector type = type_selector::consume(in);
	if (type) return simple_selector(it0, type.end(), type);

	subclass_selector sub = subclass_selector::consume(in);
	if (sub) return simple_selector(it0, sub.end(), sub);

	return simple_selector(it0, it0, std::monostate{});
}

simple_selector::simple_selector(const_iterator first, const_iterator last, value_type value)
	: selector_grammar(first, last)
	, m_value(value)
{
}

simple_selector::value_type const& simple_selector::value() const
{
	return m_value;
}

// ------------------------------------------------------------------------------------------------
// <compound-selector> = [ <type-selector>? <subclass-selector>*
//							[ <pseudo-element-selector> <pseudo-class-selector>* ]* ]!
// ------------------------------------------------------------------------------------------------

compound_selector compound_selector::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	std::vector<value_type> v;

	type_selector type = type_selector::consume(in);
	if (type)
	{
		v.push_back(std::move(type));
		in.advance(type.end());
	}

	while (true)
	{
		subclass_selector sub = subclass_selector::consume(in);
		if (sub)
		{
			v.push_back(sub);
			in.advance(sub.end());
		}
		else break;
	}

	while(true)
	{
		pseudo_element_selector pe = pseudo_element_selector::consume(in);
		if (pe)
		{
			v.push_back(pe);
			in.advance(pe.end());
		}
		else break;

		while (true)
		{
			pseudo_class_selector pc = pseudo_class_selector::consume(in);
			if (pc)
			{
				v.push_back(pc);
				in.advance(pc.end());
			}
			else break;
		}
	}

	if (v.empty()) return compound_selector(it0, it0, std::move(v));

	const_iterator it1;
	std::visit([&](auto const& x) { it1 = x.end(); }, v.back());


	return compound_selector(it0, it1, std::move(v));
}

compound_selector::compound_selector(const_iterator first, const_iterator last, std::vector<value_type>&& value)
	: selector_grammar(first, last)
	, m_value(std::move(value))
{
}

std::vector<compound_selector::value_type> const& compound_selector::value() const
{
	return m_value;
}

// ------------------------------------------------------------------------------------------------
// <complex-selector> = <compound-selector> [ <combinator>? <compound-selector> ]*
// ------------------------------------------------------------------------------------------------

complex_selector complex_selector::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	std::vector<std::variant<compound_selector, combinator>> v;

	if (compound_selector cs = compound_selector::consume(in))
	{
		in.advance(cs.end());
		v.push_back(std::move(cs));
	}
	else
	{
		return complex_selector(it0, it0, std::vector<value_type>());
	}

	while (in)
	{
		bool ws = in.skip_whitespace();
		combinator c = combinator::consume(in);
		if (!ws && !c) break;

		if (c) in.advance(c.end());

		in.skip_whitespace();
		compound_selector cs = compound_selector::consume(in);
		if (cs) in.advance(cs.end());
		else break;

		if(c) v.push_back(std::move(c));
		v.push_back(std::move(cs));
	}

	const_iterator it1;
	std::visit([&](auto const& x) { it1 = x.end(); }, v.back());

	return complex_selector(it0, it1, std::move(v));
}

complex_selector::complex_selector(const_iterator first, const_iterator last, std::vector<value_type>&& value)
	: selector_grammar(first, last)
	, m_value(std::move(value))
{
}

std::vector<complex_selector::value_type> const& complex_selector::value() const
{
	return m_value;
}

// ------------------------------------------------------------------------------------------------
// <relative-selector> = <combinator>? <complex-selector>
// ------------------------------------------------------------------------------------------------

relative_selector relative_selector::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	std::vector<std::variant<compound_selector, combinator>> v;

	combinator c = combinator::consume(in);
	if (c)
	{
		in.advance(c.end());
		v.push_back(std::move(c));
	}

	complex_selector cs = complex_selector::consume(in);
	if (cs)
	{
		in.advance(cs.end());
		for (auto const& p : cs.value()) v.push_back(p);

		return relative_selector(it0, in.begin(), std::move(v));
	}

	return relative_selector(it0, it0, std::vector<value_type>());
}

relative_selector::relative_selector(const_iterator first, const_iterator last, std::vector<value_type>&& value)
	: selector_grammar(first, last)
	, m_value(std::move(value))
{
}

std::vector<relative_selector::value_type> const& relative_selector::value() const
{
	return m_value;
}

// ------------------------------------------------------------------------------------------------
// <relative-selector-list> = <relative-selector>#
// ------------------------------------------------------------------------------------------------

relative_selector_list relative_selector_list::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	std::vector<relative_selector> v;

	relative_selector rs = relative_selector::consume(in);
	if (!rs) return relative_selector_list(it0, it0, std::vector<relative_selector>());

	in.advance(rs.end());
	v.push_back(std::move(rs));
	while (true)
	{
		in.skip_whitespace();
		syntax_primitive const& sp = in.current();
		if (sp.type() != syntax_primitive_name::CommaToken) break;

		in.consume();
		in.skip_whitespace();
		relative_selector rs = relative_selector::consume(in);
		if (!rs) return relative_selector_list(it0, it0, std::vector<relative_selector>());
		in.advance(rs.end());
		v.push_back(std::move(rs));
	}

	return relative_selector_list(it0, in.begin(), std::move(v));
}

relative_selector_list::relative_selector_list(const_iterator first, const_iterator last, std::vector<relative_selector>&& value)
	: selector_grammar(first, last)
	, m_value(std::move(value))
{
}

std::vector<relative_selector> const& relative_selector_list::value() const
{
	return m_value;
}

// ------------------------------------------------------------------------------------------------
// <simple-selector-list> = <simple-selector>#
// ------------------------------------------------------------------------------------------------

simple_selector_list simple_selector_list::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	std::vector<simple_selector> v;

	simple_selector rs = simple_selector::consume(in);
	if (!rs) return simple_selector_list(it0, it0, std::vector<simple_selector>());

	in.advance(rs.end());
	v.push_back(std::move(rs));
	while (true)
	{
		in.skip_whitespace();
		syntax_primitive const& sp = in.current();
		if (sp.type() != syntax_primitive_name::CommaToken) break;

		in.consume();
		in.skip_whitespace();
		simple_selector rs = simple_selector::consume(in);
		if (!rs) return simple_selector_list(it0, it0, std::vector<simple_selector>());
		in.advance(rs.end());
		v.push_back(std::move(rs));
	}

	return simple_selector_list(it0, in.begin(), std::move(v));
}

simple_selector_list::simple_selector_list(const_iterator first, const_iterator last, std::vector<simple_selector>&& value)
	: selector_grammar(first, last)
	, m_value(std::move(value))
{
}

std::vector<simple_selector> const& simple_selector_list::value() const
{
	return m_value;
}

// ------------------------------------------------------------------------------------------------
// <compound-selector-list> = <compound-selector>#
// ------------------------------------------------------------------------------------------------

compound_selector_list compound_selector_list::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	std::vector<compound_selector> v;

	compound_selector rs = compound_selector::consume(in);
	if (!rs) return compound_selector_list(it0, it0, std::vector<compound_selector>());

	in.advance(rs.end());
	v.push_back(std::move(rs));
	while (true)
	{
		in.skip_whitespace();
		syntax_primitive const& sp = in.current();
		if (sp.type() != syntax_primitive_name::CommaToken) break;

		in.consume();
		in.skip_whitespace();
		compound_selector rs = compound_selector::consume(in);
		if (!rs) return compound_selector_list(it0, it0, std::vector<compound_selector>());
		in.advance(rs.end());
		v.push_back(std::move(rs));
	}

	return compound_selector_list(it0, in.begin(), std::move(v));
}

compound_selector_list::compound_selector_list(const_iterator first, const_iterator last, std::vector<compound_selector>&& value)
	: selector_grammar(first, last)
	, m_value(std::move(value))
{
}

std::vector<compound_selector> const& compound_selector_list::value() const
{
	return m_value;
}

// ------------------------------------------------------------------------------------------------
// <complex-selector-list> = <complex-selector>#
// ------------------------------------------------------------------------------------------------

complex_selector_list complex_selector_list::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	std::vector<complex_selector> v;

	complex_selector rs = complex_selector::consume(in);
	if (!rs) return complex_selector_list(it0, it0, std::vector<complex_selector>());

	in.advance(rs.end());
	v.push_back(std::move(rs));
	while (true)
	{
		in.skip_whitespace();
		syntax_primitive const& sp = in.current();
		if (sp.type() != syntax_primitive_name::CommaToken) break;

		in.consume();
		in.skip_whitespace();
		complex_selector rs = complex_selector::consume(in);
		if (!rs) return complex_selector_list(it0, it0, std::vector<complex_selector>());
		in.advance(rs.end());
		v.push_back(std::move(rs));
	}

	return complex_selector_list(it0, in.begin(), std::move(v));
}

complex_selector_list::complex_selector_list(const_iterator first, const_iterator last, std::vector<complex_selector>&& value)
	: selector_grammar(first, last)
	, m_value(std::move(value))
{
}

std::vector<complex_selector> const& complex_selector_list::value() const
{
	return m_value;
}

// ------------------------------------------------------------------------------------------------
// <selector-list> = <complex-selector-list>
// ------------------------------------------------------------------------------------------------

selector_list selector_list::consume(syntax_primitive_stream in)
{
	const_iterator it0 = in.begin();

	complex_selector_list csl = complex_selector_list::consume(in);
	if (csl) return selector_list(it0, csl.end(), std::move(csl));

	return selector_list(it0, it0, complex_selector_list());
}

selector_list::selector_list(const_iterator first, const_iterator last, complex_selector_list&& value)
	: selector_grammar(first, last)
	, m_value(std::move(value))
{
}

complex_selector_list const& selector_list::value() const
{
	return m_value;
}

