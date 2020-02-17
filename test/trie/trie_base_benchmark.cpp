// test/trie/trie_base_benchmark.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/trie/trie_base.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <algorithm>
#include <chrono>
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

BOOST_AUTO_TEST_SUITE(trie_base_benchmark__test)

BOOST_AUTO_TEST_CASE(trie_base_benchmark__adjacent_find_1)
{
	setup3();
		
	auto start = std::chrono::system_clock::now();
	(void)std::adjacent_find(words3.begin(), words3.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout
		<< "std::adjacent_find(sorted words): "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"
		<< ", (" << words3.size() << "count)."
		<< std::endl;
}

BOOST_AUTO_TEST_CASE(trie_base_benchmark__is_sorted_1)
{
	setup3();

	auto start = std::chrono::system_clock::now();
	(void)std::is_sorted(words3.begin(), words3.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout
		<< "std::is_sorted(sorted words): "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"
		<< ", (" << words3.size() << "count)."
		<< std::endl;
}

BOOST_AUTO_TEST_CASE(trie_base_benchmark__sort_1)
{
	setup2();

	auto start = std::chrono::system_clock::now();
	std::sort(words2.begin(), words2.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout
		<< "std::sort: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"
		<< ", (" << words2.size() << "count)."
		<< std::endl;
}

BOOST_AUTO_TEST_CASE(trie_base_benchmark__unique_1)
{
	setup2();

	auto start = std::chrono::system_clock::now();
	words2.erase(std::unique(words2.begin(), words2.end()), words2.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout
		<< "std::unique: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"
		<< ", (" << words2.size() << "count)."
		<< std::endl;
}

BOOST_AUTO_TEST_CASE(trie_base_benchmark__assign_1)
{
	using namespace wordring;

	setup1();
	std::vector<std::string> const& words = words1;

	trie_base t1{};
	t1.assign(words.begin(), words.end());

	std::cout << "trie_base_benchmark__assign_1" << std::endl;

	auto start = std::chrono::system_clock::now();
	std::vector<std::int32_t> v(t1.ibegin(), t1.iend());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout
		<< "serialize: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"
		<< std::endl;

	trie_base t2{};
	start = std::chrono::system_clock::now();
	t2.assign(v.begin(), v.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout
		<< "deserialize: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"
		<< std::endl;

	std::cout << "size: " << t2.size() << std::endl;
}

BOOST_AUTO_TEST_CASE(trie_base_benchmark__copy_1)
{
	using namespace wordring;

	setup1();
	std::vector<std::string> const& words = words1;

	trie_base t1{}, t2{};
	t1.assign(words.begin(), words.end());

	std::cout << "trie_base_benchmark__copy_1" << std::endl;

	auto start = std::chrono::system_clock::now();
	t2 = t1;
	auto duration = std::chrono::system_clock::now() - start;

	std::cout
		<< "copy: "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"
		<< std::endl;

	std::cout << "size: " << t2.size() << std::endl;
}

BOOST_AUTO_TEST_CASE(trie_base_benchmark__raw_words_1)
{
	using namespace wordring;

	setup1();
	std::vector<std::string> const& words = words1;
	std::uint32_t error = 0;

	std::cout << "trie_base_benchmark__raw_words_1" << std::endl;

	trie_base trie1{};
	auto start = std::chrono::system_clock::now();
	trie1.assign(words.begin(), words.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "trie assign." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "size  : " << trie1.size() << std::endl;
	std::cout << "words : " << words.size() << std::endl;
	std::cout << std::endl;

	trie_base trie2{};
	start = std::chrono::system_clock::now();
	trie2.insert(words.begin(), words.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "trie insert." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "size  : " << trie2.size() << std::endl;
	std::cout << "words : " << words.size() << std::endl;
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
	for (auto const& s : words) if (trie1.find(s) == trie1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "(assigned) trie find." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : words) if (trie2.find(s) == trie2.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "(inserted) trie find." << std::endl;
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

BOOST_AUTO_TEST_CASE(trie_base_benchmark__random_words_1)
{
	using namespace wordring;

	std::cout << "trie_base_benchmark__random_words_1" << std::endl;

	auto path = std::filesystem::path(current_binary_path) / "random_words.txt";
	std::cout << path << std::endl << std::endl;

	setup2();
	std::vector<std::string> const& words = words2;
	std::uint32_t error = 0;

	trie_base trie1{};
	auto start = std::chrono::system_clock::now();
	trie1.assign(words.begin(), words.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "trie assign." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "size  : " << trie1.size() << std::endl;
	std::cout << "words : " << words.size() << std::endl;
	std::cout << std::endl;

	trie_base trie2{};
	start = std::chrono::system_clock::now();
	trie2.insert(words.begin(), words.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "trie insert." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "size  : " << trie2.size() << std::endl;
	std::cout << "words : " << words.size() << std::endl;
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
	for (auto const& s : words) if (trie1.find(s) == trie1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "(assigned) trie find." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : words) if (trie2.find(s) == trie2.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "(inserted) trie find." << std::endl;
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

BOOST_AUTO_TEST_CASE(trie_base_benchmark__sorted_words_1)
{
	using namespace wordring;

	setup3();
	std::vector<std::string> const& words = words3;
	std::uint32_t error = 0;

	std::cout << "trie_base_benchmark__sorted_words_1" << std::endl;

	trie_base trie1{};
	auto start = std::chrono::system_clock::now();
	trie1.assign(words.begin(), words.end());
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "trie assign." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "size  : " << trie1.size() << std::endl;
	std::cout << "words : " << words.size() << std::endl;
	std::cout << std::endl;

	trie_base trie2{};
	start = std::chrono::system_clock::now();
	trie2.insert(words.begin(), words.end());
	duration = std::chrono::system_clock::now() - start;

	std::cout << "trie insert." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "size  : " << trie2.size() << std::endl;
	std::cout << "words : " << words.size() << std::endl;
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
	for (auto const& s : words) if (trie1.find(s) == trie1.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "(assigned) trie find." << std::endl;
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << std::endl;

	start = std::chrono::system_clock::now();
	for (auto const& s : words) if (trie2.find(s) == trie2.end()) ++error;
	duration = std::chrono::system_clock::now() - start;

	std::cout << "(inserted) trie find." << std::endl;
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
