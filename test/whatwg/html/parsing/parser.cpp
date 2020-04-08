// test/whatwg/html/parsing/parser.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/simple_node.hpp>
#include <wordring/whatwg/html/simple_policy.hpp>
#include <wordring/whatwg/html/parsing/atom_tbl.hpp>
#include <wordring/whatwg/html/parsing/parser.hpp>
#include <wordring/whatwg/html/parsing/tokenization.hpp>
#include <wordring/whatwg/html/parsing/tree_construction_dispatcher.hpp>

#include <wordring/tree/tree.hpp>

namespace
{
	using node_type = wordring::whatwg::html::simple::basic_node<std::u32string>;
	using tree      = wordring::tree<node_type>;
	using policy    = wordring::whatwg::html::simple::tree_construction_policy<std::u32string, tree>;

	class test_parser : public wordring::whatwg::html::parsing::parser<test_parser, policy>
	{

		using base_type = wordring::whatwg::html::parsing::parser<test_parser, policy>;
		//using base_type::eof;
	public:
		bool in_html_namespace(node_pointer it) const
		{
			assert(false);
			return false;
		}
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
