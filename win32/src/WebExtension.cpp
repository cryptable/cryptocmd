/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 09/08/2020
 */

#include <iomanip>
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
                    std::stoi(std::string(inData["rsa_key_length"])));
            std::vector<unsigned char> vecData(data.begin(), data.end());
            outData["response"] = Base64Utils::toBase64(vecData);
            outData["result"] = "OK";
        }
        else if (function == "import_certificate") {
            auto cert = Base64Utils::fromBase64(inData["certificate"]);
            certificateStore.importCertificate(std::string(cert.data(), cert.size()));
            outData["result"] = "OK";
        }
        else if (function == "import_pfx_key") {
            auto cert = Base64Utils::fromBase64(inData["pkcs12"]);
            certificateStore.pfxImport(std::string(cert.data(), cert.size()), inData["password"]);
            outData["result"] = "OK";
        }
        else if (function == "export_pfx_key") {
            outData["response"] = certificateStore.pfxExport(inData["issuer"],
                                                             inData["serial_number"],
                                                             inData["password"]);
            outData["result"] = "OK";
        }
        else {
            outData["result"] = "NOK";
            outData["response"] = "Unknown request (must be create_csr, import_certificate, import_pfx_key, export_pfx_key)";
        }
    }
    catch (KSException e) {
        outData["result"] = "NOK";
        outData["response"] = e.what();
    }
    std::string tmpOut = outData.dump();
    out << setw(4) << tmpOut.size();
    out << tmpOut;
}

WebExtension::WebExtension(std::istream &in) {
    in.read((char *)&inDataLg, 4);
    in >> setw(4) >> inDataLg;
    in >> inData;
}