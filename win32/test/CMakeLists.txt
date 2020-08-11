include_directories (${TEST_SOURCE_DIR}/src) 

SET(OPENSSL_ROOT_DIR ${COMMON_DIR})
find_package(OpenSSL)
if(OPENSSL_FOUND)
    MESSAGE("Openssl Version: ${OPENSSL_VERSION}")
    MESSAGE("Openssl Include: ${OPENSSL_INCLUDE_DIR}")
    MESSAGE("Openssl Crypto library: ${OPENSSL_CRYPTO_LIBRARY}")
    MESSAGE("Openssl SSL library: ${OPENSSL_SSL_LIBRARY}")
    MESSAGE("Openssl Crypto libraries: ${OPENSSL_SSL_LIBRARIES}")
    MESSAGE("Openssl APPLINK SOURCES: ${OPENSSL_APPLINK_SOURCE}")
endif(OPENSSL_FOUND)

MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_library(CRYPTABLEPKI
            cryptablepkid
            PATHS ${COMMON_DIR}/lib
            DOC "Cryptable PKI Library")
    MESSAGE("cryptablepkid: ${CRYPTABLEPKI}")
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_library(CRYPTABLEPKI
            cryptablepki
            PATHS ${COMMON_DIR}/lib
            DOC "Cryptable PKI Library")
    MESSAGE("cryptablepki: ${CRYPTABLEPKI}")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

add_executable (tests main.cpp
        X509NameTest.cpp
        utils/HexUtils.hpp HexUtilsTest.cpp
        KeyStoreTest.cpp
        utils/KeyStoreUtil.cpp utils/KeyStoreUtil.h
        KeyPairTest.cpp
        utils/CertStoreUtil.cpp utils/CertStoreUtil.h utils/CNGHash.cpp utils/CNGHash.h utils/CNGSign.cpp utils/CNGSign.h CertificateStoreTest.cpp ../src/Base64Utils.h WebExtensionTest.cpp)

target_link_libraries(tests ${LIBRARY_NAME} 
    Rpcrt4.lib 
    Crypt32.lib 
    Ncrypt.lib 
    Ws2_32.lib 
    ${OPENSSL_CRYPTO_LIBRARY} 
    ${OPENSSL_SSL_LIBRARY} 
    ${CRYPTABLEPKI})

enable_testing()
add_test(testing tests) 