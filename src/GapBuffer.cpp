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

void GapBuffer::moveLeft() {
    if (getGapStart() == 0) {
        return;
    }
    buffer[getGapEnd() - 1] = buffer[getGapStart() - 1];
    setGapStart(getGapStart() - 1);
    setGapEnd(getGapEnd() - 1);
}

void GapBuffer::moveRight() {
    if (getGapEnd() == buffer.size()) {
        return;
    }
    buffer[getGapStart()] = buffer[getGapEnd()];
    setGapStart(getGapStart() + 1);
    setGapEnd(getGapEnd() + 1);
}

void GapBuffer::moveTo(size_t i) {
    if (getGapStart() == i) {
        return;
    }
    if (getGapStart() > i) {
        //go left
        while (getGapStart() > i) {
            moveLeft();
        }
    }
    if (getGapStart() < i) {
        //go right
        while (getGapStart() < i) {
            moveRight();
        }
    }


}

void GapBuffer::clear() {
    buffer.resize(10);
    setGapStart(0);
    setGapEnd(10);
}

void GapBuffer::deleteRange(size_t start, size_t end) {
    if (start >= end) return;

    // Move gap to start position
    moveTo(start);

    // Calculate how many characters to delete
    size_t deleteCount = end - start;

    // Expand the gap by moving gapEnd forward
    size_t newGapEnd = getGapEnd() + deleteCount;
    if (newGapEnd > buffer.size()) {
        newGapEnd = buffer.size();
    }
    setGapEnd(newGapEnd);
}

std::string GapBuffer::getRange(size_t start, size_t end) const {
    if (start >= end) return "";

    std::string result;
    size_t textSize = buffer.size() - (getGapEnd() - getGapStart());

    for (size_t i = start; i < end && i < textSize; i++) {
        result += getChar(i);
    }

    return result;
}

void GapBuffer::insertString(const std::string& str) {
    for (char c : str) {
        insert(c);
    }
}