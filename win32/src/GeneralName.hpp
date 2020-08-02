/*
 * MIT License
 * Author: David Tillemans
 */
#ifndef GENERALNAME_HPP
#define GENERALNAME_HPP

#include <windows.h>
#include <wincrypt.h>
#include <string>
#include "KSException.hpp"

/**
 * @brief      This class encapsulates the general name functions for subject and issuer names
 */
class GeneralName {
public:
    /**
     * @brief      Constructs a new instance of GeneralName.
     *
     * @param      name  The name in string version in the format as for example 
     * "cn=John Doe, o=Company, c=US"
     */
    explicit GeneralName(std::string &name) : blobEncodedName{0, nullptr} {
        DWORD encodedNameLg = 0;

        if (!CertStrToName(X509_ASN_ENCODING, 
            name.c_str(), 
            CERT_OID_NAME_STR,
            nullptr,
            nullptr,
            &encodedNameLg,
            nullptr)) {
            throw KSException(GetLastError());
        }
        blobEncodedName.pbData = new BYTE[encodedNameLg];
        blobEncodedName.cbData = encodedNameLg;
        if (!CertStrToName(X509_ASN_ENCODING, 
            name.c_str(), 
            CERT_OID_NAME_STR,
            nullptr,
            blobEncodedName.pbData,
            &blobEncodedName.cbData,
            nullptr)) {
            throw KSException(GetLastError());
        }
		generalName = name;
    }

    ~GeneralName() {
        free(blobEncodedName.pbData);
    }

    std::string &getName() {
        return generalName;
    }

    CERT_NAME_BLOB &getEncodedBlob() {
        return blobEncodedName;
    }

private:
    CERT_NAME_BLOB blobEncodedName;
    std::string    generalName;

};
#endif // GENERALNAME_HPP