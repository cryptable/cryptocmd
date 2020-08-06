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
#include <fstream>
#include <vector>
#include <functional>
#include "KSException.hpp"
#include "GeneralName.hpp"

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
    explicit KeyPair(NCRYPT_KEY_HANDLE key) : keyHandle{key} {
        DWORD publicKeyLg = 0;

        if (!CryptExportPublicKeyInfo(keyHandle,
                0,
                X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                nullptr,
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

    CERT_PUBLIC_KEY_INFO *getPublicKeyInfo() const { return publicKeyInfo; };

    /**
     * create a Certificate signing request as a PEM encoded request
     * @param subjectDN
     * @return
     */
    std::string getCertificateSigningRequest(std::string subjectDN) const {
        // TODO: https://docs.microsoft.com/en-us/windows/win32/seccrypto/example-c-program-making-a-certificate-request
        GeneralName subject(subjectDN);

        CERT_REQUEST_INFO  CertReqInfo;
        CertReqInfo.dwVersion = CERT_REQUEST_V1;
        CertReqInfo.Subject = subject.getEncodedBlob();
        CertReqInfo.SubjectPublicKeyInfo = *publicKeyInfo;
        CertReqInfo.cAttribute = 0;
        CertReqInfo.rgAttribute = nullptr;

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
               nullptr,
               nullptr,
                &certSignReqLg)) {
            throw KSException(GetLastError());
        }
        auto certSignReq = std::unique_ptr<BYTE>(new unsigned char[certSignReqLg]);
        CryptSignAndEncodeCertificate(keyHandle,
                                      0,
                                      X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                                      X509_CERT_REQUEST_TO_BE_SIGNED,
                                      &CertReqInfo,
                                      &SigAlgo,
                                      nullptr,
                                      certSignReq.get(),
                                      &certSignReqLg);
        DWORD b64CertReqLg = 0;
        if (!CryptBinaryToStringA(certSignReq.get(),
                                  certSignReqLg,
                                  CRYPT_STRING_BASE64REQUESTHEADER,
                                  nullptr,
                                  &b64CertReqLg)) {
            throw KSException(GetLastError());
        }
        auto b64CertReq = std::unique_ptr<char>(new char[b64CertReqLg]);
        CryptBinaryToStringA(certSignReq.get(),
                             certSignReqLg,
                             CRYPT_STRING_BASE64REQUESTHEADER,
                             b64CertReq.get(),
                             &b64CertReqLg);
        return std::string(b64CertReq.get());
    }

    /**
     * Import the certificate into the keystore
     * @param pemCertificate
     */
    void importCertificate(const std::string &pemCertificate) {
        DWORD certLg = 0;
        if (!CryptStringToBinaryA(pemCertificate.c_str(),
                                  pemCertificate.size(),
                                  CRYPT_STRING_BASE64HEADER,
                                  nullptr,
                                  &certLg,
                                  nullptr,
                                  nullptr)) {
            throw KSException(GetLastError());
        }
        std::vector<unsigned char> cert(certLg);
        CryptStringToBinaryA(pemCertificate.c_str(),
                             pemCertificate.size(),
                             CRYPT_STRING_BASE64HEADER,
                             cert.data(),
                             &certLg,
                             nullptr,
                             nullptr);


        HCERTSTORE storeHandle = CertOpenSystemStoreA(NULL, "MY");
        if (storeHandle == nullptr) {
            throw KSException(GetLastError());
        }
        PCCERT_CONTEXT certContext = nullptr;
        if (!CertAddEncodedCertificateToStore(storeHandle,
                                              X509_ASN_ENCODING,
                                              cert.data(),
                                              certLg,
                                              CERT_STORE_ADD_ALWAYS,
                                              &certContext)) {
            CertCloseStore(storeHandle, 0);
            throw KSException(GetLastError());
        }
        // TODO: Refactor to KeyStore or access the KeyStore
        CRYPT_KEY_PROV_INFO cryptKeyProvInfo = {
                const_cast<LPWSTR>(L"My Key"),
                const_cast<LPWSTR>(MS_KEY_STORAGE_PROVIDER),
                0,
                0,
                0,
                nullptr,
                AT_SIGNATURE
        };
        if (!CertSetCertificateContextProperty(certContext,
                                               CERT_KEY_PROV_INFO_PROP_ID,
                                               0,
                                               &cryptKeyProvInfo)) {
            CertCloseStore(storeHandle, 0);
            throw KSException(GetLastError());
        }
        if (!CertSetCertificateContextProperty(certContext,
                                               CERT_NCRYPT_KEY_HANDLE_PROP_ID,
                                               0,
                                               &keyHandle)) {
            CertCloseStore(storeHandle, 0);
            throw KSException(GetLastError());
        }
        CertCloseStore(storeHandle, 0);
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
