# test/http/
#
# このプロジェクトは http のテストを作成します。
#

cmake_minimum_required (VERSION 3.12)

project("http-test")

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
		"simple_server.cpp"
 )

add_definitions(-DCURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(
	${PROJECT_NAME}
		"wordring"
		${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

