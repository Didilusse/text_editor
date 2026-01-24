//
// Created by Adil Rahmani on 1/23/26.
//

#include "GapBuffer.h"
#include "vector"

GapBuffer::GapBuffer() {
    buffer.resize(10);
    setGapStart(0);
    setGapEnd(10);
}
void GapBuffer::insert(char c) {
    if (getGapStart() == getGapEnd()) {
        expand();
    }
    buffer[getGapStart()] = c;
    setGapStart(getGapStart() + 1);
}
void GapBuffer::expand() {
    int oldSize = buffer.size();
    //doubling the buffer size
    buffer.resize(buffer.size() * 2);
    //run the loop backwards to move the characters
    for (int i = oldSize - 1; i >= static_cast<int>(gapStart); i--) {
        buffer[i + oldSize] = buffer[i];
    }
    //gapEnd point it to the new location
    setGapEnd(getGapEnd() + oldSize);
}
void GapBuffer::backspace() {
    if (getGapStart() == 0) {
        //TODO: Start of the file
        return;
    }
    setGapStart(getGapStart() - 1);
}

char GapBuffer::getChar(size_t i) const {
    if (i < gapStart) {
        return buffer[i];
    } else {
        return buffer[i + (gapEnd - gapStart)];
    }
}

void GapBuffer::setGapStart(size_t i) {
    gapStart = i;
}
void GapBuffer::setGapEnd(size_t i) {
    gapEnd = i;
}
size_t GapBuffer::getGapStart() const {
    return gapStart;
}
size_t GapBuffer::getGapEnd() const {
    return gapEnd;
}
std::string GapBuffer::getString() const {
    std::string word;
    for (int i = 0; i < buffer.size() - (getGapEnd() - getGapStart()); i++) {
        word += getChar(i);
    }
    return word;
}