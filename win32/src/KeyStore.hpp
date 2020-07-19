/*
 * MIT License
 * Author: David Tillemans
 */
#include <windows.h>
#include <string>
#include <ntstatus.h>
#include "KSException.hpp"

/**
 * @brief      This class gives access to the keystore(s) where 
 * the cryptographic keys are located.
 */ 
class KeyStore {

public:
    /**
     * @brief      Constructs a new instance of the Keystore.
     *
     * @param      keystoreName  The name of the keystore to use.
     */
    KeyStore(std::string &keystoreName) {
        DWORD status = STATUS_SUCCESS;
        status = NCryptOpenStorageProvider(&cryptoProvider, (LPCWSTR)keystoreName.c_str(), 0);
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }
	};

    void generateKeyPair() {

	};

    /**
     * @brief      Destroys the Keystore object and all it refences.
     */
    ~KeyStore() {
        NCryptFreeObject(cryptoProvider);
	};

private:
    NCRYPT_PROV_HANDLE cryptoProvider;
};