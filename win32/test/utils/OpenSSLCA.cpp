/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 20/07/2020
 */
#include <windows.h>
#include <openssl/x509v3.h>
#include "OpenSSLCA.h"
#include "OpenSSLX509Name.h"
#include "OpenSSLException.h"

#define FIVE_YEARS   (5*365 + 1)

OpenSSLCA::OpenSSLCA(const std::string &rootName, int bitLength) {

    // TODO: Generate the Key
    EVP_PKEY_CTX *ctx = NULL;
    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (ctx == NULL) {
        throw std::bad_alloc();
    }
    int status = 0;
    if ((status = EVP_PKEY_keygen_init(ctx)) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw OpenSSLException(status);
    }
     if ((status = RSA_pkey_ctx_ctrl(ctx,
             EVP_PKEY_OP_KEYGEN,
             EVP_PKEY_CTRL_RSA_KEYGEN_BITS,
             bitLength, NULL)) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw OpenSSLException(status);
    }

    if ((status = EVP_PKEY_keygen(ctx, &keyPair)) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw OpenSSLException(status);
    }
    EVP_PKEY_CTX_free(ctx);

    // TODO: Create the Certificate
    X509 *x509Certificate = NULL;
    OpenSSLX509Name subject(rootName);
    if ((x509Certificate = X509_new()) == NULL) {
        throw std::bad_alloc();
    }
    if (!X509_set_version(x509Certificate, 2)) {
        throw OpenSSLException(ERR_get_error());
    }
    X509_set_subject_name(x509Certificate, subject.getX509Name());
    X509_set_issuer_name(x509Certificate, subject.getX509Name());

    if (X509_gmtime_adj(X509_getm_notBefore(x509Certificate), 0) == NULL) {
        throw OpenSSLException(ERR_get_error());
    }
    if (X509_time_adj_ex(X509_getm_notAfter(x509Certificate), FIVE_YEARS, 0, NULL) == NULL) {
        throw OpenSSLException(ERR_get_error());
    }

    if (!X509_set_pubkey(x509Certificate, keyPair)) {
        throw OpenSSLException(ERR_get_error());
    }

    X509_EXTENSION *basicConstraint = X509V3_EXT_conf_nid(NULL,
            NULL,
            NID_basic_constraints,
            "critical,CA:TRUE,pathLength:0");
    if (basicConstraint == NULL) {
        throw OpenSSLException(ERR_get_error());
    }
    if (!X509_add_ext(x509Certificate, basicConstraint, -1)) {
        throw OpenSSLException(ERR_get_error());
    }
    X509_EXTENSION *keyUsage = X509V3_EXT_conf_nid(NULL,
            NULL,
            NID_key_usage,
            "critical, digitalSignature, keyCertSign, cRLSign");
    if (keyUsage == NULL) {
        throw OpenSSLException(ERR_get_error());
    }
    if (!X509_add_ext(x509Certificate, keyUsage, -1)) {
        throw OpenSSLException(ERR_get_error());
    }
#if 0
    BASIC_CONSTRAINTS *bc;
    i2d_ASN1_INTEGER()
    i2d_BASIC_CONSTRAINTS(bs,)
    // NID_basic_constraints
    X509_add1_ext_i2d(x509Certificate, NID_basic_constraints, , 1, )
#endif
    // TODO: Sign Certificate
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        throw std::bad_alloc();
    }
    int def_nid = 0;
    const EVP_MD *md = EVP_sha256();
    EVP_PKEY_CTX *pkctx = NULL;
    if (!EVP_DigestSignInit(mdctx, &pkctx, md, NULL, keyPair)) {
        throw OpenSSLException(ERR_get_error());
    }
    if (!X509_sign_ctx(x509Certificate, mdctx)) {
        throw OpenSSLException(ERR_get_error());
    }
    EVP_MD_CTX_free(mdctx);
}