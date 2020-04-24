// test/whatwg/html/simple_node.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/simple_node.hpp>

BOOST_AUTO_TEST_SUITE(simple_node_test)

BOOST_AUTO_TEST_CASE(simple_node_basic_attr_equal_1)
{
	using namespace wordring::whatwg::html::simple;

	basic_attr<std::string> ba;
	ba.local_name("href");

	BOOST_CHECK(ba == attribute_name::Href);
}

BOOST_AUTO_TEST_CASE(simple_node_basic_element_construct_1)
{
	using namespace wordring::whatwg::html::simple;

	basic_element<std::string> be1, be2;

	BOOST_CHECK(be1.namespace_uri() == be2.namespace_uri());
}

BOOST_AUTO_TEST_CASE(simple_node_basic_element_namespace_uri_1)
{
	using namespace wordring::whatwg::html::simple;

	basic_element<std::string> be;
	be.namespace_uri(u8"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(be.namespace_uri_id() == ns_name::HTML);
}

BOOST_AUTO_TEST_CASE(simple_node_basic_element_namespace_uri_2)
{
	using namespace wordring::whatwg::html::simple;

	basic_element<std::u16string> be;
	be.namespace_uri(u"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(be.namespace_uri_id() == ns_name::HTML);
}

BOOST_AUTO_TEST_CASE(simple_node_basic_element_namespace_uri_3)
{
	using namespace wordring::whatwg::html::simple;

	basic_element<std::u32string> be;
	be.namespace_uri(U"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(be.namespace_uri_id() == ns_name::HTML);
}

BOOST_AUTO_TEST_CASE(simple_node_basic_element_local_name_1)
{
	using namespace wordring::whatwg::html::simple;

	basic_element<std::string> be;
	be.local_name("head");

	BOOST_CHECK(be.local_name_id() == tag_name::Head);
}

BOOST_AUTO_TEST_CASE(simple_node_basic_element_push_back_1)
{
	using namespace wordring::whatwg::html::simple;

	basic_element<std::string> be;
	basic_attr<std::string> ba;
	ba.local_name("href");
	be.push_back(ba);

	BOOST_CHECK(ba.local_name_id() == attribute_name::Href);
}

BOOST_AUTO_TEST_CASE(simple_node_basic_element_find_1)
{
	using namespace wordring::whatwg::html::simple;

	basic_element<std::string> be;
	basic_attr<std::string> ba;
	ba.local_name("href");
	be.push_back(ba);

	auto it = be.find(attribute_name::Href);

	BOOST_CHECK(it->local_name_id() == attribute_name::Href);
}

BOOST_AUTO_TEST_SUITE_END()
