# wordring_cpp/lib
#
# This project will create a wordring_cpp library.
#

cmake_minimum_required (VERSION 3.12)

project(wordring_cpp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package (
	Boost 1.67 REQUIRED
	COMPONENTS
		system
		filesystem
#		unit_test_framework
)

#find_library (
#	Gumbo_static_LIBRARY
#		NAMES libgumbo.a libgumbo_static.a gumbo.a gumbo_static.a libgumbo.lib libgumbo_static.lib gumbo.lib gumbo_static.lib
#		HINTS ${CMAKE_FIND_ROOT_PATH}
#		PATHS ${CMAKE_FIND_ROOT_PATH}
#		PATH_SUFFIXES "lib" "local/lib"
#)

#find_path (
#	Gumbo_INCLUDE_DIR
#		NAMES gumbo.h
#		PATHS ${CMAKE_FIND_ROOT_PATH}
#)

#find_package (
#	ICU REQUIRED
#	COMPONENTS
#		uc
#		data
#)

#find_package(
#	Sqlite3 REQUIRED
#)

include_directories (
#	${Gumbo_INCLUDE_DIR}
	${Boost_INCLUDE_DIRS}
#	${ICU_INCLUDE_DIR}
	${Wordring_INCLUDE_DIR}
)

set(INCLUDE_DIR ${Wordring_INCLUDE_DIR}/wordring)

#add_subdirectory (html)
#add_subdirectory (lang)
#add_subdirectory (sqlite)
#add_subdirectory (string)
#add_subdirectory (tag_tree)
#add_subdirectory (util)
add_subdirectory (whatwg)
add_subdirectory (wwwc)

add_library (
	${PROJECT_NAME}
#		$<TARGET_OBJECTS:html>
#		$<TARGET_OBJECTS:lang>
#		$<TARGET_OBJECTS:sqlite>
#		$<TARGET_OBJECTS:string>
#		$<TARGET_OBJECTS:tag_tree>
#		$<TARGET_OBJECTS:util>
		$<TARGET_OBJECTS:whatwg>
		$<TARGET_OBJECTS:wwwc>
)

target_link_libraries (
	${PROJECT_NAME}
#		${Gumbo_static_LIBRARY}
#		${ICU_LIBRARIES}
#		sqlite3
)

