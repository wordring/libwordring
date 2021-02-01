# lib/whatwg
#
# このファイルは libwordring のために whatwg オブジェクトを作成します。
#

cmake_minimum_required (VERSION 3.12)

set(INCLUDE_DIR ${Wordring_INCLUDE_DIR}/wordring/whatwg)

add_library (
	whatwg OBJECT
# encoding --------------------------------------------------------------------
		"encoding/encoding.cpp"
		"encoding/indexes_code_point.cpp"
		"encoding/indexes_pointer.cpp"

# html ------------------------------------------------------------------------
		"html/parsing/atom_tbl.cpp"
# infra -----------------------------------------------------------------------
# url -------------------------------------------------------------------------
		"url/host.cpp"
)

