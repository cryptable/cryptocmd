/*
 * MIT License
 * Author: David Tillemans
 */
#include <windows.h>
#include <strsafe.h>

class KSException : public std::exception {

public:
	explicit KSException(DWORD cd) {
        LPSTR tmpMsgBuf = NULL;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,        
            NULL,
            cd,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&tmpMsgBuf,
            0, NULL );
        msgBuf = tmpMsgBuf;
        LocalFree(tmpMsgBuf);
		errorCode = cd;
	};

    DWORD code() {
        return errorCode;
    };

    virtual ~KSException() override {};

    virtual char const * what() const noexcept {
        return msgBuf.c_str();
	};

    operator std::string() const { return msgBuf; };

private:
    DWORD errorCode;
    std::string   msgBuf;
};