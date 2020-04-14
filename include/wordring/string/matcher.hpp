#pragma once

#include <initializer_list>
#include <vector>

namespace wordring
{
	/*! @brief プッシュ型文字列マッチャー
	
	@tparam String 対象とする文字列型

	現在の実装は最短マッチしか提供しない。
	*/
	template <typename String>
	class string_matcher
	{
	public:
		using string_type = String;
		using character = typename string_type::value_type;

		enum class match_result : std::uint32_t
		{
			partial = 1,
			succeed,
			failed,
		};

		struct container_entry
		{
			container_entry() : m_failed(false) {}

			container_entry(string_type const& s) : m_s(s), m_failed(false) {}

			string_type m_s;
			bool        m_failed;
		};

		using container = std::vector<container_entry>;

	public:
		/*! @brief 複数の文字列を指定してマッチャーを構築する

		@param il [in] 文字列のリスト
		*/
		string_matcher(std::initializer_list<string_type> il)
			: m_c(il.begin(), il.end())
		{
		}

		/*! @brief これまでに入力された文字のリストを返す
		
		@return 文字列
		*/
		string_type const& data() const { return m_s; }


		/*! @brief これまでに入力された文字数を返す

		@return 文字数
		*/
		auto size() const { return m_s.size(); }

		/*! @brief マッチャーを初期化する
		*/
		void clear()
		{
			for (container_entry& entry : m_c) entry.m_failed = false;
			m_s.clear();
		}

		/*! @brief 文字を追加する
		
		@param [in] ch 文字

		@return
			内蔵する文字列のいずれかにマッチした場合 match_result::succeed 、マッチが完全に失敗した場合 match_result::failed 、
			マッチする可能性のある文字列が残っている場合 match_result::partial を返す。

		match_result::partial 以外を返した場合、clear() を呼び出すまで、このメンバを呼び出してはならない。
		*/
		match_result push_back(character ch)
		{
			std::uint32_t pos = m_s.size();
			std::uint32_t n = 0;

			m_s.push_back(ch);

			auto it1 = m_c.begin();
			auto it2 = m_c.end();
			while (it1 != it2)
			{
				if (it1->m_failed || it1->m_s.size() == pos || it1->m_s.operator[](pos) != ch)
				{
					++n;
					it1->m_failed = true;
				}
				else if (m_s.size() == it1->m_s.size()) return match_result::succeed;

				++it1;
			}

			return (n == m_c.size()) ? match_result::failed : match_result::partial;
		}

	protected:
		container   m_c;
		string_type m_s;
	};

}
