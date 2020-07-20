include_directories (${TEST_SOURCE_DIR}/src) 

add_executable (tests main.cpp GeneralNameTest.cpp utils/HexUtils.hpp HexUtilsTest.cpp KeyStoreTest.cpp utils/KeyStoreUtil.cpp utils/KeyStoreUtil.h KeyPairTest.cpp utils/OpenSSLCA.cpp utils/OpenSSLCA.h utils/OpenSSLCertificate.cpp utils/OpenSSLCertificate.h)
target_link_libraries(tests Crypt32.lib Ncrypt.lib)

enable_testing()
add_test(testing tests)