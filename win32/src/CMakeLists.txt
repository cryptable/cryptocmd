
# add the executable
add_library(${LIBRARY_NAME} STATIC
        X509Name.cpp X509Name.h
        KeyPair.cpp KeyPair.h
        KeyStore.cpp KeyStore.h
        KSException.cpp KSException.h
        CertificateStore.cpp CertificateStore.h
        WebExtension.cpp WebExtension.h
        LogEvent.cpp LogEvent.h event_codes.h
        Base64Utils.h
        common.h)

target_include_directories(${LIBRARY_NAME} PUBLIC "${PROJECT_BINARY_DIR}")

add_executable(${EXECUTABLE_NAME} main.cpp)
target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME}
        Rpcrt4.lib
        Crypt32.lib
        Ncrypt.lib
        Ws2_32.lib
        shlwapi.lib)
