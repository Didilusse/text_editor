//
// Created by Adil Rahmani on 1/23/26.
//

#ifndef GAPBUFFER_H
#define GAPBUFFER_H
#include <vector>


class GapBuffer {
private:
    std::vector<char> buffer;
    std::size_t gapStart;
    std::size_t gapEnd;
    void expand();
public:
    GapBuffer();
    std::size_t getGapStart() const;
    std::size_t getGapEnd() const;
    void setGapStart(std::size_t gapStart);
    void setGapEnd(std::size_t gapEnd);
    void insert(char c);
    void backspace();
    char getChar(size_t i) const;
    std::string getString() const;
    void moveLeft();
    void moveRight();
};



#endif //GAPBUFFER_H
