/*
 * MIT License
 * Author: David Tillemans
 */
#ifndef KEYSTORE_HPP
#define KEYSTORE_HPP
#include "common.h"
#include <string>
#include "KeyPair.h"

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
    explicit KeyStore(const wchar_t *keystoreName);

    /**
     * @brief Generate a RSA Signing Key in the Windows Key Store
     *
     * @param keyIdentifier Name of the key
     * @param bitLength length for the RSA key
     */
    std::unique_ptr<KeyPair> generateKeyPair(const std::wstring &keyIdentifier, u_long bitLength) const;

    /**
     * Get the Key Pair with the corresponding name
     * @param keyIdentifier
     * @return unique pointer of the asymmetric key pair
     */
    std::unique_ptr<KeyPair> getKeyPair(const std::wstring &keyIdentifier) const;

    /**
     * Get the CNG key pair using the public key
     * @param publicKeyInfo
     * @return
     */
    std::unique_ptr<KeyPair> getKeyPair(const CERT_PUBLIC_KEY_INFO &publicKeyInfo) const;

    /**
     * @brief      Destroys the Keystore object and all it references.
     */
    ~KeyStore();

private:
    bool compareCNGKeyWithPublicKey(NCRYPT_KEY_HANDLE rsaKeyHandle, const CERT_PUBLIC_KEY_INFO &toTestPublicKeyInfo) const;

    NCRYPT_PROV_HANDLE cryptoProvider;
};
#endif // KEYSTORE_HPP
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