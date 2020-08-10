/*
 * MIT License
 * Author: David Tillemans
 */
#ifndef KEYPAIR_HPP
#define KEYPAIR_HPP
#include "common.h"
#include <ncrypt.h>
#include <string>

/**
 * @brief This class defines the asymmetric keypair
 */
class KeyPair {
public:

    /**
     * @brief Constructor to create a key pair, mainly used by KeyStore
     * @param cryptoProvider CryptoProvider
     * @param key handle to the keys in the Keystore
     */
    explicit KeyPair(NCRYPT_KEY_HANDLE key, const std::wstring &name);

    /**
     * Return the public key information of the CNG key
     * @return
     */
    const CERT_PUBLIC_KEY_INFO *getPublicKeyInfo();

    /**
     * Get the KeyHandle
     */
    NCRYPT_KEY_HANDLE getHandle();

    /**
     * Get name of key
     */
    const std::wstring &getName();

    /**
     * Destructor
     */
    ~KeyPair();

public:
    NCRYPT_KEY_HANDLE keyHandle;
    CERT_PUBLIC_KEY_INFO *publicKeyInfo;
    std::wstring keyName;
};

#endif // KEYPAIR
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2020 Cryptable BV
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/