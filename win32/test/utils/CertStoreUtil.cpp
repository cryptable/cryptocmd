/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 02/08/2020
 */

#include <KSException.h>
#include <utility>
#include <vector>
#include <iostream>
#include <locale>
#include <codecvt>
#include "CertStoreUtil.h"
#include "HexUtils.hpp"
#include "CNGHash.h"
#include "CNGSign.h"

CertStoreUtil::CertStoreUtil() : hStoreHandle{nullptr},
    name{"MY"},
    keyStoreUtil(MS_KEY_STORAGE_PROVIDER),
    storeOpen{true} {
    if ((hStoreHandle = CertOpenSystemStoreA(
            NULL,
            name.c_str())) == nullptr)
    {
        throw KSException(GetLastError());
    }
}

CertStoreUtil::CertStoreUtil(const std::string &certStoreName, const std::wstring &keyStoreProviderName) :
    hStoreHandle{nullptr}, name{certStoreName}, keyStoreUtil(keyStoreProviderName.c_str())  {
    if ((hStoreHandle = CertOpenSystemStoreA(
            NULL,
            name.c_str())) == nullptr)
    {
        throw KSException(GetLastError());
    }
}

void CertStoreUtil::close() {
    if (storeOpen) {
        if (!CertCloseStore(hStoreHandle, 0)) {
            throw KSException(GetLastError());
        }
        storeOpen = false;
    }
}

void CertStoreUtil::reopen() {
    if (!storeOpen) {
        if ((hStoreHandle = CertOpenSystemStoreA(
                NULL,
                name.c_str())) == nullptr)
        {
            throw KSException(GetLastError());
        }
        storeOpen = true;
    }
}

void CertStoreUtil::showCertificatesOfCertStore() {
    PCCERT_CONTEXT  pCertContext = nullptr;
    std::cout << std::endl;

    pCertContext = CertEnumCertificatesInStore(
            hStoreHandle,
            pCertContext);
    while(pCertContext)
    {
        DWORD size;

        if(!(size = CertGetNameString(
                pCertContext,
                CERT_NAME_SIMPLE_DISPLAY_TYPE,
                0,
                nullptr,
                nullptr,
                0)))
        {
            throw KSException(GetLastError());
        }

        std::vector<TCHAR> subjectName(size);
        if(!CertGetNameString(
                pCertContext,
                CERT_NAME_SIMPLE_DISPLAY_TYPE,
                0,
                nullptr,
                subjectName.data(),
                size)) {
            throw KSException(GetLastError());
        }
        std::cout << "Subject -> " << subjectName.data() << std::endl;
        std::cout << "Serial : " << HexUtils::binToHex(pCertContext->pCertInfo->SerialNumber.pbData,
                                                       pCertContext->pCertInfo->SerialNumber.cbData) << std::endl;

        // showPropertiesOfCertificate(std::wstring(subjectName.begin(), subjectName.end()));
        pCertContext = CertEnumCertificatesInStore(
                hStoreHandle,
                pCertContext);
    }
}

bool CertStoreUtil::hasPrivateKey(const std::wstring &subject) {
    PCCERT_CONTEXT  pCertContext;
    HCRYPTPROV_OR_NCRYPT_KEY_HANDLE keyHandle;
    DWORD keySpecs;
    BOOL  mustFreeKeyHandle;

    pCertContext=CertFindCertificateInStore(hStoreHandle,
                                            X509_ASN_ENCODING,
                                            0,
                                            CERT_FIND_SUBJECT_STR,
                                            subject.c_str(),
                                            nullptr);
    if (pCertContext == nullptr) {
        throw KSException(GetLastError());
    }

    if (!CryptAcquireCertificatePrivateKey(pCertContext,
                                          CRYPT_ACQUIRE_COMPARE_KEY_FLAG | CRYPT_ACQUIRE_ALLOW_NCRYPT_KEY_FLAG,
                                          nullptr,
                                           &keyHandle,
                                           &keySpecs,
                                           &mustFreeKeyHandle)) {
        throw KSException(GetLastError());
    }

    CNGHash hash;
    std::string plainData("This is test data");
    hash.update(plainData.data(), plainData.size());
    auto hashedData = hash.finalize();
    CNGSign sign(keyHandle);
    auto signature = sign.sign(hashedData.data(), hashedData.size());

    return true;
}

bool CertStoreUtil::hasCertificates(const std::wstring &subject) {
    if (CertFindCertificateInStore(hStoreHandle,
                                   X509_ASN_ENCODING,
                                   0,
                                   CERT_FIND_SUBJECT_STR,
                                   subject.c_str(),
                                   nullptr)) {
        return true;
    }
    return false;
}

void CertStoreUtil::deleteCNGKeyIfAvailable(PCCERT_CONTEXT pCertContext) {

    try {
        auto data = getData(pCertContext, CERT_KEY_PROV_INFO_PROP_ID);
        CRYPT_KEY_PROV_INFO *privInfo = (CRYPT_KEY_PROV_INFO *)data.data();
        keyStoreUtil.deleteKeyFromKeyStore(privInfo->pwszContainerName);
    }
    catch (KSException &e) {
        std::cout << e.what();
    }
}

void CertStoreUtil::deleteCertificates(const std::wstring &subject) {
    PCCERT_CONTEXT  pCertContext;

    pCertContext = CertFindCertificateInStore(hStoreHandle,
                                              X509_ASN_ENCODING,
                                              0,
                                              CERT_FIND_SUBJECT_STR,
                                              subject.c_str(),
                                              nullptr);
    while (pCertContext) {
        // First delete the linked private key
        deleteCNGKeyIfAvailable(pCertContext);
        if (!CertDeleteCertificateFromStore(pCertContext)) {
            throw KSException(GetLastError());
        }
        pCertContext = CertFindCertificateInStore(hStoreHandle,
                                                  X509_ASN_ENCODING,
                                                  0,
                                                  CERT_FIND_SUBJECT_STR,
                                                  subject.c_str(),
                                                  nullptr);
    }

}

CertStoreUtil::~CertStoreUtil() {
    if (storeOpen) {
        CertCloseStore(hStoreHandle, 0);
    }
}

std::vector<unsigned char> CertStoreUtil::getData(PCCERT_CONTEXT pCertContext, DWORD propertyId) {
    DWORD dataLg = 0;
    if(!CertGetCertificateContextProperty(
            pCertContext,
            propertyId ,
            NULL ,
            &dataLg))
    {
        throw KSException(GetLastError());
    }

    std::vector<unsigned char> data(dataLg);
    CertGetCertificateContextProperty(
            pCertContext,
            propertyId,
            data.data(),
            &dataLg);

    return data;
}

std::string ws2s(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

void CertStoreUtil::showPropertiesOfCertificate(const std::wstring &subject) {
    PCCERT_CONTEXT  pCertContext;
    std::string tmpSubject = ws2s(subject);
    pCertContext = CertFindCertificateInStore(hStoreHandle,
                                              X509_ASN_ENCODING,
                                              0,
                                              CERT_FIND_SUBJECT_STR,
                                              subject.c_str(),
                                              nullptr);
    if (pCertContext == nullptr) {
        throw KSException(GetLastError());
    }

    DWORD propertyId = 0;
    propertyId = CertEnumCertificateContextProperties(
            pCertContext,
            propertyId);
    while(propertyId)
    {
        std::cout << "Property number: " << propertyId << std::endl;

        switch(propertyId)
        {
            case CERT_FRIENDLY_NAME_PROP_ID:
            {
                std::cout << "Display name: ";
                std::cout << std::endl;
                break;
            }
            case CERT_SIGNATURE_HASH_PROP_ID:
            {
                std::cout << "Signature hash identifier ";
                std::cout << std::endl;
                break;
            }
            case CERT_KEY_PROV_HANDLE_PROP_ID:
            {
                std::cout << "KEY PROVE HANDLE";
                std::cout << std::endl;
                break;
            }
            case CERT_KEY_PROV_INFO_PROP_ID:
            {
                std::cout << "KEY PROV INFO PROP ID ";
                std::cout << std::endl;
                auto data = getData(pCertContext, propertyId);
                CRYPT_KEY_PROV_INFO *privInfo = (CRYPT_KEY_PROV_INFO *)data.data();
                std::wstring tmpData1(privInfo->pwszProvName);
                std::cout << tmpSubject << ": Provider Name: " << ws2s(tmpData1) << std::endl;
                std::wstring tmpData2(privInfo->pwszContainerName);
                std::cout << tmpSubject << ": Container Name: " << ws2s(tmpData2) << std::endl;
                break;
            }
            case CERT_SHA1_HASH_PROP_ID:
            {
                std::cout << "SHA1 HASH identifier: ";
                auto data = getData(pCertContext, propertyId);
                std::cout << "The Property Content is " << HexUtils::binToHex(data.data(), data.size());
                std::cout << std::endl;
                break;
            }
            case CERT_MD5_HASH_PROP_ID:
            {
                std::cout << "md5 hash identifier: ";
                auto data = getData(pCertContext, propertyId);
                std::cout << "The Property Content is " << HexUtils::binToHex(data.data(), data.size());
                std::cout << std::endl;
                break;
            }
            case CERT_KEY_CONTEXT_PROP_ID:
            {
                std::cout << "KEY CONTEXT PROP identifier";
                std::cout << std::endl;
                break;
            }
            case CERT_KEY_SPEC_PROP_ID:
            {
                std::cout << "KEY SPEC PROP identifier";
                std::cout << std::endl;
                break;
            }
            case CERT_ENHKEY_USAGE_PROP_ID:
            {
                std::cout << "ENHKEY USAGE PROP identifier";
                std::cout << std::endl;
                break;
            }
            case CERT_NEXT_UPDATE_LOCATION_PROP_ID:
            {
                std::cout << "NEXT UPDATE LOCATION PROP identifier";
                std::cout << std::endl;
                break;
            }
            case CERT_PVK_FILE_PROP_ID:
            {
                std::cout << "PVK FILE PROP identifier ";
                std::cout << std::endl;
                break;
            }
            case CERT_DESCRIPTION_PROP_ID:
            {
                std::cout << "DESCRIPTION PROP identifier ";
                std::cout << std::endl;
                break;
            }
            case CERT_ACCESS_STATE_PROP_ID:
            {
                std::cout << "ACCESS STATE PROP identifier ";
                std::cout << std::endl;
                break;
            }
            case CERT_SMART_CARD_DATA_PROP_ID:
            {
                std::cout << "SMART_CARD DATA PROP identifier ";
                std::cout << std::endl;
                break;
            }
            case CERT_EFS_PROP_ID:
            {
                std::cout << "EFS PROP identifier ";
                std::cout << std::endl;
                break;
            }
            case CERT_FORTEZZA_DATA_PROP_ID:
            {
                std::cout << "FORTEZZA DATA PROP identifier ";
                std::cout << std::endl;
                break;
            }
            case CERT_ARCHIVED_PROP_ID:
            {
                std::cout << "ARCHIVED PROP identifier ";
                std::cout << std::endl;
                break;
            }
            case CERT_KEY_IDENTIFIER_PROP_ID:
            {
                std::cout << "KEY IDENTIFIER PROP identifier ";
                std::cout << std::endl;
                break;
            }
            case CERT_AUTO_ENROLL_PROP_ID:
            {
                std::cout << "AUTO ENROLL identifier. ";
                std::cout << std::endl;
                break;
            }
            case CERT_NCRYPT_KEY_HANDLE_PROP_ID:
            {
                std::cout << "Key handle to CNG. ";
                std::cout << std::endl;
                break;
            }
            default:
                std::cout << "Unknown Identifier. ";
                std::cout << std::endl;
        }

        propertyId = CertEnumCertificateContextProperties(
                pCertContext,
                propertyId);
    }
}