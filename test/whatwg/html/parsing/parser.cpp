// test/whatwg/html/parsing/parser.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/parser.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>
#include <wordring/whatwg/html/parsing/tree_construction.hpp>

namespace
{
	class test_parser : public wordring::whatwg::html::parsing::parser<test_parser>
	{

		using base_type = wordring::whatwg::html::parsing::parser<test_parser>;
		//using base_type::eof;
	};
}

BOOST_AUTO_TEST_SUITE(whatwg__html__parsing__parser__test)

BOOST_AUTO_TEST_CASE(parser__1)
{
	using namespace wordring::whatwg::html;
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(true);

	test_parser p;
	p.emit_code_point(U'A');
	p.eof();

	p.emit_token(1);

	//attribute_set.contains(std::u32string());
}

BOOST_AUTO_TEST_SUITE_END()
