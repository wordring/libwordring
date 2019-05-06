#include <algorithm>
#include <fstream>
#include <string>
#include <map>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define STRING(str) #str
#define TO_STRING(str) STRING(str)

struct index
{
	std::string name;
	std::vector<std::string> data;
};

struct range
{
	std::string name;
	std::vector<std::pair<std::string, std::string>> data;
};

int main()
{
	std::vector<index> indexes_code_point{};
	range gb18030_range{};

	{
		boost::property_tree::ptree pt;
		boost::property_tree::read_json(TO_STRING(INDEXES_JSON_PATH), pt);

		std::vector<std::string> names{};
		for (auto const& child : pt) names.push_back(child.first);

		for (auto const& name : names)
		{
			std::string cpp_name{ name };
			std::replace(cpp_name.begin(), cpp_name.end(), '-', '_');

			if (name == "gb18030-ranges")
			{
				gb18030_range.name = cpp_name;

				for (auto& v : pt.get_child(name))
				{
					std::vector<std::string> array{};
					for (auto& val : v.second) array.push_back(val.second.data());
					assert(array.size() == 2);
					gb18030_range.data.push_back(std::make_pair(array[0], array[1]));
				}
			}
			else
			{
				indexes_code_point.push_back(index());
				index& current = indexes_code_point.back();
				current.name = cpp_name;

				for (auto& v : pt.get_child(name))
				{
					std::string val = v.second.data();
					assert(val != "0");
					if (val == "null") val = "0";

					current.data.push_back(val);
				}
			}
		}
	}

	// indexes.hpp
	{
		std::ofstream hpp{};
		hpp.open(TO_STRING(INDEXES_HPP_PATH), std::ios::out);

		hpp << "#pragma once" << std::endl;
		hpp << u8"// wordring_cpp/generator/whatwg/encoding/encoding.cpp によって作成された。" << std::endl;
		hpp << std::endl;
		hpp << "#include <array>" << std::endl;
		hpp << "#include <unordered_map>" << std::endl;
		hpp << std::endl;

		hpp << "namespace wordring::whatwg::encoding" << std::endl;
		hpp << "{" << std::endl;

		// index code point
		for (auto const& idx : indexes_code_point) hpp << "\textern std::array<uint32_t, " << idx.data.size() << "> const index_code_point_" << idx.name << ";" << std::endl;
		hpp << std::endl;
		hpp << "\textern std::array<std::pair<uint32_t, uint32_t>, " << gb18030_range.data.size() << "> const index_code_point_" << gb18030_range.name << ";" << std::endl;
		hpp << std::endl;

		// index pointer


		hpp << "}" << std::endl;
		hpp << std::endl;
	}

	// indexes.cpp
	{
		std::ofstream cpp{};
		cpp.open(TO_STRING(INDEXES_CPP_PATH), std::ios::out);

		cpp << u8"// wordring_cpp/generator/whatwg/encoding/encoding.cpp によって作成された。" << std::endl;
		cpp << std::endl;
		cpp << "#include \"indexes.hpp\"" << std::endl;
		cpp << std::endl;

		for (auto const& idx : indexes_code_point)
		{
			cpp << "static std::array<uint32_t, " << idx.data.size() << "> const wordring::whatwg::encoding::index_code_point_" << idx.name << " { ";
			for (auto const& n : idx.data) cpp << n << ", ";
			cpp << "};" << std::endl;
		}
		cpp << std::endl;

		cpp << "static std::array<std::pair<uint32_t, uint32_t>, " << gb18030_range.data.size() << "> const wordring::whatwg::encoding::index_code_point_" << gb18030_range.name << " {{ ";
		for (auto const& pair : gb18030_range.data) cpp << "{ " << pair.first << ", " << pair.second << " }, ";
		cpp << " }};" << std::endl;

		cpp << std::endl;
	}

	return 0;
}
