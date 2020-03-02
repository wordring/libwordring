// test/std/algorithm.cpp

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>
#include <string>

#include <vector>

#define STRING(str) #str
#define TO_STRING(str) STRING(str)

std::string const current_source_path{ TO_STRING(CURRENT_SOURCE_PATH) };
std::string const current_binary_path{ TO_STRING(CURRENT_BINARY_PATH) };

BOOST_AUTO_TEST_SUITE(std__algorithm__test)

BOOST_AUTO_TEST_CASE(std_algorithm__adjacent_find_1)
{
#ifdef NDEBUG
	std::uint32_t n = 100 * 1024 * 1024;
#else
	std::uint32_t n = 1024;
#endif

	std::vector<std::uint32_t> v1;
	v1.reserve(n);
	std::uint32_t i = 0;
	std::generate_n(std::back_inserter(v1), n, [&i]()->std::uint32_t { return ++i; });

	std::cout.imbue(std::locale(""));

	std::cout << "---------- std_algorithm__adjacent_find_1 ----------" << std::endl;
	std::cout << "std::vector<std::uint32_t>" << std::endl;
	std::cout << "\tsize():\t" << v1.size() << std::endl;
	auto start = std::chrono::system_clock::now();
	(void)std::adjacent_find(v1.begin(), v1.end());
	auto duration = std::chrono::system_clock::now() - start;
	std::cout << "\tstd::adjacent_find:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(std_algorithm__is_sorted_1)
{
#ifdef NDEBUG
	std::uint32_t n = 100 * 1024 * 1024;
#else
	std::uint32_t n = 1024;
#endif

	std::vector<std::uint32_t> v1;
	v1.reserve(n);
	std::uint32_t i = 0;
	std::generate_n(std::back_inserter(v1), n, [&i]()->std::uint32_t { return ++i; });

	std::cout.imbue(std::locale(""));

	std::cout << "---------- std_algorithm__is_sorted_1 ----------" << std::endl;
	std::cout << "std::vector<std::uint32_t>" << std::endl;
	std::cout << "\tsize():\t" << v1.size() << std::endl;
	auto start = std::chrono::system_clock::now();
	(void)std::is_sorted(v1.begin(), v1.end());
	auto duration = std::chrono::system_clock::now() - start;
	std::cout << "\tstd::is_sorted:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(std_algorithm__sort_1)
{
#ifdef NDEBUG
	std::uint32_t n = 1 * 1024 * 1024;
#else
	std::uint32_t n = 1024;
#endif

	std::vector<std::uint32_t> v1;
	v1.reserve(n);
	std::random_device rd;
	std::mt19937 mt(rd());
	std::generate_n(std::back_inserter(v1), n, std::ref(mt));

	std::cout.imbue(std::locale(""));

	std::cout << "---------- std_algorithm__sort_1 ----------" << std::endl;
	std::cout << "std::vector<std::uint32_t>" << std::endl;
	std::cout << "\tsize():\t" << v1.size() << std::endl;
	auto start = std::chrono::system_clock::now();
	std::sort(v1.begin(), v1.end());
	auto duration = std::chrono::system_clock::now() - start;
	std::cout << "\tstd::sort:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(std_algorithm__unique_1)
{
#ifdef NDEBUG
	std::uint32_t n = 5 * 1024 * 1024;
#else
	std::uint32_t n = 1024;
#endif

	std::vector<std::uint32_t> v1;
	v1.reserve(n);
	std::random_device rd;
	std::mt19937 mt(rd());
	std::generate_n(std::back_inserter(v1), n, std::ref(mt));
	std::sort(v1.begin(), v1.end());

	std::cout.imbue(std::locale(""));

	std::cout << "---------- std_algorithm__unique_1 ----------" << std::endl;
	std::cout << "std::vector<std::uint32_t>" << std::endl;
	std::cout << "\tsize():\t" << v1.size() << std::endl;
	auto start = std::chrono::system_clock::now();
	(void)std::unique(v1.begin(), v1.end());
	auto duration = std::chrono::system_clock::now() - start;
	std::cout << "\tstd::unique:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
