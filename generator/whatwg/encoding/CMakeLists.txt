﻿# wordring_cpp/generator/whatwg/encoding
#
# このプロジェクトはwhatwg/encoding用のソースコード生成プログラムを作成します。
#

cmake_minimum_required (VERSION 3.12)

project(generator__whatwg__encoding CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package (
	Boost 1.67 REQUIRED
	COMPONENTS
		system
		filesystem
)

include_directories (
	${Boost_INCLUDE_DIRS}
)

add_executable(
	${PROJECT_NAME}
		encoding.cpp
)

add_definitions(-DINDEXES_JSON_PATH=${Wordring_DIR}/submodules/whatwg/encoding/indexes.json)
add_definitions(-DINDEXES_HPP_PATH=${Wordring_INCLUDE_DIR}/wordring/whatwg/encoding/indexes.hpp)
add_definitions(-DINDEXES_CODE_POINT_CPP_PATH=${Wordring_DIR}/lib/whatwg/encoding/indexes_code_point.cpp)
add_definitions(-DINDEXES_POINTER_CPP_PATH=${Wordring_DIR}/lib/whatwg/encoding/indexes_pointer.cpp)

target_link_libraries(
	${PROJECT_NAME}
		${Boost_FILESYSTEM_LIBRARY}
)