# wordring_cpp/test/trie
#
# このプロジェクトはtrieのテストを作成します。
#

cmake_minimum_required (VERSION 3.12)

project(trie_test)

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
		list_trie_iterator.cpp
		stable_trie.cpp
		stable_trie_benchmark.cpp
		stable_trie_iterator.cpp
		stable_trie_base.cpp
		stable_trie_base_benchmark.cpp
		stable_trie_base_iterator.cpp
		trie.cpp
		trie_benchmark.cpp
		trie_base.cpp
		trie_base_iterator.cpp
		trie_base_benchmark.cpp
		trie_construct_iterator.cpp
		trie_heap.cpp
		trie_heap_iterator.cpp
		trie_iterator.cpp
)

add_definitions(-DCURRENT_SOURCE_PATH=${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DCURRENT_BINARY_PATH=${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DENGLISH_WORDS_PATH=${Wordring_DIR}/submodules/dwyl/english-words/words.txt)
add_definitions(-DJAPANESE_WORDS_PATH=${Wordring_DIR}/submodules/hingston/japanese/44998-japanese-words.txt)

target_link_libraries(
	${PROJECT_NAME}
		wordring_cpp
		${Boost_FILESYSTEM_LIBRARY}
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

