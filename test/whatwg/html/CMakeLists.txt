# wordring_cpp/test/whatwg/html/
#
# このプロジェクトはwhatwg/htmlのテストを作成します。
#

cmake_minimum_required (VERSION 3.12)

project("whatwg_html_test")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS ON)
find_package (
	Boost 1.67 REQUIRED
	COMPONENTS
		"unit_test_framework"
)

include_directories (
	${Boost_INCLUDE_DIRS}
	${Wordring_INCLUDE_DIR}
)

add_executable(
	${PROJECT_NAME}
		"test_module.cpp"
		"html_atom.cpp"
		"parsing/atom_defs.cpp"
		"parsing/atom_tbl.cpp"
		"parsing/input_stream.cpp"
		"parsing/tokenization.cpp"
		"parsing/tree_construction_dispatcher.cpp"
		"parsing/on_initial_insertion_mode.cpp"
		"parsing/serializing.cpp"
		"parsing/on_in_head_insertion_mode.cpp"
		"url.cpp"
)

add_definitions(-DCURRENT_SOURCE_PATH=${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
	${PROJECT_NAME}
		"wordring_cpp"
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

