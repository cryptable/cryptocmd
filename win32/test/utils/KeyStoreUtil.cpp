/*
 * MIT License
 * Author: David Tillemans
 */
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <stdexcept>
#include <iostream>
#include "KSException.h"
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

void KeyStoreUtil::deleteTestKeysOfKeystore() {
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
        // Initial keys of my keystore
        if ((wcscmp(nCryptKeyName->pszName, L"te-3b573721-84ed-434e-8bd3-a9eb6857d0ec") == 0)
            || (wcscmp(nCryptKeyName->pszName, L"Microsoft Connected Devices Platform device certificate") == 0)
            || (wcscmp(nCryptKeyName->pszName, L"d53acff0-35ac-4942-b6c1-f3b75ecfc960") == 0)) {
            std::wcout << "Skipping Key: " << nCryptKeyName->pszName << " (" << nCryptKeyName->pszAlgid << ")\n";
            continue;
        }
        std::wcout << "Deleting Key: " << nCryptKeyName->pszName << " (" << nCryptKeyName->pszAlgid << ")\n";
        deleteKeyFromKeyStore(nCryptKeyName->pszName);
    }

    NCryptFreeBuffer(nCryptKeyName);
    NCryptFreeBuffer(ptr);
}

KeyStoreUtil::~KeyStoreUtil() {
    NCryptFreeObject(cryptoProvider);
}