/*
 * MIT License
 * Author: David Tillemans
 */
#include <catch2/catch.hpp>
#include <GeneralName.hpp>
#include "utils/HexUtils.hpp"
using Catch::Matchers::Message;

TEST_CASE( "GeneralNameTests", "[success]" ) {

    SECTION( "Generate a GeneralName" ) {
        // Arrange
		std::string dname = "cn=John Doe";
        std::stringstream ref_data = HexUtils::hexToBin("30133111300f060355040313084a6f686e20446f65");

		// Act
	    GeneralName generalName(dname);

	    // Assert
        REQUIRE( dname == generalName.getName() );
        REQUIRE( ref_data.str().size() == generalName.getEncodedBlob().cbData );
        REQUIRE( memcmp(ref_data.str().data(), generalName.getEncodedBlob().pbData, generalName.getEncodedBlob().cbData) == 0 );
    }
}

TEST_CASE( "Failed GeneralNameTests", "[failed]" ) {

    SECTION( "Generate a GeneralName which throws" ) {
        // Arrange
        std::string dname = "=John Doe";

        // Act && Assert
        REQUIRE_THROWS_MATCHES( GeneralName(dname), KSException,
                Message("The string contains an invalid X500 name attribute key, oid, value or delimiter.\r\n"));

    }
}