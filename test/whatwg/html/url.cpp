// test/whatwg/html/url.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/html/url.hpp>

#include <wordring/whatwg/html/html_defs.hpp>

#include <wordring/compatibility.hpp>

BOOST_AUTO_TEST_SUITE(html_url_test)


/*! @brief ラベルからエンコーディングを取得する

@sa https://encoding.spec.whatwg.org/#concept-encoding-get
@sa https://triple-underscore.github.io/Encoding-ja.html#concept-encoding-get
*/
BOOST_AUTO_TEST_CASE(html_url_extract_character_encoding_from_meta_element_1)
{
	using namespace wordring::whatwg::html;

	encoding_name en = extract_character_encoding_from_meta_element(UR"(charset="UTF-8")");

	BOOST_CHECK(en == encoding_name::UTF_8);
}

BOOST_AUTO_TEST_CASE(html_url_extract_character_encoding_from_meta_element_2)
{
	using namespace wordring::whatwg::html;

	encoding_name en = extract_character_encoding_from_meta_element(UR"(charset='UTF-8')");

	BOOST_CHECK(en == encoding_name::UTF_8);
}

BOOST_AUTO_TEST_CASE(html_url_extract_character_encoding_from_meta_element_3)
{
	using namespace wordring::whatwg::html;

	encoding_name en = extract_character_encoding_from_meta_element(UR"(text/html; charset=UTF-8)");

	BOOST_CHECK(en == encoding_name::UTF_8);
}

BOOST_AUTO_TEST_CASE(html_url_extract_character_encoding_from_meta_element_4)
{
	using namespace wordring::whatwg::html;

	encoding_name en = extract_character_encoding_from_meta_element(UR"( charset=UTF-8; content="text/html;")");

	BOOST_CHECK(en == encoding_name::UTF_8);
}

BOOST_AUTO_TEST_CASE(html_url_extract_character_encoding_from_meta_element_5)
{
	using namespace wordring::whatwg::html;

	encoding_name en = extract_character_encoding_from_meta_element(UR"( charset=UTF-8 ")");

	BOOST_CHECK(en == encoding_name::UTF_8);
}

BOOST_AUTO_TEST_SUITE_END()
