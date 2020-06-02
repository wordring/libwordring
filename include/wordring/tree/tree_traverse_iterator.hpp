#pragma once

#include <deque>
#include <iterator>

namespace wordring
{
	/*! @class basic_tree_traverse_iterator tree_traverse_iterator.hpp wordring/tree/tree_traverse_iterator.hpp

	@brief 階層の通知付きで木を走査するイテレータ・アダプター

	@tparam Iterator  ベースとなる木のイテレータ
	@tparam Container detail::tree_iterator_stack あるいは detail::tree_iterator_queue
	@tparam Allocator アロケータ

	プレ・オーダー走査用の <b>tree_iterator</b> とレベル・オーダー走査用の
	<b>level_order_tree_iterator</b> が事前に定義されている。

	@code
		template <typename Iterator, typename  Allocator = std::allocator<Iterator>>
		using tree_iterator = basic_tree_iterator<Iterator, detail::tree_iterator_stack<Iterator, Allocator>, Allocator>;

		template <typename Iterator, typename  Allocator = std::allocator<Iterator>>
		using level_order_tree_iterator = basic_tree_iterator<Iterator, detail::tree_iterator_queue<Iterator, Allocator>, Allocator>;
	@endcode

	このクラスは、道順の調査に、スタックあるいはキューを使う。
	従って、コピーのコストは相応に高い。

	<b>operator++()</b> によって、走査順に従い次の要素へ移動する。
	<b>operator*()</b> によって、指している要素の逆参照を得る。
	元となる木のイテレータを得るには、 <b>base()</b> を使う。

	走査順のイメージを以下に示す。\n
	※要素についている番号は移動順を示す。

	@par プレ・オーダー

	@image html tree_traverse_pre_order.svg

	@par レベル・オーダー

	@image html tree_traverse_level_order.svg
	*/
	template <typename Iterator>
	class basic_tree_traverse_iterator
	{
	public:
		using base_type = Iterator;

	protected:
		std::deque<base_type> m_x;
		std::deque<base_type> m_y;
	};
}
