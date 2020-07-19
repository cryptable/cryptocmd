//
// Created by david on 19/07/2020.
//
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <stdexcept>
#include <iostream>
#include "KSException.hpp"
#include "KeyStoreUtil.h"

KeyStoreUtil::KeyStoreUtil(const wchar_t *keystoreName) {
    DWORD status = STATUS_SUCCESS;
    status = NCryptOpenStorageProvider(&cryptoProvider, keystoreName, 0);
    if (status != STATUS_SUCCESS) {
        throw KSException(status);
    }
}

void KeyStoreUtil::deleteKeyFromKeyStore(const wchar_t *keyId) {
    DWORD status = STATUS_SUCCESS;
    NCRYPT_KEY_HANDLE keyHandle;

    status = NCryptOpenKey(cryptoProvider, &keyHandle, keyId, 0, 0);
    if (status != STATUS_SUCCESS) {
        throw KSException(status);
    }

    status = NCryptDeleteKey(keyHandle, 0);
    if (status != STATUS_SUCCESS) {
        throw KSException(status);
    }

    NCryptFreeObject(keyHandle);
}

boolean KeyStoreUtil::isKeyInKeystore(const wchar_t *keyName) {
    DWORD status = STATUS_SUCCESS;
    NCryptKeyName *nCryptKeyName = NULL;
    void *ptr = NULL;
    boolean found = false;

    while (true) {
        status = NCryptEnumKeys(cryptoProvider, NULL, &nCryptKeyName, &ptr, 0);
        if (status == NTE_NO_MORE_ITEMS) {
            break;
        }
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }
        if (wcsncmp(nCryptKeyName->pszName, keyName, wcsnlen(keyName, NCRYPT_MAX_KEY_NAME_LENGTH)) == 0) {
            found = true;
            break;
        }
    }

    NCryptFreeBuffer(nCryptKeyName);
    NCryptFreeBuffer(ptr);

    return found;
}

void KeyStoreUtil::showKeysOfKeystore() {
    DWORD status = STATUS_SUCCESS;
    NCryptKeyName *nCryptKeyName = NULL;
    void *ptr = NULL;

    while (true) {
        status = NCryptEnumKeys(cryptoProvider, NULL, &nCryptKeyName, &ptr, 0);
        if (status == NTE_NO_MORE_ITEMS) {
            break;
        }
        if (status != STATUS_SUCCESS) {
            throw KSException(status);
        }
        std::wcout << "Key: " << nCryptKeyName->pszName << " (" << nCryptKeyName->pszAlgid << ")\n";
    }

    NCryptFreeBuffer(nCryptKeyName);
    NCryptFreeBuffer(ptr);
}

KeyStoreUtil::~KeyStoreUtil() {
    NCryptFreeObject(cryptoProvider);
}