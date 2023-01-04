cmake_minimum_required (VERSION 3.8)

project ("LibraryTester")
set(PROTOS
	algorithm
	numeric
)
list(TRANSFORM PROTOS PREPEND "proto-")
list(TRANSFORM PROTOS APPEND ".cpp")
add_library(LibraryTester SHARED "LibraryTester.cpp" "LibraryTester.h" "variants.h" ${PROTOS})
#add_executable(LibraryTester "LibraryTester.cpp" "LibraryTester.h" "variants.h" "std-headers.h" ${PROTOS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Zc:inline- /bigobj")
set(LINK_FLAGS "${LINK_FLAGS} /OPT:NOREF /WHOLEARCHIVE /DEBUG:FULL")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LibraryTester PROPERTY CXX_STANDARD 17)
endif()

# TODO: Add tests and install targets if needed.
