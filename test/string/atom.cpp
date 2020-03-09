// test/string/atom.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/string/atom.hpp>

#include <algorithm>
#include <iterator>
#include <string>

BOOST_AUTO_TEST_SUITE(atom__test)

// ----------------------------------------------------------------------------
// basic_atom
// ----------------------------------------------------------------------------

// basic_atom()
BOOST_AUTO_TEST_CASE(basic_atom__construct__1)
{
	using namespace wordring;

	basic_atom<std::string> a;

	//BOOST_CHECK(stack.empty());
}

// ----------------------------------------------------------------------------
// basic_atom_set
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(basic_atom_set__construct__1)
{
	using namespace wordring;

	basic_atom_set<std::string> as;

	//BOOST_CHECK(stack.empty());
}

BOOST_AUTO_TEST_CASE(basic_atom_set__assign__1)
{
	using namespace wordring;

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	basic_atom_set<std::string> as;
	as.assign(v.begin(), v.end());

	int i = 0;
	//BOOST_CHECK(stack.empty());
}


BOOST_AUTO_TEST_SUITE_END()
