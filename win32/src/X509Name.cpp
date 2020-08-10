/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 09/08/2020
 */
#include "X509Name.h"
#include <windows.h>
#include <wincrypt.h>
#include <string>
#include "KSException.h"

X509Name::X509Name(const std::string &name, bool utf8) : blobEncodedName{0, nullptr} {
    DWORD encodedNameLg = 0;
    DWORD flags = CERT_OID_NAME_STR;
    if (utf8) {
        flags |= CERT_NAME_STR_FORCE_UTF8_DIR_STR_FLAG;
    }
    if (!CertStrToName(X509_ASN_ENCODING,
                       name.c_str(),
                       flags,
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
                       flags,
                       nullptr,
                       blobEncodedName.pbData,
                       &blobEncodedName.cbData,
                       nullptr)) {
        throw KSException(GetLastError());
    }
    generalName = name;
}

X509Name::~X509Name() {
    free(blobEncodedName.pbData);
}

std::string &X509Name::getName() {
    return generalName;
}

CERT_NAME_BLOB &X509Name::getEncodedBlob() {
    return blobEncodedName;
}