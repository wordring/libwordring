// test/http/simple_server.cpp

#include <boost/test/unit_test.hpp>

#include <wordring/http/simple_server.hpp>

#include <filesystem>
#include <iostream>

#define STRING(str) #str
#define TO_STRING(str) STRING(str)

std::string current_binary_dir = TO_STRING(CURRENT_BINARY_DIR);

BOOST_AUTO_TEST_SUITE(simple_server_test)

BOOST_AUTO_TEST_CASE(request_matcher_1)
{
	using namespace wordring::http;

	request_matcher_item item;
	item.m_regex = "";

	std::cout << std::filesystem::temp_directory_path() << std::endl;
	std::cout << current_binary_dir << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
