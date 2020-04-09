// test/whatwg/html/simple_node.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/simple_node.hpp>

BOOST_AUTO_TEST_SUITE(simple_node_test)


BOOST_AUTO_TEST_CASE(simple_node_construct_1)
{
	using namespace wordring::whatwg::html::simple;

	basic_element<std::string> be1, be2;

	BOOST_CHECK(be1.namespace_uri() == be2.namespace_uri());
}

BOOST_AUTO_TEST_CASE(simple_node_namespace_uri_1)
{
	using namespace wordring::whatwg::html::simple;

	basic_element<std::string> be;
	be.namespace_uri(u8"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(be.namespace_uri_id() == ns_name::HTML);
}

BOOST_AUTO_TEST_CASE(simple_node_namespace_uri_2)
{
	using namespace wordring::whatwg::html::simple;

	basic_element<std::u16string> be;
	be.namespace_uri(u"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(be.namespace_uri_id() == ns_name::HTML);
}

BOOST_AUTO_TEST_CASE(simple_node_namespace_uri_3)
{
	using namespace wordring::whatwg::html::simple;

	basic_element<std::u32string> be;
	be.namespace_uri(U"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(be.namespace_uri_id() == ns_name::HTML);
}

BOOST_AUTO_TEST_SUITE_END()
