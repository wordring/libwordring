# wordring_cpp/test/tag_tree
#
# このプロジェクトはtag_treeのテストを作成します。
#

cmake_minimum_required (VERSION 3.12)

project("tag_tree_test")

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
		"cast_iterator.cpp"
		"character_iterator.cpp"
		"css_selector.cpp"
		"serial_iterator.cpp"
		"simple_html.cpp"
		"tag_tree.cpp"
		"tag_tree_iterator.cpp"
		"tree_iterator.cpp"

)

add_definitions(-DCURRENT_SOURCE_PATH=${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
	${PROJECT_NAME}
		"wordring_cpp"
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
