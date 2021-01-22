// test/wwwc/selectors/grammar_1.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/wwwc/selectors/grammar.hpp>

#include <wordring/wwwc/css_syntax/parsing.hpp>
#include <wordring/wwwc/css_syntax/token.hpp>

namespace
{
	namespace css = wordring::wwwc::css;

	using document_type_name = wordring::html::document_type_name;
	using document_mode_name = wordring::html::document_mode_name;
	
	inline std::u32string print(css::combinator const& c)
	{
		std::u32string s;
		if (c.type() == U'|') s += U"||";
		else s.push_back(c.type());
		return s;
	}

	inline std::u32string print(css::ns_prefix const& ns)
	{
		return ns.string();
	}

	inline std::u32string print(css::wq_name const& name)
	{
		std::u32string s;
		if (name.prefix())
		{
			s += name.prefix()->string();
			s.push_back(U'|');
		}
		s += name.name();
		return s;
	}

	inline std::u32string print(css::id_selector const& id)
	{
		std::u32string s(1, U'#');
		s += id.value();
		return s;
	}

	inline std::u32string print(css::class_selector const& cls)
	{
		std::u32string s(1, U'.');
		s += cls.name();
		return s;
	}

	inline std::u32string print(css::attr_matcher const& m)
	{
		std::u32string s;
		if (m.prefix() != U'\0') s.push_back(m.prefix());
		s.push_back(U'=');
		return s;
	}

	inline std::u32string print(css::attr_modifier const& m)
	{
		return std::u32string(1, m.value());
	}

	inline std::u32string print(css::pseudo_class_selector const& c)
	{
		std::u32string s(1, U':');
		switch (c.type())
		{
		case css::syntax_primitive_name::IdentToken:
			s += c.get<css::ident_token>().m_value;
			break;
		case css::syntax_primitive_name::Function:
			s += c.get<css::function>().m_name;
			s += U"(...)";
			break;
		default:
			break;
		}
		return s;
	}
	
	inline std::u32string print(css::pseudo_element_selector const& elm)
	{
		std::u32string s(U"::");
		switch (elm.type())
		{
		case css::syntax_primitive_name::IdentToken:
			s += elm.get<css::ident_token>().m_value;
			break;
		case css::syntax_primitive_name::Function:
			s += elm.get<css::function>().m_name;
			s += U"(...)";
			break;
		default:
			break;
		}
		return s;
	}
	
	inline std::u32string print(css::attribute_selector const& attr)
	{
		std::u32string s(1, '[');
		if (attr.prefix())
		{
			s += attr.prefix()->string();
			s.push_back(U'|');
		}
		s += attr.name();
		if (attr.matcher() != U'\0') s.push_back(attr.matcher());
		if(!attr.value().empty())
		{
			if (attr.matcher() != U'=') s.push_back(U'=');
			s += attr.value();
			if (attr.modifier() != U'\0') s.push_back(attr.modifier());
		}
		s.push_back(U']');
		return s;
	}

	inline std::u32string print(css::subclass_selector const& sub)
	{
		std::u32string s;
		std::visit([&](auto const& x) { s += print(x); }, sub.value());
		return s;
	}
	
	inline std::u32string print(css::type_selector const& tag)
	{
		std::u32string s;
		if (tag.value().index() == 0) s += print(std::get<css::wq_name>(tag.value()));
		else if (tag.value().index() == 1)
		{
			auto const& opt = std::get< std::optional<css::ns_prefix>>(tag.value());
			if (opt) print(*opt);
			else s.push_back(U'*');
		}
		return s;
	}

	
	inline std::u32string print(std::monostate)
	{
		std::u32string s;
		return s;
	}
	
	inline std::u32string print(css::simple_selector const& ss)
	{
		std::u32string s;
		std::visit([&](auto const& x) { s += print(x); }, ss.value());
		return s;
	}

	inline std::u32string print(css::compound_selector const& cs)
	{
		std::u32string s;
		std::vector<css::compound_selector::value_type> const& v = cs.value();
		for (auto const& c : v) std::visit([&](auto const& x) { s += print(x); }, c);
		return s;
	}

	inline std::u32string print(css::complex_selector const& cs)
	{
		std::u32string s;
		std::vector<css::complex_selector::value_type> const& v = cs.value();
		int st = 0;
		for (auto const& c : v)
		{
			switch (st)
			{
			case 0:
				if (c.index() == 0)
				{
					s += print(std::get<css::compound_selector>(c));
					st = 1;
				}
				else s += print(std::get<css::combinator>(c));
				break;
			case 1:
				if (c.index() == 0)
				{
					s.push_back(U' ');
					s += print(std::get<css::compound_selector>(c));
				}
				else
				{
					s += print(std::get<css::combinator>(c));
					st = 0;
				}
			}
		}
		return s;
	}
	
	inline std::u32string print(css::relative_selector const& rs)
	{
		std::u32string s;
		std::vector<css::complex_selector::value_type> const& v = rs.value();
	
		int st = 0;
		for (auto const& c : v)
		{
			switch (st)
			{
			case 0:
				if (c.index() == 0)
				{
					s += print(std::get<css::compound_selector>(c));
					st = 1;
				}
				else s += print(std::get<css::combinator>(c));
				break;
			case 1:
				if (c.index() == 0)
				{
					s.push_back(U' ');
					s += print(std::get<css::compound_selector>(c));
				}
				else
				{
					s += print(std::get<css::combinator>(c));
					st = 0;
				}
			}
		}
		return s;
	}

	inline std::u32string print(css::relative_selector_list const& rsl)
	{
		bool comma = false;
		std::u32string s;
		for (auto const& rs : rsl.value())
		{
			if (comma) s.push_back(U',');
			s += print(rs);
			comma = true;
		}
		return s;
	}

	inline std::u32string print(css::simple_selector_list const& ssl)
	{
		bool comma = false;
		std::u32string s;
		for (auto const& ss : ssl.value())
		{
			if (comma) s.push_back(U',');
			s += print(ss);
			comma = true;
		}
		return s;
	}

	inline std::u32string print(css::compound_selector_list const& csl)
	{
		bool comma = false;
		std::u32string s;
		for (auto const& ss : csl.value())
		{
			if (comma) s.push_back(U',');
			s += print(ss);
			comma = true;
		}
		return s;
	}

	inline std::u32string print(css::complex_selector_list const& csl)
	{
		bool comma = false;
		std::u32string s;
		for (auto const& ss : csl.value())
		{
			if (comma) s.push_back(U',');
			s += print(ss);
			comma = true;
		}
		return s;
	}

	inline std::u32string print(css::selector_list const& sl)
	{
		return print(sl.value());
	}
}

BOOST_AUTO_TEST_SUITE(css_selectors_grammar_1_test)

// ------------------------------------------------------------------------------------------------
//	<combinator> = '>' | '+' | '~' | [ '|' '|' ]
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_combinator_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U">";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = combinator::consume(in);
	BOOST_CHECK(r);
	BOOST_CHECK(r.type() == U'>');
}

BOOST_AUTO_TEST_CASE(grammar_combinator_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"||";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = combinator::consume(in);
	BOOST_CHECK(r);
	BOOST_CHECK(r.type() == U'|');
}

BOOST_AUTO_TEST_CASE(grammar_combinator_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"*";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = combinator::consume(in);
	BOOST_CHECK(!r);
	BOOST_CHECK(r.type() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_combinator_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"|";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = combinator::consume(in);
	BOOST_CHECK(!r);
	BOOST_CHECK(r.type() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_combinator_consume_5)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = combinator::consume(in);
	BOOST_CHECK(!r);
	BOOST_CHECK(r.type() == U'\0');
}

// ------------------------------------------------------------------------------------------------
//	<ns-prefix> = [ <ident-token> | '*' ]? '|'
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_ns_prefix_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"ns|";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto ns = ns_prefix::consume(in);
	BOOST_CHECK(ns);
	BOOST_CHECK(ns.string() == U"ns");
}

BOOST_AUTO_TEST_CASE(grammar_ns_prefix_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"*|";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto ns = ns_prefix::consume(in);
	BOOST_CHECK(ns);
	BOOST_CHECK(ns.string() == U"*");
}

BOOST_AUTO_TEST_CASE(grammar_ns_prefix_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"|";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto ns = ns_prefix::consume(in);
	BOOST_CHECK(ns);
	BOOST_CHECK(ns.string() == U"");
}

BOOST_AUTO_TEST_CASE(grammar_ns_prefix_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"ns";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = ns_prefix::consume(in);
	BOOST_CHECK(!r);
}

BOOST_AUTO_TEST_CASE(grammar_ns_prefix_consume_5)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = ns_prefix::consume(in);
	BOOST_CHECK(!r);
}

// ------------------------------------------------------------------------------------------------
//	<wq-name> = <ns-prefix>? <ident-token>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_wq_name_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"ns|name";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto name = wq_name::consume(in);
	BOOST_CHECK(name);
	BOOST_CHECK(name.prefix()->string() == U"ns");
	BOOST_CHECK(name.name() == U"name");
}

BOOST_AUTO_TEST_CASE(grammar_wq_name_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"*|name";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto name = wq_name::consume(in);
	BOOST_CHECK(name);
	BOOST_CHECK(name.prefix()->string() == U"*");
	BOOST_CHECK(name.name() == U"name");
}

BOOST_AUTO_TEST_CASE(grammar_wq_name_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"name";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto name = wq_name::consume(in);
	BOOST_CHECK(name);
	BOOST_CHECK(!name.prefix());
	BOOST_CHECK(name.name() == U"name");
}

BOOST_AUTO_TEST_CASE(grammar_wq_name_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto name = wq_name::consume(in);
	BOOST_CHECK(!name);
}

// ------------------------------------------------------------------------------------------------
//	<id-selector> = <hash-token>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_id_selector_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"#id";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = id_selector::consume(in);
	BOOST_CHECK(r);
	BOOST_CHECK(r.value() == U"id");
}

BOOST_AUTO_TEST_CASE(grammar_id_selector_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U".id";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = id_selector::consume(in);
	BOOST_CHECK(!r);
}

BOOST_AUTO_TEST_CASE(grammar_id_selector_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = id_selector::consume(in);
	BOOST_CHECK(!r);
}

// ------------------------------------------------------------------------------------------------
//	<class-selector> = '.' <ident-token>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_class_selector_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U".class";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = class_selector::consume(in);
	BOOST_CHECK(r);
	BOOST_CHECK(r.name() == U"class");
}

BOOST_AUTO_TEST_CASE(grammar_class_selector_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"class";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = class_selector::consume(in);
	BOOST_CHECK(!r);
}

// ------------------------------------------------------------------------------------------------
//	<attr-matcher> = [ '~' | '|' | '^' | '$' | '*' ]? '='
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_attr_matcher_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"=";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = attr_matcher::consume(in);
	BOOST_CHECK(r);
	BOOST_CHECK(r.prefix() == U'=');
}

BOOST_AUTO_TEST_CASE(grammar_attr_matcher_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"~=";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = attr_matcher::consume(in);
	BOOST_CHECK(r);
	BOOST_CHECK(r.prefix() == U'~');
}

BOOST_AUTO_TEST_CASE(grammar_attr_matcher_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"~~";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = attr_matcher::consume(in);
	BOOST_CHECK(!r);
	BOOST_CHECK(r.prefix() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_attr_matcher_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto r = attr_matcher::consume(in);
	BOOST_CHECK(!r);
	BOOST_CHECK(r.prefix() == U'\0');
}

// ------------------------------------------------------------------------------------------------
//	<attr-modifier> = i | s
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_attr_modifier_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"i";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = attr_modifier::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.value() == U'i');
}

BOOST_AUTO_TEST_CASE(grammar_attr_modifier_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"I";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = attr_modifier::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.value() == U'i');
}

BOOST_AUTO_TEST_CASE(grammar_attr_modifier_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"s";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = attr_modifier::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.value() == U's');
}

BOOST_AUTO_TEST_CASE(grammar_attr_modifier_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"S";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = attr_modifier::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.value() == U's');
}

BOOST_AUTO_TEST_CASE(grammar_attr_modifier_consume_5)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"a";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = attr_modifier::consume(in);
	BOOST_CHECK(!s);
}

BOOST_AUTO_TEST_CASE(grammar_attr_modifier_consume_6)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = attr_modifier::consume(in);
	BOOST_CHECK(!s);
}

// ------------------------------------------------------------------------------------------------
//	<pseudo-class-selector>	= ':' <ident-token> | ':' <function-token> <any-value> ')'
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_pseudo_class_selector_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U":any-link";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = pseudo_class_selector::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.get<ident_token>().m_value == U"any-link");
}

BOOST_AUTO_TEST_CASE(grammar_pseudo_class_selector_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U":is(ul, ol, .list)";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = pseudo_class_selector::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.type() == syntax_primitive_name::Function);
}

BOOST_AUTO_TEST_CASE(grammar_pseudo_class_selector_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U":before";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = pseudo_class_selector::consume(in);
	BOOST_CHECK(!s);
}

// ------------------------------------------------------------------------------------------------
// <pseudo-element-selector> = ':' <pseudo-class-selector>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_pseudo_element_selector_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"::before";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = pseudo_element_selector::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.type() == syntax_primitive_name::IdentToken);
	BOOST_CHECK(s.get<ident_token>().m_value == U"before");
}

BOOST_AUTO_TEST_CASE(grammar_pseudo_element_selector_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U":before";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = pseudo_element_selector::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.type() == syntax_primitive_name::IdentToken);
	BOOST_CHECK(s.get<ident_token>().m_value == U"before");
}

BOOST_AUTO_TEST_CASE(grammar_pseudo_element_selector_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"::part()";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = pseudo_element_selector::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.type() == syntax_primitive_name::Function);
	BOOST_CHECK(s.get<function>().m_name == U"part");
}

BOOST_AUTO_TEST_CASE(grammar_pseudo_element_selector_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U":any-link";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = pseudo_element_selector::consume(in);
	BOOST_CHECK(!s);
}

// ------------------------------------------------------------------------------------------------
//	<attribute-selector> = '[' <wq-name> ']' |
//		'[' < wq - name > <attr - matcher>[<string - token> | <ident - token>] <attr - modifier> ? ']'
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr ]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto as = attribute_selector::consume(in);
	BOOST_CHECK(as);
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'\0');
	BOOST_CHECK(as.value() == U"");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr = val ]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto as = attribute_selector::consume(in);
	BOOST_CHECK(as);
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'=');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr ~= val ]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto as = attribute_selector::consume(in);
	BOOST_CHECK(as);
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'~');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr |=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto as = attribute_selector::consume(in);
	BOOST_CHECK(as);
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'|');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_consume_5)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr^=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto as = attribute_selector::consume(in);
	BOOST_CHECK(as);
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'^');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_consume_6)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr$=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto as = attribute_selector::consume(in);
	BOOST_CHECK(as);
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'$');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_consume_7)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr*=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto as = attribute_selector::consume(in);
	BOOST_CHECK(as);
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'*');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_consume_8)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr=val i]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto as = attribute_selector::consume(in);
	BOOST_CHECK(as);
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'=');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'i');
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_consume_9)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr=val I]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto as = attribute_selector::consume(in);
	BOOST_CHECK(as);
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'=');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'i');
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_consume_10)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr=val s]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto as = attribute_selector::consume(in);
	BOOST_CHECK(as);
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'=');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U's');
}

BOOST_AUTO_TEST_CASE(grammar_attribute_selector_consume_11)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr=val S ]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto as = attribute_selector::consume(in);
	BOOST_CHECK(as);
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'=');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U's');
}

// ------------------------------------------------------------------------------------------------
// <subclass-selector> = <id-selector> | <class-selector> | <attribute-selector> | <pseudo-class-selector>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"#id";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = subclass_selector::consume(in);
	BOOST_CHECK(s);

	id_selector id = std::get<id_selector>(s.value());
	BOOST_CHECK(id.value() == U"id");
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U".cls";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = subclass_selector::consume(in);
	BOOST_CHECK(s);

	class_selector cls = std::get<class_selector>(s.value());
	BOOST_CHECK(cls.name() == U"cls");
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr ]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = subclass_selector::consume(in);
	BOOST_CHECK(s);

	attribute_selector as = std::get<attribute_selector>(s.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'\0');
	BOOST_CHECK(as.value() == U"");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr = val ]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto ss = subclass_selector::consume(in);
	BOOST_CHECK(ss);

	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'=');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_5)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr ~= val ]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto ss = subclass_selector::consume(in);
	BOOST_CHECK(ss);

	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'~');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_6)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr |=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto ss = subclass_selector::consume(in);
	BOOST_CHECK(ss);

	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'|');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_7)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr^=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto ss = subclass_selector::consume(in);
	BOOST_CHECK(ss);

	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'^');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_8)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr$=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto ss = subclass_selector::consume(in);
	BOOST_CHECK(ss);

	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'$');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_9)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr*=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto ss = subclass_selector::consume(in);
	BOOST_CHECK(ss);

	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'*');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_10)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr=val i]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto ss = subclass_selector::consume(in);
	BOOST_CHECK(ss);

	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'=');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'i');
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_11)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U":any-link";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = subclass_selector::consume(in);
	BOOST_CHECK(s);

	pseudo_class_selector pcls = std::get<pseudo_class_selector>(s.value());

	BOOST_CHECK(pcls.get<ident_token>().m_value == U"any-link");
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_12)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U":is(ul, ol, .list)";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = subclass_selector::consume(in);
	BOOST_CHECK(s);

	pseudo_class_selector pcls = std::get<pseudo_class_selector>(s.value());

	BOOST_CHECK(pcls.type() == syntax_primitive_name::Function);
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_13)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U":before";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = subclass_selector::consume(in);
	BOOST_CHECK(!s);
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_14)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"*";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = subclass_selector::consume(in);
	BOOST_CHECK(!s);
}

BOOST_AUTO_TEST_CASE(grammar_subclass_selector_consume_15)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = subclass_selector::consume(in);
	BOOST_CHECK(!s);
}

// ------------------------------------------------------------------------------------------------
// <type-selector> = <wq-name> | <ns-prefix>? '*'
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_type_selector_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"h1";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = type_selector::consume(in);
	BOOST_CHECK(s);

	wq_name name = std::get<wq_name>(s.value());
	BOOST_CHECK(name.name() == U"h1");
}

BOOST_AUTO_TEST_CASE(grammar_type_selector_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"*";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = type_selector::consume(in);
	BOOST_CHECK(s);

	std::optional<ns_prefix> ns = std::get<std::optional<ns_prefix>>(s.value());
	BOOST_CHECK(!ns);
}

BOOST_AUTO_TEST_CASE(grammar_type_selector_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"foo|h1";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = type_selector::consume(in);
	BOOST_CHECK(s);

	wq_name name = std::get<wq_name>(s.value());
	BOOST_CHECK(name.prefix()->string() == U"foo");
	BOOST_CHECK(name.name() == U"h1");
}

BOOST_AUTO_TEST_CASE(grammar_type_selector_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"foo|*";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = type_selector::consume(in);
	BOOST_CHECK(s);

	std::optional<ns_prefix> ns = std::get<std::optional<ns_prefix>>(s.value());
	BOOST_CHECK(ns);
	BOOST_CHECK(ns->string() == U"foo");
}

BOOST_AUTO_TEST_CASE(grammar_type_selector_consume_5)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"|h1";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = type_selector::consume(in);
	BOOST_CHECK(s);

	wq_name name = std::get<wq_name>(s.value());
	BOOST_CHECK(name.prefix()->string() == U"");
	BOOST_CHECK(name.name() == U"h1");
}

BOOST_AUTO_TEST_CASE(grammar_type_selector_consume_6)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"*|h1";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = type_selector::consume(in);
	BOOST_CHECK(s);

	wq_name name = std::get<wq_name>(s.value());
	BOOST_CHECK(name.prefix()->string() == U"*");
	BOOST_CHECK(name.name() == U"h1");
}

BOOST_AUTO_TEST_CASE(grammar_type_selector_consume_7)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U".cls";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = type_selector::consume(in);
	BOOST_CHECK(!s);
}

BOOST_AUTO_TEST_CASE(grammar_type_selector_consume_8)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = type_selector::consume(in);
	BOOST_CHECK(!s);
}

// ------------------------------------------------------------------------------------------------
// <simple-selector> = <type-selector> | <subclass-selector>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"h1";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	type_selector t = std::get<type_selector>(s.value());
	wq_name name = std::get<wq_name>(t.value());
	BOOST_CHECK(name.name() == U"h1");
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"*";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	type_selector t = std::get<type_selector>(s.value());
	std::optional<ns_prefix> ns = std::get<std::optional<ns_prefix>>(t.value());
	BOOST_CHECK(!ns);
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"foo|h1";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	type_selector t = std::get<type_selector>(s.value());
	wq_name name = std::get<wq_name>(t.value());
	BOOST_CHECK(name.prefix()->string() == U"foo");
	BOOST_CHECK(name.name() == U"h1");
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"foo|*";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	type_selector t = std::get<type_selector>(s.value());
	std::optional<ns_prefix> ns = std::get<std::optional<ns_prefix>>(t.value());
	BOOST_CHECK(ns);
	BOOST_CHECK(ns->string() == U"foo");
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_5)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"|h1";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	type_selector t = std::get<type_selector>(s.value());
	wq_name name = std::get<wq_name>(t.value());
	BOOST_CHECK(name.prefix()->string() == U"");
	BOOST_CHECK(name.name() == U"h1");
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_6)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"*|h1";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	type_selector t = std::get<type_selector>(s.value());
	wq_name name = std::get<wq_name>(t.value());
	BOOST_CHECK(name.prefix()->string() == U"*");
	BOOST_CHECK(name.name() == U"h1");
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_7)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"#id";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector t = std::get<subclass_selector>(s.value());
	id_selector id = std::get<id_selector>(t.value());
	BOOST_CHECK(id.value() == U"id");
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_8)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U".cls";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector t = std::get<subclass_selector>(s.value());
	class_selector cls = std::get<class_selector>(t.value());
	BOOST_CHECK(cls.name() == U"cls");
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_9)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr ]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector ss = std::get<subclass_selector>(s.value());
	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'\0');
	BOOST_CHECK(as.value() == U"");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_10)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr = val ]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector ss = std::get<subclass_selector>(s.value());
	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'=');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_11)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr ~= val ]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector ss = std::get<subclass_selector>(s.value());
	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'~');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_12)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr |=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector ss = std::get<subclass_selector>(s.value());
	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'|');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_13)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr^=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector ss = std::get<subclass_selector>(s.value());
	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'^');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_14)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr$=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector ss = std::get<subclass_selector>(s.value());
	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'$');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_15)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr*=val]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector ss = std::get<subclass_selector>(s.value());
	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'*');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'\0');
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_16)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[attr=val i]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector ss = std::get<subclass_selector>(s.value());
	attribute_selector as = std::get<attribute_selector>(ss.value());
	BOOST_CHECK(!as.prefix());
	BOOST_CHECK(as.name() == U"attr");
	BOOST_CHECK(as.matcher() == U'=');
	BOOST_CHECK(as.value() == U"val");
	BOOST_CHECK(as.modifier() == U'i');
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_17)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U":any-link";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector t = std::get<subclass_selector>(s.value());
	pseudo_class_selector pcls = std::get<pseudo_class_selector>(t.value());

	BOOST_CHECK(pcls.get<ident_token>().m_value == U"any-link");
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_consume_18)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U":is(ul, ol, .list)";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = simple_selector::consume(in);
	BOOST_CHECK(s);

	subclass_selector t = std::get<subclass_selector>(s.value());
	pseudo_class_selector pcls = std::get<pseudo_class_selector>(t.value());

	BOOST_CHECK(pcls.type() == syntax_primitive_name::Function);
}

// ------------------------------------------------------------------------------------------------
// <compound-selector> = [ <type-selector>? <subclass-selector>*
//							[ <pseudo-element-selector> <pseudo-class-selector>* ]* ]!
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_compound_selector_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = compound_selector::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.value().size() == 1);
	type_selector t = std::get<type_selector>(s.value().front());
	wq_name name = std::get<wq_name>(t.value());
	BOOST_CHECK(name.name() == U"P");
}

BOOST_AUTO_TEST_CASE(grammar_compound_selector_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"#id";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = compound_selector::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.value().size() == 1);
	subclass_selector t = std::get<subclass_selector>(s.value().front());
	id_selector id = std::get<id_selector>(t.value());
	BOOST_CHECK(id.value() == U"id");
}

BOOST_AUTO_TEST_CASE(grammar_compound_selector_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"::before";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = compound_selector::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.value().size() == 1);
	pseudo_element_selector p = std::get<pseudo_element_selector>(s.value().front());
	BOOST_CHECK(p.get<ident_token>().m_value == U"before");
}

BOOST_AUTO_TEST_CASE(grammar_compound_selector_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"::before:hover:focus";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = compound_selector::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.value().size() == 3);
	pseudo_element_selector p = std::get<pseudo_element_selector>(s.value().front());
	BOOST_CHECK(p.get<ident_token>().m_value == U"before");
}

// ------------------------------------------------------------------------------------------------
// <complex-selector> = <compound-selector> [ <combinator>? <compound-selector> ]*
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_complex_selector_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P A";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = complex_selector::consume(in);
	BOOST_CHECK(s);
	BOOST_CHECK(s.value().size() == 3);

	std::u32string sz = print(s);
	BOOST_CHECK(sz == U"P A");
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr=\"val\" ] > A";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = complex_selector::consume(in);
	BOOST_CHECK(s);

	std::u32string sz = print(s);
	BOOST_CHECK(sz == U"[attr=val]>A");
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"[ attr^=val ] > A";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto s = complex_selector::consume(in);
	BOOST_CHECK(s);

	std::u32string sz = print(s);
	BOOST_CHECK(sz == U"[attr^=val]>A");
}

// ------------------------------------------------------------------------------------------------
// <relative-selector> = <combinator>? <complex-selector>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_relative_selector_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P  A";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rs = relative_selector::consume(in);
	BOOST_CHECK(rs);
	BOOST_CHECK(rs.value().size() == 3);

	std::u32string sz = print(rs);
	BOOST_CHECK(sz == U"P A");
}

BOOST_AUTO_TEST_CASE(grammar_relative_selector_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U">P  A";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rs = relative_selector::consume(in);
	BOOST_CHECK(rs);
	BOOST_CHECK(rs.value().size() == 4);

	std::u32string sz = print(rs);
	BOOST_CHECK(sz == U">P A");
}

BOOST_AUTO_TEST_CASE(grammar_relative_selector_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"()";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rs = relative_selector::consume(in);
	BOOST_CHECK(!rs);
}

BOOST_AUTO_TEST_CASE(grammar_relative_selector_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rs = relative_selector::consume(in);
	BOOST_CHECK(!rs);
}

// ------------------------------------------------------------------------------------------------
// <relative-selector-list> = <relative-selector>#
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_relative_selector_list_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P  A,[attr]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = relative_selector_list::consume(in);
	BOOST_CHECK(rsl);
	BOOST_CHECK(rsl.value().size() == 2);

	std::u32string sz = print(rsl);
	BOOST_CHECK(sz == U"P A,[attr]");
}

BOOST_AUTO_TEST_CASE(grammar_relative_selector_list_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P  A , [ attr ] , A:before";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = relative_selector_list::consume(in);
	BOOST_CHECK(rsl);
	BOOST_CHECK(rsl.value().size() == 3);

	std::u32string sz = print(rsl);
	BOOST_CHECK(sz == U"P A,[attr],A::before");
}

BOOST_AUTO_TEST_CASE(grammar_relative_selector_list_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P  A , ()";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = relative_selector_list::consume(in);
	BOOST_CHECK(!rsl);
}

BOOST_AUTO_TEST_CASE(grammar_relative_selector_list_consume_4)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = relative_selector_list::consume(in);
	BOOST_CHECK(!rsl);
}

// ------------------------------------------------------------------------------------------------
// <simple-selector-list> = <simple-selector>#
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_simple_selector_list_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P , #id , .cls , [attr]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = simple_selector_list::consume(in);
	BOOST_CHECK(rsl);
	BOOST_CHECK(rsl.value().size() == 4);

	std::u32string sz = print(rsl);
	BOOST_CHECK(sz == U"P,#id,.cls,[attr]");
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_list_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P , #id , .cls , [attr] ,()";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = simple_selector_list::consume(in);
	BOOST_CHECK(!rsl);
}

BOOST_AUTO_TEST_CASE(grammar_simple_selector_list_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = simple_selector_list::consume(in);
	BOOST_CHECK(!rsl);
}

// ------------------------------------------------------------------------------------------------
// <compound-selector-list> = <compound-selector>#
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_compound_selector_list_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P:before , #id , .cls , [attr]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = compound_selector_list::consume(in);
	BOOST_CHECK(rsl);
	BOOST_CHECK(rsl.value().size() == 4);

	std::u32string sz = print(rsl);
	BOOST_CHECK(sz == U"P::before,#id,.cls,[attr]");
}

BOOST_AUTO_TEST_CASE(grammar_compound_selector_list_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P:before , #id , .cls , [attr],()";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = compound_selector_list::consume(in);
	BOOST_CHECK(!rsl);
}

BOOST_AUTO_TEST_CASE(grammar_compound_selector_list_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = compound_selector_list::consume(in);
	BOOST_CHECK(!rsl);
}

// ------------------------------------------------------------------------------------------------
// <complex-selector-list> = <complex-selector>#
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_complex_selector_list_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P:before A > B , #id > A , .cls , [attr]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = complex_selector_list::consume(in);
	BOOST_CHECK(rsl);
	BOOST_CHECK(rsl.value().size() == 4);

	std::u32string sz = print(rsl);
	BOOST_CHECK(sz == U"P::before A>B,#id>A,.cls,[attr]");
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_list_consume_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P:before A > B , #id > A , .cls , [attr],()";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = complex_selector_list::consume(in);
	BOOST_CHECK(!rsl);
}

BOOST_AUTO_TEST_CASE(grammar_complex_selector_list_consume_3)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = complex_selector_list::consume(in);
	BOOST_CHECK(!rsl);
}

// ------------------------------------------------------------------------------------------------
// <selector-list> = <complex-selector-list>
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_selector_list_consume_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"P:before A > B , #id > A , .cls , [attr]";
	std::optional<std::vector<syntax_primitive>> v = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(v);
	syntax_primitive_stream in(*v);

	auto rsl = selector_list::consume(in);
	BOOST_CHECK(rsl);

	std::u32string sz = print(rsl);
	BOOST_CHECK(sz == U"P::before A>B,#id>A,.cls,[attr]");
}















// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(grammar_selector_parser_pseudo_class_1)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U":dir(ltr)";
	std::optional<std::vector<syntax_primitive>> ret = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(ret);

	auto const& v = *ret;
	BOOST_CHECK(v.size() == 2);
	BOOST_CHECK(v[0].type() == syntax_primitive_name::ColonToken);
	BOOST_CHECK(v[1].type() == syntax_primitive_name::Function);
}

BOOST_AUTO_TEST_CASE(grammar_selector_parser_pseudo_class_2)
{
	using namespace wordring::wwwc::css;

	std::u32string css = U"::after";
	std::optional<std::vector<syntax_primitive>> ret = parse_grammar(std::move(css)
		, [](std::vector<syntax_primitive> const&)->bool { return true; });
	BOOST_CHECK(ret);

	auto const& v = *ret;
	BOOST_CHECK(v.size() == 3);
	BOOST_CHECK(v[0].type() == syntax_primitive_name::ColonToken);
	BOOST_CHECK(v[1].type() == syntax_primitive_name::ColonToken);
	BOOST_CHECK(v[2].type() == syntax_primitive_name::IdentToken);
}

// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
