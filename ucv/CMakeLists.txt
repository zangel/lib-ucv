cmake_minimum_required(VERSION 3.12)

project(ucv CXX)

set(ucv_dependencies)

find_package(Boost 1.68.0 REQUIRED COMPONENTS unit_test_framework)
message(STATUS "Found Boost: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
include_directories (${Boost_INCLUDE_DIR})
list(APPEND ucv_dependencies ${Boost_LIBRARIES})

find_package(PNG REQUIRED)
message(STATUS "Found PNG: ${PNG_VERSION_STRING}")
include_directories (${PNG_INCLUDE_DIRS})
list(APPEND ucv_dependencies ${PNG_LIBRARIES})

include_directories(3rd_party/libkdtree)
include_directories(lib_math/include)
include_directories(lib_ucv/include)

add_subdirectory(lib_math)
add_subdirectory(lib_ucv)

set(BUILD_TESTS NO CACHE BOOL "Build Tests")

if(BUILD_TESTS)
	add_subdirectory(lib_ucv_test)
endif()
