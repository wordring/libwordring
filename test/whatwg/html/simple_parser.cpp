// test/whatwg/html/parsing/parser.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/simple_node.hpp>
#include <wordring/whatwg/html/simple_parser.hpp>
#include <wordring/whatwg/html/simple_policy.hpp>

#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>
#include <wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp>

#include <wordring/tree/tree.hpp>

namespace
{
	using namespace wordring::whatwg::html::simple;

	using node_type      = basic_node<std::u32string>;
	using tree           = wordring::tree<node_type>;
	using policy_type    = tree_construction_policy<std::u32string, tree>;

	class test_parser : public parser<test_parser, std::u32string, tree>
	{

		using base_type = parser<test_parser, std::u32string, tree>;
		//using base_type::eof;
	};
}

BOOST_AUTO_TEST_SUITE(whatwg__html__simple__simple_parser__test)

BOOST_AUTO_TEST_CASE(simple__parser__construct__1)
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
