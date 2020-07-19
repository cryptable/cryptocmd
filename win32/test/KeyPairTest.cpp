/*
 * MIT License
 * Author: David Tillemans
 */
#include <catch2/catch.hpp>
#include <KeyStore.hpp>
#include <fstream>
#include "utils/KeyStoreUtil.h"

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
        REQUIRE( csr.size() != NULL );

        // TODO: use openssl to verify the CSR

        // Cleanup
        keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
    }

}
