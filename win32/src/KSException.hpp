/*
 * MIT License
 * Author: David Tillemans
 */
#include <windows.h>
#include <strsafe.h>
#include <string>
#include <exception>

#ifndef KSEXCEPTION_HPP
#define KSEXCEPTION_HPP

class KSException : public std::exception {

public:
	explicit KSException(DWORD cd) noexcept : tmpMsgBuf{nullptr}, errorNumber{cd} {
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            cd,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&tmpMsgBuf,
            0,
            nullptr );
	};

    KSException(const KSException& from) noexcept : std::exception(from), tmpMsgBuf{nullptr}, errorNumber{from.errorNumber} {
        tmpMsgBuf = static_cast<LPSTR>(LocalAlloc(0, LocalSize(from.tmpMsgBuf)));
        strncpy_s(tmpMsgBuf, LocalSize(tmpMsgBuf), from.tmpMsgBuf, LocalSize(from.tmpMsgBuf));
        errorNumber = from.errorNumber;
    };

    DWORD code() const noexcept {
        return errorNumber;
    };

    ~KSException() noexcept override {
        LocalFree(tmpMsgBuf);
        tmpMsgBuf = nullptr;
    }

    char const * what() const noexcept override {
        return tmpMsgBuf;
	};

    explicit operator std::string() const { return tmpMsgBuf; };

private:
    DWORD errorNumber;
    LPSTR tmpMsgBuf;
};

#endif // KSEXCEPTION_H