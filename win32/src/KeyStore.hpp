/*
 * MIT License
 * Author: David Tillemans
 */
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <string>
#include <ntstatus.h>
#include "KSException.hpp"
#include "GeneralName.hpp"
#include "KeyPair.hpp"

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
    explicit KeyStore(const wchar_t *keystoreName): cryptoProvider{NULL} {
        DWORD status = STATUS_SUCCESS;
        status = NCryptOpenStorageProvider(&cryptoProvider, keystoreName, 0);
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }
	};

    /**
     * @brief Generate a RSA Signing Key in the Windows Key Store
     *
     * @param keyIdentifier Name of the key
     * @param bitLength length for the RSA key
     */
    std::unique_ptr<KeyPair> generateSigningKeyPair(const std::wstring &keyIdentifier, u_long bitLength) {
        DWORD status = STATUS_SUCCESS;
        NCRYPT_KEY_HANDLE rsaKeyHandle;
        status = NCryptCreatePersistedKey(cryptoProvider,
                &rsaKeyHandle,
                BCRYPT_RSA_ALGORITHM,
                keyIdentifier.c_str(),
                0,
                0);
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }

        DWORD exportPolicy = NCRYPT_ALLOW_EXPORT_FLAG;
        status = NCryptSetProperty(rsaKeyHandle,
                                   NCRYPT_EXPORT_POLICY_PROPERTY,
                                   reinterpret_cast<PBYTE>(&exportPolicy),
                                   sizeof(DWORD),
                                   NCRYPT_PERSIST_FLAG);
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }

        DWORD keyUsage = NCRYPT_ALLOW_SIGNING_FLAG;
        status = NCryptSetProperty(rsaKeyHandle,
                                   NCRYPT_KEY_USAGE_PROPERTY,
                                   reinterpret_cast<PBYTE>(&keyUsage),
                                   sizeof(DWORD),
                                   NCRYPT_PERSIST_FLAG);
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }

        DWORD keyLength = bitLength;
        status = NCryptSetProperty(rsaKeyHandle,
                                   NCRYPT_LENGTH_PROPERTY,
                                   reinterpret_cast<PBYTE>(&keyLength),
                                   sizeof(DWORD),
                                   NCRYPT_PERSIST_FLAG);
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }

        status = NCryptFinalizeKey(rsaKeyHandle, NCRYPT_WRITE_KEY_TO_LEGACY_STORE_FLAG );
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }

        return std::make_unique<KeyPair>(rsaKeyHandle);
	};

    std::unique_ptr<KeyPair> getKeyPair(const std::wstring &keyIdentifier) {
        DWORD status = STATUS_SUCCESS;
        NCRYPT_KEY_HANDLE rsaKeyHandle;

        status = NCryptOpenKey(cryptoProvider, &rsaKeyHandle, keyIdentifier.c_str(), 0, 0);
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }

        return std::make_unique<KeyPair>(rsaKeyHandle);
    }

    /**
     * @brief      Destroys the Keystore object and all it refences.
     */
    ~KeyStore() {
        NCryptFreeObject(cryptoProvider);
	};

private:

    NCRYPT_PROV_HANDLE cryptoProvider;
};