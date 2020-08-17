/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 10/08/2020
 */

#ifndef KSMGMNT_BASE64UTILS_H
#define KSMGMNT_BASE64UTILS_H
#include <string>
#include <vector>
#include <rpc.h>
#include <KSException.h>

class Base64Utils {
public:
    static std::string toBase64(const std::vector<unsigned char> &data) {
        DWORD b64DataLg = 0;
        if (!CryptBinaryToString(reinterpret_cast<const BYTE *>(data.data()),
                                 data.size(),
                                 CRYPT_STRING_BASE64,
                                 NULL,
                                 &b64DataLg)) {
            throw KSException(__func__, __LINE__, GetLastError());
        }
        std::unique_ptr<char[]> b64Data(new char[b64DataLg]);
        CryptBinaryToString(reinterpret_cast<const BYTE *>(data.data()),
                            data.size(),
                            CRYPT_STRING_BASE64,
                            b64Data.get(),
                            &b64DataLg);
        return std::string(b64Data.get(), b64DataLg);
    }

    static std::vector<char> fromBase64(const std::string &b64Data) {
        DWORD dataLg=0;
        if (!CryptStringToBinary(b64Data.c_str(),
                                 b64Data.size(),
                                 CRYPT_STRING_BASE64,
                                 NULL,
                                 &dataLg,
                                 0,
                                 0)) {
            throw KSException(__func__, __LINE__, GetLastError());
        }
        std::unique_ptr<BYTE[]> data(new BYTE[dataLg]);
        CryptStringToBinary(b64Data.c_str(),
                            b64Data.size(),
                            CRYPT_STRING_BASE64,
                            data.get(),
                            &dataLg,
                            0,
                            0);
        return std::vector<char>(data.get(), data.get()+dataLg); // That's ugly :-(
    }
};

#endif //KSMGMNT_BASE64UTILS_H
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2020 Cryptable BV                                                */
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/
