#pragma once

#include <utility>
#include <vector>

namespace wordring
{
	class basic_double_array
	{
	private:
		struct node { uint32_t base, check; };
		using iterator = std::vector<node>::iterator;

	public:
		void erase(uint32_t index);
		void find();
		void insert(uint32_t index, uint8_t label);
		uint32_t next(uint32_t index, uint8_t label);

	private:
		std::vector<node> m_array;
	};
}
