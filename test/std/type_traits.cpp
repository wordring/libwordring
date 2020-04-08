
#include <boost/test/unit_test.hpp>

#include <string>
#include <type_traits>

BOOST_AUTO_TEST_SUITE(type_traits_test)

BOOST_AUTO_TEST_CASE(type_traits_remove_reference_1)
{
	std::string s1;
	std::string const& s2 = s1;

	BOOST_CHECK(!(std::is_same_v<std::remove_reference_t<decltype(s2)>, decltype(s1)>));

	BOOST_CHECK(!(std::is_same_v<std::remove_const_t<decltype(s2)>, decltype(s1)>));

	BOOST_CHECK((std::is_same_v<std::remove_const_t<std::remove_reference_t<decltype(s2)>>, decltype(s1)>));

	BOOST_CHECK((std::is_same_v<std::remove_cv_t<std::remove_reference_t<decltype(s2)>>, decltype(s1)>));

	BOOST_CHECK(!(std::is_same_v<std::remove_reference_t<std::remove_const_t<decltype(s2)>>, decltype(s1)>));
}

BOOST_AUTO_TEST_SUITE_END()
