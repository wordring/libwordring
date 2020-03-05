// test/trie/trie_base_benchmark.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/trie_base.hpp>
#include <wordring/tree/tree_iterator.hpp>

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
	std::string const english_words_path{ TO_STRING(ENGLISH_WORDS_PATH) };
	std::string const current_source_path{ TO_STRING(CURRENT_SOURCE_PATH) };
	std::string const current_binary_path{ TO_STRING(CURRENT_BINARY_PATH) };

	std::vector<std::string> words1; // 生単語集
	std::vector<std::string> words2; // ランダム単語集
	std::vector<std::string> words3; // ソート済み単語集

	void setup1()
	{
		if (!words1.empty()) return;

		std::ifstream is(english_words_path);
		BOOST_REQUIRE(is.is_open());

		std::string buf{};

#ifdef NDEBUG
		while (std::getline(is, buf)) words1.push_back(buf);
#else
		for (size_t i = 0; i < 1000 && std::getline(is, buf); ++i) words1.push_back(buf);
#endif
	}

	void setup2()
	{
		if (!words2.empty()) return;
		setup1();

		auto path = std::filesystem::path(current_binary_path) / "random_words.txt";

		std::string buf{};

		if (std::filesystem::exists(path))
		{
			std::ifstream is(path);
			BOOST_REQUIRE(is.is_open());
			while (std::getline(is, buf)) words2.push_back(buf);
		}
		else
		{
			words2 = words1;
			std::random_device rd;
			std::mt19937 mt(rd());
			std::shuffle(words2.begin(), words2.end(), mt);

			std::ofstream os(path);
			BOOST_REQUIRE(os.is_open());

			for (std::string const& s : words2) os << s << std::endl;
		}
	}

	void setup3()
	{
		if (!words3.empty()) return;
		setup1();

		words3 = words1;
		std::sort(words3.begin(), words3.end());
		words3.erase(std::unique(words3.begin(), words3.end()), words3.end());
	}
}

BOOST_AUTO_TEST_SUITE(trie_base_benchmark__test)

BOOST_AUTO_TEST_CASE(trie_base_benchmark__serialize_1)
{
	using namespace wordring;

	setup1();

	std::vector<std::string> const& words = words1;

	detail::trie_base<> t1{};
	t1.assign(words.begin(), words.end());

	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_base_benchmark__serialize_1 ----------" << std::endl;
	std::cout << "detail::trie_base<>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	std::vector<std::int32_t> v{ t1.ibegin(), t1.iend() };
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tstd::vector<std::int32_t> v{ trie.ibegin(), trie.iend() }:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	detail::trie_base<> t2{};
	start = std::chrono::system_clock::now();
	t2.assign(v.begin(), v.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\ttrie.assign(v.begin(), v.end()):\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(trie_base_benchmark__serialize_2)
{
	using namespace wordring;

	setup1();

	detail::trie_base<> t1{};
	t1.assign(words1.begin(), words1.end());

	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));
	std::cout << "---------- trie_base_benchmark__serialize_2 ----------" << std::endl;
	std::cout << "detail::trie_base<>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	std::vector<std::uint8_t> v{ serialize_iterator(t1.ibegin()), serialize_iterator(t1.iend()) };
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "std::vector<std::uint8_t> v{ serialize_iterator(trie.ibegin()), serialize_iterator(trie.iend()) }: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "\tv.size():\t" << v.size() << std::endl;

	start = std::chrono::system_clock::now();
	detail::trie_base<> t2{ v.begin(), v.end() };
	duration = std::chrono::system_clock::now() - start;

	std::cout << "detail::trie_base<> t2{ v.begin(), v.end() }: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "\tsize():\t" << t2.size() << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(trie_base_benchmark__copy_assignment_1)
{
	using namespace wordring;

	setup1();

	std::vector<std::string> const& words = words1;

	detail::trie_base<> t1{};
	t1.assign(words.begin(), words.end());
	std::vector<std::uint32_t> buf;
	std::copy(t1.ibegin(), t1.iend(), std::back_inserter(buf));

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_base_benchmark__copy_assignment_1 ----------" << std::endl;
	std::cout << "detail::trie_base<>" << std::endl;
	std::cout << "\tsize():\t" << t1.size() << std::endl;
	std::cout << "\tnodes:\t" << (buf.size() / 2) << std::endl;

	auto start = std::chrono::system_clock::now();
	auto t2 = t1;
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tcopy assignment:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;
}


BOOST_AUTO_TEST_CASE(trie_base_benchmark__words_raw_1)
{
	using namespace wordring;

	setup1();

	std::vector<std::string> const& w = words1;
	std::uint32_t error = 0;

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_base_benchmark__words_raw_1 ----------" << std::endl;

	std::cout << "std::vector<std::string> w{ (raw words...) };" << std::endl;
	std::cout << "\tsize:\t" << w.size() << std::endl;

	std::cout << "detail::trie_base<>" << std::endl;

	// trie.assign() ----------------------------------------------------------

	detail::trie_base<> t1{};
	auto start = std::chrono::system_clock::now();
	t1.assign(w.begin(), w.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tassign:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t1.find(s) == t1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// trie.insert() ----------------------------------------------------------

	detail::trie_base<> t2{};
	start = std::chrono::system_clock::now();
	for(std::string const& s : w) t2.insert(s);
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
}

BOOST_AUTO_TEST_CASE(trie_base_benchmark__words_random_1)
{
	using namespace wordring;

	setup2();

	std::vector<std::string> const& w = words2;
	std::uint32_t error = 0;

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_base_benchmark__words_random_1 ----------" << std::endl;

	std::cout << "std::vector<std::string> w{ (random words...) };" << std::endl;
	std::cout << "\tsize:\t" << w.size() << std::endl;

	std::cout << "detail::trie_base<>" << std::endl;

	// trie.assign() ----------------------------------------------------------

	detail::trie_base<> t1{};
	auto start = std::chrono::system_clock::now();
	t1.assign(w.begin(), w.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tassign:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t1.find(s) == t1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// trie.insert() ----------------------------------------------------------

	detail::trie_base<> t2{};
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
}

BOOST_AUTO_TEST_CASE(trie_base_benchmark__words_sorted_1)
{
	using namespace wordring;

	setup3();

	std::vector<std::string> const& w = words3;
	std::uint32_t error = 0;

	std::cout.imbue(std::locale(""));

	std::cout << "---------- trie_base_benchmark__words_sorted_1 ----------" << std::endl;

	std::cout << "std::vector<std::string> w{ (sorted words...) };" << std::endl;
	std::cout << "\tsize:\t" << w.size() << std::endl;

	std::cout << "detail::trie_base<>" << std::endl;

	// trie.assign() ----------------------------------------------------------

	detail::trie_base<> t1{};
	auto start = std::chrono::system_clock::now();
	t1.assign(w.begin(), w.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "\tassign:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : w) if (t1.find(s) == t1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "\tfind:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	// trie.insert() ----------------------------------------------------------

	detail::trie_base<> t2{};
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
}

BOOST_AUTO_TEST_SUITE_END()
