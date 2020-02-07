// test/trie/trie_base.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/trie_base.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <algorithm>
#include <chrono>
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

std::string const english_words_path{ TO_STRING(ENGLISH_WORDS_PATH) };

namespace wordring
{
	class test_trie : public trie_base<>
	{
	public:
		using base_type = trie_base<>;

		using base_type::add;
		using base_type::m_c;

	public:
		/*! 保持する文字列の数を数える
		- size()と同じ数を返せば良好。
		*/
		std::size_t count() const
		{
			using namespace wordring;
			std::size_t n = 0;

			auto it1 = tree_iterator<decltype(begin())>(begin());
			auto it2 = tree_iterator<decltype(begin())>();

			while (it1 != it2)
			{
				if (it1.base()) ++n;
				++it1;
			}

			return n;
		}
	};

	inline bool operator==(wordring::trie_node const& lhs, wordring::trie_node const& rhs)
	{
		return lhs.m_base == rhs.m_base && lhs.m_check == rhs.m_check;
	}
}

BOOST_AUTO_TEST_SUITE(trie_base__test)

// ----------------------------------------------------------------------------
// trie_base
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(trie_base__constrcut__1)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<trie_node> v{ { 0, 0 }, { 0, 0 } };
	BOOST_CHECK(trie.m_c == v);
}

// 要素アクセス ----------------------------------------------------------------

// イテレータ ------------------------------------------------------------------

// const_iterator begin() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__begin__1)
{
	using namespace wordring;
	test_trie trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

// const_iterator cbegin() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__cbegin__1)
{
	using namespace wordring;
	test_trie trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

// const_iterator end() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__end__1)
{
	using namespace wordring;
	test_trie trie{};
	BOOST_CHECK(trie.begin() != trie.end());
}

// const_iterator cend() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__cend__1)
{
	using namespace wordring;
	test_trie trie{};
	BOOST_CHECK(trie.cbegin() != trie.cend());
}

// 容量 -----------------------------------------------------------------------

// bool empty() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__empty__1)
{
	using namespace wordring;
	test_trie trie{};
	BOOST_CHECK(trie.empty());
}

BOOST_AUTO_TEST_CASE(trie_base__empty__2)
{
	using namespace wordring;
	test_trie trie{};
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.empty() == false);
}

// size_type size() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__size__1)
{
	using namespace wordring;
	test_trie trie{};
	BOOST_CHECK(trie.size() == 0);
}

BOOST_AUTO_TEST_CASE(trie_base__size__2)
{
	using namespace wordring;
	test_trie trie{};
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.size() == 1);
	trie.insert(std::string("abc"));
	BOOST_CHECK(trie.size() == 1);
}

// size_type max_size() const noexcept
BOOST_AUTO_TEST_CASE(trie_base__max_size__1)
{
	using namespace wordring;
	test_trie trie{};
	BOOST_CHECK(trie.max_size());
}

// 変更 -----------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(trie_base__clear__1)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "\1\1", "\1" };
	trie.insert(v.begin(), v.end());

	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.size() == 2);

	trie.clear();

	BOOST_CHECK(trie.count() == 0);
	BOOST_CHECK(trie.size() == 0);
}

// const_iterator insert(Key const& key)
BOOST_AUTO_TEST_CASE(trie_base__insert__1)
{
	using namespace wordring;
	test_trie trie{};

	trie.insert(std::string("\1"));

	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.count() == 1);
	BOOST_CHECK(trie.contains(std::string("\1")));
}

BOOST_AUTO_TEST_CASE(trie_base__insert__2)
{
	using namespace wordring;
	test_trie trie{};

	trie.insert(std::string("\1\1"));

	BOOST_CHECK(trie.size() == 1);
	BOOST_CHECK(trie.count() == 1);
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

BOOST_AUTO_TEST_CASE(trie_base__insert__3)
{
	using namespace wordring;
	test_trie trie{};

	trie.insert(std::string("\1"));
	trie.insert(std::string("\1\1"));

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("\1")));
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

BOOST_AUTO_TEST_CASE(trie_base__insert__4)
{
	using namespace wordring;
	test_trie trie{};

	trie.insert(std::string("\1\1"));
	trie.insert(std::string("\1"));

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("\1")));
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

BOOST_AUTO_TEST_CASE(trie_base__insert__5)
{
	using namespace wordring;
	test_trie trie{};

	trie.insert(std::string("\1\1"));
	trie.insert(std::string("\1"));
	trie.insert(std::string("\1\1"));

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("\1")));
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

BOOST_AUTO_TEST_CASE(trie_base__insert__6)
{
	using namespace wordring;
	test_trie trie{};

	trie.insert(std::string("\1\1"));
	trie.insert(std::string("\1"));
	trie.insert(std::string("\1"));

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("\1")));
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

BOOST_AUTO_TEST_CASE(trie_base__insert__7)
{
	using namespace wordring;
	test_trie trie{};

	trie.insert(std::string("\1\1"));
	trie.insert(std::string("\1"));
	trie.insert(std::string(""));

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("\1")));
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

// void insert(InputIterator first, InputIterator last)
BOOST_AUTO_TEST_CASE(trie_base__insert__8)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "\1\1", "\1" };
	trie.insert(v.begin(), v.end());

	BOOST_CHECK(trie.size() == 2);
	BOOST_CHECK(trie.count() == 2);
	BOOST_CHECK(trie.contains(std::string("\1")));
	BOOST_CHECK(trie.contains(std::string("\1\1")));
}

// void erase(const_iterator pos)
BOOST_AUTO_TEST_CASE(trie_base__erase__1)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.insert(v.begin(), v.end());

	BOOST_CHECK(trie.size() == 5);
	BOOST_CHECK(trie.count() == 5);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));

	trie.erase(std::string("a"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.count() == 4);
	BOOST_CHECK(trie.contains(std::string("a")) == false);
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base__erase__2)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.insert(v.begin(), v.end());

	trie.erase(std::string("ac"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.count() == 4);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")) == false);
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base__erase__3)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.insert(v.begin(), v.end());

	trie.erase(std::string("b"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.count() == 4);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")) == false);
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base__erase__4)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.insert(v.begin(), v.end());

	trie.erase(std::string("cab"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.count() == 4);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")) == false);
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base__erase__5)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.insert(v.begin(), v.end());

	trie.erase(std::string("cd"));

	BOOST_CHECK(trie.size() == 4);
	BOOST_CHECK(trie.count() == 4);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")) == false);
}

BOOST_AUTO_TEST_CASE(trie_base__erase__6)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.insert(v.begin(), v.end());

	trie.erase(std::string(""));

	BOOST_CHECK(trie.size() == 5);
	BOOST_CHECK(trie.count() == 5);
	BOOST_CHECK(trie.contains(std::string("a")));
	BOOST_CHECK(trie.contains(std::string("ac")));
	BOOST_CHECK(trie.contains(std::string("b")));
	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

// 検索 -----------------------------------------------------------------------

// bool contains(Key const& key) const
BOOST_AUTO_TEST_CASE(trie_base__contains__1)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.insert(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::string("a")));
}

BOOST_AUTO_TEST_CASE(trie_base__contains__2)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.insert(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::string("ac")));
}

BOOST_AUTO_TEST_CASE(trie_base__contains__3)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.insert(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::string("cab")));
	BOOST_CHECK(trie.contains(std::string("cd")));
}

BOOST_AUTO_TEST_CASE(trie_base__contains__4)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.insert(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::string("")) == false);
}

BOOST_AUTO_TEST_CASE(trie_base__contains__5)
{
	using namespace wordring;
	test_trie trie{};

	std::vector<std::string> v{ "a", "ac", "b", "cab", "cd" };
	trie.insert(v.begin(), v.end());

	BOOST_CHECK(trie.contains(std::string("ab")) == false);
	BOOST_CHECK(trie.contains(std::string("cb")) == false);
	BOOST_CHECK(trie.contains(std::string("caba")) == false);
	BOOST_CHECK(trie.contains(std::string("cda")) == false);
}

// 内部 -----------------------------------------------------------------------

// index_type add(index_type parent, label_vector const& labels)
BOOST_AUTO_TEST_CASE(trie_base__add__1)
{
	using namespace wordring;
	test_trie trie{};

	trie.add(1, { 0, 1, 2 });

	std::vector<trie_node> v{ { 0, 0 }, { 2, 0 }, { 0, 1 }, { 0, 1 }, { 0, 1 } };
	BOOST_CHECK(trie.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_base__add__2)
{
	using namespace wordring;
	test_trie trie{};

	trie.add(1, { 1, 2, 3 });

	std::vector<trie_node> v{ { 0, 0 }, { 1, 0 }, { 0, 1 }, { 0, 1 }, { 0, 1 } };
	BOOST_CHECK(trie.m_c == v);
}

BOOST_AUTO_TEST_CASE(trie_base__relocate__1)
{
	using namespace wordring;

	std::vector<std::string> words{};
	{
		std::string buf{};
		std::ifstream is(english_words_path);
		BOOST_REQUIRE(is.is_open());

		for (size_t i = 0; i < 100000 && std::getline(is, buf); ++i) words.push_back(buf);
	}

	while (true)
	{

		std::random_device rd;
		std::mt19937 mt(rd());
		std::shuffle(words.begin(), words.end(), mt);

		//for(std::string const& )std::cout << 

		test_trie trie{};
		trie.insert(words.begin(), words.end());
	}
}

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(trie_base__english_words_txt__1)
{
	using namespace wordring;

	std::vector<std::string> words{};
	{
		std::string buf{};
		std::ifstream is(english_words_path);
		BOOST_REQUIRE(is.is_open());
#ifdef NDEBUG
		while (std::getline(is, buf)) words.push_back(buf);
#else
		for (size_t i = 0; i < 1000 && std::getline(is, buf); ++i) words.push_back(buf);
		std::mt19937 mt;
		std::shuffle(words.begin(), words.end(), mt);
#endif
	}

	test_trie trie{};

	auto start = std::chrono::system_clock::now();

	for (auto const& s : words) trie.insert(s);

	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "trie_base__english_words_txt__1 (dynamic insert.)" << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "node  : " << trie.m_c.size() << std::endl;
	std::cout << "count : " << trie.count() << std::endl;
	std::cout << "size  : " << trie.size() << std::endl;
	std::cout << "vector: " << words.size() << std::endl;

	BOOST_CHECK(trie.count() == words.size());
	BOOST_CHECK(trie.size() == words.size());
}

BOOST_AUTO_TEST_CASE(trie_base__english_words_benchmark__1)
{
	using namespace wordring;

	std::vector<std::string> words{};
	{
		std::string buf{};
		std::ifstream is(english_words_path);
		BOOST_REQUIRE(is.is_open());
#ifdef NDEBUG
		while (std::getline(is, buf)) words.push_back(buf);
#else
		for (size_t i = 0; i < 50000 && std::getline(is, buf); ++i) words.push_back(buf);
		std::mt19937 mt;
		std::shuffle(words.begin(), words.end(), mt);
#endif
	}

	std::uint32_t error = 0;

	std::cout << "trie_base__english_words_benchmark__1" << std::endl;
/*
	test_trie trie{};
	auto start = std::chrono::system_clock::now();
	trie.assign(words.begin(), words.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "trie assign." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "node  : " << trie.m_c.size() << std::endl;
	std::cout << "size  : " << trie.size() << std::endl;
	std::cout << std::endl;
*/
	test_trie trie{};
	auto start = std::chrono::system_clock::now();
	trie.insert(words.begin(), words.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "trie insert." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "node  : " << trie.m_c.size() << std::endl;
	std::cout << "size  : " << trie.size() << std::endl;
	std::cout << std::endl;

	std::set<std::string> set{};
	start = std::chrono::system_clock::now();
	set.insert(words.begin(), words.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "std::set insert." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << std::endl;

	std::unordered_set<std::string> uset{};
	start = std::chrono::system_clock::now();
	uset.insert(words.begin(), words.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "std::unordered_set insert." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : words) if (trie.find(s) == trie.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "trie find." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : words) if (set.find(s) == set.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "std::set find." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : words) if (uset.find(s) == uset.cend()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "std::unordered_set find." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << std::endl;

	BOOST_CHECK(error == 0);
}

BOOST_AUTO_TEST_SUITE_END()
