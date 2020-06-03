# wordring_cpp/test/html/
#
# このプロジェクトはhtmlのテストを作成します。
#

cmake_minimum_required (VERSION 3.12)

project(html_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS ON)
find_package (
	Boost 1.67 REQUIRED
	COMPONENTS
		system
		filesystem
		unit_test_framework
)

include_directories (
	${Boost_INCLUDE_DIRS}
	${Wordring_INCLUDE_DIR}
)

add_executable(
	${PROJECT_NAME}
		"test_module.cpp"
#		"simple_node.cpp"
#		"simple_parser.cpp"
#		"simple_traits.cpp"
#		"simple_html_sample_common.cpp"
#		"simple_html_sample_document.cpp"
#		"simple_html_sample_edits.cpp"
		"simple_html_sample_embedded.cpp"
#		"simple_html_sample_grouping.cpp"
#		"simple_html_sample_introduction.cpp"
#		"simple_html_sample_links.cpp"
#		"simple_html_sample_parsing.cpp"
#		"simple_html_sample_sections.cpp"
#		"simple_html_sample_semantics.cpp"
#		"simple_html_sample_syntax.cpp"
#		"simple_html_sample_text.cpp"
)

add_definitions(-DCURRENT_SOURCE_PATH=${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
	${PROJECT_NAME}
		wordring_cpp
		${Boost_FILESYSTEM_LIBRARY}
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

