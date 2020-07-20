cmake_minimum_required(VERSION 3.8)
project(ksmgmnt VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(COMMON_DIR "${CMAKE_SOURCE_DIR}/../common")

include_directories(include src ${COMMON_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)