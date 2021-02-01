# test/whatwg
#
# このプロジェクトは whatwg 配下のすべてのテストを作成します。
#

cmake_minimum_required (VERSION 3.12)

add_subdirectory("dom")
add_subdirectory("encoding")
add_subdirectory("html")
add_subdirectory("infra")
add_subdirectory("url")
