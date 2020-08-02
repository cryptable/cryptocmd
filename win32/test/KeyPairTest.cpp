/*
 * MIT License
 * Author: David Tillemans
 */
#include <catch2/catch.hpp>
#include <KeyStore.hpp>
#include <fstream>
#include <iostream>
#include "utils/KeyStoreUtil.h"
#include "OpenSSLCertificateRequest.h"
#include "OpenSSLCA.h"
#include "utils/CertStoreUtil.h"

TEST_CASE( "KeyPairTests", "[success]" ) {

    SECTION( "Create a KeyPair" ) {
        // Arrange
        KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
        if (keyStoreUtil.isKeyInKeystore(L"My Key")) {
            keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
        }
        KeyStore keyStore(MS_KEY_STORAGE_PROVIDER);

        // Act
        auto keyPair = keyStore.generateSigningKeyPair(L"My Key", 2048);

        // Assert
        REQUIRE( keyPair->getPublicKeyInfo() != NULL );

        // Cleanup
        keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
    }

    SECTION( "Create a CSR" ) {
        // Arrange
        KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
        if (keyStoreUtil.isKeyInKeystore(L"My Key")) {
            keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
        }
        KeyStore keyStore(MS_KEY_STORAGE_PROVIDER);
        auto keyPair = keyStore.generateSigningKeyPair(L"My Key", 2048);

        // Act
        std::string csr = keyPair->getCertificateSigningRequest("cn=david");

        // Assert
        REQUIRE( csr.data() != NULL );
        REQUIRE( csr.size() != 0 );
        OpenSSLCertificateRequest verifyCSR(csr);
        REQUIRE( verifyCSR.verify() );

        // Cleanup
        keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
    }

    SECTION( "Import the certificate corresponding the CSR" ) {
        // Arrange
        KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
        if (keyStoreUtil.isKeyInKeystore(L"My Key")) {
            keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
        }
        CertStoreUtil certStoreUtil;
        if (certStoreUtil.hasCertificates(L"david")) {
            certStoreUtil.deleteCertificates(L"david");
        }
        certStoreUtil.close();

        KeyStore keyStore(MS_KEY_STORAGE_PROVIDER);
        auto keyPair = keyStore.generateSigningKeyPair(L"My Key", 2048);
        std::string csr = keyPair->getCertificateSigningRequest("cn=david");
        OpenSSLCertificateRequest openSslCertificateRequest(csr);
        OpenSSLCA openSslca("/CN=rootCA", 2048);
        auto cert = openSslca.certify(&openSslCertificateRequest);

        // Act
        REQUIRE_NOTHROW(keyPair->importCertificate(cert->getPEM()));

        // Assert
        certStoreUtil.reopen();
        certStoreUtil.showCertificatesOfCertStore();
        keyStoreUtil.showKeysOfKeystore();
        std::cout << "David :" << std::endl;
        certStoreUtil.showPropertiesOfCertificate(L"david");
        std::cout << "b18c7cd4-d54c-486c-93be-974837d02cec :" << std::endl;
        certStoreUtil.showPropertiesOfCertificate(L"b18c7cd4-d54c-486c-93be-974837d02cec");

        REQUIRE(certStoreUtil.hasPrivateKey(L"david"));

        // Cleanup
//        keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
//        certStoreUtil.showCertificateOfCertstore();
    }

}
