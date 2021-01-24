# wordring_cpp/generator/whatwg/html
#
# このプロジェクトはwhatwg/html用のソースコード生成プログラムを作成します。
#

cmake_minimum_required (VERSION 3.12)

project(generator__whatwg__html CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package (
	Boost 1.67 REQUIRED
)

include_directories (
	${Boost_INCLUDE_DIRS}
	${Wordring_INCLUDE_DIR}
)

add_executable(
	atom_tbl
		atom_tbl.cpp
)

add_definitions(-DDATA_PATH=${CMAKE_CURRENT_SOURCE_DIR}/data/)

add_definitions(-DATOM_HPP_PATH=${Wordring_INCLUDE_DIR}/wordring/whatwg/html/parsing/atom_tbl.hpp)
add_definitions(-DATOM_DEFS_HPP_PATH=${Wordring_INCLUDE_DIR}/wordring/whatwg/html/parsing/atom_defs.hpp)
add_definitions(-DATOM_CPP_PATH=${Wordring_DIR}/lib/whatwg/html/parsing/atom_tbl.cpp)
