//
// Created by david on 19/07/2020.
//

#ifndef KSMGMNT_HEXUTILS_H
#define KSMGMNT_HEXUTILS_H
#include <iostream>
#include <sstream>
#include <stdexcept>

class HexUtils {
public:
    static void hexToBin(std::string &hexString, std::ostream &byteStream) {
        if ((hexString.size() % 2) == 1)
            throw std::length_error("Hex string is not even length");
        for (int i=0; i<hexString.size(); i=i+2) {
             char byte = hexString[i+1] - '0';
             byte += (hexString[i] - '0') << 4;
             byteStream << byte;
        }
    }

    static std::string binToHex(std::istream &byteStream) {
        std::ostringstream ss;
        while (byteStream.eof()) {
            char byte = 0;
            byteStream >> byte;
            ss << std::hex << byte;
        }
        std::string result = ss.str();
        return result;
    }
};


// std::setfill('0') << std::setw(2)
#endif //KSMGMNT_HEXUTILS_H
