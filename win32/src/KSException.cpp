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

KSException::KSException(DWORD cd) noexcept : tmpMsgBuf{nullptr}, errorNumber{cd} {
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  nullptr,
                  cd,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPSTR)&tmpMsgBuf,
                  0,
                  nullptr );
}

KSException::KSException(const KSException& from) noexcept : std::exception(from), tmpMsgBuf{nullptr}, errorNumber{from.errorNumber} {
    tmpMsgBuf = static_cast<LPSTR>(LocalAlloc(0, LocalSize(from.tmpMsgBuf)));
    strncpy_s(tmpMsgBuf, LocalSize(tmpMsgBuf), from.tmpMsgBuf, LocalSize(from.tmpMsgBuf));
    errorNumber = from.errorNumber;
}

DWORD KSException::code() const noexcept {
    return errorNumber;
}

KSException::~KSException() noexcept {
    LocalFree(tmpMsgBuf);
    tmpMsgBuf = nullptr;
}

char const * KSException::what() const noexcept {
    return tmpMsgBuf;
}

KSException::operator std::string() const {
    return tmpMsgBuf;
}