/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 09/08/2020
 */
#include "KeyStore.h"
#include <string>
#include "KSException.h"
#include "KeyPair.h"

KeyStore::KeyStore(const wchar_t *keystoreName): cryptoProvider{NULL} {
    DWORD status = STATUS_SUCCESS;
    status = NCryptOpenStorageProvider(&cryptoProvider, keystoreName, 0);
    if (status != STATUS_SUCCESS) {
        throw KSException(status);
    }
};

std::unique_ptr<KeyPair> KeyStore::generateKeyPair(const std::wstring &keyIdentifier, u_long bitLength) const {
    DWORD status = STATUS_SUCCESS;
    NCRYPT_KEY_HANDLE rsaKeyHandle;
    status = NCryptCreatePersistedKey(cryptoProvider,
                                      &rsaKeyHandle,
                                      BCRYPT_RSA_ALGORITHM,
                                      keyIdentifier.c_str(),
                                      AT_SIGNATURE,
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

    return std::move(std::make_unique<KeyPair>(rsaKeyHandle, keyIdentifier));
};

std::unique_ptr<KeyPair> KeyStore::getKeyPair(const std::wstring &keyIdentifier) const {
    DWORD status = STATUS_SUCCESS;
    NCRYPT_KEY_HANDLE rsaKeyHandle;

    status = NCryptOpenKey(cryptoProvider, &rsaKeyHandle, keyIdentifier.c_str(), 0, 0);
    if (status != STATUS_SUCCESS) {
        throw KSException(status);
    }

    return std::make_unique<KeyPair>(rsaKeyHandle, keyIdentifier);
}

bool KeyStore::compareCNGKeyWithPublicKey(NCRYPT_KEY_HANDLE rsaKeyHandle, const CERT_PUBLIC_KEY_INFO &toTestPublicKeyInfo) const {
    DWORD publicKeyLg = 0;
    CERT_PUBLIC_KEY_INFO *publicKeyInfo;

    if (!CryptExportPublicKeyInfo(rsaKeyHandle,
                                  0,
                                  X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                                  nullptr,
                                  &publicKeyLg)) {
        throw KSException(GetLastError());
    }
    publicKeyInfo = reinterpret_cast<CERT_PUBLIC_KEY_INFO *>(new unsigned char[publicKeyLg]);
    CryptExportPublicKeyInfo(rsaKeyHandle,
                             0,
                             X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                             publicKeyInfo,
                             &publicKeyLg);
    bool result = CertComparePublicKeyInfo(X509_ASN_ENCODING, publicKeyInfo,
                                           const_cast<CERT_PUBLIC_KEY_INFO *>(&toTestPublicKeyInfo));

    delete publicKeyInfo;

    return result;
}

std::unique_ptr<KeyPair> KeyStore::getKeyPair(const CERT_PUBLIC_KEY_INFO &publicKeyInfo) const {
    DWORD status = STATUS_SUCCESS;
    NCryptKeyName *nCryptKeyName = NULL;
    NCRYPT_KEY_HANDLE rsaKeyHandle = 0;
    void *ptr = NULL;

    while (true) {
        status = NCryptEnumKeys(cryptoProvider, NULL, &nCryptKeyName, &ptr, 0);
        if (status == NTE_NO_MORE_ITEMS) {
            break;
        }
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }

        status = NCryptOpenKey(cryptoProvider, &rsaKeyHandle, nCryptKeyName->pszName, 0, 0);
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }

        if (compareCNGKeyWithPublicKey(rsaKeyHandle, publicKeyInfo)) {
            std::wstring keyName = std::wstring(nCryptKeyName->pszName);
            auto keyPair = std::make_unique<KeyPair>(rsaKeyHandle, keyName);
            NCryptFreeBuffer(nCryptKeyName);
            NCryptFreeBuffer(ptr);
            return keyPair;
        }

        NCryptFreeObject(rsaKeyHandle);
    }

    NCryptFreeBuffer(nCryptKeyName);
    NCryptFreeBuffer(ptr);
    return nullptr;
}

KeyStore::~KeyStore() {
    NCryptFreeObject(cryptoProvider);
};