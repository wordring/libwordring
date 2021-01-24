// test/std/vector.cpp

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

BOOST_AUTO_TEST_SUITE(std_vector__test)

BOOST_AUTO_TEST_CASE(std__vector__copy_1)
{
#ifdef NDEBUG
	std::uint32_t n = 10 * 1024 * 1024;
#else
	std::uint32_t n = 1024;
#endif

	std::vector<std::uint32_t> v1;
	v1.reserve(n);

	std::random_device rd;
	std::mt19937 mt(rd());
	std::generate_n(std::back_inserter(v1), n, std::ref(mt));

	std::uint32_t* a1 = new std::uint32_t[n];
	std::generate_n(a1, n, std::ref(mt));
	
	std::cout.imbue(std::locale(""));
	
	std::cout << "---------- std__vector__copy_1 ----------" << std::endl;
	std::cout << "std::vector<std::uint32_t>" << std::endl;
	std::cout << "\tsize():\t" << v1.size() << std::endl;
	auto start = std::chrono::system_clock::now();
	auto v2 = v1;
	auto duration = std::chrono::system_clock::now() - start;
	std::cout << "\tcopy assignment:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;

	std::cout << "new std::uint32_t[]" << std::endl;
	std::cout << "\tsize:\t" << n << std::endl;
	start = std::chrono::system_clock::now();
	std::uint32_t* a2 = new std::uint32_t[n];
	std::memmove(a2, a1, n);
	duration = std::chrono::system_clock::now() - start;
	std::cout << "\tstd::memmove:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	if (a2[1] == 0) std::cout << "";
	delete[] a1;
	delete[] a2;

	std::cout << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
