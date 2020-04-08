// test/whatwg/html/parsing/atom_defs.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/string/atom.hpp>
#include <wordring/whatwg/html/parsing/atom_defs.hpp>
#include <wordring/whatwg/html/parsing/atom_tbl.hpp>

BOOST_AUTO_TEST_SUITE(whatwg_html_parsing_atom_defs_test)

BOOST_AUTO_TEST_CASE(atom_defs_ns_name_1)
{
	using namespace wordring::whatwg::html;
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(static_cast<std::u32string>(ns_uri_tbl.at(static_cast<std::uint32_t>(ns_name::HTML))) == U"http://www.w3.org/1999/xhtml");
	BOOST_CHECK(static_cast<std::u32string>(ns_uri_tbl.at(static_cast<std::uint32_t>(ns_name::MathML))) == U"http://www.w3.org/1998/Math/MathML");
	BOOST_CHECK(static_cast<std::u32string>(ns_uri_tbl.at(static_cast<std::uint32_t>(ns_name::SVG))) == U"http://www.w3.org/2000/svg");
	BOOST_CHECK(static_cast<std::u32string>(ns_uri_tbl.at(static_cast<std::uint32_t>(ns_name::XLink))) == U"http://www.w3.org/1999/xlink");
	BOOST_CHECK(static_cast<std::u32string>(ns_uri_tbl.at(static_cast<std::uint32_t>(ns_name::XML))) == U"http://www.w3.org/XML/1998/namespace");
	BOOST_CHECK(static_cast<std::u32string>(ns_uri_tbl.at(static_cast<std::uint32_t>(ns_name::XMLNS))) == U"http://www.w3.org/2000/xmlns/");
}


BOOST_AUTO_TEST_SUITE_END()
