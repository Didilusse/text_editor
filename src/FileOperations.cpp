//
// Created by Adil Rahmani on 2/5/26.
//

#include "FileOperations.h"
#include <fstream>
#include "nfd.h"
#include <string>

void saveToFile(const GapBuffer& buffer) {
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_SaveDialog(nullptr, nullptr, &outPath);

    if (result == NFD_OKAY) {
        std::string path(outPath);
        if (path.size() < 4 || path.substr(path.size() - 4) != ".txt") {
            path += ".txt";
        }
        std::ofstream outputFile(path);
        if (outputFile.is_open()) {
            outputFile << buffer.getString() << std::endl;
            outputFile.close();
        }
        free(outPath);
    } else {
        if (outPath) free(outPath);
    }
}

void loadFromFile(GapBuffer& buffer) {
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

    if (result == NFD_OKAY) {
        std::ifstream inputFile(outPath);
        if (inputFile.is_open()) {
            buffer.clear();
            char ch;
            while (inputFile.get(ch)) {
                buffer.insert(ch);
            }
            inputFile.close();
        }
        free(outPath);
    } else {
        if (outPath) free(outPath);
    }
}