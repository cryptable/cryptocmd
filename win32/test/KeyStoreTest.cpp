/*
 * MIT License
 * Author: David Tillemans
 */
#include <catch2/catch.hpp>
#include "KeyStore.h"
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
        keyStore.generateKeyPair(L"My Key", rsaLength);

        // Assert
        REQUIRE(keyStoreUtil.isKeyInKeystore(L"My Key"));

        // Cleanup
        keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
    }
}

