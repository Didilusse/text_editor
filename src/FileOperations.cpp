//
// Created by Adil Rahmani on 2/5/26.
//

#include "FileOperations.h"
#include <fstream>
#include "nfd.h"
#include <string>

std::string saveToFile(const GapBuffer& buffer, const std::string& suggestedName) {
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_SaveDialog(
        nullptr,
        suggestedName.empty() ? nullptr : suggestedName.c_str(),
        &outPath
    );

    if (result == NFD_OKAY && outPath) {
        std::string path(outPath);

        if (path.size() < 4 || path.substr(path.size() - 4) != ".txt") {
            path += ".txt";
        }

        std::ofstream outputFile(path);
        if (outputFile.is_open()) {
            outputFile << buffer.getString();
            outputFile.close();
        }

        free(outPath);
        return path;
    }

    if (outPath) free(outPath);
    return "";
}

std::string loadFromFile(GapBuffer& buffer) {
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

    if (result == NFD_OKAY && outPath) {
        std::ifstream inputFile(outPath);
        if (inputFile.is_open()) {
            buffer.clear();
            char ch;
            while (inputFile.get(ch)) {
                buffer.insert(ch);
            }
            inputFile.close();
        }

        std::string path(outPath);
        free(outPath);
        return path;
    }

    if (outPath) free(outPath);
    return "";
}