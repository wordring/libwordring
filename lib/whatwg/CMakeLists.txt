# wordring_cpp/lib/whatwg
#
# このファイルはwordring_cppのためにwhatwgオブジェクトを作成します。
#

cmake_minimum_required (VERSION 3.12)

set(INCLUDE_DIR ${Wordring_INCLUDE_DIR}/wordring/whatwg)

add_library (
	whatwg OBJECT
#		${INCLUDE_DIR}/encoding.hpp
		${INCLUDE_DIR}/encoding/encoding.hpp
#		${INCLUDE_DIR}/encoding/encoding_atom.hpp
#		${INCLUDE_DIR}/encoding/encoding_enum.hpp
		${INCLUDE_DIR}/infra/infra.hpp
#		${INCLUDE_DIR}/url.hpp
#		${INCLUDE_DIR}/url/host.hpp
#		${INCLUDE_DIR}/url/percent_encode.hpp
#		${INCLUDE_DIR}/url/url.hpp
		encoding/encoding.cpp
#		encoding/encoding_atom.cpp
#		encoding/encoding_hash.hpp
#		url/url.cpp
)

