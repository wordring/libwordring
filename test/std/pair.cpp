// test/std/pair.cpp

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <utility>

#define STRING(str) #str
#define TO_STRING(str) STRING(str)

std::string const current_source_path{ TO_STRING(CURRENT_SOURCE_PATH) };
std::string const current_binary_path{ TO_STRING(CURRENT_BINARY_PATH) };

namespace
{
#ifdef NDEBUG
	std::uint32_t constexpr n = 10 * 1024 * 1024;
#else
	std::uint32_t constexpr n = 1024;
#endif

	std::pair<std::uint32_t, std::uint32_t> a1[n];

	struct twice { std::uint32_t i1, i2; };
	twice a2[n];
}

BOOST_AUTO_TEST_SUITE(std_pair__test)

BOOST_AUTO_TEST_CASE(std__pair__construct_1)
{

	std::cout.imbue(std::locale(""));

	std::cout << "---------- std__pair__construct_1 ----------" << std::endl;

	auto start = std::chrono::system_clock::now();
	auto duration = std::chrono::system_clock::now() - start;
	std::cout << "std::pair<std::uint32_t, std::uint32_t>[]" << std::endl;
	std::cout << "\tsize:\t" << std::size(a1) << std::endl;
	std::cout << "\tconstruct:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (std::uint32_t i = 0; i < n; ++i) a1[i].second = i;
	duration = std::chrono::system_clock::now() - start;
	std::cout << "\tfor (std::uint32_t i = 0; i < n; ++i) a1[i].second = i;\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "\t\a1[n - 1].second\t" << a1[n - 1].second << std::endl;

	start = std::chrono::system_clock::now();
	duration = std::chrono::system_clock::now() - start;
	std::cout << "struct twice { std::uint32_t i1, i2; }[]" << std::endl;
	std::cout << "\tsize:\t" << std::size(a2) << std::endl;
	std::cout << "\tconstruct:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	start = std::chrono::system_clock::now();
	for (std::uint32_t i = 0; i < n; ++i) a2[i].i2 = i;
	duration = std::chrono::system_clock::now() - start;
	std::cout << "\tfor (std::uint32_t i = 0; i < n; ++i) a2[i].i2 = i;\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	std::cout << "\ta2[n - 1].i2\t" << a2[n - 1].i2 << std::endl;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
