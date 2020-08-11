/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 11/08/2020
 */
#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>
#include <WebExtension.h>
#include <CertificateStore.h>
#include <OpenSSLCertificateRequest.h>
#include <OpenSSLCA.h>
#include <Base64Utils.h>
#include <sstream>
#include <iomanip>
#include <OpenSSLPKCS12.h>
#include "utils/CertStoreUtil.h"

TEST_CASE( "WebExtensionTests", "[success]" ) {

    SECTION( "Create CSR request" ) {
        // Arrange
        CertStoreUtil certStoreUtil;
        if (certStoreUtil.hasCertificates(L"John Doe")) {
            certStoreUtil.deleteCertificates(L"John Doe");
        }
        nlohmann::json create_csr = R"(
            {
                "request":"create_csr",
                "key_id":"XH45E45MLk0",
                "subject_name":"cn=John Doe,o=Company,c=US",
                "rsa_key_length":2048
            }
        )"_json;
        auto input = create_csr.dump();
        std::stringstream in;
        uint32_t length = input.size();
        in.write((char *)&length, 4);
        in << input;

        // Act
        WebExtension webExtension(in);
        std::stringstream out;
        webExtension.runFunction(out);

        // Assert
        uint32_t outLg;
        out.read((char *)&outLg, 4);
        nlohmann::json result;
        out >> result;

        REQUIRE(result["result"] == "OK");
        REQUIRE(result["key_id"] == create_csr["key_id"]);
        auto pemCert = Base64Utils::fromBase64(result["response"]);
        OpenSSLCertificateRequest openSslCertificateRequest(std::string(pemCert.data(), pemCert.size()));
        REQUIRE(openSslCertificateRequest.verify());

        // Cleanup
        certStoreUtil.reopen();
        certStoreUtil.deleteCertificates(L"John Doe");
        certStoreUtil.close();
    }

    SECTION( "Import the certificate" ) {
        // Arrange
        CertStoreUtil certStoreUtil;
        if (certStoreUtil.hasCertificates(L"John Doe")) {
            certStoreUtil.deleteCertificates(L"John Doe");
        }
        certStoreUtil.close();
        OpenSSLCertificateRequest openSslCertificateRequest(std::string("/CN=John Doe/O=Company/C=US"), 2048);
        OpenSSLCA openSslca("/CN=rootCA", 2048);
        auto cert = openSslca.certify(openSslCertificateRequest)->getPEM();
        auto b64Cert = Base64Utils::toBase64(std::vector<unsigned char>(cert.begin(), cert.end()));
        nlohmann::json import_cert;
        import_cert["request"]="import_certificate";
        import_cert["key_id"]="GHTEO93df6";
        import_cert["certificate"]=b64Cert;
        auto input = import_cert.dump();
        std::stringstream in;
        uint32_t length = input.size();
        in.write((char *)&length, 4);
        in << input;

        // Act
        WebExtension webExtension(in);
        std::stringstream out;
        webExtension.runFunction(out);

        // Assert
        uint32_t outLg;
        out.read((char *)&outLg, 4);
        nlohmann::json result;
        out >> result;

        REQUIRE(result["result"] == "OK");
        REQUIRE(result["key_id"] == import_cert["key_id"]);
        REQUIRE(result["response"] == "import certificate successful");

        // Cleanup
        certStoreUtil.reopen();
        certStoreUtil.deleteCertificates(L"John Doe");
        certStoreUtil.close();
    }

    SECTION( "Import pfx file" ) {
        // Arrange
        CertStoreUtil certStoreUtil;
        if (certStoreUtil.hasCertificates(L"John Doe")) {
            certStoreUtil.deleteCertificates(L"John Doe");
        }
        certStoreUtil.close();
        OpenSSLCertificateRequest openSslCertificateRequest(std::string("/CN=John Doe/O=Company/C=US"), 2048);
        OpenSSLCA openSslca("/CN=RootCA/O=Company/C=US", 4096);
        auto cert = openSslca.certify(openSslCertificateRequest);
        OpenSSLPKCS12 openSslpkcs12(*(cert.get()),
                                    openSslca.getCertificate(),
                                    openSslCertificateRequest.getKeyPair(),
                                    std::string("system"));
        auto pkcs12 = openSslpkcs12.getPKCS12();
        auto b64pkcs12 = Base64Utils::toBase64(pkcs12);
        nlohmann::json import_pfx_key;
        import_pfx_key["request"]="import_pfx_key";
        import_pfx_key["key_id"]="OPhJT83fgT";
        import_pfx_key["pkcs12"]=b64pkcs12;
        import_pfx_key["password"]="system";
        auto input = import_pfx_key.dump();
        std::stringstream in;
        uint32_t length = input.size();
        in.write((char *)&length, 4);
        in << input;

        // Act
        WebExtension webExtension(in);
        std::stringstream out;
        webExtension.runFunction(out);

        // Assert
        uint32_t outLg;
        out.read((char *)&outLg, 4);
        nlohmann::json result;
        out >> result;

        REQUIRE(result["result"] == "OK");
        REQUIRE(result["key_id"] == import_pfx_key["key_id"]);
        REQUIRE(result["response"] == "import pfx successful");

        // Cleanup
        certStoreUtil.reopen();
        certStoreUtil.deleteCertificates(L"John Doe");
        certStoreUtil.close();
    }

    SECTION( "Export signing pfx file" ) {
        // Arrange
        CertStoreUtil certStoreUtil;
        if (certStoreUtil.hasCertificates(L"John Doe")) {
            certStoreUtil.deleteCertificates(L"John Doe");
        }
        certStoreUtil.close();
        CertificateStore certificateStore;
        auto csr = certificateStore.createCertificateRequest(std::string("CN=John Doe,O=Company,C=US"), 2048);
        OpenSSLCertificateRequest openSslCertificateRequest(csr);
        OpenSSLCA openSslca("/CN=RootCA/O=Company/C=US", 4096);
        auto cert = openSslca.certify(openSslCertificateRequest);
        certificateStore.importCertificate(cert->getPEM());
        nlohmann::json export_pfx_key;
        export_pfx_key["request"]="export_pfx_key";
        export_pfx_key["key_id"]="OPhKl83fgT";
        export_pfx_key["issuer"]="cn=RootCA,o=Company,c=US";
        export_pfx_key["serial_number"]="03";
        export_pfx_key["password"]="system";
        auto input = export_pfx_key.dump();
        std::stringstream in;
        uint32_t length = input.size();
        in.write((char *)&length, 4);
        in << input;

        // Act
        WebExtension webExtension(in);
        std::stringstream out;
        webExtension.runFunction(out);

        // Assert
        uint32_t outLg;
        out.read((char *)&outLg, 4);
        nlohmann::json result;
        out >> result;

        REQUIRE(result["result"] == "OK");
        REQUIRE(result["key_id"] == export_pfx_key["key_id"]);
        auto pkcs12 = Base64Utils::fromBase64(result["response"]);
        OpenSSLPKCS12 openSslpkcs12(pkcs12.data(), pkcs12.size(), "system");
        REQUIRE(openSslpkcs12.getCertificate().getCommonName() == "John Doe");
        REQUIRE(openSslpkcs12.getCAs().size() == 0);
        REQUIRE(openSslpkcs12.getPrivateKey().getKeyBitlength() == 2048);

        // Cleanup
        certStoreUtil.reopen();
        certStoreUtil.deleteCertificates(L"John Doe");
        certStoreUtil.close();
    }

    SECTION( "Export encryption pfx file" ) {
        // Arrange
        CertStoreUtil certStoreUtil;
        if (certStoreUtil.hasCertificates(L"John Doe")) {
            certStoreUtil.deleteCertificates(L"John Doe");
        }
        certStoreUtil.close();
        OpenSSLCertificateRequest openSslCertificateRequest(std::string("/CN=John Doe/O=Company/C=US"), 2048);
        OpenSSLCA openSslca("/CN=RootCA/O=Company/C=US", 4096);
        auto cert = openSslca.certify(openSslCertificateRequest);
        OpenSSLPKCS12 openSslpkcs12(*(cert.get()),
                                    openSslca.getCertificate(),
                                    openSslCertificateRequest.getKeyPair(),
                                    std::string("system"));
        auto pkcs12 = openSslpkcs12.getPKCS12();
        auto b64pkcs12 = Base64Utils::toBase64(pkcs12);
        CertificateStore certificateStore;
        certificateStore.pfxImport(b64pkcs12, L"system");
        nlohmann::json export_pfx_key;
        export_pfx_key["request"]="export_pfx_key";
        export_pfx_key["key_id"]="OPhKl83fgT";
        export_pfx_key["issuer"]="cn=RootCA,o=Company,c=US";
        export_pfx_key["serial_number"]="03";
        export_pfx_key["password"]="system";
        auto input = export_pfx_key.dump();
        std::stringstream in;
        uint32_t length = input.size();
        in.write((char *)&length, 4);
        in << input;

        // Act
        WebExtension webExtension(in);
        std::stringstream out;
        webExtension.runFunction(out);

        // Assert
        uint32_t outLg;
        out.read((char *)&outLg, 4);
        nlohmann::json result;
        out >> result;

        REQUIRE(result["result"] == "OK");
        REQUIRE(result["key_id"] == export_pfx_key["key_id"]);
        auto pkcs12out = Base64Utils::fromBase64(result["response"]);
        OpenSSLPKCS12 openSslpkcs12out(pkcs12out.data(), pkcs12out.size(), "system");
        REQUIRE(openSslpkcs12out.getCertificate().getCommonName() == "John Doe");
        REQUIRE(openSslpkcs12out.getCAs().size() == 0);
        REQUIRE(openSslpkcs12out.getPrivateKey().getKeyBitlength() == 2048);

        // Cleanup
        certStoreUtil.reopen();
        certStoreUtil.deleteCertificates(L"John Doe");
        certStoreUtil.close();
    }
}
