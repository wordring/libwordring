// test/whatwg/html/parsing/atom_defs.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/string/atom.hpp>
#include <wordring/whatwg/html/parsing/atom_defs.hpp>
#include <wordring/whatwg/html/parsing/atom_tbl.hpp>

BOOST_AUTO_TEST_SUITE(whatwg__html__parsing__atom_defs__test)

BOOST_AUTO_TEST_CASE(atom_defs__namespaces__1)
{
	using namespace wordring::whatwg::html;
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(static_cast<std::u32string>(atom_tbl.at(static_cast<std::uint32_t>(namespaces::HTML))) == U"http://www.w3.org/1999/xhtml");
	BOOST_CHECK(static_cast<std::u32string>(atom_tbl.at(static_cast<std::uint32_t>(namespaces::MathML))) == U"http://www.w3.org/1998/Math/MathML");
	BOOST_CHECK(static_cast<std::u32string>(atom_tbl.at(static_cast<std::uint32_t>(namespaces::SVG))) == U"http://www.w3.org/2000/svg");
	BOOST_CHECK(static_cast<std::u32string>(atom_tbl.at(static_cast<std::uint32_t>(namespaces::XLink))) == U"http://www.w3.org/1999/xlink");
	BOOST_CHECK(static_cast<std::u32string>(atom_tbl.at(static_cast<std::uint32_t>(namespaces::XML))) == U"http://www.w3.org/XML/1998/namespace");
	BOOST_CHECK(static_cast<std::u32string>(atom_tbl.at(static_cast<std::uint32_t>(namespaces::XMLNS))) == U"http://www.w3.org/2000/xmlns/");
}

BOOST_AUTO_TEST_CASE(atom_defs__namespaces__2)
{
	using namespace wordring::whatwg::html;
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(static_cast<std::uint32_t>(atom_tbl.at(U"http://www.w3.org/1999/xhtml")) == static_cast<std::uint32_t>(namespaces::HTML));
	BOOST_CHECK(static_cast<std::uint32_t>(atom_tbl.at(U"http://www.w3.org/1998/Math/MathML")) == static_cast<std::uint32_t>(namespaces::MathML));
	BOOST_CHECK(static_cast<std::uint32_t>(atom_tbl.at(U"http://www.w3.org/2000/svg")) == static_cast<std::uint32_t>(namespaces::SVG));
	BOOST_CHECK(static_cast<std::uint32_t>(atom_tbl.at(U"http://www.w3.org/1999/xlink")) == static_cast<std::uint32_t>(namespaces::XLink));
	BOOST_CHECK(static_cast<std::uint32_t>(atom_tbl.at(U"http://www.w3.org/XML/1998/namespace")) == static_cast<std::uint32_t>(namespaces::XML));
	BOOST_CHECK(static_cast<std::uint32_t>(atom_tbl.at(U"http://www.w3.org/2000/xmlns/")) == static_cast<std::uint32_t>(namespaces::XMLNS));
}

BOOST_AUTO_TEST_SUITE_END()
