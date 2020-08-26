/*
 * Copyright (c) 2020 Cryptable BV. All rights reserved.
 * (MIT License)
 * Author: "David Tillemans"
 * Date: 11/08/2020
 */
#include "common.h"
#include <shlwapi.h>
#include <iostream>
#include <sstream>
#include "WebExtension.h"
#include "KSException.h"
#include "LogEvent.h"

void write_config() {
    CHAR path[MAX_PATH];
    if (!GetModuleFileName(nullptr, path, MAX_PATH)) {
        throw KSException(__func__, __LINE__, GetLastError());
    }
    std::string exeFilename(path);
    if (!PathRemoveFileSpec(path)) {
        throw KSException(__func__, __LINE__, GetLastError());
    };
    std::string configFilename(path);
    configFilename += "\\org.cryptable.pki.keymgmnt.json";
    nlohmann::json config;
    std::vector<std::string> extArray;
    extArray.push_back("keymgmnt@pki.cryptable.org");
    config["name"] = "org.cryptable.pki.keymgmnt";
    config["description"] = "Native messaging interface for key management";
    config["path"] = exeFilename;
    config["type"] = "stdio";
    config["allowed_extensions"] = extArray;

    std::string jsonConfig(config.dump());

    DWORD dataWritten;
    HANDLE fileHandle =CreateFile(configFilename.c_str(),
                                  GENERIC_WRITE,
                                  0,
                                  nullptr,
                                  CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  nullptr);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        throw KSException(__func__, __LINE__, GetLastError());
    };
    if (!WriteFile(fileHandle, jsonConfig.c_str(), jsonConfig.size(), &dataWritten, NULL)) {
        CloseHandle(fileHandle);
        throw KSException(__func__, __LINE__, GetLastError());
    }
    CloseHandle(fileHandle);
}

int main(int argc, char* argv[]) {

    if (argc > 1) {
        if (strncmp(argv[1], "--config", strlen("--config")) == 0) {
            try {
                write_config();
                return 0;
            }
            catch (KSException &e) {
                LogEvent::GetInstance().error(e.code(), e.what());
                return e.code();
            }
            return 1;
        }
    }

    WebExtension webExtension(std::cin);

    webExtension.runFunction(std::cout);

    return 0;
}