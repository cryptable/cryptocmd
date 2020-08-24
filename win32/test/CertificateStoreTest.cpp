/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 08/08/2020
 */
#include <catch2/catch.hpp>
#include <OpenSSLCertificateRequest.h>
#include <OpenSSLCA.h>
#include <iostream>
#include <OpenSSLPKCS12.h>
#include <locale>
#include <codecvt>
#include "CertificateStore.h"
#include "utils/KeyStoreUtil.h"
#include "utils/CertStoreUtil.h"
#include "Base64Utils.h"

#define KEY_MGMGNT 0

#if KEY_MGMGNT
/*
 * Key management testing to remove keys.
 * Be careful: Delete CNG keys is optimalized for each PC
 */
TEST_CASE( "CertificateStoreUtilTests", "[mgmnt]" ) {
    SECTION( "List Certificates" ) {
        CertStoreUtil certStoreUtil;
        certStoreUtil.showCertificatesOfCertStore();
        certStoreUtil.close();
    }
}

TEST_CASE( "KeyStoreUtilTests", "[mgmnt]" ) {
    SECTION( "List CNG Keys" ) {
        KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
        keyStoreUtil.showKeysOfKeystore();
    }
}

TEST_CASE( "Delete CNG Keys KeyStoreUtilTests", "[mgmnt]" ) {
    /*
     * Don't delete
     * b18c7cd4-d54c-486c-93be-974837d02cec\x00: Provider Name: Microsoft Software Key Storage Provider
     * b18c7cd4-d54c-486c-93be-974837d02cec\x00: Container Name: d53acff0-35ac-4942-b6c1-f3b75ecfc960
     */
    SECTION( "Delete CNG Keys" ) {
        KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
        keyStoreUtil.deleteTestKeysOfKeystore();
    }
}
#endif

TEST_CASE( "CertificateStoreTests", "[success]" ) {

    SECTION("Create a CSR") {
        // Arrange
        KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
        if (keyStoreUtil.isKeyInKeystore(L"My Key")) {
            keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
        }
    }

    SECTION("Create a CSR") {
        // Arrange
        KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
        if (keyStoreUtil.isKeyInKeystore(L"My Key")) {
            keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
        }
        CertificateStore certificateStore;

        // Act
        std::string csr = certificateStore.createCertificateRequest("cn=John Doe, o=Company, c=US", 2048);

        // Assert
        REQUIRE(csr.data() != NULL);
        REQUIRE(csr.size() != 0);
        OpenSSLCertificateRequest verifyCSR(csr);
        REQUIRE(verifyCSR.verify());

        // Cleanup
        keyStoreUtil.deleteKeyFromKeyStore(certificateStore.getLastKeyId().c_str());
    }

    SECTION("Import the certificate corresponding the CSR") {
        // Arrange
        CertStoreUtil certStoreUtil;
        if (certStoreUtil.hasCertificates(L"John Doe")) {
            certStoreUtil.deleteCertificates(L"John Doe");
        }
        certStoreUtil.close();
        CertificateStore certificateStore;
        auto csr = certificateStore.createCertificateRequest(std::string("cn=John Doe, o=Company, c=US"), 2048);
        OpenSSLCertificateRequest openSslCertificateRequest(csr);
        OpenSSLCA openSslca("/CN=rootCA", 2048);
        auto cert = openSslca.certify(openSslCertificateRequest);

        // Act
        REQUIRE_NOTHROW(certificateStore.importCertificate(cert->getPEM()));

        // Assert
        certStoreUtil.reopen();
#pragma warning(disable:4996) // DISABLE(has no impact) warning C4996: 'getenv': This function or variable may be unsafe. Consider using _dupenv_s instead.
        if (getenv("SHOW_INFO") != NULL) {
#pragma warning(default:4996)
            certStoreUtil.showCertificatesOfCertStore();
            KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
            keyStoreUtil.showKeysOfKeystore();
            std::cout << "David :" << std::endl;
            certStoreUtil.showPropertiesOfCertificate(L"david");
        }

        REQUIRE(certStoreUtil.hasPrivateKey(L"John Doe"));

        // Cleanup
        certStoreUtil.deleteCertificates(L"John Doe");
    }

    SECTION("Import a PKCS12 file") {
        // Arrange
        CertStoreUtil certStoreUtil;
        if (certStoreUtil.hasCertificates(L"John Doe")) {
            certStoreUtil.deleteCertificates(L"John Doe");
        }
        certStoreUtil.close();
        OpenSSLCertificateRequest openSslCertificateRequest(std::string("/CN=John Doe/O=Company/C=US"), 2048);
        OpenSSLCA openSslca("/CN=RootCA/O=Company/C=US", 4096);
        auto cert = openSslca.certify(openSslCertificateRequest);
        OpenSSLPKCS12 openSslpkcs12(*(cert.get()),
                                    openSslca.getCertificate(),
                                    openSslCertificateRequest.getKeyPair(),
                                    std::string("system"));
        auto pkcs12 = openSslpkcs12.getPKCS12();
        auto b64pkcs12 = Base64Utils::toBase64(pkcs12);
        CertificateStore certificateStore;

        // Act
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        REQUIRE_NOTHROW(certificateStore.pfxImport(b64pkcs12, converter.from_bytes("system")));

        // Assert
        certStoreUtil.reopen();
#pragma warning(disable:4996) // DISABLE(has no impact) warning C4996: 'getenv': This function or variable may be unsafe. Consider using _dupenv_s instead.
        if (getenv("SHOW_INFO") != NULL) {
#pragma warning(default:4996)
            certStoreUtil.showCertificatesOfCertStore();
            KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
            keyStoreUtil.showKeysOfKeystore();
            std::cout << "John Doe :" << std::endl;
            certStoreUtil.showPropertiesOfCertificate(L"John Doe");
        }

        REQUIRE(certStoreUtil.hasCertificates(L"John Doe"));
        REQUIRE(certStoreUtil.hasPrivateKey(L"John Doe"));

        // Cleanup
        certStoreUtil.deleteCertificates(L"John Doe");
    }

    SECTION("Export to a PKCS12 file") {
        // Arrange
        {
            CertStoreUtil certStoreUtil;
            if (certStoreUtil.hasCertificates(L"John Doe")) {
                certStoreUtil.deleteCertificates(L"John Doe");
            }
            certStoreUtil.close();
            CertificateStore certificateStore;
            auto csr = certificateStore.createCertificateRequest(std::string("cn=John Doe, o=Company, c=US"), 2048);
            OpenSSLCertificateRequest openSslCertificateRequest(csr);
            OpenSSLCA openSslca("/CN=RootCA", 2048);
            auto cert = openSslca.certify(openSslCertificateRequest);
            REQUIRE_NOTHROW(certificateStore.importCertificate(cert->getPEM()));
        }

        // Act
        std::string p12;
        {
            CertificateStore certificateStore;
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            REQUIRE_NOTHROW(p12 = certificateStore.pfxExport(std::string("cn=RootCA"), std::string("03"),
                                                             converter.from_bytes("system")));
        }

        // Assert
        auto p12Data = Base64Utils::fromBase64(p12);
        OpenSSLPKCS12 openSslpkcs12(p12Data.data(), p12Data.size(), "system");
        REQUIRE(openSslpkcs12.getCertificate().getCommonName() == "John Doe");
        REQUIRE(openSslpkcs12.getCAs().size() == 0);
        REQUIRE(openSslpkcs12.getPrivateKey().getKeyBitlength() == 2048);

        // Cleanup
        {
            CertStoreUtil certStoreUtil;
            certStoreUtil.deleteCertificates(L"John Doe");
        }
    }
}

/*
 * User Interfase testing
 */
TEST_CASE( "Key enforcement test with imported key", "[ui]" ) {

    // Arrange
    CertStoreUtil certStoreUtil;
    if (certStoreUtil.hasCertificates(L"John Doe")) {
        certStoreUtil.deleteCertificates(L"John Doe");
    }
    certStoreUtil.close();
    OpenSSLCertificateRequest openSslCertificateRequest(std::string("/CN=John Doe/O=Company/C=US"), 2048);
    OpenSSLCA openSslca("/CN=RootCA/O=Company/C=US", 4096);
    auto cert = openSslca.certify(openSslCertificateRequest);
    OpenSSLPKCS12 openSslpkcs12(*(cert.get()),
                                openSslca.getCertificate(),
                                openSslCertificateRequest.getKeyPair(),
                                std::string("system"));
    auto pkcs12 = openSslpkcs12.getPKCS12();
    auto b64pkcs12 = Base64Utils::toBase64(pkcs12);

    //Act
    CertificateStore certificateStore;
    certificateStore.forcePasswordPINProtection(CertificateStore::strongKeyProtection::UIAlways);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    REQUIRE_NOTHROW(certificateStore.pfxImport(b64pkcs12, converter.from_bytes("system"), true));

    certStoreUtil.reopen();
    REQUIRE(certStoreUtil.hasCertificates(L"John Doe"));
    REQUIRE(certStoreUtil.hasPrivateKey(L"John Doe"));
}

TEST_CASE( "Key enforcement test with generated key", "[ui]" ) {
    // Arrange
    CertStoreUtil certStoreUtil;
    if (certStoreUtil.hasCertificates(L"John Doe")) {
        certStoreUtil.deleteCertificates(L"John Doe");
    }
    certStoreUtil.close();
    CertificateStore certificateStore;
    certificateStore.forcePasswordPINProtection(CertificateStore::strongKeyProtection::UIAlways);
    auto csr = certificateStore.createCertificateRequest(std::string("cn=John Doe, o=Company, c=US"),
                                                         2048,
                                                         true);
    OpenSSLCertificateRequest openSslCertificateRequest(csr);
    OpenSSLCA openSslca("/CN=rootCA", 2048);
    auto cert = openSslca.certify(openSslCertificateRequest);

    // Act
    REQUIRE_NOTHROW(certificateStore.importCertificate(cert->getPEM()));

    // Assert
    certStoreUtil.reopen();
#pragma warning(disable:4996) // DISABLE(has no impact) warning C4996: 'getenv': This function or variable may be unsafe. Consider using _dupenv_s instead.
    if (getenv("SHOW_INFO") != NULL) {
#pragma warning(default:4996)
        certStoreUtil.showCertificatesOfCertStore();
        KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
        keyStoreUtil.showKeysOfKeystore();
        std::cout << "David :" << std::endl;
        certStoreUtil.showPropertiesOfCertificate(L"david");
    }

    REQUIRE(certStoreUtil.hasCertificates(L"John Doe"));
    REQUIRE(certStoreUtil.hasPrivateKey(L"John Doe"));

    // Cleanup
    // certStoreUtil.deleteCertificates(L"John Doe");
}

TEST_CASE( "Sign using CryptoAPI", "[ui]" ) {
    CertStoreUtil certStoreUtil;

    REQUIRE(certStoreUtil.hasPrivateKey(L"John Doe"));

}

TEST_CASE( "Export using CryptoAPI", "[ui]" ) {

    // Act
    std::string p12;
    CertificateStore certificateStore;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    REQUIRE_NOTHROW(p12 = certificateStore.pfxExport(std::string("cn=rootCA"), std::string("03"),
                                                     converter.from_bytes("system")));

    // Assert
    auto p12Data = Base64Utils::fromBase64(p12);
    OpenSSLPKCS12 openSslpkcs12(p12Data.data(), p12Data.size(), "system");
    REQUIRE(openSslpkcs12.getCertificate().getCommonName() == "John Doe");
    REQUIRE(openSslpkcs12.getCAs().size() == 0);
    REQUIRE(openSslpkcs12.getPrivateKey().getKeyBitlength() == 2048);

}