
#include <wordring/trie/trie.hpp>
#include <wordring/whatwg/infra/infra.hpp>
#include <wordring/whatwg/infra/unicode.hpp>

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>

#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <unordered_map>

#define STRING(str) #str
#define TO_STRING(str) STRING(str)

std::filesystem::path data_path{ TO_STRING(DATA_PATH) };
std::filesystem::path atom_hpp_path{ TO_STRING(ATOM_HPP_PATH) };
std::filesystem::path atom_defs_hpp_path{ TO_STRING(ATOM_DEFS_HPP_PATH) };
std::filesystem::path atom_cpp_path{ TO_STRING(ATOM_CPP_PATH) };

/*
引数で指定されたファイルを読み込み、行で分割して配列として返す

テンプレート引数　Container 配列の配列が渡された場合、行をさらにタブ基準で分割し、格納する
*/
template <typename Container = std::vector<std::u32string>>
Container read(std::filesystem::path const& path)
{
	using namespace wordring::whatwg;

	Container result;

	auto in = std::ifstream(path);
	assert(!in.fail());

	std::vector<std::u32string> buf;
	std::string line;
	while (std::getline(in, line))
	{
		boost::algorithm::trim(line);
		if (line.empty()) continue;
		buf.push_back(encoding_cast<std::u32string>(line));
	}

	if constexpr (std::is_same_v<Container, std::vector<std::u32string>>)
	{
		std::sort(buf.begin(), buf.end());
		buf.erase(std::unique(buf.begin(), buf.end()), buf.end());
		return buf;
	}
	else
	{
		for (std::u32string const& s : buf)
		{
			std::vector<std::u32string> tmp;
			boost::algorithm::split(tmp, s, boost::is_any_of(U"\t"));
			typename Container::value_type a;
			assert(tmp.size() <= a.size());
			for (std::uint32_t i = 0; i < tmp.size(); ++i) a[i] = tmp[i];
			result.push_back(a);
		}
	}

	return result;
}

std::string cpp_cast(std::u32string const& s)
{
	using namespace wordring::whatwg;

	std::string ret = encoding_cast<std::string>(s);
	for (auto& c : ret) if (!is_ascii_alphanumeric(c)) c = '_';
	if (is_ascii_lower_alpha(ret[0])) ret[0] -= 0x20;

	return ret;
}

int main()
{
	using namespace wordring;
	using namespace wordring::whatwg;

	// ファイル読み込み --------------------------------------------------------

	// HTML タグ名
	auto html_elements = read(data_path / "html_elements.txt");
	// SVGタグ名
	auto svg_elements = read(data_path / "svg_elements.txt");
	// MathML タグ名
	auto mathml_elements = read(data_path / "mathml_elements.txt");

	// HTML 属性
	auto html_attributes = read(data_path / "html_attributes.txt");
	// HTML イベント
	auto html_events = read(data_path / "html_events.txt");
	// SVG 属性
	auto svg_attributes = read(data_path / "svg_attributes.txt");
	// MathML属性名
	auto mathml_attributes = read(data_path / "mathml_attributes.txt");

	// 外来属性 [ attr, prefix, local_name ]
	auto foreign_attributes = read<std::vector<std::array<std::u32string, 4>>>(data_path / "foreign_attributes_conversion.txt");

	// 名前空間
	auto namespaces = read<std::vector<std::array<std::u32string, 2>>>(data_path / "namespaces.txt");

	// 名前指定文字参照表
	auto named_character_reference = read<std::vector<std::array<std::u32string, 3>>>(data_path / "named_character_reference.txt");
	// 文字参照コード変換表
	auto character_reference_code = read<std::vector<std::array<std::u32string, 2>>>(data_path / "character_reference_code.txt");

	// SVG属性変換表
	auto svg_attributes_conversion_tbl = read<std::vector<std::array<std::u32string, 2>>>(data_path / "svg_attributes_conversion.txt");
	// 外来属性変換表
	auto foreign_attributes_conversion_tbl = read<std::vector<std::array<std::u32string, 4>>>(data_path / "foreign_attributes_conversion.txt");
	// 互換性モードテーブル
	auto quirks_mode_tbl = read(data_path / "quirks_mode.txt");
	// SVGタグ名変換表
	auto svg_elements_conversion_tbl = read<std::vector<std::array<std::u32string, 2>>>(data_path/ "svg_elements_conversion.txt");
	// タグ名 -----------------------------------------------------------------

	std::vector<std::u32string> tag_names;
	{
		tag_names = html_elements;
		for (auto const& s : svg_elements)
		{
			std::u32string lower;
			to_ascii_lowercase(s.begin(), s.end(), std::back_inserter(lower));
			tag_names.push_back(s);
			tag_names.push_back(lower);
		}
		for (auto const& s : mathml_elements)
		{
			std::u32string lower;
			to_ascii_lowercase(s.begin(), s.end(), std::back_inserter(lower));
			tag_names.push_back(s);
			tag_names.push_back(lower);
		}

		for (auto const& a : svg_elements_conversion_tbl)
		{
			tag_names.push_back(a[0]);
			tag_names.push_back(a[1]);
		}

		std::sort(tag_names.begin(), tag_names.end());
		tag_names.erase(std::unique(tag_names.begin(), tag_names.end()), tag_names.end());
	}
	// アトム集合
	std::unordered_map<std::u32string, std::uint32_t> tag_atom_tbl;
	for (std::uint32_t i = 0; i < tag_names.size(); ++i) tag_atom_tbl[tag_names[i]] = i + 1;

	// 属性名 -----------------------------------------------------------------

	std::vector<std::u32string> attribute_names;
	{
		attribute_names = html_attributes;
		std::copy(html_events.begin(), html_events.end(), std::back_inserter(attribute_names));
		for (auto const& s : svg_attributes)
		{
			if (s.empty()) continue;
			std::u32string lower;
			to_ascii_lowercase(s.begin(), s.end(), std::back_inserter(lower));
			attribute_names.push_back(s);
			attribute_names.push_back(lower);
		}
		for (auto const& s : mathml_attributes)
		{
			if (s.empty()) continue;
			std::u32string lower;
			to_ascii_lowercase(s.begin(), s.end(), std::back_inserter(lower));
			attribute_names.push_back(s);
			attribute_names.push_back(lower);
		}
		for (auto const& a : foreign_attributes)
		{
			std::u32string const& name       = a[0];
			//std::u32string const& prefix     = a[1];
			std::u32string const& local_name = a[2];

			std::u32string lower;
			to_ascii_lowercase(name.begin(), name.end(), std::back_inserter(lower));
			if(!name.empty()) attribute_names.push_back(name);
			if (!lower.empty()) attribute_names.push_back(lower);

			lower.clear();
			to_ascii_lowercase(local_name.begin(), local_name.end(), std::back_inserter(lower));
			if (!local_name.empty()) attribute_names.push_back(local_name);
			if (!lower.empty()) attribute_names.push_back(lower);
		}
		for (auto const& a : svg_attributes_conversion_tbl)
		{
			if (!a[0].empty()) attribute_names.push_back(a[0]);
			if (!a[1].empty()) attribute_names.push_back(a[1]);
		}
		for (auto const& a : foreign_attributes_conversion_tbl)
		{
			if (!a[0].empty()) attribute_names.push_back(a[0]);
			if (!a[1].empty()) attribute_names.push_back(a[1]);
		}

		std::sort(attribute_names.begin(), attribute_names.end());
		attribute_names.erase(std::unique(attribute_names.begin(), attribute_names.end()), attribute_names.end());
	}
	// アトム集合
	std::unordered_map<std::u32string, std::uint32_t> attribute_atom_tbl;
	for (std::uint32_t i = 0; i < attribute_names.size(); ++i) attribute_atom_tbl[attribute_names[i]] = i + 1;

	// 名前指定文字参照表索引を作成
	std::uint32_t named_character_reference_max_length = 0;
	wordring::trie<char32_t> named_character_reference_idx_tbl;
	std::vector<std::array<std::u32string, 2>> named_character_reference_map_tbl;
	{
		std::uint32_t i = 0;
		for (auto const& a : named_character_reference)
		{
			named_character_reference_max_length = std::max(a[0].size(), named_character_reference_max_length);
			named_character_reference_idx_tbl.insert(a[0], i++);
			named_character_reference_map_tbl.push_back({ a[1], a[2] });
		}
	}

	// atom_defs.hpp
	{
		using namespace wordring::whatwg;

		std::ofstream hpp;
		hpp.open(atom_defs_hpp_path, std::ios::out);
		assert(!hpp.fail());

		hpp << "#pragma once" << std::endl;
		hpp << "// generated by wordring_cpp/generator/whatwg/html/atom_tbl.cpp" << "" << std::endl;
		hpp << std::endl;
		hpp << "#include <cstdint>" << std::endl;
		hpp << std::endl;
		hpp << "namespace wordring::whatwg::html::parsing" << std::endl;
		hpp << "{" << std::endl;

		// 名前空間
		hpp << "\t" << "enum class ns_name : std::uint32_t" << std::endl;
		hpp << "\t" << "{" << std::endl;
		for (std::uint32_t i = 0; i < namespaces.size(); ++i)
			hpp << "\t\t" << cpp_cast(namespaces[i][0]) << " = " << i + 1 << "," << std::endl;
		hpp << "\t};" << std::endl;
		hpp << std::endl;

		// タグ名
		hpp << "\t" << "enum class tag_name : std::uint32_t" << std::endl;
		hpp << "\t" << "{" << std::endl;
		for (std::uint32_t i = 0; i < tag_names.size(); ++i)
			hpp << "\t\t" << cpp_cast(tag_names[i]) << " = " << i + 1 << "," << std::endl;
		hpp << "\t};" << std::endl;
		hpp << std::endl;

		// 属性名
		hpp << "\t" << "enum class attribute_name : std::uint32_t" << std::endl;
		hpp << "\t" << "{" << std::endl;
		for (std::uint32_t i = 0; i < attribute_names.size(); ++i)
			hpp << "\t\t" << cpp_cast(attribute_names[i]) << " = " << i + 1 << "," << std::endl;
		hpp << "\t};" << std::endl;
		hpp << std::endl;

		hpp << "}" << std::endl;
	}

	// atom_tbl.hpp
	{
		std::ofstream hpp;
		hpp.open(atom_hpp_path, std::ios::out);
		assert(!hpp.fail());

		hpp << "#pragma once" << std::endl;
		hpp << "// generated by wordring_cpp/generator/whatwg/html/atom_tbl.cpp" << "" << std::endl;
		hpp << std::endl;
		hpp << "#include <wordring/whatwg/html/parsing/atom_defs.hpp>" << std::endl;
		hpp << std::endl;
		hpp << "#include <wordring/string/atom.hpp>" << std::endl;
		hpp << "#include <wordring/trie/trie.hpp>" << std::endl;
		hpp << std::endl;
		hpp << "#include <array>" << std::endl;
		hpp << "#include <string>" << std::endl;
		hpp << "#include <unordered_map>" << std::endl;
		hpp << "#include <unordered_set>" << std::endl;
		hpp << std::endl;
		hpp << "namespace wordring::whatwg::html::parsing" << std::endl;
		hpp << "{" << std::endl;

		// タグ・アトム表
		hpp << "\t" << "extern std::unordered_map<std::u32string, tag_name> const tag_atom_tbl;" << std::endl;
		hpp << std::endl;
		// 属性アトム表
		hpp << "\t" << "extern std::unordered_map<std::u32string, attribute_name> const attribute_atom_tbl;" << std::endl;
		hpp << std::endl;
		// 名前空間アトム表
		hpp << "\t" << "extern std::unordered_map<std::u32string, ns_name> const ns_uri_atom_tbl;" << std::endl;
		hpp << std::endl;

		// タグ文字列表
		hpp << "\t" << "extern std::array<std::u32string, " << tag_names.size() + 1 << "> const tag_name_tbl;" << std::endl;
		hpp << std::endl;
		// 属性文字列表
		hpp << "\t" << "extern std::array<std::u32string, " << attribute_names.size() + 1 << "> const attribute_name_tbl;" << std::endl;
		hpp << std::endl;
		// 名前空間URI文字列表
		hpp << "\t" << "extern std::array<std::u32string, " << namespaces.size() + 1 << "> const ns_uri_tbl;" << std::endl;
		hpp << std::endl;

		// 名前指定文字参照表
		hpp << "\t" << "std::uint32_t constexpr named_character_reference_max_length = " << named_character_reference_max_length << ";" << std::endl;
		hpp << "\t" << "extern wordring::trie<char32_t> const named_character_reference_idx_tbl;" << std::endl;
		hpp << "\t" << "extern std::array<std::array<char32_t, 2>, " << named_character_reference_map_tbl.size() << "> const named_character_reference_map_tbl;" << std::endl;
		hpp << std::endl;
		// 文字参照コード変換表
		hpp << "\t" << "extern std::unordered_map<char32_t, char32_t> const character_reference_code_tbl;" << std::endl;
		hpp << std::endl;

		// SVG属性変換表
		hpp << "\t" << "extern std::unordered_map<std::u32string, std::u32string> const svg_attributes_conversion_tbl;" << std::endl;
		hpp << std::endl;
		// 外来属性変換表
		hpp << "\tstruct foreign_attributes_conversion_entry" << std::endl;
		hpp << "\t{" << std::endl;
		hpp << "\t\tstd::u32string m_prefix;" << std::endl;
		hpp << "\t\tstd::u32string m_local_name;" << std::endl;
		hpp << "\t\tns_name m_namespace;" << std::endl;
		hpp << "\t}; " << std::endl;
		hpp << "\t" << "extern std::unordered_map<std::u32string, foreign_attributes_conversion_entry> const foreign_attributes_conversion_tbl;" << std::endl;
		hpp << std::endl;
		// 互換性モード表
		hpp << "\t" << "extern wordring::trie<char32_t> const quirks_mode_tbl;" << std::endl;
		hpp << std::endl;
		// SVGタグ名変換表
		hpp << "\t" << "extern std::unordered_map<std::u32string, std::u32string> const svg_elements_conversion_tbl;" << std::endl;

		hpp << "}" << std::endl;
	}

	// atom_tbl.cpp
	{
		std::ofstream cpp;
		cpp.open(atom_cpp_path, std::ios::out);
		assert(!cpp.fail());

		cpp << "// generated by wordring_cpp/generator/whatwg/html/atom_tbl.cpp" << "" << std::endl;
		cpp << std::endl;
		cpp << "#include <wordring/whatwg/html/parsing/atom_tbl.hpp>" << std::endl;
		cpp << std::endl;
		cpp << "using namespace wordring::whatwg::html::parsing;" << std::endl;
		cpp << std::endl;

		// タグ名・アトム表
		cpp << "std::unordered_map<std::u32string, tag_name> const wordring::whatwg::html::parsing::tag_atom_tbl = {" << std::endl;
		for (auto const& s : tag_names) cpp << "\t{ U\"" << encoding_cast<std::string>(s) << "\", tag_name::" <<  cpp_cast(s) << " }," << std::endl;
		cpp << "};" << std::endl;
		cpp << std::endl;

		// 属性名・アトム表
		cpp << "std::unordered_map<std::u32string, attribute_name> const wordring::whatwg::html::parsing::attribute_atom_tbl = {" << std::endl;
		for (auto const& s : attribute_names) cpp << "\t{ U\"" << encoding_cast<std::string>(s) << "\", attribute_name::" << cpp_cast(s) << " }," << std::endl;
		cpp << "};" << std::endl;
		cpp << std::endl;

		// 名前空間アトム表
		cpp << "std::unordered_map<std::u32string, ns_name> const wordring::whatwg::html::parsing::ns_uri_atom_tbl = {" << std::endl;
		for (auto const& a : namespaces) cpp << "\t{ U\"" << encoding_cast<std::string>(a[1]) << "\", ns_name::" << encoding_cast<std::string>(a[0]) << " }," << std::endl;
		cpp << "};" << std::endl;
		cpp << std::endl;

		// タグ名・文字列表
		cpp << "std::array<std::u32string, " << tag_names.size() + 1 << "> const wordring::whatwg::html::parsing::tag_name_tbl = {{" << std::endl;
		{
			std::vector<std::u32string> v(1, U"");
			std::copy(tag_names.begin(), tag_names.end(), std::back_inserter(v));
			std::uint32_t n = 0;
			for (std::u32string const& s : v)
			{
				++n;
				if (n == 1) cpp << "\t";
				cpp << "U\"" << encoding_cast<std::string>(s) << "\", ";
				if (n == 9)
				{
					cpp << std::endl;
					n = 0;
				}
			}
		}
		cpp << "}};" << std::endl;
		cpp << std::endl;

		// 属性名・文字列表
		cpp << "std::array<std::u32string, " << attribute_names.size() + 1 << "> const wordring::whatwg::html::parsing::attribute_name_tbl = {{" << std::endl;
		{
			std::vector<std::u32string> v(1, U"");
			std::copy(attribute_names.begin(), attribute_names.end(), std::back_inserter(v));
			std::uint32_t n = 0;
			for (std::u32string const& s : v)
			{
				++n;
				if (n == 1) cpp << "\t";
				cpp << "U\"" << encoding_cast<std::string>(s) << "\", ";
				if (n == 9)
				{
					cpp << std::endl;
					n = 0;
				}
			}
		}
		cpp << "}};" << std::endl;
		cpp << std::endl;

		// 名前空間・文字列表
		cpp << "std::array<std::u32string, " << namespaces.size() + 1 << "> const wordring::whatwg::html::parsing::ns_uri_tbl = {{" << std::endl;
		{
			std::vector<std::array<std::u32string, 2>> v(1, { U"", U"" });
			std::copy(namespaces.begin(), namespaces.end(), std::back_inserter(v));
			for (auto const& a : v) cpp << "\tU\"" << encoding_cast<std::string>(a[1]) << "\", " << std::endl;
		}
		cpp << "}};" << std::endl;
		cpp << std::endl;

		// 名前指定文字参照表
		cpp << "wordring::trie<char32_t> const wordring::whatwg::html::parsing::named_character_reference_idx_tbl = {" << std::endl;
		{
			auto it1 = named_character_reference_idx_tbl.ibegin();
			auto it2 = named_character_reference_idx_tbl.iend();
			std::uint32_t n = 0;
			for (std::uint32_t i = 0; it1 != it2; ++i)
			{
				++n;
				std::int32_t j = *it1++;
				if (n == 1) cpp << "\t";
				if (i % 2 == 0) cpp << "{ " << j << ", ";
				else cpp << j << " }, ";
				if (n == 20)
				{
					cpp << std::endl;
					n = 0;
				}
			}
		}
		cpp << std::endl;
		cpp << "};" << std::endl;
		cpp << std::endl;

		cpp << "std::array<std::array<char32_t, 2>, " << named_character_reference_map_tbl.size() << "> const wordring::whatwg::html::parsing::named_character_reference_map_tbl = {{" << std::endl;
		{
			std::uint32_t n = 0;
			for (std::array<std::u32string, 2> const& a : named_character_reference_map_tbl)
			{
				++n;
				if (n == 1) cpp << "\t";
				cpp << "{ " << encoding_cast<std::string>(a[0]) << ", "
					<< (a[1].empty() ? "0" : encoding_cast<std::string>(a[1])) << " }, ";
				if (n == 10)
				{
					cpp << std::endl;
					n = 0;
				}
			}
		}
		cpp << std::endl;
		cpp << "}};" << std::endl;
		cpp << std::endl;

		// 文字参照コード変換表
		cpp << "std::unordered_map<char32_t, char32_t> const wordring::whatwg::html::parsing::character_reference_code_tbl = {" << std::endl;
		{
			std::uint32_t n = 0;
			for (std::array<std::u32string, 2> const& a : character_reference_code)
			{
				++n;
				if (n == 1) cpp << "\t";
				cpp << "{ " << encoding_cast<std::string>(a[0]) << ", " << encoding_cast<std::string>(a[1]) << " }, ";
				if (n == 10)
				{
					cpp << std::endl;
					n = 0;
				}
			}
		}
		cpp << std::endl;
		cpp << "};" << std::endl;
		cpp << std::endl;

		// SVG属性変換表
		cpp << "std::unordered_map<std::u32string, std::u32string> const wordring::whatwg::html::parsing::svg_attributes_conversion_tbl = {" << std::endl;
		{
			std::uint32_t n = 0;
			for (std::array<std::u32string, 2>& a : svg_attributes_conversion_tbl)
			{
				++n;
				if (n == 1) cpp << "\t";
				cpp << "{ U\"" << encoding_cast<std::string>(a[0]) << "\", U\"" << encoding_cast<std::string>(a[1]) << "\" }, ";
				if (n == 6)
				{
					cpp << std::endl;
					n = 0;
				}
			}
		}
		cpp << std::endl;
		cpp << "};" << std::endl;
		cpp << std::endl;

		// 外来属性変換表
		cpp << "std::unordered_map<std::u32string, wordring::whatwg::html::parsing::foreign_attributes_conversion_entry> const wordring::whatwg::html::parsing::foreign_attributes_conversion_tbl = {" << std::endl;
		{
			std::uint32_t n = 0;
			for (std::array<std::u32string, 4> const& a : foreign_attributes_conversion_tbl)
			{
				++n;
				if (n == 1) cpp << "\t";
				cpp << "{ U\"" << encoding_cast<std::string>(a[0]) << "\", "
					<< "{ U\"" << encoding_cast<std::string>(a[1]) << "\", "
					<< "U\"" << encoding_cast<std::string>(a[2]) << "\", "
					<< "ns_name::" << encoding_cast<std::string>(a[3]) << " } }, ";
				if (n == 6)
				{
					cpp << std::endl;
					n = 0;
				}
			}
		}
		cpp << std::endl;
		cpp << "};" << std::endl;
		cpp << std::endl;

		// 互換性モードテーブル
		cpp << "wordring::trie<char32_t> const wordring::whatwg::html::parsing::quirks_mode_tbl = {" << std::endl;
		{
			std::sort(quirks_mode_tbl.begin(), quirks_mode_tbl.end());
			quirks_mode_tbl.erase(std::unique(quirks_mode_tbl.begin(), quirks_mode_tbl.end()), quirks_mode_tbl.end());
			auto qmode_tbl = wordring::trie<char32_t>(quirks_mode_tbl.begin(), quirks_mode_tbl.end());
			auto it1 = qmode_tbl.ibegin();
			auto it2 = qmode_tbl.iend();
			std::uint32_t n = 0;
			for (std::uint32_t i = 0; it1 != it2; ++i)
			{
				++n;
				std::int32_t j = *it1++;
				if (n == 1) cpp << "\t";
				if (i % 2 == 0) cpp << "{ " << j << ", ";
				else cpp << j << " }, ";
				if (n == 20)
				{
					cpp << std::endl;
					n = 0;
				}
			}
		}
		cpp << std::endl;
		cpp << "};" << std::endl;
		cpp << std::endl;

		// SVGタグ名変換表
		cpp << "std::unordered_map<std::u32string, std::u32string> const wordring::whatwg::html::parsing::svg_elements_conversion_tbl = {" << std::endl;
		{
			std::uint32_t n = 0;
			for (std::array<std::u32string, 2> const& a : svg_elements_conversion_tbl)
			{
				++n;
				if (n == 1) cpp << "\t";
				cpp << "{ U\"" << encoding_cast<std::string>(a[0]) << "\", U\"" << encoding_cast<std::string>(a[1]) << "\" }, ";
				if (n == 10)
				{
					cpp << std::endl;
					n = 0;
				}
			}
		}
		cpp << std::endl;
		cpp << "};" << std::endl;
	}

	return 0;
}
