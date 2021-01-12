# wordring_cpp/lib/wwwc
#
# このファイルはwordring_cppのためにwwwcオブジェクトを作成します。
#

cmake_minimum_required (VERSION 3.12)

set(INCLUDE_DIR ${Wordring_INCLUDE_DIR}/wordring/wwwc)

add_library (
	wwwc OBJECT
# css_syntax --------------------------------------------------------------------
		css_syntax/token.cpp
 "selectors/grammar.cpp")

