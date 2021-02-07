# wordring_cpp/lib
#
# This project will create a libwordring library.
#

cmake_minimum_required (VERSION 3.12)

project("wordring" CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS ON)
find_package (
	Boost 1.67 REQUIRED
)

find_package (
	ICU REQUIRED
	COMPONENTS
		uc
		data
)

find_package(OpenSSL REQUIRED)

include_directories (
	${Boost_INCLUDE_DIRS}
	${ICU_INCLUDE_DIRS}
	${Wordring_INCLUDE_DIR}
)

set(INCLUDE_DIR "${Wordring_INCLUDE_DIR}/wordring")

add_subdirectory ("whatwg")
add_subdirectory ("wwwc")

add_library (
	${PROJECT_NAME}
		$<TARGET_OBJECTS:whatwg>
		$<TARGET_OBJECTS:wwwc>
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
		OpenSSL::SSL OpenSSL::Crypto
		${ICU_LIBRARIES}
)
