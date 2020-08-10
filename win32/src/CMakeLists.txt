
# add the executable
add_library(${LIBRARY_NAME} STATIC
        X509Name.h
        KeyPair.h
        KeyStore.h
        KSException.h
        CertificateStore.cpp CertificateStore.h KeyPair.cpp KeyStore.cpp KSException.cpp X509Name.cpp common.h WebExtension.cpp WebExtension.h)

target_include_directories(${LIBRARY_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
