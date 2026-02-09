//
// File Operations Implementation
//

#include "FileOperations.h"
#include <fstream>
#include "nfd.h"
#include <string>

std::string saveToFile(const GapBuffer& buffer, const std::string& existingFileName) {
    std::string pathToSave;

    // If we have an existing filename, save directly to it (no dialog)
    if (!existingFileName.empty() && existingFileName != "Untitled") {
        pathToSave = existingFileName;
    }
    // Otherwise, show save dialog
    else {
        nfdchar_t *outPath = nullptr;
        nfdresult_t result = NFD_SaveDialog(nullptr, nullptr, &outPath);

        if (result == NFD_OKAY) {
            pathToSave = std::string(outPath);
            free(outPath);
        } else {
            if (outPath) free(outPath);
            return ""; // User cancelled
        }
    }

    // Add .txt extension if not present
    if (pathToSave.size() < 4 || pathToSave.substr(pathToSave.size() - 4) != ".txt") {
        pathToSave += ".txt";
    }

    // Save the file
    std::ofstream outputFile(pathToSave);
    if (outputFile.is_open()) {
        outputFile << buffer.getString();
        outputFile.close();
        return pathToSave; // Return the saved filename
    }

    return ""; // Save failed
}

std::string loadFromFile(GapBuffer& buffer) {
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

    if (result == NFD_OKAY) {
        std::string pathLoaded(outPath);
        std::ifstream inputFile(pathLoaded);

        if (inputFile.is_open()) {
            buffer.clear();
            char ch;
            while (inputFile.get(ch)) {
                buffer.insert(ch);
            }
            inputFile.close();
            free(outPath);
            return pathLoaded;
        }
        free(outPath);
    } else {
        if (outPath) free(outPath);
    }

    return "";
}