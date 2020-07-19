/*
 * MIT License
 * Author: David Tillemans
 */
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
    GeneralName(std::string &name) {
        DWORD encodedNameLg = 0;

        if (!CertStrToName(X509_ASN_ENCODING, 
            name.c_str(), 
            CERT_OID_NAME_STR,
            NULL,
            NULL,
            &encodedNameLg,
            NULL)) {
            throw KSException(GetLastError());
        }
        blobEncodedName.pbData = new BYTE[encodedNameLg];
        blobEncodedName.cbData = encodedNameLg;
        if (!CertStrToName(X509_ASN_ENCODING, 
            name.c_str(), 
            CERT_OID_NAME_STR,
            NULL,
            blobEncodedName.pbData,
            &blobEncodedName.cbData,
            NULL)) {
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

    CERT_BLOB &getEncodedBlob() {
        return blobEncodedName;
    }

private:
    CERT_BLOB blobEncodedName;
    std::string    generalName;

};