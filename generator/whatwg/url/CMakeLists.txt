# wordring_cpp/generator/whatwg/url
#
# このプロジェクトはwhatwg/url用のソースコード生成プログラムを作成します。
#

cmake_minimum_required (VERSION 3.12)

project(generator__whatwg__url CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package (
	Boost 1.67 REQUIRED
)

include_directories (
	${Boost_INCLUDE_DIRS}
)

add_executable(
	public_suffix_list
		public_suffix_list.cpp
)

add_definitions(-DPSL_DATA_PATH=${Wordring_DIR}/submodules/publicsuffix/list/public_suffix_list.dat)
add_definitions(-DPSL_HPP_PATH=${Wordring_INCLUDE_DIR}/wordring/whatwg/encoding/indexes.hpp)
add_definitions(-DPSL_CPP_PATH=${Wordring_DIR}/lib/whatwg/encoding/indexes_code_point.cpp)
