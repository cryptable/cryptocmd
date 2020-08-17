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

using namespace std;

WebExtension::WebExtension() : inDataLg{0} {
}

void WebExtension::runFunction(std::ostream &out) {
    string function = inData["request"];
    nlohmann::json outData;
    outData["key_id"] = inData["key_id"];
    try {
        CertificateStore certificateStore;
        if (function == "create_csr") {
            auto data = certificateStore.createCertificateRequest(
                    inData["subject_name"],
                    inData["rsa_key_length"]);
            std::vector<unsigned char> vecData(data.begin(), data.end());
            outData["response"] = Base64Utils::toBase64(vecData);
            outData["result"] = "OK";
        }
        else if (function == "import_certificate") {
            auto cert = Base64Utils::fromBase64(inData["certificate"]);
            certificateStore.importCertificate(std::string(cert.data(), cert.size()));
            outData["result"] = "OK";
            outData["response"] = "import certificate successful";
        }
        else if (function == "import_pfx_key") {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            certificateStore.pfxImport( inData["pkcs12"], converter.from_bytes(inData["password"]));
            outData["result"] = "OK";
            outData["response"] = "import pfx successful";
        }
        else if (function == "export_pfx_key") {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            outData["response"] = certificateStore.pfxExport(inData["issuer"],
                                                             inData["serial_number"],
                                                             converter.from_bytes(inData["password"]));
            outData["result"] = "OK";
        }
        else {
            outData["result"] = "NOK";
            outData["response"] = "Unknown request (must be create_csr, import_certificate, import_pfx_key, export_pfx_key)";
        }
    }
    catch (KSException e) {
        std::string error(e.what());
        outData["result"] = "NOK";
        outData["response"] = error;
        LogEvent::GetInstance().error(0, error);
    }
    catch (exception e) {
        std::string error(e.what());
        outData["result"] = "NOK";
        outData["response"] = error;
        LogEvent::GetInstance().error(0, error);
    }
    std::string tmpOut = outData.dump();
    uint32_t tmpOutLg = tmpOut.size();
    out.write((char *)&tmpOutLg, 4);
    out << tmpOut;
}

WebExtension::WebExtension(std::istream &in) : inDataLg{0} {
    in.read((char *)&inDataLg, 4);
    in >> inData;
}