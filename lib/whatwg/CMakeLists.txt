# wordring_cpp/lib/whatwg
#
# このファイルはwordring_cppのためにwhatwgオブジェクトを作成します。
#

cmake_minimum_required (VERSION 3.12)

set(INCLUDE_DIR ${Wordring_INCLUDE_DIR}/wordring/whatwg)

add_library (
	whatwg OBJECT
# encoding --------------------------------------------------------------------
		${INCLUDE_DIR}/encoding/encoding.hpp
		${INCLUDE_DIR}/encoding/encoding_stream.hpp
		encoding/encoding.cpp
# infra -----------------------------------------------------------------------
		${INCLUDE_DIR}/infra/infra.hpp
		${INCLUDE_DIR}/infra/scalar_value.hpp
# url -------------------------------------------------------------------------
		${INCLUDE_DIR}/url/url.hpp
)

