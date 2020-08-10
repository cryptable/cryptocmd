cmake_minimum_required(VERSION 3.8)
project(ksmgmnt VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT DEFINED COMMON_DIR)
    set(COMMON_DIR "${CMAKE_SOURCE_DIR}/../common")
endif(NOT DEFINED COMMON_DIR)

include_directories(include src ${COMMON_DIR}/include)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_NAME  "${CMAKE_PROJECT_NAME}d")
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIBRARY_NAME  "${CMAKE_PROJECT_NAME}")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

add_subdirectory(src)
add_subdirectory(test)
