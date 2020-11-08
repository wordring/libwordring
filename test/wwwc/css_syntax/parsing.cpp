// test/wwwc/css_syntax/parsing.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/wwwc/css_syntax/parsing.hpp>

#include <algorithm>
#include <iterator>
#include <string>



#include <variant>
#include <vector>

struct S1;
struct S2;

static_assert(std::is_object_v<S1>);

using V1 = std::variant<S1>;

struct S1
{
	std::vector<V1> m_v;
};



BOOST_AUTO_TEST_SUITE(css_syntax_parsing_test)

BOOST_AUTO_TEST_CASE(parsing_normalize_into_token_stream_1)
{
	using namespace wordring::wwwc::css;

	std::u32string in = UR"*( p { color: red; } )*";
	std::vector<css_token> v1;
	tokenize(in.begin(), in.end(), std::back_inserter(v1));

	std::vector<std::any> v2;
	normalize_into_token_stream(v1.begin(), v1.end(), std::back_inserter(v2));

	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
