/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 09/08/2020
 */

#include <iomanip>
#include "WebExtension.h"
#include "sstream"

using namespace std;

WebExtension::WebExtension() : inDataLg{0} {

}

void WebExtension::runFunction(std::ostream out) {
    string function = inData["request"];
    nlohmann::json outData;
    if (function == "generatePKCS10AsPEM") {

    }
    else if (function == "acceptCertificateAsPEM") {

    }
    else if (function == "importPFXAsBase64") {

    }
    else if (function == "exportPFXAsBase64") {

    }
    else {
        outData["Result"] = "NOK";
        outData["Response"] = "Unknown request (must be generatePKCS10AsPEM, acceptCertificateAsPEM, importPFX, exportPFX)";
    }
    stringstream tmpOut;
    out << setw(4) << tmpOut.str().size();
    out << tmpOut.str();
}

WebExtension::WebExtension(std::istream in) {
    in.read((char *)&inDataLg, 4);
    in >> setw(4) >> inDataLg;
    in >> inData;
}