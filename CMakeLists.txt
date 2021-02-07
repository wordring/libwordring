# libwordring
#
# このプロジェクトは libwordring の各種ライブラリとテストを作ります。
#

cmake_minimum_required (VERSION 3.12)

project("libwordring")

enable_testing()

set(Wordring_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(Wordring_INCLUDE_DIR "${Wordring_DIR}/include")

#add_subdirectory("generator")
add_subdirectory("lib")
#add_subdirectory("sample")
add_subdirectory("test")
