/*
 * MIT License
 * Author: David Tillemans
 */
#include <catch2/catch.hpp>
#include <X509Name.h>
#include <KSException.h>
#include "utils/HexUtils.hpp"
using Catch::Matchers::Message;

TEST_CASE( "GeneralNameTests", "[success]" ) {

    SECTION( "Generate a X509Name" ) {
        // Arrange
		std::string dname = "cn=John Doe";
        std::stringstream ref_data = HexUtils::hexToBin("30133111300f060355040313084a6f686e20446f65");

		// Act
	    X509Name generalName(dname, false);

	    // Assert
        REQUIRE( dname == generalName.getName() );
        REQUIRE( ref_data.str().size() == generalName.getEncodedBlob().cbData );
        REQUIRE( memcmp(ref_data.str().data(), generalName.getEncodedBlob().pbData, generalName.getEncodedBlob().cbData) == 0 );
    }
}

TEST_CASE( "GeneralNameTests UTF8 test", "[success]" ) {

    SECTION( "Generate a X509Name" ) {
        // Arrange
        std::string dname = "cn=John Doe";
        std::stringstream ref_data = HexUtils::hexToBin("30133111300f06035504030c084a6f686e20446f65");

        // Act
        X509Name generalName(dname);

        // Assert
        REQUIRE( dname == generalName.getName() );
        REQUIRE( ref_data.str().size() == generalName.getEncodedBlob().cbData );
        REQUIRE( memcmp(ref_data.str().data(), generalName.getEncodedBlob().pbData, generalName.getEncodedBlob().cbData) == 0 );
    }
}

TEST_CASE( "Failed GeneralNameTests", "[failed]" ) {

    SECTION( "Generate a X509Name which throws" ) {
        // Arrange
        std::string dname = "=John Doe";

        // Act && Assert
        REQUIRE_THROWS_MATCHES(X509Name(dname), KSException,
                               Message("The string contains an invalid X500 name attribute key, oid, value or delimiter.\r\n"));

    }
}