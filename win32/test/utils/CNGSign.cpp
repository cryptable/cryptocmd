/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 03/08/2020
 */

#include "CNGSign.h"
#include <ntstatus.h>
#include <KSException.h>

CNGSign::CNGSign(NCRYPT_KEY_HANDLE k) : key{k} {
    DWORD status = 0;

    status = BCryptOpenAlgorithmProvider(&signAlgo,
                                         BCRYPT_RSA_SIGN_ALGORITHM,
                                         nullptr,
                                         0);
    if (status != STATUS_SUCCESS) {
        throw KSException(__func__, __LINE__, status);
    }
}

const std::vector<unsigned char> & CNGSign::sign(unsigned char *data, size_t dataLg) {
    DWORD status = 0;
    DWORD signatureLg = 0;

    if (dataLg > MAXDWORD) {
        throw std::overflow_error("DWORD overflow");
    }
    status = NCryptSignHash(key,
                            nullptr,
                            data,
                            (DWORD)dataLg,
                            nullptr,
                            0,
                            &signatureLg,
                            0);
    if (status != STATUS_SUCCESS) {
        throw KSException(__func__, __LINE__, status);
    }
    BCRYPT_PSS_PADDING_INFO bcryptPssPaddingInfo {
        BCRYPT_SHA1_ALGORITHM,
        20
    };
    signatureValue.resize(signatureLg);
    status = NCryptSignHash(key,
                            &bcryptPssPaddingInfo,
                            data,
                            (DWORD)dataLg,
                            signatureValue.data(),
                            signatureLg,
                            &signatureLg,
                            BCRYPT_PAD_PSS);
    if (status != STATUS_SUCCESS) {
        throw KSException(__func__, __LINE__, status);
    }

    return signatureValue;
}

CNGSign::~CNGSign() {
    BCryptCloseAlgorithmProvider(signAlgo, 0);
}