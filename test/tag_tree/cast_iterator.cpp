// test/tag_tree/cast_iterator.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/tag_tree/tag_tree.hpp>

#include <wordring/css/selector.hpp>
#include <wordring/html/simple_html.hpp>
#include <wordring/html/simple_traits.hpp>

#include <iterator>
#include <regex>
#include <string>
#include <string_view>
#include <type_traits>

#include <iostream>

namespace wordring
{
	//static_assert(std::is_copy_constructible_v<cast_iterator<wchar_t, html::u16simple_tree::iterator>>);
	//static_assert(std::is_copy_assignable_v<cast_iterator<wchar_t, html::u16simple_tree::iterator>>);
}

BOOST_AUTO_TEST_SUITE(tag_tree_cast_iterator_test)

BOOST_AUTO_TEST_CASE(std_regex_1)
{
	std::wstring s = L"ABCあいうDEF";

	std::basic_regex<wchar_t> rx(L"あ.う");
	std::match_results<std::wstring::iterator> mr;
	std::regex_search(s.begin(), s.end(), mr, rx);

	BOOST_CHECK(mr.length() == 3);
	BOOST_CHECK(mr.str() == L"あいう");
}

// ------------------------------------------------------------------------------------------------
// class cast_iterator
// ------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(cast_iterator_regex_1)
{
	using namespace wordring;
	using namespace wordring::html;

#ifdef __linux__
	using tree_type = u32simple_tree;
#else
	using tree_type = u16simple_tree;
#endif // __linux__

	std::string_view sv = u8"<span>ABCあ</span><a>い</a><strong>うDEF</strong>";
	auto doc = make_document<tree_type>(sv.begin(), sv.end());

	using wchar_iterator = tree_type::wchar_iterator;

	auto it1 = wchar_iterator(doc.begin());
	auto it2 = wchar_iterator(doc.end());

	std::basic_regex<wchar_t> rx(L"あ.う");
	std::match_results<wchar_iterator> mr;
	std::regex_search(it1, it2, mr, rx);

	BOOST_CHECK(mr.length() == 3);
	BOOST_CHECK(mr.str() == L"あいう");

	tree_type::iterator it3 = mr.begin()->first;
	auto tag = encoding_cast<std::u16string>(it3.parent()->local_name());

	BOOST_CHECK(tag == u"span");
}

BOOST_AUTO_TEST_SUITE_END()
