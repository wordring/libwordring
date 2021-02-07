
#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/encoding//terminology.hpp>

#include <string>
#include <string_view>

BOOST_AUTO_TEST_SUITE(whatwg_encoding_io_queue_test)

BOOST_AUTO_TEST_CASE(io_queue_construct_1)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char32_t> q;
	BOOST_CHECK(q.read().m_wait == true);
}

BOOST_AUTO_TEST_CASE(io_queue_read_1)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "1";
	auto q = to_io_queue_convert(sv.begin(), sv.end());

	BOOST_CHECK(**q.read() == '1');
	BOOST_CHECK(q.read()->m_eoq == true);
}

BOOST_AUTO_TEST_CASE(io_queue_read_2)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "1";
	io_queue<char> q;
	q.push(sv.begin(), sv.end());

	BOOST_CHECK(**q.read() == '1');
	BOOST_CHECK(q.read().m_wait == true);
}

BOOST_AUTO_TEST_CASE(io_queue_read_3)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char> q;
	BOOST_CHECK(q.read().m_wait == true);
}

BOOST_AUTO_TEST_CASE(io_queue_read_4)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "123";
	auto q = to_io_queue_convert(sv.begin(), sv.end());

	BOOST_CHECK(**q.read() == '1');
	BOOST_CHECK(**q.read() == '2');
	BOOST_CHECK(**q.read() == '3');
	BOOST_CHECK(q.read()->m_eoq == true);
}

BOOST_AUTO_TEST_CASE(io_queue_read_5)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "123";
	io_queue<char> q;
	q.push(sv.begin(), sv.end());

	BOOST_CHECK(**q.read() == '1');
	BOOST_CHECK(**q.read() == '2');
	BOOST_CHECK(**q.read() == '3');
	BOOST_CHECK(q.read().m_wait == true);
}

BOOST_AUTO_TEST_CASE(io_queue_read_6)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "012";
	auto q = to_io_queue_convert(sv.begin(), sv.end());

	auto v = q.read(3);

	BOOST_CHECK(v.m_wait == false);
	BOOST_CHECK(*v->at(0) == '0');
	BOOST_CHECK(*v->at(1) == '1');
	BOOST_CHECK(*v->at(2) == '2');
	BOOST_CHECK(q.read()->m_eoq == true);
}

BOOST_AUTO_TEST_CASE(io_queue_read_7)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "012";
	io_queue<char> q;
	q.push(sv.begin(), sv.end());

	auto v = q.read(3);

	BOOST_CHECK(v.m_wait == false);
	BOOST_CHECK(*v->at(0) == '0');
	BOOST_CHECK(*v->at(1) == '1');
	BOOST_CHECK(*v->at(2) == '2');
	BOOST_CHECK(q.read().m_wait == true);
}

BOOST_AUTO_TEST_CASE(io_queue_read_8)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char> q;
	BOOST_CHECK(q.read(1).m_wait == true);
}

BOOST_AUTO_TEST_CASE(io_queue_peek_1)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "012";
	auto q = to_io_queue_convert(sv.begin(), sv.end());

	auto v = q.peek(3);

	BOOST_CHECK(v.m_wait == false);
	BOOST_CHECK(*v->at(0) == '0');
	BOOST_CHECK(*v->at(1) == '1');
	BOOST_CHECK(*v->at(2) == '2');
	BOOST_CHECK(q.read().m_wait == false);
	BOOST_CHECK(q.read()->m_eoq == false);
}

BOOST_AUTO_TEST_CASE(io_queue_peek_2)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "012";
	io_queue<char> q;
	q.push(sv.begin(), sv.end());

	auto v = q.peek(3);

	BOOST_CHECK(q.read().m_wait == false);
	BOOST_CHECK(q.read()->m_eoq == false);
}

BOOST_AUTO_TEST_CASE(io_queue_peek_3)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "01";
	io_queue<char> q;
	q.push(sv.begin(), sv.end());

	auto v = q.peek(3);

	BOOST_CHECK(v.m_wait == true);
}

BOOST_AUTO_TEST_CASE(io_queue_peek_4)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char> q;
	BOOST_CHECK(q.peek(1).m_wait == true);
}

BOOST_AUTO_TEST_CASE(io_queue_push_1)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char> q;
	q.push(io_item<char>{ '0' });
	q.push(io_item<char>{ '1' });
	q.push(io_item<char>{ '2' });

	std::string s;
	from_io_queue_convert(q, std::back_inserter(s));

	BOOST_CHECK(s == "012");
}

BOOST_AUTO_TEST_CASE(io_queue_push_2)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char> q;
	q.push('0');
	q.push('1');
	q.push('2');

	std::string s;
	from_io_queue_convert(q, std::back_inserter(s));

	BOOST_CHECK(s == "012");
}

BOOST_AUTO_TEST_CASE(io_queue_push_3)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char> q;

	std::string_view sv = "012";
	q.push(sv.begin(), sv.end());

	std::string s;
	from_io_queue_convert(q, std::back_inserter(s));

	BOOST_CHECK(s == "012");
}

BOOST_AUTO_TEST_CASE(io_queue_prepend_1)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char> q;
	q.prepend(io_item<char>{ '0' });
	q.prepend(io_item<char>{ '1' });
	q.prepend(io_item<char>{ '2' });

	std::string s;
	from_io_queue_convert(q, std::back_inserter(s));

	BOOST_CHECK(s == "210");
}

BOOST_AUTO_TEST_CASE(io_queue_prepend_2)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char> q;
	q.prepend('0');
	q.prepend('1');
	q.prepend('2');

	std::string s;
	from_io_queue_convert(q, std::back_inserter(s));

	BOOST_CHECK(s == "210");
}

BOOST_AUTO_TEST_CASE(io_queue_prepend_3)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char> q;

	std::string_view sv = "012";
	q.prepend(sv.begin(), sv.end());

	std::string s;
	from_io_queue_convert(q, std::back_inserter(s));

	BOOST_CHECK(s == "012");
}

BOOST_AUTO_TEST_CASE(io_queue_prepend_4)
{
	using namespace wordring::whatwg::encoding;

	io_queue<char> q;

	std::string_view sv1 = "012";
	q.prepend(sv1.begin(), sv1.end());

	std::string_view sv = "345";
	q.prepend(sv.begin(), sv.end());

	std::string s;
	from_io_queue_convert(q, std::back_inserter(s));

	BOOST_CHECK(s == "345012");
}


BOOST_AUTO_TEST_CASE(io_queue_convert_into_1)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "012";
	auto q = to_io_queue_convert(sv.begin(), sv.end());

	std::string s;
	from_io_queue_convert(q, std::back_inserter(s));

	BOOST_CHECK(s == "012");
}

BOOST_AUTO_TEST_CASE(io_queue_convert_into_io_queue_1)
{
	using namespace wordring::whatwg::encoding;

	std::string_view sv = "012";
	auto q = to_io_queue_convert(sv.begin(), sv.end());

	std::string s;
	from_io_queue_convert(q, std::back_inserter(s));

	BOOST_CHECK(s == "012");
}


BOOST_AUTO_TEST_SUITE_END()
