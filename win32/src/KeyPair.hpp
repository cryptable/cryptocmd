/*
 * MIT License
 * Author: David Tillemans
 */
#ifndef KEYPAIR_HPP
#define KEYPAIR_HPP

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ncrypt.h>
#include <ntstatus.h>
#include "KSException.hpp"
#include "GeneralName.hpp"
#include <fstream>

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
    KeyPair(const NCRYPT_PROV_HANDLE cryptoProvider, NCRYPT_KEY_HANDLE key) : keyHandle{key} {
        DWORD publicKeyLg = 0;

        if (!CryptExportPublicKeyInfo(keyHandle,
                0,
                X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                NULL,
                &publicKeyLg)) {
            throw KSException(GetLastError());
        }
        publicKeyInfo = reinterpret_cast<CERT_PUBLIC_KEY_INFO *>(new unsigned char[publicKeyLg]);
        CryptExportPublicKeyInfo(keyHandle,
                                0,
                                X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                                publicKeyInfo,
                                &publicKeyLg);
    }

    CERT_PUBLIC_KEY_INFO *getPublicKeyInfo() { return publicKeyInfo; };

    std::string getCertificateSigningRequest(std::string subjectDN) {
        // TODO: https://docs.microsoft.com/en-us/windows/win32/seccrypto/example-c-program-making-a-certificate-request
        GeneralName subject(subjectDN);

        CERT_REQUEST_INFO  CertReqInfo;
        CertReqInfo.dwVersion = CERT_REQUEST_V1;
        CertReqInfo.Subject = subject.getEncodedBlob();
        CertReqInfo.SubjectPublicKeyInfo = *publicKeyInfo;
        CertReqInfo.cAttribute = 0;
        CertReqInfo.rgAttribute = NULL;

        DWORD certSignReqLg = 0;
        CRYPT_OBJID_BLOB  Parameters;
        CRYPT_ALGORITHM_IDENTIFIER  SigAlgo;
        memset(&Parameters, 0, sizeof(Parameters));
        SigAlgo.pszObjId = szOID_RSA_SHA256RSA;
        SigAlgo.Parameters = Parameters;
        if (!CryptSignAndEncodeCertificate(keyHandle,
                0,
                X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                X509_CERT_REQUEST_TO_BE_SIGNED,
                &CertReqInfo,
                &SigAlgo,
                NULL,
                NULL,
                &certSignReqLg)) {
            throw KSException(GetLastError());
        }
        BYTE* certSignReq = new unsigned char[certSignReqLg];
        CryptSignAndEncodeCertificate(keyHandle,
                                      0,
                                      X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                                      X509_CERT_REQUEST_TO_BE_SIGNED,
                                      &CertReqInfo,
                                      &SigAlgo,
                                      NULL,
                                      certSignReq,
                                      &certSignReqLg);

        return std::string((const char *)certSignReq, certSignReqLg);
    }

    ~KeyPair() {
        NCryptFreeObject(keyHandle);
        delete publicKeyInfo;
    }

public:
    NCRYPT_KEY_HANDLE keyHandle;
    CERT_PUBLIC_KEY_INFO *publicKeyInfo;
};

#endif // KEYPAIR