/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 08/08/2020
 */

#ifndef KSMGMNT_CERTIFICATESTORE_H
#define KSMGMNT_CERTIFICATESTORE_H
#include "common.h"
#include <string>
#include <wincrypt.h>
#include "KeyStore.h"

class CertificateStore {

public:
    /**
     * Using default Microsoft key store
     */
    CertificateStore();

    /**
     * Using default a possible other Microsoft key store
     */
    CertificateStore(const std::wstring &keyStoreProvider);

    /**
     * Destructor
     */
     ~CertificateStore();

    /**
     * Create a certificate request with the subject name as dname
     * @param subjectName
     * @param bitLength RSA key length
     * @return
     */
    std::string createCertificateRequest(const std::string &subjectName, size_t bitLength);

    /**
     * Import the certificate into the KeyStore and link it to the CNG key
     * @param pemCert
     */
    void importCertificate(const std::string &pemCert);

    /**
     * Export the Micrsoft PFX file (PKCS12)
     * @param issuer This is the CA of the certificate
     * @param serial This is the hex string of the certificate to export
     * @param Password to use to export the certificate
     */
    std::string pfxExport(const std::string &issuer, const std::string &serial, const std::wstring &password);

    /**
     * Import the Micrsoft PFX file (PKCS12)
     * @param pfxInBase64 is the PFX(PKCS12) data in base64 format
     * @param Password to use to import the certificate
     */
    void pfxImport(const std::string &pfxInBase64, const std::wstring &password);

    /**
     * return the last CNG key created so it can be deleted during tests if necessary
     */
    const std::wstring &getLastKeyId();

private:
    std::string createCertificateRequestFromCNG(const std::string &subjectName, KeyPair *keyPair);

    KeyStore keyStore;

    std::wstring lastKeyId;

    HCERTSTORE storeHandle;

    bool isCACertificate(PCCERT_CONTEXT certificateCtx);
};


#endif //KSMGMNT_CERTIFICATESTORE_H
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
