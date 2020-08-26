/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 09/08/2020
 */

/*
 * MIT License
 * Author: David Tillemans
 */
#include <windows.h>
#include <strsafe.h>
#include <string>
#include <exception>
#include "KSException.h"

using namespace std;

KSException::KSException(const char *fName, int lineNumber, DWORD cd) noexcept : errorNumber{cd} {
    LPTSTR tmpMsgBuf = NULL;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  nullptr,
                  cd,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPSTR)&tmpMsgBuf,
                  0,
                  nullptr );
    result = std::string(fName) + "(" + std::to_string(lineNumber) + ") : error(" + std::to_string(errorNumber) + ") : " + tmpMsgBuf;
    if (tmpMsgBuf) {
        LocalFree(tmpMsgBuf);
    }
}

KSException::KSException(const char *fName, int lineNumber, const std::string &error) noexcept : errorNumber{ 0x90010001 } {
    result = std::string(fName) + "(" + std::to_string(lineNumber) + ") : " + error;
}

KSException::KSException(const KSException& from) noexcept : std::exception(from), errorNumber{from.errorNumber} {
    result = from.result;
    errorNumber = from.errorNumber;
}


DWORD KSException::code() const noexcept {
    return errorNumber;
}

KSException::~KSException() noexcept {
}

char const * KSException::what() const {
    return result.c_str();
}

KSException::operator std::string() const {
    return result;
}