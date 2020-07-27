/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 21/07/2020
 */
#include "OpenSSLCertificateRequest.h"
#include <openssl/err.h>
#include "OpenSSLException.h"

OpenSSLCertificateRequest::OpenSSLCertificateRequest(const char *req, size_t reqLg) {
    certificateRequest = d2i_X509_REQ(NULL, reinterpret_cast<const unsigned char **>(&req), reqLg);
    if (certificateRequest == NULL) {
        throw OpenSSLException(ERR_get_error());
    }
}

OpenSSLCertificateRequest::OpenSSLCertificateRequest(const std::string &dname, size_t bitLength) {


}

bool OpenSSLCertificateRequest::verify() {
    EVP_PKEY *pkey = NULL;

    if ((pkey = X509_REQ_get0_pubkey(certificateRequest)) == NULL) {
        throw OpenSSLException(ERR_get_error());
    }
    int i = X509_REQ_verify(certificateRequest, pkey);
    if (i < 0) {
        return false;
    }

    return true;
}

OpenSSLCertificateRequest::~OpenSSLCertificateRequest() {
    X509_REQ_free(certificateRequest);
}
