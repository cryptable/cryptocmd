/*
 * MIT License
 * Author: David Tillemans
 */
#include <catch2/catch.hpp>
#include "KeyStore.hpp"
#include "utils/KeyStoreUtil.h"

TEST_CASE( "KeyStoreTests", "[success]" ) {

    SECTION( "Generate a 2048 bit Signing Key" ) {
        // Arrange
        const unsigned long rsaLength = 2048;
        KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
        if (keyStoreUtil.isKeyInKeystore(L"My Key")) {
            keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
        }

        // Act
        KeyStore keyStore(MS_KEY_STORAGE_PROVIDER);
        keyStore.generateSigningKeyPair(L"My Key", rsaLength);

        // Assert
        REQUIRE(keyStoreUtil.isKeyInKeystore(L"My Key"));

        // Cleanup
        keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
    }

    SECTION( "Generate a CSR in KeyStore" ) {
        // Arrange
        const unsigned long rsaLength = 2048;
        KeyStoreUtil keyStoreUtil(MS_KEY_STORAGE_PROVIDER);
        if (keyStoreUtil.isKeyInKeystore(L"My Key")) {
            keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
        }

        // Act
        // TODO: keyStore.createCertificateSigningRequest(L"My Key", rsaLength);

        // Assert
        REQUIRE(keyStoreUtil.isKeyInKeystore(L"My Key"));
        // TODO: OpenSSL validation of the CSR

        // Cleanup
        keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
    }
}

