#pragma once

#include <algorithm>
#include <deque>
#include <iterator>
#include <optional>
#include <vector>

// ------------------------------------------------------------------------------------------------
// 3. Terminology
//
// https://encoding.spec.whatwg.org/#terminology
// https://triple-underscore.github.io/Encoding-ja.html#terminology
// ------------------------------------------------------------------------------------------------

namespace wordring::whatwg::encoding
{
	/*! @brief 文字にキュー終端表現を追加するラッパー
	*
	* EOF を表現するために必要となった。
	* 規格は item と呼んでいる。
	* 
	* @sa https://triple-underscore.github.io/Encoding-ja.html#concept-stream
	*/
	template <typename T>
	struct io_item
	{
		T m_value = T();
		bool m_eoq = false;

		T& operator*() { return m_value; }
	};

	/*! @brief 待機状態を追加するラッパー
	*/
	template <typename T>
	struct io_result
	{
		using value_type = T;

		value_type m_value;
		bool m_wait = false;

		T* operator->() { return &m_value; }
		T& operator*() { return m_value; }
	};

	/*! @brief 入出力ストリーム
	*
	* @tparam T byte 、 ユニコード・スカラー値、コード・ポイント等の値型
	*
	* @sa https://triple-underscore.github.io/Encoding-ja.html#concept-stream
	*/
	template <typename T>
	class io_queue
	{
	public:
		using value_type = io_item<T>;
		using character_type = T;

	public:
		io_queue() : m_q() {}

		/*! @brief 値を一つ読み取る
		*
		* @return キューが空の場合、待機（m_wait == true）を返す
		* 
		* @sa https://encoding.spec.whatwg.org/#concept-stream-read
		* @sa https://triple-underscore.github.io/Encoding-ja.html#concept-stream-read
		* 
		* @internal
		* <hr>
		* 
		* キューが空の場合、オブジェクトは待機状態となる。
		* EOF は空の値とみなされないらしい。
		* 戻り値として　EOF を返す場合がある。
		* EOF はキューから決して取り除かれない。
		*/
		io_result<value_type> read()
		{
			if (m_q.empty()) return io_result<value_type>{ value_type{}, true };

			value_type c = m_q.front();
			if (c.m_eoq) return io_result<value_type>{ c, false };
			m_q.pop_front();
			return io_result<value_type>{ c, false };
		}

		/*! @brief 値を読み取る
		*
		* @param [in]  n   読み取る個数
		* 
		* @return キューが n に満たない場合、待機（m_wait == true）を返す
		*
		* @sa https://encoding.spec.whatwg.org/#concept-stream-read
		* @sa https://triple-underscore.github.io/Encoding-ja.html#concept-stream-read
		* 
		* @internal
		* <hr>
		* 
		* 一個読み取る部分で待機となるので、キューが満たない場合、オブジェクトは待機状態となる。
		* EOF を含む場合、 n より一つ少ない値が返される場合があり得そう。
		* 
		* 待機状態があるため、規格から改変が必要。
		*/
		io_result<std::vector<value_type>> read(std::uint32_t n)
		{
			// 1.
			std::vector<value_type> readItems;
			// 規格に追加
			if (m_q.size() < n) return { readItems, true }; // 待機を返す
			// 2.
			for (std::uint32_t i = 0; i < n; ++i) readItems.push_back(*read());
			// 3.
			if (readItems.back().m_eoq) readItems.pop_back();
			// 4.
			return { readItems, false };
		}

		/*! @brief Peek
		*
		* @sa https://triple-underscore.github.io/Encoding-ja.html#i-o-queue-peek
		* 
		*/
		io_result<std::vector<value_type>> peek(std::uint32_t n)
		{
			assert(1 <= n);

			// 1.
			if (m_q.size() < n && !(!m_q.empty() && m_q.back().m_eoq)) return { {}, true };
			// 2.
			std::vector<value_type> prefix;
			// 3.
			for (value_type const& p : m_q)
			{
				if (p.m_eoq) break;
				prefix.push_back(p);
			}
			// 4.
			return { prefix, false };
		}

		/*! @brief Push
		*
		* @sa https://triple-underscore.github.io/Encoding-ja.html#concept-stream-push
		* 
		* @internal
		* <hr>
		* 
		* 二重に EOQ が追加されることは無さそう。
		*/
		void push(value_type item)
		{
			// 1.
			if (!m_q.empty() && m_q.back().m_eoq)
			{
				if (item.m_eoq);
				else m_q.insert(--m_q.end(), item);
			}
			// 2.
			else m_q.push_back(item);
		}

		void push(T ch)
		{
			push(value_type{ ch, false });
		}

		template <typename InputIterator>
		void push(InputIterator first, InputIterator last)
		{
			while (first != last)
			{
				push(*first);
				++first;
			}
		}

		/*! @brief Prepend
		*
		* @sa https://triple-underscore.github.io/Encoding-ja.html#concept-stream-prepend
		*/
		void prepend(value_type item)
		{
			//assert(!item.m_eoq); ISO2022_JP が EOQ を挿入する場合がある。
			m_q.push_front(item);
		}

		/*! @brief Prepend
		*
		* @sa https://triple-underscore.github.io/Encoding-ja.html#concept-stream-prepend
		*/
		void prepend(T ch)
		{
			prepend(value_type{ ch, false });
		}

		/*! @brief Prepend
		*
		* @sa https://triple-underscore.github.io/Encoding-ja.html#concept-stream-prepend
		*/
		template <typename InputIterator>
		void prepend(InputIterator first, InputIterator last)
		{
			auto it = m_q.begin();
			while (first != last) it = ++m_q.insert(it, value_type{ static_cast<T>(*first++), false });
		}

	private:
		std::deque<value_type> m_q;
	};

	/*! @brief io_queue を文字列へ変換する
	*
	* @sa https://encoding.spec.whatwg.org/#from-i-o-queue-convert
	* 
	* @internal
	* <hr>
	* 
	* 読み込んだ文字はキューから消費されるらしい。
	*/
	template <typename T, typename OutputIterator>
	inline void from_io_queue_convert(io_queue<T> q, OutputIterator out)
	{
		while (true)
		{
			auto item = q.read();
			if (item.m_wait || item->m_eoq) return;
			*out++ = **item;
		}
	}

	/*! @brief 文字列を io_queue へ変換する
	*
	* @sa https://triple-underscore.github.io/Encoding-ja.html#to-i-o-queue-convert
	*/
	template <typename InputIterator>
	inline auto to_io_queue_convert(InputIterator first, InputIterator last)
	{
		using value_type = typename std::iterator_traits<InputIterator>::value_type;
		io_queue<value_type> q;
		q.push(first, last);
		q.push({ 0, true });
		return q;
	}
}
