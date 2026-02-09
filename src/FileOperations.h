//
// Created by Adil Rahmani on 2/5/26.
//

#pragma once
#include "GapBuffer.h"

#include <string>

std::string saveToFile(const GapBuffer& buffer, const std::string& suggestedName);
std::string loadFromFile(GapBuffer& buffer);