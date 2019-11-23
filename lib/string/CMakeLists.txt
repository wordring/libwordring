# wordring_cpp/lib/string
#
# このファイルはwordring_cppのためにstringオブジェクトを作成します。
#

cmake_minimum_required (VERSION 3.12)

set(INCLUDE_DIR ${Wordring_INCLUDE_DIR}/wordring/string)

add_library (
	string OBJECT
# encoding --------------------------------------------------------------------
		${INCLUDE_DIR}/encoding/coder.hpp
		${INCLUDE_DIR}/encoding/encoding.hpp
		${INCLUDE_DIR}/encoding/indexes.hpp
		${INCLUDE_DIR}/encoding/stream.hpp
		encoding/encoding.cpp
		encoding/indexes_code_point.cpp
		encoding/indexes_pointer.cpp
# infra -----------------------------------------------------------------------
		${INCLUDE_DIR}/infra/infra.hpp
#		${INCLUDE_DIR}/infra/scalar_value.hpp
# url -------------------------------------------------------------------------
		${INCLUDE_DIR}/url/url.hpp
)

