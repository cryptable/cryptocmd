include_directories (${TEST_SOURCE_DIR}/src) 

SET(OPENSSL_ROOT_DIR ${COMMON_DIR})
# SET(OPENSSL_USE_STATIC_LIBS TRUE)
# SET(OPENSSL_MSVC_STATIC_RT TRUE)
find_package(OpenSSL)
if(OPENSSL_FOUND)
    MESSAGE("Openssl Version: ${OPENSSL_VERSION}")
    MESSAGE("Openssl Include: ${OPENSSL_INCLUDE_DIR}")
    MESSAGE("Openssl Crypto library: ${OPENSSL_CRYPTO_LIBRARY}")
    MESSAGE("Openssl SSL library: ${OPENSSL_SSL_LIBRARY}")
    MESSAGE("Openssl Crypto libraries: ${OPENSSL_SSL_LIBRARIES}")
    MESSAGE("Openssl APPLINK SOURCES: ${OPENSSL_APPLINK_SOURCE}")
endif(OPENSSL_FOUND)

add_executable (tests main.cpp
        GeneralNameTest.cpp
        utils/HexUtils.hpp HexUtilsTest.cpp
        KeyStoreTest.cpp
        utils/KeyStoreUtil.cpp utils/KeyStoreUtil.h
        KeyPairTest.cpp
        utils/OpenSSLCA.cpp utils/OpenSSLCA.h
        utils/OpenSSLCertificate.cpp utils/OpenSSLCertificate.h
        utils/OpenSSLCertificateRequest.cpp utils/OpenSSLCertificateRequest.h
        utils/OpenSSLException.h utils/OpenSSLX509Name.cpp utils/OpenSSLX509Name.h utils/OpenSSL.cpp utils/OpenSSL.h)
target_link_libraries(tests Crypt32.lib Ncrypt.lib ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY})

# standard libs
# legacy_stdio_definitions.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

enable_testing()
add_test(testing tests)