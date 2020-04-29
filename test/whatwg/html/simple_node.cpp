// test/whatwg/html/simple_node.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/simple_node.hpp>

BOOST_AUTO_TEST_SUITE(simple_node_test)

BOOST_AUTO_TEST_CASE(simple_node_simple_attr_equal_1)
{
	using namespace wordring::whatwg::html;

	simple_attr<std::string> ba;
	ba.local_name("href");

	BOOST_CHECK(ba == attribute_name::Href);
}

BOOST_AUTO_TEST_CASE(simple_node_simple_element_construct_1)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> be1, be2;

	BOOST_CHECK(be1.namespace_uri() == be2.namespace_uri());
}

BOOST_AUTO_TEST_CASE(simple_node_simple_element_namespace_uri_1)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> be;
	be.namespace_uri(u8"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(be.namespace_uri_id() == ns_name::HTML);
}

BOOST_AUTO_TEST_CASE(simple_node_simple_element_namespace_uri_2)
{
	using namespace wordring::whatwg::html;

	simple_element<std::u16string> be;
	be.namespace_uri(u"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(be.namespace_uri_id() == ns_name::HTML);
}

BOOST_AUTO_TEST_CASE(simple_node_simple_element_namespace_uri_3)
{
	using namespace wordring::whatwg::html;

	simple_element<std::u32string> be;
	be.namespace_uri(U"http://www.w3.org/1999/xhtml");

	BOOST_CHECK(be.namespace_uri_id() == ns_name::HTML);
}

BOOST_AUTO_TEST_CASE(simple_node_simple_element_local_name_1)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> be;
	be.local_name("head");

	BOOST_CHECK(be.local_name_id() == tag_name::Head);
}

BOOST_AUTO_TEST_CASE(simple_node_simple_element_push_back_1)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> be;
	simple_attr<std::string> ba;
	ba.local_name("href");
	be.push_back(ba);

	BOOST_CHECK(ba.local_name_id() == attribute_name::Href);
}

BOOST_AUTO_TEST_CASE(simple_node_simple_element_find_1)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> be;
	simple_attr<std::string> ba;
	ba.local_name("href");
	be.push_back(ba);

	auto it = be.find(attribute_name::Href);

	BOOST_CHECK(it->local_name_id() == attribute_name::Href);
}

/*
ノードの文字列データを参照する

String& data(simple_node<String>& node)
String const& data(simple_node<String> const& node)
*/
BOOST_AUTO_TEST_CASE(simple_node_data_1)
{
	using namespace wordring::whatwg::html;

	simple_node<std::string> sn = simple_text<std::string>("text");

	BOOST_CHECK(data(sn) == "text");
	data(sn) = "string";
	BOOST_CHECK(data(sn) == "string");
}

BOOST_AUTO_TEST_CASE(simple_node_data_2)
{
	using namespace wordring::whatwg::html;

	simple_node<std::string> sn = simple_comment<std::string>("text");

	BOOST_CHECK(data(sn) == "text");
	data(sn) = "string";
	BOOST_CHECK(data(sn) == "string");
}

BOOST_AUTO_TEST_CASE(simple_node_data_3)
{
	using namespace wordring::whatwg::html;

	simple_node<std::string> const sn = simple_text<std::string>("text");

	BOOST_CHECK(data(sn) == "text");
}

BOOST_AUTO_TEST_CASE(simple_node_data_4)
{
	using namespace wordring::whatwg::html;

	simple_node<std::string> const sn = simple_comment<std::string>("text");

	BOOST_CHECK(data(sn) == "text");
}

/*
ノードを要素と解釈して属性の開始を返す
ノードを要素と解釈して属性の終端を返す

typename simple_element<String>::iterator begin(simple_node<String>& node)
typename simple_element<String>::const_iterator begin(simple_node<String> const& node)
typename simple_element<String>::iterator end(simple_node<String>& node)
typename simple_element<String>::const_iterator end(simple_node<String> const& node)
*/
BOOST_AUTO_TEST_CASE(simple_node_begin_1)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> el;
	el.push_back({ "a1", "v1" });
	el.push_back({ attribute_name::Abbr, "v2" });

	simple_node<std::string> const sn = el;

	BOOST_CHECK(*begin(sn) == std::string("a1"));
	BOOST_CHECK(*(++begin(sn)) == attribute_name::Abbr);
}

BOOST_AUTO_TEST_CASE(simple_node_begin_2)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> el;
	el.push_back({ attribute_name::Abbr, "v1" });

	simple_node<std::string> sn = el;
	*begin(sn) = simple_attr<std::string>(attribute_name::Accent);
	BOOST_CHECK(*begin(sn) == std::string("accent"));
}

BOOST_AUTO_TEST_CASE(simple_node_end_1)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> el;
	el.push_back({ "a1", "v1" });
	el.push_back({ attribute_name::Abbr, "v2" });

	simple_node<std::string> const sn = el;

	BOOST_CHECK(*begin(sn) == std::string("a1"));
	BOOST_CHECK(*(--end(sn)) == attribute_name::Abbr);
}

/*
ノードを要素と解釈して属性を追加する

void push_back(simple_node<String> const& node, simple_attr<String> const& attr)
*/
BOOST_AUTO_TEST_CASE(simple_node_push_back_1)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> el;
	simple_node<std::string> sn = el;

	push_back(sn, { "a1", "v1" });

	BOOST_CHECK(*begin(sn) == std::string("a1"));
}

/*
ノードを要素と解釈して属性を検索する

typename simple_element<String>::const_iterator find(simple_node<String> const& node, simple_attr<String> const& attr)
*/
BOOST_AUTO_TEST_CASE(simple_node_find_1)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> el;
	simple_node<std::string> sn = el;

	push_back(sn, { "a1", "v1" });
	push_back(sn, { attribute_name::Abbr, "v2" });

	auto it = find(sn, simple_attr<std::string>("abbr", ""));

	BOOST_CHECK(*it == std::string("abbr"));
}

BOOST_AUTO_TEST_CASE(simple_node_find_2)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> el;
	simple_node<std::string> sn = el;

	push_back(sn, { "a1", "v1" });
	push_back(sn, { "a2", "v2" });

	auto it = find(sn, std::string("a2"));

	BOOST_CHECK(*it == std::string("a2"));
}

BOOST_AUTO_TEST_CASE(simple_node_find_3)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> el;
	simple_node<std::string> sn = el;

	push_back(sn, { "a1", "v1" });
	push_back(sn, { attribute_name::Abbr, "v2" });

	auto it = find(sn, std::string("abbr"));

	BOOST_CHECK(*it == attribute_name::Abbr);
}

BOOST_AUTO_TEST_CASE(simple_node_find_4)
{
	using namespace wordring::whatwg::html;

	simple_element<std::string> el;
	simple_node<std::string> sn = el;

	push_back(sn, { "a1", "v1" });
	push_back(sn, { attribute_name::Abbr, "v2" });

	auto it = find(sn, std::string("XXX"));

	BOOST_CHECK(it == end(sn));
}


BOOST_AUTO_TEST_SUITE_END()
