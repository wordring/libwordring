// test/string/matcher.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/string/matcher.hpp>

#include <string>

namespace
{
	struct test_matcher : wordring::string_matcher<std::string>
	{
		using base_type = wordring::string_matcher<std::string>;

		using base_type::match_result;

		using base_type::m_c;

		using base_type::string_matcher;
		using base_type::push_back;
	};
}

BOOST_AUTO_TEST_SUITE(string_matcher_test)

BOOST_AUTO_TEST_CASE(string_matcher_construct_1)
{
	test_matcher tm({ "ABB", "ABC" });
	BOOST_CHECK(tm.m_c.size() == 2);
}

BOOST_AUTO_TEST_CASE(string_matcher_data_1)
{
	test_matcher tm({ "ABC", "ABB" });

	BOOST_CHECK(tm.push_back('A') == test_matcher::match_result::partial);
	BOOST_CHECK(tm.push_back('B') == test_matcher::match_result::partial);
	BOOST_CHECK(tm.push_back('C') == test_matcher::match_result::succeed);
	BOOST_CHECK(tm.data() == "ABC");
}

BOOST_AUTO_TEST_CASE(string_matcher_size_1)
{
	test_matcher tm({ "ABC", "ABB" });

	BOOST_CHECK(tm.push_back('A') == test_matcher::match_result::partial);
	BOOST_CHECK(tm.push_back('B') == test_matcher::match_result::partial);
	BOOST_CHECK(tm.push_back('C') == test_matcher::match_result::succeed);
	BOOST_CHECK(tm.size() == 3);
}

BOOST_AUTO_TEST_CASE(string_matcher_clear_1)
{
	test_matcher tm({ "A" });

	BOOST_CHECK(tm.push_back('A') == test_matcher::match_result::succeed);
	tm.clear();
	BOOST_CHECK(tm.push_back('A') == test_matcher::match_result::succeed);
}

BOOST_AUTO_TEST_CASE(string_matcher_push_back_1)
{
	test_matcher tm({ "ABC", "ABB" });

	BOOST_CHECK(tm.push_back('A') == test_matcher::match_result::partial);
	BOOST_CHECK(tm.push_back('B') == test_matcher::match_result::partial);
	BOOST_CHECK(tm.push_back('C') == test_matcher::match_result::succeed);
	BOOST_CHECK(tm.data() == "ABC");
}

BOOST_AUTO_TEST_CASE(string_matcher_push_back_2)
{
	test_matcher tm({});

	BOOST_CHECK(tm.push_back('A') == test_matcher::match_result::failed);
}

BOOST_AUTO_TEST_CASE(string_matcher_push_back_3)
{
	test_matcher tm({ "ABC", "ABC" });

	BOOST_CHECK(tm.push_back('A') == test_matcher::match_result::partial);
	BOOST_CHECK(tm.push_back('B') == test_matcher::match_result::partial);
	BOOST_CHECK(tm.push_back('C') == test_matcher::match_result::succeed);
	BOOST_CHECK(tm.data() == "ABC");
}

BOOST_AUTO_TEST_CASE(string_matcher_push_back_4)
{
	test_matcher tm({ "BC", "CBC" });

	BOOST_CHECK(tm.push_back('A') == test_matcher::match_result::failed);
}

BOOST_AUTO_TEST_CASE(string_matcher_push_back_5)
{
	test_matcher tm({ "" });

	BOOST_CHECK(tm.push_back('A') == test_matcher::match_result::failed);
}

BOOST_AUTO_TEST_CASE(string_matcher_push_back_6)
{
	test_matcher tm({ "A", "AB" });

	BOOST_CHECK(tm.push_back('A') == test_matcher::match_result::succeed);
}

BOOST_AUTO_TEST_CASE(string_matcher__1)
{

}

BOOST_AUTO_TEST_SUITE_END()
