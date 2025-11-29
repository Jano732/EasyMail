# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\EasyMail_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\EasyMail_autogen.dir\\ParseCache.txt"
  "EasyMail_autogen"
  )
endif()
