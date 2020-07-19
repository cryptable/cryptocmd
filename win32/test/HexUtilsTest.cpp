/*
 * MIT License
 * Author: David Tillemans
 */
#include <catch2/catch.hpp>
#include <iostream>
#include "utils/HexUtils.hpp"

TEST_CASE( "HexUtilsTests", "[success]" ) {

    SECTION( "Hex to binary" ) {
        // Arrange
        std::string dname = "01020304";
        unsigned char ref_data[] = { 0x01, 0x02, 0x03, 0x04 };
        std::stringstream byteStream;

        // Act
        byteStream = HexUtils::hexToBin(dname);

        // Assert
        REQUIRE( memcmp(byteStream.str().data(), ref_data, sizeof(ref_data)) == 0);
    }

    SECTION( "Hex to binary using character string" ) {
        // Arrange
        char dname[] = "01020304";
        unsigned char ref_data[] = { 0x01, 0x02, 0x03, 0x04 };
        std::stringstream byteStream;

        // Act
        byteStream = HexUtils::hexToBin(dname);

        // Assert
        REQUIRE( memcmp(byteStream.str().data(), ref_data, sizeof(ref_data)) == 0);
    }

    SECTION( "Hex to binary using character string" ) {
        // Arrange
        char dname[] = "30133111300f060355040313084a6f686e20446f65";
        unsigned char ref_data[] = { 0x30, 0x13, 0x31, 0x11, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13,
                                     0x08, 0x4a, 0x6f, 0x68, 0x6e, 0x20, 0x44, 0x6f, 0x65 };
        std::stringstream byteStream;

        // Act
        byteStream = HexUtils::hexToBin(dname);

        // Assert
        REQUIRE( memcmp(byteStream.str().data(), ref_data, sizeof(ref_data)) == 0);
    }

    SECTION( "Binary to Hex" ) {
        // Arrange
        std::string ref_data = "01020304";
        unsigned char input[] = { 0x01, 0x02, 0x03, 0x04 };

        // Act
        std::string hexData = HexUtils::binToHex(input, sizeof(input));

        // Assert
        REQUIRE( ref_data == hexData );
    }

}

TEST_CASE( "Failed HexUtilsTests", "[failed]" ) {

    SECTION( "Hex to binary with incorrect length" ) {
        // Arrange
        std::string dname = "0102034";
        unsigned char ref_data[] = { 0x01, 0x02, 0x03, 0x04 };
        std::stringstream byteStream;

        // Act
        REQUIRE_THROWS_AS(byteStream = HexUtils::hexToBin(dname), std::length_error);

        // Assert
    }
}
