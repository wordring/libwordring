// test/whatwg/html/parsing/html_atom.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/html_atom.hpp>
#include <wordring/whatwg/html/html_defs.hpp>

#include <wordring/compatibility.hpp>

BOOST_AUTO_TEST_SUITE(parsing_html_atom_test)

/*
basic_html_atom()
*/
BOOST_AUTO_TEST_CASE(basic_html_atom_construct_1)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, ns_name> a1, a2(u8"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(a1 != a2);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_2)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u32string, tag_name> a1, a2("abbr");

	BOOST_CHECK(a1 != a2);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_3)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u32string, attribute_name> a1, a2("accent");

	BOOST_CHECK(a1 != a2);
}

/*
basic_html_atom(String1 const& s)
*/
BOOST_AUTO_TEST_CASE(basic_html_atom_construct_4)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, ns_name> a1(ns_name::HTML), a2(u8"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(static_cast<ns_name>(a1) == ns_name::HTML);
	BOOST_CHECK(static_cast<ns_name>(a2) == ns_name::HTML);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_5)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, tag_name> a1(tag_name::Abbr), a2(u8"abbr");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(static_cast<tag_name>(a1) == tag_name::Abbr);
	BOOST_CHECK(static_cast<tag_name>(a2) == tag_name::Abbr);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_6)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, attribute_name> a1(attribute_name::Accent), a2(u8"accent");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(static_cast<attribute_name>(a1) == attribute_name::Accent);
	BOOST_CHECK(static_cast<attribute_name>(a2) == attribute_name::Accent);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_7)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, ns_name> a1(u8"1"), a2(u8"1"), a3(u8"2");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(a1 != a3);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_8)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u32string, ns_name> a1(ns_name::HTML), a2("http://www.w3.org/1999/xhtml");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(static_cast<ns_name>(a1) == ns_name::HTML);
	BOOST_CHECK(static_cast<ns_name>(a2) == ns_name::HTML);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_9)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u32string, tag_name> a1(tag_name::Abbr), a2("abbr");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(static_cast<tag_name>(a1) == tag_name::Abbr);
	BOOST_CHECK(static_cast<tag_name>(a2) == tag_name::Abbr);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_10)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u32string, attribute_name> a1(attribute_name::Accent), a2("accent");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(static_cast<attribute_name>(a1) == attribute_name::Accent);
	BOOST_CHECK(static_cast<attribute_name>(a2) == attribute_name::Accent);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_11)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u32string, ns_name> a1("1"), a2("1"), a3("2");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(a1 != a3);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_12)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, ns_name> a1(ns_name::HTML), a2(U"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(static_cast<ns_name>(a1) == ns_name::HTML);
	BOOST_CHECK(static_cast<ns_name>(a2) == ns_name::HTML);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_13)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, tag_name> a1(tag_name::Abbr), a2(U"abbr");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(static_cast<tag_name>(a1) == tag_name::Abbr);
	BOOST_CHECK(static_cast<tag_name>(a2) == tag_name::Abbr);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_14)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, attribute_name> a1(attribute_name::Accent), a2(U"accent");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(static_cast<attribute_name>(a1) == attribute_name::Accent);
	BOOST_CHECK(static_cast<attribute_name>(a2) == attribute_name::Accent);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_15)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, ns_name> a1(U"1"), a2(U"1"), a3(U"2");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(a1 != a3);
}

/*
basic_html_atom(atom_type i)
*/
BOOST_AUTO_TEST_CASE(basic_html_atom_construct_16)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, ns_name> a1(ns_name::HTML), a2(ns_name::HTML), a3(ns_name::MathML);

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(a1 != a3);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_17)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, tag_name> a1(tag_name::A), a2(tag_name::A), a3(tag_name::Abbr);

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(a1 != a3);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_18)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, attribute_name> a1(attribute_name::Accent), a2(attribute_name::Accent), a3(attribute_name::Accentunder);

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(a1 != a3);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_construct_19)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, attribute_name> a1(attribute_name::Accent), a2(static_cast<attribute_name>(0));

	BOOST_CHECK(a1 != a2);
}

/*
operator string_type const&() const
*/
BOOST_AUTO_TEST_CASE(basic_html_atom_operator_string_type_1)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, ns_name> a1(ns_name::HTML), a2(ns_name::HTML), a3(ns_name::MathML);

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(a1 != a3);
}

/*
operator atom_type() const
*/
BOOST_AUTO_TEST_CASE(basic_html_atom_operator_atom_type_1)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, ns_name> a(u8"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(a == ns_name::HTML);
}

/*
inline bool operator==(basic_html_atom<String1, Atom1> const& lhs, basic_html_atom<String1, Atom1> const& rhs)
*/
BOOST_AUTO_TEST_CASE(basic_html_atom_equal_1)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, ns_name> a1(u8"http://www.w3.org/1999/xhtml"), a2(u8"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(a1 == a2);
}

BOOST_AUTO_TEST_CASE(basic_html_atom_equal_2)
{
	using namespace wordring::whatwg::html;

	basic_html_atom<std::u8string, ns_name> a1(ns_name::HTML), a2(u8"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(a1 == a2);
	BOOST_CHECK(a2 == a1);
}

BOOST_AUTO_TEST_SUITE_END()
