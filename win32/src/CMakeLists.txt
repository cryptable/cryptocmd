
# add the executable
add_library(ksmgmnt createcsr.cpp 
    GeneralName.hpp 
    KeyPair.hpp 
    KeyStore.hpp 
    KSException.hpp 
    ksmgmnt.h)

target_include_directories(ksmgmnt PUBLIC "${PROJECT_BINARY_DIR}")
