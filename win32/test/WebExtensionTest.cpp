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
        webExtension.setPasswordProtect(false);
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
        webExtension.setPasswordProtect(false);
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
        webExtension.setPasswordProtect(false);
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
        webExtension.setPasswordProtect(false);
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
        webExtension.setPasswordProtect(false);
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

/*
 * User Interface Testing
 */
TEST_CASE( "WebExtensionTests with UI", "[ui]" ) {

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
        {
            CertificateStore certificateStore;
            auto csr = certificateStore.createCertificateRequest(std::string("CN=John Doe,O=Company,C=US"), 2048, true);
            OpenSSLCertificateRequest openSslCertificateRequest(csr);
            OpenSSLCA openSslca("/CN=RootCA/O=Company/C=US", 4096);
            auto cert = openSslca.certify(openSslCertificateRequest);
            certificateStore.importCertificate(cert->getPEM());
        }
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
        {
            CertificateStore certificateStore;
            certificateStore.pfxImport(b64pkcs12, L"system", true);
        }
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

#if 0
TEST_CASE( "WebExtensionTests from the field 1", "[firefox]" ) {
    SECTION( "Import pfx file 1" ) {
        // TODO intergrate to real test: add the user to it (export it)

        // Arrange
        CertStoreUtil certStoreUtil;
        if (certStoreUtil.hasCertificates(L"John Doe Encryption")) {
            certStoreUtil.deleteCertificates(L"John Doe Encryption");
        }
        std::string input("{ \"request\": \"import_pfx_key\", \"key_id\": \"HdIsVJqpbU\", \"pkcs12\": \"LS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUVmRENDQW1TZ0F3SUJBZ0lVVnBBR3BJdUlMRkhOK2JKSDFoZi9tKzF5cTRZd0RRWUpLb1pJaHZjTkFRRUwKQlFBd2JERUxNQWtHQTFVRUJoTUNabWt4RXpBUkJnTlZCQW9NQ2xaaGRXeDBhWFFnUVVJeEVUQVBCZ05WQkFzTQpDRk5sWTNWeWFYUjVNUXd3Q2dZRFZRUUxEQU5RUzBreEVEQU9CZ05WQkFzTUIxUmxjM1JwYm1jeEZUQVRCZ05WCkJBTU1ERkp2YjNRZ1ZHVnpkQ0JEUVRBZUZ3MHlNREE0TVRZeU1qQTVNekJhRncweU1UQTRNVGN5TWpBNU16QmEKTUhBeEhEQWFCZ05WQkFNTUUwcHZhRzRnUkc5bElFVnVZM0o1Y0hScGIyNHhFREFPQmdOVkJBb01CME52YlhCaApibmt4RURBT0JnTlZCQXNNQjFSbGMzUnBibWN4RVRBUEJnTlZCQXNNQ0ZObFkzVnlhWFI1TVF3d0NnWURWUVFMCkRBTlFTMmt4Q3pBSkJnTlZCQVlUQWxWVE1JSUJJakFOQmdrcWhraUc5dzBCQVFFRkFBT0NBUThBTUlJQkNnS0MKQVFFQXVkVy9wY2lYRjl5ZWZTVjNjYlRzaXR1ZWRlT2VlKzZPYzdIbE5FQmh2bmJ3SnFYYy9odE5ENklMVWtabgprNzk5cDNIUE9rd1UzTVJnZEllRXZGVGRmdzd0aU04SzNPdmZUaTNuM1E2SE5ReUxSY0RuRGZlakNFMU1kenVVCm1CSzFSKzdWNVBJeVRmUlR1TWhpYm5wYmVsWTJOUWx5aGJDVVl1ZkxiL3VQVmZ4OWxoNnJyOXJvSWMrQkE3eTgKUlhsclk5bStRUCt5YnVZdWdMSzRyTW1XamJZbHpQeE5RNkxFTnl3OC9sVkZYcE5aYzN0RHgrRGIxQ1NNa3daRQpGMSszd09CN3hZSnlZWFRPNFhGR0dYY25jYjNJTDZ1Qy9lM2d0MzhySmc0NmloRkRvakRhdWFUU2xEb2pwWk9iCml2dVhNcUgyOHdVNWprK2tpYzVBb2c5UWxRSURBUUFCb3hJd0VEQU9CZ05WSFE4QkFmOEVCQU1DQkRBd0RRWUoKS29aSWh2Y05BUUVMQlFBRGdnSUJBSEJuREprSktWYTNwUDQxMmk1Y3p4cHQ1dExWaWZZeWY4VzBrdnJJaUVLYQpYSWFPUUl4aUpHWHREd3dqQUlncndRbHFFaTZtTFBoUWtKeVpVQ29jZlJvWG1qSWcvN0RycHo0blU5eVRGV0lDCmE1MTNNUm5TTlBLZTRGVTdaZThEK3psUTF0L1FKR0VPMFg3SlpuTDFucFUvVGMwZ3VkRnd1QU9HUXdIbzhUSmIKT3BXQ0V4TXk2eW12eXRkNDJaVHZNNm8zVkdxK1M0dTVvRytWQ3hIcisvZDZlQ1AxUlF5SWhRcnJyY1NqSnIraQp0MkJqZDBkc2FIRXRrSjFjMnk4WUxIV2lXN1k5UUFaZzJkbVJpRGxMTkhZdkNjYXc1WTNxM2VBVDBHbEJQUjIxCjg0RDdwdnBvalpneG1PMkYzcWJUZVQzZm03aWZCOHJPcDJHQTEya2lKcWlYU05tMGNHY1pMM1RyM0VrQzB4N2YKN3ZTY1JCQTltSGFNcnFaR05lS2d2SmlyemdsSG9QV0RDbUowU3R1bDlIWmlnQitvYnkzRFhOK1dUS1pKSWxDQwppR2czNzczTWZYeU40K0t6SnI2ZElmbjZ4K29TWUdGQjRkcGF3YW5OYXVUU1pBQkdWditBR1R6dERGajYwUkJkCm5MbStJVnpvUWVMRE5tOFZzYW9YQ2ZscG0zb0UwTktFWlZWVjRvc2k4RHl5aUJwNVhnQTJreEZpUnNPbkljZnEKL2dkRU00dEJRMFNYTTRWYXNRY093YW90cm5GWGdkQkZKRTUxaDhNV09GQ3hPL29qU0YweTZSWWlXb29rREpkOApUUDAwRnlNb2g4ak1UTlR2RmNSbTVPMXdrTFh6NXhjeFdGbGY4emNQeUl0K2RiaURkT3k0bTBTRnJBd1JPckpyCi0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0K\", \"password\": \"system\" }");
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
        REQUIRE(result["key_id"] == "HdIsVJqpbU");
        REQUIRE(result["response"] == "import pfx successful");

        // Cleanup
        certStoreUtil.reopen();
        certStoreUtil.deleteCertificates(L"John Doe Encryption");
        certStoreUtil.close();
    }
}

TEST_CASE( "WebExtensionTests from the field 2", "[firefox]" ) {
    SECTION( "Export pfx file 1" ) {
        // TODO add the user to it (export it): odd lenght for HEX serial number

        std::string input("{ \"request\": \"export_pfx_key\", \"key_id\": \"tMCt83zrZJ\", \"issuer\": \"C=fi,O=Vaultit AB,OU=Security,OU=PKI,OU=Testing,CN=Root Test CA\", \"serial_number\": \"0xcf6aa362c478fee86c2fcdbc4eb0ad3e1934642\", \"password\": \"system\" }");
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
    }
}

#endif