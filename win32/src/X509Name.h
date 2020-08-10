/*
 * MIT License
 * Author: David Tillemans
 */
#ifndef X509NAME_HPP
#define X509NAME_HPP
#include "common.h"
#include <string>

/**
 * @brief      This class encapsulates the general name functions for subject and issuer names
 */
class X509Name {
public:
    /**
     * @brief      Constructs a new instance of X509Name.
     *
     * @param      name  The name in string version in the format as for example 
     * "cn=John Doe, o=Company, c=US"
     * @param  Optional param to support UTF8 for RDNs
     */
    explicit X509Name(const std::string &name,  bool utf8 = true);

    /**
     * Destructor
     */
    ~X509Name();

    /**
     * Get the general as a X500 name
     * @return
     */
    std::string &getName();

    /**
     * Get the general as en ASN1 encoded blob
     * @return
     */
    CERT_NAME_BLOB &getEncodedBlob();

private:
    CERT_NAME_BLOB blobEncodedName;
    std::string    generalName;

};
#endif // X509NAME_HPP
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2020 Cryptable BV
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