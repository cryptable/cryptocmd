/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 03/08/2020
 */
#include "CNGHash.h"
#include <ntstatus.h>
#include <KSException.hpp>

CNGHash::CNGHash() : hashAlgo{0}, hash{0} {
    DWORD status=0;

    status = BCryptOpenAlgorithmProvider(&hashAlgo,
                                         BCRYPT_SHA256_ALGORITHM,
                                         nullptr,
                                         0);
    if (status != STATUS_SUCCESS) {
        throw KSException(status);
    }

    DWORD outputLg = 0;
    DWORD hashObjectLg = 0;
    status = BCryptGetProperty(hashAlgo,
                               BCRYPT_OBJECT_LENGTH,
                               (PBYTE)&hashObjectLg,
                               sizeof(DWORD),
                               &outputLg,
                               0);
    if (status != STATUS_SUCCESS) {
        throw KSException(status);
    }
    hashObject = std::make_unique<BYTE[]>(hashObjectLg);

    status = BCryptCreateHash(hashAlgo,
                              &hash,
                              hashObject.get(),
                              hashObjectLg,
                              nullptr,
                              0,
                              0);
    if (status != STATUS_SUCCESS) {
        throw KSException(status);
    }

    DWORD hashLg = 0;
    status = BCryptGetProperty(hashAlgo,
                               BCRYPT_HASH_LENGTH,
                               (PBYTE)&hashLg,
                               sizeof(DWORD),
                               &outputLg,
                               0);
    if (status != STATUS_SUCCESS) {
        throw KSException(status);
    }

    hashValue.resize(hashLg);
}

void CNGHash::update(const char *data, const size_t dataLg) {
    DWORD status=0;
    status = BCryptHashData(hash,
                            (PBYTE)data,
                            dataLg,
                            0);
    if (status != STATUS_SUCCESS) {
        throw KSException(status);
    }
}

const std::vector<unsigned char> &CNGHash::finalize() {
    DWORD status=0;
    status = BCryptFinishHash(hash,
                              hashValue.data(),
                              hashValue.size(),
                              0);
    if (status != STATUS_SUCCESS) {
        throw KSException(status);
    }
    return hashValue;
}

CNGHash::~CNGHash() {
    BCryptCloseAlgorithmProvider(hashAlgo,0);
    BCryptDestroyHash(hash);
}