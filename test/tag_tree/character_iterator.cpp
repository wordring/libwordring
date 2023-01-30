// test/tag_tree/character_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tag_tree/character_iterator.hpp>

#include <wordring/css/selector.hpp>
#include <wordring/html/simple_html.hpp>
#include <wordring/html/simple_traits.hpp>

#include <iterator>
#include <regex>
#include <string_view>

#include <iostream>


BOOST_AUTO_TEST_SUITE(character_iterator_test)

BOOST_AUTO_TEST_CASE(character_iterator_construct_1)
{
	using namespace wordring::html;

	std::string_view sv = "<p>1</p><p>2</p><p>3</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());

	BOOST_CHECK(*it == u8'1');
}

BOOST_AUTO_TEST_CASE(character_iterator_equal_1)
{
	using namespace wordring::html;

	std::string_view sv = "";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());
	BOOST_CHECK(it == it.end());
}

BOOST_AUTO_TEST_CASE(character_iterator_reference_1)
{
	using namespace wordring::html;

	std::string_view sv = "<p>1</p><p>2</p><p>3</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());

	BOOST_CHECK(*it == u8'1');
}

BOOST_AUTO_TEST_CASE(character_iterator_increment_1)
{
	using namespace wordring::html;

	std::string_view sv = "<p>1</p><p>2</p><p>3</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());

	BOOST_CHECK(*++it == u8'2');
}

BOOST_AUTO_TEST_CASE(character_iterator_increment_2)
{
	using namespace wordring::html;

	std::string_view sv = "<p>1</p><p>2</p><p>3</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());

	BOOST_CHECK(*it++ == u8'1');
	BOOST_CHECK(*it++ == u8'2');
	BOOST_CHECK(*it == u8'3');
}

BOOST_AUTO_TEST_CASE(character_iterator_decrement_1)
{
	using namespace wordring::html;

	std::string_view sv = "<p>1</p><p>2</p><p>3</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());

	BOOST_CHECK(*--(it.end()) == u8'3');
}

BOOST_AUTO_TEST_CASE(character_iterator_decrement_2)
{
	using namespace wordring::html;

	std::string_view sv = "<p>1</p><p>2</p><p>3</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());
	it = --it.end();

	BOOST_CHECK(*it-- == u8'3');
	BOOST_CHECK(*it-- == u8'2');
	BOOST_CHECK(*it-- == u8'1');
	BOOST_CHECK(it == it.end());
}

BOOST_AUTO_TEST_CASE(character_iterator_decrement_3)
{
	using namespace wordring::html;

	std::string_view sv = "123";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());
	it = --it.end();

	BOOST_CHECK(*it-- == u8'3');
	BOOST_CHECK(*it-- == u8'2');
	BOOST_CHECK(*it-- == u8'1');
	BOOST_CHECK(it == it.end());
}

/*
* 正規表現をユニコード対応にする提案が出されているが、未だに規格化されていない。
* http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0169r0.html
* 
* C++20 で char8_t が規格に入ると、このコードは動かなくなると思われる。
* 残念ながら、Boost.Regex も char**_t に対応していない。
*/
/*
BOOST_AUTO_TEST_CASE(character_iterator_regex_1)
{
	using namespace wordring::html;

	std::string_view sv = "<p>あいう1</p><p>2</p><p>3かきく</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());
	std::basic_regex<char> rx("[0-9]+");
	std::match_results<u8simple_tree::character_iterator> mr;
	std::regex_search(it, it.end(), mr, rx);

	BOOST_CHECK(mr.length() == 3);
	BOOST_CHECK(mr.str() == u8"123");
}

BOOST_AUTO_TEST_CASE(character_iterator_regex_2)
{
	using namespace wordring::html;

	setlocale(LC_ALL, ".UTF8");

	std::u8string_view sv = u8"<p>ABCあ</p><p>い</p><p>うDEF</p>";
	auto doc = make_document<u8simple_tree>(sv.begin(), sv.end());

	auto it = u8simple_tree::character_iterator(doc.begin());
	std::basic_regex<char> rx("あ.う");
	std::match_results<u8simple_tree::character_iterator> mr;
	std::regex_search(it, it.end(), mr, rx);

	// BOOST_CHECK(mr.length() == 9);
	// BOOST_CHECK(mr.str() == u8"あいう");

	BOOST_CHECK(std::locale().name() == "C");
}
*/
BOOST_AUTO_TEST_SUITE_END()
