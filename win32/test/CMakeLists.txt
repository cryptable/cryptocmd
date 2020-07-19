include_directories (${TEST_SOURCE_DIR}/src) 

add_executable (tests main.cpp GeneralNameTest.cpp utils/HexUtils.hpp HexUtilsTest.cpp)
target_link_libraries(tests Crypt32.lib)

enable_testing()
add_test(testing tests)