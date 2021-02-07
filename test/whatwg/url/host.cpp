#include <boost/test/unit_test.hpp>

#include <wordring/whatwg/url/host.hpp>
#include <wordring/whatwg/url/url_defs.hpp>

#include <wordring/compatibility.hpp>

#include <algorithm>
#include <string>
#include <string_view>

BOOST_AUTO_TEST_SUITE(whatwg_url_host_test)


BOOST_AUTO_TEST_CASE(host__1)
{
	using namespace wordring::whatwg;

}

BOOST_AUTO_TEST_CASE(host_is_forbidden_host_code_point_1)
{
	using namespace wordring::whatwg;

	BOOST_CHECK(is_forbidden_host_code_point(0));
	BOOST_CHECK(is_forbidden_host_code_point(0x9u));
	BOOST_CHECK(is_forbidden_host_code_point(0xAu));
	BOOST_CHECK(is_forbidden_host_code_point(0xDu));
	BOOST_CHECK(is_forbidden_host_code_point(0x20u));
	BOOST_CHECK(is_forbidden_host_code_point(0x23u));
	BOOST_CHECK(is_forbidden_host_code_point(0x25u));
	BOOST_CHECK(is_forbidden_host_code_point(0x2Fu));
	BOOST_CHECK(is_forbidden_host_code_point(0x3Au));
	BOOST_CHECK(is_forbidden_host_code_point(0x3Fu));
	BOOST_CHECK(is_forbidden_host_code_point(0x40u));
	BOOST_CHECK(is_forbidden_host_code_point(0x5Bu));
	BOOST_CHECK(is_forbidden_host_code_point(0x5Cu));
	BOOST_CHECK(is_forbidden_host_code_point(0x5Du));
}

BOOST_AUTO_TEST_CASE(host_domain_to_ascii_1)
{
	using namespace wordring::whatwg;

	std::error_code ec;
	std::u32string s = domain_to_ascii(U"あいうえお.net", ec);
	BOOST_CHECK(!ec);
	BOOST_CHECK(s == U"xn--l8jegik.net");
}

BOOST_AUTO_TEST_CASE(host_domain_to_unicode_1)
{
	using namespace wordring::whatwg;

	std::error_code ec;
	std::u32string s = domain_to_unicode(U"xn--l8jegik.net", ec);
	BOOST_CHECK(!ec);
	BOOST_CHECK(s == U"あいうえお.net");
}

BOOST_AUTO_TEST_CASE(host_is_valid_ipv4_address_string_1)
{
	using namespace wordring::whatwg;

	bool b = is_valid_ipv4_address_string(U"127.0.0.1");
	BOOST_CHECK(b);
}

BOOST_AUTO_TEST_CASE(host_is_valid_ipv4_address_string_2)
{
	using namespace wordring::whatwg;

	bool b = is_valid_ipv4_address_string(U".0.0.1");
	BOOST_CHECK(!b);
}

BOOST_AUTO_TEST_CASE(host_is_valid_ipv4_address_string_3)
{
	using namespace wordring::whatwg;

	bool b = is_valid_ipv4_address_string(U"256.0.0.1");
	BOOST_CHECK(!b);
}

BOOST_AUTO_TEST_CASE(host_is_valid_ipv4_address_string_4)
{
	using namespace wordring::whatwg;

	bool b = is_valid_ipv4_address_string(U"");
	BOOST_CHECK(!b);
}

BOOST_AUTO_TEST_CASE(host_parse_host_1)
{
	using namespace wordring::whatwg;

	std::u32string in = U"127.0.0.1";
	std::error_code ec;
	host_base host = parse_host(in, ec);

	BOOST_CHECK(host.get<ipv4_address_base>().m_value == 2130706433u);
}

BOOST_AUTO_TEST_CASE(host_parse_host_2)
{
	using namespace wordring::whatwg;

	std::u32string in = U"[2001:0db8:1234:5678:90ab:cdef:0000:0000]";
	std::error_code ec;

	host_base host = parse_host(in, ec);

	std::array<std::uint16_t, 8> a = { 0x2001, 0xDB8, 0x1234, 0x5678, 0x90ab, 0xcdef, 0, 0 };
	BOOST_CHECK(host.get<ipv6_address_base>().m_value == a);
}

BOOST_AUTO_TEST_CASE(host_parse_host_3)
{
	using namespace wordring::whatwg;

	std::u32string in = U"%10";
	std::error_code ec;

	host_base host = parse_host(in, true, ec);

	BOOST_CHECK(!ec);
	BOOST_CHECK(host.type() == host_type_name::OpaqueHost);
}

BOOST_AUTO_TEST_CASE(host_parse_host_4)
{
	using namespace wordring::whatwg;

	std::u32string in = U"%1";
	std::error_code ec;

	host_base host = parse_host(in, true, ec);

	BOOST_CHECK(ec);
	BOOST_CHECK(host.type() == host_type_name::OpaqueHost);
}

BOOST_AUTO_TEST_CASE(host_parse_host_5)
{
	using namespace wordring::whatwg;

	std::u32string in = U"%";
	std::error_code ec;

	host_base host = parse_host(in, true, ec);

	BOOST_CHECK(ec);
	BOOST_CHECK(host.type() == host_type_name::OpaqueHost);
}

BOOST_AUTO_TEST_CASE(host_parse_host_6)
{
	using namespace wordring::whatwg;

	std::u32string in = U"";
	std::error_code ec;

	host_base host = parse_host(in, true, ec);

	BOOST_CHECK(!ec);
	BOOST_CHECK(host.type() == host_type_name::OpaqueHost);
}

BOOST_AUTO_TEST_CASE(host_parse_host_7)
{
	using namespace wordring::whatwg;

	std::u32string in = U"localhost";
	std::error_code ec;

	host_base host = parse_host(in, true, ec);

	BOOST_CHECK(!ec);
	BOOST_CHECK(host.type() == host_type_name::OpaqueHost);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv4_1)
{
	using namespace wordring::whatwg;

	std::u32string in = U"127.0.0.1";
	std::error_code ec;
	host_base ipv4 = parse_ipv4(in, ec);

	BOOST_CHECK(ipv4.get<ipv4_address_base>().m_value == 2130706433u);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv4_2)
{
	using namespace wordring::whatwg;

	std::u32string in = U"127.0";
	std::error_code ec;
	host_base ipv4 = parse_ipv4(in, ec);

	BOOST_CHECK(ipv4.get<ipv4_address_base>().m_value == 2130706432u);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv4_3)
{
	using namespace wordring::whatwg;

	std::u32string in = U"";
	std::error_code ec;
	host_base ipv4 = parse_ipv4(in, ec);

	BOOST_CHECK(ipv4.type() == host_type_name::OpaqueHost);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv4_4)
{
	using namespace wordring::whatwg;

	std::u32string in = U".1";
	std::error_code ec;
	host_base ipv4 = parse_ipv4(in, ec);

	BOOST_CHECK(ipv4.type() == host_type_name::OpaqueHost);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv4_number_1)
{
	using namespace wordring::whatwg;

	auto r = parse_ipv4_number(U"0");
	BOOST_CHECK(*r.first == 0);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv4_number_2)
{
	using namespace wordring::whatwg;

	auto r = parse_ipv4_number(U"00");
	BOOST_CHECK(*r.first == 0);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv4_number_3)
{
	using namespace wordring::whatwg;

	auto r = parse_ipv4_number(U"0x0");
	BOOST_CHECK(*r.first == 0);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv4_number_4)
{
	using namespace wordring::whatwg;

	auto r = parse_ipv4_number(U"255");
	BOOST_CHECK(*r.first == 255);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv4_number_5)
{
	using namespace wordring::whatwg;

	auto r = parse_ipv4_number(U"0377");
	BOOST_CHECK(*r.first == 255);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv4_number_6)
{
	using namespace wordring::whatwg;

	auto r = parse_ipv4_number(U"0xFF");
	BOOST_CHECK(*r.first == 255);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv4_number_7)
{
	using namespace wordring::whatwg;

	auto r = parse_ipv4_number(U"0xFFF");
	BOOST_CHECK(*r.first == 4095);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv6_1)
{
	using namespace wordring::whatwg;

	std::u32string in = U"2001:0db8:1234:5678:90ab:cdef:0000:0000";
	std::error_code ec;
	host_base ipv6 = parse_ipv6(in, ec);

	std::array<std::uint16_t, 8> a = { 0x2001, 0xDB8, 0x1234, 0x5678, 0x90ab, 0xcdef, 0, 0 };
	BOOST_CHECK(ipv6.get<ipv6_address_base>().m_value == a);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv6_2)
{
	using namespace wordring::whatwg;

	std::u32string in = U"2001:0db8:0000:0000:3456:0000:0000:0000";
	std::error_code ec;
	host_base ipv6 = parse_ipv6(in, ec);

	std::array<std::uint16_t, 8> a = { 0x2001, 0xDB8, 0, 0, 0x3456, 0, 0, 0 };
	BOOST_CHECK(ipv6.get<ipv6_address_base>().m_value == a);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv6_3)
{
	using namespace wordring::whatwg;

	std::u32string in = U"2001:0db8:0000:0000:3456::";
	std::error_code ec;
	host_base ipv6 = parse_ipv6(in, ec);

	std::array<std::uint16_t, 8> a = { 0x2001, 0xDB8, 0, 0, 0x3456, 0, 0, 0 };
	BOOST_CHECK(ipv6.get<ipv6_address_base>().m_value == a);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv6_4)
{
	using namespace wordring::whatwg;

	std::u32string in = U"2001:0db8::3456:0000:0000:0000";
	std::error_code ec;
	host_base ipv6 = parse_ipv6(in, ec);

	std::array<std::uint16_t, 8> a = { 0x2001, 0xDB8, 0, 0, 0x3456, 0, 0, 0 };
	BOOST_CHECK(ipv6.get<ipv6_address_base>().m_value == a);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv6_5)
{
	using namespace wordring::whatwg;

	std::u32string in = U"2001:db8:0:0:3456::";
	std::error_code ec;
	host_base ipv6 = parse_ipv6(in, ec);

	std::array<std::uint16_t, 8> a = { 0x2001, 0xDB8, 0, 0, 0x3456, 0, 0, 0 };
	BOOST_CHECK(ipv6.get<ipv6_address_base>().m_value == a);
}

BOOST_AUTO_TEST_CASE(host_parse_ipv6_6)
{
	using namespace wordring::whatwg;

	std::u32string in = U"2001:0db8::3456::";
	std::error_code ec;
	host_base ipv6 = parse_ipv6(in, ec);

	BOOST_CHECK(ipv6.type() == static_cast<host_type_name>(0));
}

BOOST_AUTO_TEST_CASE(host_serialize_ipv4_1)
{
	using namespace wordring::whatwg;

	std::error_code ec;
	host_base ipv4 = parse_ipv4(U"127.0.0.1", ec);
	std::u32string s = serialize_ipv4(ipv4);

	BOOST_CHECK(s == U"127.0.0.1");
}

BOOST_AUTO_TEST_CASE(host_serialize_ipv6_1)
{
	using namespace wordring::whatwg;

	std::array<std::uint16_t, 8> a = { 0x2001, 0xDB8, 0x1234, 0x5678, 0x90ab, 0xcdef, 0, 0 };
	host_base ipv6(ipv6_address_base{ a });

	std::u32string s = serialize_ipv6(ipv6);
	BOOST_CHECK(s == U"2001:db8:1234:5678:90ab:cdef::");
}

BOOST_AUTO_TEST_CASE(host_serialize_ipv6_2)
{
	using namespace wordring::whatwg;

	std::array<std::uint16_t, 8> a = { 0x2001, 0xDB8, 0, 0, 0, 0, 0, 1 };
	host_base ipv6(ipv6_address_base{ a });

	std::u32string s = serialize_ipv6(ipv6);
	BOOST_CHECK(s == U"2001:db8::1");
}

BOOST_AUTO_TEST_CASE(host_serialize_ipv6_3)
{
	using namespace wordring::whatwg;

	std::array<std::uint16_t, 8> a = { 0x2001, 0xDB8, 0, 0, 0, 0, 2, 1 };
	host_base ipv6(ipv6_address_base{ a });

	std::u32string s = serialize_ipv6(ipv6);
	BOOST_CHECK(s == U"2001:db8::2:1");
}

BOOST_AUTO_TEST_CASE(host_serialize_ipv6_4)
{
	using namespace wordring::whatwg;

	std::array<std::uint16_t, 8> a = { 0x2001, 0xDB8, 1, 1, 1, 0, 0, 0 };
	host_base ipv6(ipv6_address_base{ a });

	std::u32string s = serialize_ipv6(ipv6);
	BOOST_CHECK(s == U"2001:db8:1:1:1::");
}

BOOST_AUTO_TEST_CASE(host_serialize_ipv6_5)
{
	using namespace wordring::whatwg;

	std::array<std::uint16_t, 8> a = { 0x2001, 0xDB8, 0, 1, 1, 1, 1, 1 };
	host_base ipv6(ipv6_address_base{ a });

	std::u32string s = serialize_ipv6(ipv6);
	BOOST_CHECK(s == U"2001:db8:0:1:1:1:1:1");
}

BOOST_AUTO_TEST_CASE(host_serialize_ipv6_6)
{
	using namespace wordring::whatwg;

	std::array<std::uint16_t, 8> a = { 0x2001, 0, 0, 1, 0, 0, 0, 1 };
	host_base ipv6(ipv6_address_base{ a });

	std::u32string s = serialize_ipv6(ipv6);
	BOOST_CHECK(s == U"2001:0:0:1::1");
}

BOOST_AUTO_TEST_CASE(host_serialize_ipv6_7)
{
	using namespace wordring::whatwg;

	std::array<std::uint16_t, 8> a = { 0x2001, 0xdb8, 0, 0, 1, 0, 0, 1 };
	host_base ipv6(ipv6_address_base{ a });

	std::u32string s = serialize_ipv6(ipv6);
	BOOST_CHECK(s == U"2001:db8::1:0:0:1");
}

BOOST_AUTO_TEST_CASE(host_equal_1)
{
	using namespace wordring::whatwg;

	std::array<std::uint16_t, 8> a = { 0x2001, 0xdb8, 0, 0, 1, 0, 0, 1 };
	host_base h1(ipv6_address_base{ a });
	host_base h2(ipv6_address_base{ a });

	BOOST_CHECK(h1 == h2);
}

BOOST_AUTO_TEST_CASE(host_equal_2)
{
	using namespace wordring::whatwg;

	std::array<std::uint16_t, 8> a = { 0x2001, 0xdb8, 0, 0, 1, 0, 0, 1 };
	host_base h1(ipv6_address_base{ a });
	host_base h2(ipv4_address_base{});

	BOOST_CHECK((h1 == h2) == false);
}

BOOST_AUTO_TEST_CASE(host_equal_3)
{
	using namespace wordring::whatwg;

	std::array<std::uint16_t, 8> a = { 0x2001, 0xdb8, 0, 0, 1, 0, 0, 1 };
	host_base h1(ipv6_address_base{ a });
	host_base h2(ipv6_address_base{});

	BOOST_CHECK((h1 == h2) == false);
}

BOOST_AUTO_TEST_SUITE_END()
