#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <iostream>

#define STRING(str) #str
#define TO_STRING(str) STRING(str)

int main()
{
	std::map<std::string, uint16_t> word_map{};
	std::set<std::vector<uint16_t>> positive_list{}, negative_list{};

	{
		std::filesystem::path data_path{ TO_STRING(PSL_DATA_PATH) };
		std::ifstream in{ data_path };
		assert(!in.fail());

		std::set<std::vector<std::string>> positive_string_list{}, negative_string_list{};

		std::string line{};
		while (std::getline(in, line))
		{
			bool positive{ true };

			boost::algorithm::trim(line);
			if (line.empty()) continue;
			if(boost::algorithm::starts_with(line, "//")) continue;

			if (boost::algorithm::starts_with(line, "!"))
			{
				positive = false;
				line = line.substr(1);
			}

			std::vector<std::string> words{};
			boost::algorithm::split(words, line, boost::is_any_of("."));
			std::reverse(words.begin(), words.end());
			for (std::string word : words) word_map[word] = 0;
			
			if (positive) positive_string_list.insert(words);
			else negative_string_list.insert(words);
		}

		uint16_t i{ 1 };
		for (auto& entry : word_map) entry.second = i++;

		for (auto& entry : positive_string_list)
		{
			std::vector<uint16_t> positive{};
			for (auto& s : entry) positive.push_back(word_map[s]);
			positive_list.insert(positive);
		}

		for (auto& entry : negative_string_list)
		{
			std::vector<uint16_t> negative{};
			for (auto& s : entry) negative.push_back(word_map[s]);
			negative_list.insert(negative);
		}
	}

	std::cout << word_map.size() << std::endl;
	for (auto m : word_map) std::cout << m.first << ": " << m.second << std::endl;

	return 0;
}