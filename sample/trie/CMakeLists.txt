# wordring_cpp/sample/trie
#
# このプロジェクトはtrieのサンプルを作成します。
#

cmake_minimum_required (VERSION 3.12)

project(trie_sample)

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
		test_module.cpp
#		list_trie_iterator.cpp
#		stable_trie.cpp
#		stable_trie_iterator.cpp
#		stable_trie_base.cpp
#		stable_trie_base_benchmark.cpp
#		stable_trie_base_iterator.cpp
		trie.cpp
		trie_base.cpp
#		trie_base_iterator.cpp
#		trie_construct_iterator.cpp
		trie_heap.cpp
#		trie_heap_iterator.cpp
#		trie_iterator.cpp
)

target_link_libraries(
	${PROJECT_NAME}
		wordring_cpp
		${Boost_FILESYSTEM_LIBRARY}
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
