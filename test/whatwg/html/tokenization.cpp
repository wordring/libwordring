// test/whatwg/html/tokenization.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/parsing/tokenization.hpp>
#include <wordring/whatwg/html/parsing/atom_tbl.hpp>

namespace
{
	struct test_tokenizer : wordring::whatwg::html::parsing::tokenizer_base<test_tokenizer>
	{
		using base_type = wordring::whatwg::html::parsing::tokenizer_base<test_tokenizer>;
		using base_type::on_data_state;
	};

}

BOOST_AUTO_TEST_SUITE(whatwg_html__tokenization__test)

BOOST_AUTO_TEST_CASE(tokenizer__1)
{
	using namespace wordring::whatwg::html;
	using namespace wordring::whatwg::html::parsing;

	BOOST_CHECK(true);

	test_tokenizer tt;
	//attribute_set.contains(std::u32string());
}

BOOST_AUTO_TEST_SUITE_END()
