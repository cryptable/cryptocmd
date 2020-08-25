include_directories (${TEST_SOURCE_DIR}/src) 

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
    ${CRYPTABLEPKI}
    ${CONAN_LIBS})

enable_testing()
add_test(testing tests) 