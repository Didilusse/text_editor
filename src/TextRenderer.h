#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "GapBuffer.h"

struct DisplayState {
    std::string content;
    size_t cursorIndex;
};

DisplayState wrapText(const GapBuffer& buffer, sf::Text& textObj, float maxWidth);
void moveCursorVertical(GapBuffer& buffer, const sf::Text& text, const sf::Font& font, bool down);
void drawSelection(sf::RenderWindow& window, const sf::Text& text, const sf::Font& font, 
                  int selectionAnchor, int gapStart);

size_t mapRawToDisplay(const std::string& raw, size_t rawPos, sf::Text& textObj, float maxWidth);
