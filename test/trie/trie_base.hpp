#pragma once

#include <wordring/trie/trie_base.hpp>
#include <wordring/tree/tree_iterator.hpp>

#include <memory>
#include <string>
#include <vector>

#define STRING(str) #str
#define TO_STRING(str) STRING(str)

std::string const english_words_path{ TO_STRING(ENGLISH_WORDS_PATH) };

namespace wordring
{
	inline bool operator==(wordring::trie_node const& lhs, wordring::trie_node const& rhs)
	{
		return lhs.m_base == rhs.m_base && lhs.m_check == rhs.m_check;
	}
}

using base_iterator = wordring::const_trie_base_iterator<std::vector<wordring::trie_node, std::allocator<wordring::trie_node>> const>;

class test_iterator : public base_iterator
{
public:
	using base_iterator::index_type;

	using base_iterator::find;
	using base_iterator::mother;
	using base_iterator::base;
	using base_iterator::tail;
	using base_iterator::has_null;
	using base_iterator::has_sibling;

	using base_iterator::m_c;
	using base_iterator::m_index;

public:
	test_iterator() : const_trie_base_iterator() {}

	test_iterator(container& c, index_type index)
		: const_trie_base_iterator(c, index) {}

	test_iterator(const_trie_base_iterator const& base)
		: const_trie_base_iterator(base) {}
};

class test_trie_base : public wordring::trie_base<>
{
public:
	using wordring::trie_base<>::container;

	using wordring::trie_base<>::reserve;
	using wordring::trie_base<>::free;
	using wordring::trie_base<>::is_free;
	using wordring::trie_base<>::allocate;
	using wordring::trie_base<>::add;

	using wordring::trie_base<>::m_c;

	/*! 状態数を数える
	*/
	std::size_t count_s() const
	{
		std::size_t n = 1; // 状態1のCHECKは常に0なので
		for (std::size_t i = 1; i < m_c.size(); ++i) if (0 < (m_c.data() + i)->m_check) ++n;
		return n;
	}

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
