cmake_minimum_required(VERSION 3.12)

project(ucv CXX)

find_package(Boost 1.68.0 REQUIRED COMPONENTS unit_test_framework)
message(STATUS "Found Boost: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")

add_subdirectory(lib_math)
add_subdirectory(lib_ucv)

set(BUILD_TESTS NO CACHE BOOL "Build Tests")

if(BUILD_TESTS)
	add_subdirectory(lib_ucv_test)
endif()
