file(REMOVE_RECURSE
  "libwordring_cpp.pdb"
  "libwordring_cpp.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/wordring_cpp.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
