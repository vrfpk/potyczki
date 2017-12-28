project (HelloCMake)
cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 14)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
find_package(GMP REQUIRED)
add_subdirectory (src)
