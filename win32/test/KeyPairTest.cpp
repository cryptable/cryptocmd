/*
 * MIT License
 * Author: David Tillemans
 */
#include <catch2/catch.hpp>
#include <KeyStore.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
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
        auto keyPair = keyStore.generateKeyPair(L"My Key", 2048);

        // Assert
        REQUIRE( keyPair.get() != NULL );

        // Cleanup
        keyStoreUtil.deleteKeyFromKeyStore(L"My Key");
    }
}
