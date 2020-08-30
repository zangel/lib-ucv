list(APPEND ucv_dependencies Boost::unit_test_framework)

find_package(OpenCV REQUIRED)
include_directories (${OpenCV_INCLUDE_DIRS})
list(APPEND ucv_dependencies ${OpenCV_LIBRARIES})

add_executable(test_canny test_canny.cpp test.cpp)
target_link_libraries(test_canny ucv math ${ucv_dependencies})

