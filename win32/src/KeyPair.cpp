/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 09/08/2020
 */
#include "KeyPair.h"
#include <vector>
#include "KSException.h"

KeyPair::KeyPair(NCRYPT_KEY_HANDLE key, const std::wstring &name) : keyHandle{key}, keyName(name) {
    DWORD publicKeyLg = 0;

    if (!CryptExportPublicKeyInfo(keyHandle,
                                  0,
                                  X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                                  nullptr,
                                  &publicKeyLg)) {
        throw KSException(__func__, __LINE__, GetLastError());
    }
    publicKeyInfo = reinterpret_cast<CERT_PUBLIC_KEY_INFO *>(new unsigned char[publicKeyLg]);
    CryptExportPublicKeyInfo(keyHandle,
                             0,
                             X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                             publicKeyInfo,
                             &publicKeyLg);
}

const CERT_PUBLIC_KEY_INFO *KeyPair::getPublicKeyInfo() {
    return publicKeyInfo;
};

NCRYPT_KEY_HANDLE KeyPair::getHandle() {
    return keyHandle;
}

const std::wstring &KeyPair::getName() {
    return keyName;
}

KeyPair::~KeyPair() {
    NCryptFreeObject(keyHandle);
    delete publicKeyInfo;
}
