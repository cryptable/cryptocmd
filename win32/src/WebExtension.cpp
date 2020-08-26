/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 09/08/2020
 */

#include <iomanip>
#include <locale>
#include <codecvt>
#include "WebExtension.h"
#include "sstream"
#include "CertificateStore.h"
#include "Base64Utils.h"
#include "KSException.h"

using namespace std;

WebExtension::WebExtension() : inDataLg{0}, passwordProtect{true} {
}

void WebExtension::runFunction(std::ostream &out) {
    nlohmann::json outData;
    try {
        if ((!inData.contains("request_id")) ||
            (!inData.contains("request"))){
            throw KSException(__func__, __LINE__, "Missing Parameters");
        }
        string function = inData["request"];
        outData["request_id"] = inData["request_id"];
        CertificateStore certificateStore;
        if (function == "create_csr") {
            auto data = certificateStore.createCertificateRequest(
                    inData["subject_name"],
                    inData["rsa_key_length"],
                    passwordProtect);
            std::vector<unsigned char> vecData(data.begin(), data.end());
            outData["response"] = Base64Utils::toBase64(vecData);
            outData["result"] = "OK";
        }
        else if (function == "import_certificate") {
            if (!inData.contains("certificate")) {
                throw KSException(__func__, __LINE__, "Missing Parameters");
            }
            auto cert = Base64Utils::fromBase64(inData["certificate"]);
            certificateStore.importCertificate(std::string(cert.data(), cert.size()));
            outData["result"] = "OK";
            outData["response"] = "import certificate successful";
        }
        else if (function == "import_pfx_key") {
            if ((!inData.contains("pkcs12")) ||
                (!inData.contains("password"))){
                throw KSException(__func__, __LINE__, "Missing Parameters");
            }
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            certificateStore.pfxImport( inData["pkcs12"],
                                        converter.from_bytes(inData["password"]),
                                        passwordProtect);
            outData["result"] = "OK";
            outData["response"] = "import pfx successful";
        }
        else if (function == "export_pfx_key") {
            if ((!inData.contains("issuer")) ||
                (!inData.contains("serial_number")) ||
                (!inData.contains("password"))) {
                throw KSException(__func__, __LINE__, "Missing Parameters");
            }
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            outData["response"] = certificateStore.pfxExport(inData["issuer"],
                                                             inData["serial_number"],
                                                             converter.from_bytes(inData["password"]));
            outData["result"] = "OK";
        }
        else {
            outData["result"] = "NOK";
            outData["response"] = "Bad Request";
            LogEvent::GetInstance().error(0, "Invalid function called");
        }
    }
    catch (KSException e) {
        outData["result"] = "NOK";
        outData["response"] = "Bad Request";
        LogEvent::GetInstance().error(0, e.what());
    }
    catch (exception e) {
        outData["result"] = "NOK";
        outData["response"] = "Bad Request";
        LogEvent::GetInstance().error(0, e.what());
    }
    std::string tmpOut = outData.dump();
    uint32_t tmpOutLg = tmpOut.size();
    out.write((char *)&tmpOutLg, 4);
    out << tmpOut;
}

void WebExtension::setPasswordProtect(bool onOff) {
    passwordProtect = onOff;
};

WebExtension::WebExtension(std::istream &in) : inDataLg{0}, passwordProtect{true} {
    in.read((char *)&inDataLg, 4);
    if (inDataLg == 0) {
        throw KSException(__func__, __LINE__, "No data (length = 0)");
    }
    in >> inData;
}

void WebExtension::process_request(std::istream &in, std::ostream &out) {

    try {
        WebExtension webExtension(in);

        webExtension.runFunction(out);
    }
    catch (std::exception &e) {
        nlohmann::json outData;
        outData["result"] = "NOK";
        outData["response"] = "Bad Request";
        LogEvent::GetInstance().error(0, e.what());
        std::string tmpOut = outData.dump();
        uint32_t tmpOutLg = tmpOut.size();
        out.write((char *)&tmpOutLg, 4);
        out << tmpOut;
    }
}