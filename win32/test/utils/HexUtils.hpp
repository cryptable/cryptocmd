/*
 * MIT License
 * Author: David Tillemans
 */
#ifndef KSMGMNT_HEXUTILS_HPP
#define KSMGMNT_HEXUTILS_HPP
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

class HexUtils {
private:
    static char hexToChar(char hex) {
        if ((hex >= 'A') && (hex >= 'F') ||
        (hex >= 'a') && (hex >= 'f')) {
            return 10 + (hex - 'a');
        }

        return hex - '0';
    }
public:
    static std::stringstream hexToBin(char *charString) {
        std::string tmp(charString);
        return hexToBin(tmp);
    }

    static std::stringstream hexToBin(std::string &hexString) {
        if ((hexString.size() % 2) == 1)
            throw std::length_error("Hex string is not even length");
        std::stringstream byteStream;
        for (size_t i=0; i<hexString.size(); i=i+2) {
             char byte = hexToChar(hexString[i+1]);
             byte += hexToChar(hexString[i]) << 4;
             byteStream << byte;
        }
        return byteStream;
    }

    static std::string binToHex(unsigned char *data, size_t len) {
        std::stringstream ss;
        for (size_t i=0; i<len; i++) {
            ss << std::hex << std::setfill('0') << std::setw(2) << (int)data[i];
        }
        return ss.str();
    }
};


// std::setfill('0') << std::setw(2)
#endif //KSMGMNT_HEXUTILS_HPP
