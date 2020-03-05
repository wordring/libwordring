// test/trie/trie_benchmark.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/trie.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <wordring/whatwg/infra/unicode.hpp>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#define STRING(str) #str
#define TO_STRING(str) STRING(str)

namespace
{
	std::string const japanese_words_path{ TO_STRING(JAPANESE_WORDS_PATH) };
	std::string const current_source_path{ TO_STRING(CURRENT_SOURCE_PATH) };
	std::string const current_binary_path{ TO_STRING(CURRENT_BINARY_PATH) };

	std::vector<std::string> words_8;
	std::vector<std::u16string> words_16;
	std::vector<std::u32string> words_32;

	void setup1()
	{
		using wordring::whatwg::encoding_cast;

		if (!words_8.empty()) return;

		std::ifstream is(japanese_words_path);
		BOOST_REQUIRE(is.is_open());

		std::string buf{};

#ifdef NDEBUG
		while (std::getline(is, buf)) words_8.push_back(buf);
#else
		for (size_t i = 0; i < 1000 && std::getline(is, buf); ++i) words_8.push_back(buf);
#endif
		for (std::string const& s : words_8)
		{
			words_16.push_back(encoding_cast<std::u16string>(s));
			words_32.push_back(encoding_cast<std::u32string>(s));
		}
	}
}

BOOST_AUTO_TEST_SUITE(trie_benchmark__test)

BOOST_AUTO_TEST_CASE(trie_benchmark__serialize_1)
{
	using namespace wordring;

	setup1();

	std::vector<std::u32string> const& words = words_32;

	trie<char32_t> t1{};
	t1.assign(words.begin(), words.end());

	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__serialize_1 ----------" << std::endl;
	std::cout << "trie<char32_t>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	std::vector<std::int32_t> v{ t1.ibegin(), t1.iend() };
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tstd::vector<std::int32_t> v{ trie.ibegin(), trie.iend() }:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	trie<char32_t> t2{};
	start = std::chrono::system_clock::now();
	t2.assign(v.begin(), v.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\ttrie.assign(v.begin(), v.end()):\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(trie_benchmark__serialize_2)
{
	using namespace wordring;

	setup1();

	std::vector<std::u16string> const& words = words_16;

	trie<char16_t> t1{};
	t1.assign(words.begin(), words.end());

	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__serialize_2 ----------" << std::endl;
	std::cout << "trie<char16_t>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	std::vector<std::int32_t> v{ t1.ibegin(), t1.iend() };
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tstd::vector<std::int32_t> v{ trie.ibegin(), trie.iend() }:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	trie<char16_t> t2{};
	start = std::chrono::system_clock::now();
	t2.assign(v.begin(), v.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\ttrie.assign(v.begin(), v.end()):\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(trie_benchmark__serialize_3)
{
	using namespace wordring;

	setup1();

	std::vector<std::string> const& words = words_8;

	trie<char> t1{};
	t1.assign(words.begin(), words.end());

	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__serialize_3 ----------" << std::endl;
	std::cout << "trie<char>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	std::vector<std::int32_t> v{ t1.ibegin(), t1.iend() };
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tstd::vector<std::int32_t> v{ trie.ibegin(), trie.iend() }:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	trie<char> t2{};
	start = std::chrono::system_clock::now();
	t2.assign(v.begin(), v.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\ttrie.assign(v.begin(), v.end()):\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(trie_benchmark__serialize_4)
{
	using namespace wordring;

	setup1();

	trie<char32_t> t1{};
	t1.assign(words_32.begin(), words_32.end());

	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));
	std::cout << "---------- trie_benchmark__serialize_4 ----------" << std::endl;
	std::cout << "trie<char32_t>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	std::vector<std::uint8_t> v{ serialize_iterator(t1.ibegin()), serialize_iterator(t1.iend()) };
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "std::vector<std::uint8_t> v{ serialize_iterator(trie.ibegin()), serialize_iterator(trie.iend()) }: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "\tv.size():\t" << v.size() << std::endl;

	start = std::chrono::system_clock::now();
	trie<char32_t> t2{ v.begin(), v.end() };
	duration = std::chrono::system_clock::now() - start;

	std::cout << "trie<char32_t> t2{ v.begin(), v.end() }: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "\tsize():\t" << t2.size() << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(trie_benchmark__serialize_5)
{
	using namespace wordring;

	setup1();

	trie<char16_t> t1{};
	t1.assign(words_16.begin(), words_16.end());

	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));
	std::cout << "---------- trie_benchmark__serialize_5 ----------" << std::endl;
	std::cout << "trie<char16_t>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	std::vector<std::uint8_t> v{ serialize_iterator(t1.ibegin()), serialize_iterator(t1.iend()) };
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "std::vector<std::uint8_t> v{ serialize_iterator(trie.ibegin()), serialize_iterator(trie.iend()) }: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "\tv.size():\t" << v.size() << std::endl;

	start = std::chrono::system_clock::now();
	trie<char16_t> t2{ v.begin(), v.end() };
	duration = std::chrono::system_clock::now() - start;

	std::cout << "trie<char32_t> t2{ v.begin(), v.end() }: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "\tsize():\t" << t2.size() << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(trie_benchmark__serialize_6)
{
	using namespace wordring;

	setup1();

	trie<char> t1{};
	t1.assign(words_8.begin(), words_8.end());

	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));
	std::cout << "---------- trie_benchmark__serialize_6 ----------" << std::endl;
	std::cout << "trie<char>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	std::vector<std::uint8_t> v{ serialize_iterator(t1.ibegin()), serialize_iterator(t1.iend()) };
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "std::vector<std::uint8_t> v{ serialize_iterator(trie.ibegin()), serialize_iterator(trie.iend()) }: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "\tv.size():\t" << v.size() << std::endl;

	start = std::chrono::system_clock::now();
	trie<char> t2{ v.begin(), v.end() };
	duration = std::chrono::system_clock::now() - start;

	std::cout << "trie<char> t2{ v.begin(), v.end() }: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "\tsize():\t" << t2.size() << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(trie_benchmark__copy_assignment_1)
{
	using namespace wordring;

	setup1();

	std::vector<std::u32string> const& words = words_32;

	trie<char32_t> t1{};
	t1.assign(words.begin(), words.end());
	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__copy_assignment_1 ----------" << std::endl;
	std::cout << "trie<char32_t>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	trie<char32_t> t2 = t1;
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tcopy assignment:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;

	BOOST_CHECK(t1.size() == t2.size());
}

BOOST_AUTO_TEST_CASE(trie_benchmark__copy_assignment_2)
{
	using namespace wordring;

	setup1();

	std::vector<std::u16string> const& words = words_16;

	trie<char16_t> t1{};
	t1.assign(words.begin(), words.end());
	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__copy_assignment_2 ----------" << std::endl;
	std::cout << "trie<char16_t>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	trie<char16_t> t2 = t1;
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tcopy assignment:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;

	BOOST_CHECK(t1.size() == t2.size());
}

BOOST_AUTO_TEST_CASE(trie_benchmark__copy_assignment_3)
{
	using namespace wordring;

	setup1();

	std::vector<std::string> const& words = words_8;

	trie<char> t1{};
	t1.assign(words.begin(), words.end());
	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__copy_assignment_3 ----------" << std::endl;
	std::cout << "trie<char>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	trie<char> t2 = t1;
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tcopy assignment:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;

	BOOST_CHECK(t1.size() == t2.size());
}

BOOST_AUTO_TEST_CASE(trie_benchmark__words_raw_1)
{
	using namespace wordring;

	setup1();

	std::vector<std::u32string> const& w = words_32;
	std::uint32_t error = 0;

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__words_raw_1 ----------" << std::endl;

	std::cout << "std::vector<std::u32string> w{ (raw words...) };" << std::endl;
	std::cout << "\tsize:\t" << w.size() << std::endl;

	std::cout << "trie<char32_t>" << std::endl;

	// trie.assign() ----------------------------------------------------------

	trie<char32_t> t1{};
	auto start = std::chrono::system_clock::now();
	t1.assign(w.begin(), w.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tassign:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t1.find(s) == t1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// trie.insert() ----------------------------------------------------------

	trie<char32_t> t2{};
	start = std::chrono::system_clock::now();
	for (auto const& s : w) t2.insert(s);
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t2.find(s) == t2.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::set.insert() ------------------------------------------------------

	std::cout << "std::set<std::u32string>" << std::endl;

	std::set<std::u32string> set{};
	start = std::chrono::system_clock::now();
	set.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (set.find(s) == set.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::unordered_set.insert() --------------------------------------------

	std::cout << "std::unordered_set<std::u32string>" << std::endl;

	std::unordered_set<std::u32string> uset{};
	start = std::chrono::system_clock::now();
	uset.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (uset.find(s) == uset.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;

	BOOST_CHECK(error == 0);
	BOOST_CHECK(t1.size() == set.size());
	BOOST_CHECK(t2.size() == set.size());
}

BOOST_AUTO_TEST_CASE(trie_benchmark__words_raw_2)
{
	using namespace wordring;

	setup1();

	std::vector<std::u16string> const& w = words_16;
	std::uint32_t error = 0;

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__words_raw_2 ----------" << std::endl;

	std::cout << "std::vector<std::u16string> w{ (raw words...) };" << std::endl;
	std::cout << "\tsize:\t" << w.size() << std::endl;

	std::cout << "trie<char16_t>" << std::endl;

	// trie.assign() ----------------------------------------------------------

	trie<char16_t> t1{};
	auto start = std::chrono::system_clock::now();
	t1.assign(w.begin(), w.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tassign:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t1.find(s) == t1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// trie.insert() ----------------------------------------------------------

	trie<char16_t> t2{};
	start = std::chrono::system_clock::now();
	for (auto const& s : w) t2.insert(s);
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t2.find(s) == t2.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::set.insert() ------------------------------------------------------

	std::cout << "std::set<std::u16string>" << std::endl;

	std::set<std::u16string> set{};
	start = std::chrono::system_clock::now();
	set.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (set.find(s) == set.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::unordered_set.insert() --------------------------------------------

	std::cout << "std::unordered_set<std::u16string>" << std::endl;

	std::unordered_set<std::u16string> uset{};
	start = std::chrono::system_clock::now();
	uset.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (uset.find(s) == uset.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;

	BOOST_CHECK(error == 0);
	BOOST_CHECK(t1.size() == set.size());
	BOOST_CHECK(t2.size() == set.size());
}

BOOST_AUTO_TEST_CASE(trie_benchmark__words_raw_3)
{
	using namespace wordring;

	setup1();

	std::vector<std::string> const& w = words_8;
	std::uint32_t error = 0;

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__words_raw_3 ----------" << std::endl;

	std::cout << "std::vector<std::string> w{ (raw words...) };" << std::endl;
	std::cout << "\tsize:\t" << w.size() << std::endl;

	std::cout << "trie<char>" << std::endl;

	// trie.assign() ----------------------------------------------------------

	trie<char> t1{};
	auto start = std::chrono::system_clock::now();
	t1.assign(w.begin(), w.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tassign:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t1.find(s) == t1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// trie.insert() ----------------------------------------------------------

	trie<char> t2{};
	start = std::chrono::system_clock::now();
	for (auto const& s : w) t2.insert(s);
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t2.find(s) == t2.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::set.insert() ------------------------------------------------------

	std::cout << "std::set<std::string>" << std::endl;

	std::set<std::string> set{};
	start = std::chrono::system_clock::now();
	set.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (set.find(s) == set.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::unordered_set.insert() --------------------------------------------

	std::cout << "std::unordered_set<std::string>" << std::endl;

	std::unordered_set<std::string> uset{};
	start = std::chrono::system_clock::now();
	uset.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (uset.find(s) == uset.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;

	BOOST_CHECK(error == 0);
	BOOST_CHECK(t1.size() == set.size());
	BOOST_CHECK(t2.size() == set.size());
}

BOOST_AUTO_TEST_CASE(trie_benchmark__words_sorted_1)
{
	using namespace wordring;

	setup1();

	std::vector<std::u32string> w = words_32;
	std::sort(w.begin(), w.end());
	std::uint32_t error = 0;

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__words_sorted_1 ----------" << std::endl;

	std::cout << "std::vector<std::u32string> w{ (sorted words...) };" << std::endl;
	std::cout << "\tsize:\t" << w.size() << std::endl;

	std::cout << "trie<char32_t>" << std::endl;

	// trie.assign() ----------------------------------------------------------

	trie<char32_t> t1{};
	auto start = std::chrono::system_clock::now();
	t1.assign(w.begin(), w.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tassign:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t1.find(s) == t1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// trie.insert() ----------------------------------------------------------

	trie<char32_t> t2{};
	start = std::chrono::system_clock::now();
	for (auto const& s : w) t2.insert(s);
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t2.find(s) == t2.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::set.insert() ------------------------------------------------------

	std::cout << "std::set<std::u32string>" << std::endl;

	std::set<std::u32string> set{};
	start = std::chrono::system_clock::now();
	set.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (set.find(s) == set.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::unordered_set.insert() --------------------------------------------

	std::cout << "std::unordered_set<std::u32string>" << std::endl;

	std::unordered_set<std::u32string> uset{};
	start = std::chrono::system_clock::now();
	uset.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (uset.find(s) == uset.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;

	BOOST_CHECK(error == 0);
	BOOST_CHECK(t1.size() == set.size());
	BOOST_CHECK(t2.size() == set.size());
}

BOOST_AUTO_TEST_CASE(trie_benchmark__words_sorted_2)
{
	using namespace wordring;

	setup1();

	std::vector<std::u16string> w = words_16;
	std::sort(w.begin(), w.end());
	std::uint32_t error = 0;

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__words_sorted_2 ----------" << std::endl;

	std::cout << "std::vector<std::u16string> w{ (sorted words...) };" << std::endl;
	std::cout << "\tsize:\t" << w.size() << std::endl;

	std::cout << "trie<char16_t>" << std::endl;

	// trie.assign() ----------------------------------------------------------

	trie<char16_t> t1{};
	auto start = std::chrono::system_clock::now();
	t1.assign(w.begin(), w.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tassign:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t1.find(s) == t1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// trie.insert() ----------------------------------------------------------

	trie<char16_t> t2{};
	start = std::chrono::system_clock::now();
	for (auto const& s : w) t2.insert(s);
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t2.find(s) == t2.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::set.insert() ------------------------------------------------------

	std::cout << "std::set<std::u16string>" << std::endl;

	std::set<std::u16string> set{};
	start = std::chrono::system_clock::now();
	set.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (set.find(s) == set.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::unordered_set.insert() --------------------------------------------

	std::cout << "std::unordered_set<std::u16string>" << std::endl;

	std::unordered_set<std::u16string> uset{};
	start = std::chrono::system_clock::now();
	uset.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (uset.find(s) == uset.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;

	BOOST_CHECK(error == 0);
	BOOST_CHECK(t1.size() == set.size());
	BOOST_CHECK(t2.size() == set.size());
}

BOOST_AUTO_TEST_CASE(trie_benchmark__words_sorted_3)
{
	using namespace wordring;

	setup1();

	std::vector<std::string> w = words_8;
	std::sort(w.begin(), w.end());
	std::uint32_t error = 0;

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_benchmark__words_sorted_3 ----------" << std::endl;

	std::cout << "std::vector<std::string> w{ (sorted words...) };" << std::endl;
	std::cout << "\tsize:\t" << w.size() << std::endl;

	std::cout << "trie<char>" << std::endl;

	// trie.assign() ----------------------------------------------------------

	trie<char> t1{};
	auto start = std::chrono::system_clock::now();
	t1.assign(w.begin(), w.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tassign:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t1.find(s) == t1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// trie.insert() ----------------------------------------------------------

	trie<char> t2{};
	start = std::chrono::system_clock::now();
	for (auto const& s : w) t2.insert(s);
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t2.find(s) == t2.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::set.insert() ------------------------------------------------------

	std::cout << "std::set<std::string>" << std::endl;

	std::set<std::string> set{};
	start = std::chrono::system_clock::now();
	set.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (set.find(s) == set.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// std::unordered_set.insert() --------------------------------------------

	std::cout << "std::unordered_set<std::string>" << std::endl;

	std::unordered_set<std::string> uset{};
	start = std::chrono::system_clock::now();
	uset.insert(w.begin(), w.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tinsert:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (uset.find(s) == uset.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;

	BOOST_CHECK(error == 0);
	BOOST_CHECK(t1.size() == set.size());
	BOOST_CHECK(t2.size() == set.size());
}

BOOST_AUTO_TEST_SUITE_END()
