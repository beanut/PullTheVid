# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/YTVideoDownloader_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/YTVideoDownloader_autogen.dir/ParseCache.txt"
  "YTVideoDownloader_autogen"
  )
endif()
