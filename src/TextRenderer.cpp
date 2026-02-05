#include "TextRenderer.h"
#include <cmath>
#include <limits>
#include <algorithm>

DisplayState wrapText(const GapBuffer& buffer, sf::Text& textObj, float maxWidth) {
    std::string raw = buffer.getString();
    std::string displayString;
    std::string currentLine;
    std::string wordBuffer;

    size_t rawCursorIndex = buffer.getGapStart();
    size_t displayCursorIndex = 0;
    bool cursorFound = false;

    for (size_t i = 0; i < raw.size(); i++) {
        char c = raw[i];

        if (i == rawCursorIndex) {
            displayCursorIndex = displayString.size() + currentLine.size() + wordBuffer.size();
            cursorFound = true;
        }

        if (c == '\n') {
            currentLine += wordBuffer;
            displayString += currentLine + "\n";
            currentLine.clear();
            wordBuffer.clear();
            continue;
        }

        wordBuffer += c;

        if (c == ' ' || i == raw.size() - 1) {
            sf::Text temp = textObj;
            temp.setString(currentLine + wordBuffer);

            if (temp.getLocalBounds().size.x > maxWidth) {
                displayString += currentLine + "\n";
                currentLine.clear();
            }

            currentLine += wordBuffer;
            wordBuffer.clear();
        }
    }

    if (!cursorFound) {
        displayCursorIndex = displayString.size() + currentLine.size() + wordBuffer.size();
    }

    displayString += currentLine + wordBuffer;
    return {displayString, displayCursorIndex};
}

void moveCursorVertical(GapBuffer& buffer, const sf::Text& text, const sf::Font& font, bool down) {
    int currentY = text.findCharacterPos(buffer.getGapStart()).y;
    int currentX = text.findCharacterPos(buffer.getGapStart()).x;
    int spacing = font.getLineSpacing(text.getCharacterSize());
    int targetY = down ? currentY + spacing : currentY - spacing;

    int bestIndex = -1;
    float bestDistance = std::numeric_limits<float>::max();

    for (size_t i = 0; i < text.getString().getSize(); i++) {
        sf::Vector2f charPos = text.findCharacterPos(i);
        if (static_cast<int>(charPos.y) == targetY) {
            float dist = std::abs(charPos.x - static_cast<float>(currentX));
            if (dist < bestDistance) {
                bestDistance = dist;
                bestIndex = i;
            }
        }
    }

    if (bestIndex != -1) {
        buffer.moveTo(bestIndex);
    }
}

void drawSelection(sf::RenderWindow& window, const sf::Text& text, const sf::Font& font, 
                  int selectionAnchor, int gapStart) {
    if (selectionAnchor != -1 && selectionAnchor != gapStart) {
        size_t start = std::min((size_t)selectionAnchor, (size_t)gapStart);
        size_t end   = std::max((size_t)selectionAnchor, (size_t)gapStart);

        for (size_t i = start; i < end; ++i) {
            sf::Vector2f charPos = text.findCharacterPos(i);
            sf::Vector2f nextCharPos = text.findCharacterPos(i + 1);

            float width = nextCharPos.x - charPos.x;
            float height = font.getLineSpacing(text.getCharacterSize());

            if (nextCharPos.y != charPos.y || width <= 0) {
                width = 10.f; // Fallback width for newlines
            }

            sf::RectangleShape selectionRect({width, height});
            selectionRect.setFillColor(sf::Color(100, 100, 255, 128));
            selectionRect.setPosition(charPos);
            window.draw(selectionRect);
        }
    }
}