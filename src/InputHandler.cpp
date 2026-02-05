//
// Created by Adil Rahmani on 2/5/26.
//

#include "InputHandler.h"
#include <cmath>
#include <limits>

void handleMouseClick(sf::Vector2i mousePos, GapBuffer& buffer, const sf::Text& text,
                      const Button& saveBtn, const Button& loadBtn,
                      const sf::RenderWindow& window, const sf::View& textView) {

    sf::Vector2f uiPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Button checks can also be done here, or strictly in main.
    // This function preserves the logic from the original main.cpp
    if (saveBtn.shape.getGlobalBounds().contains(uiPos)) {
        // saveToFile call is handled in main loop in the refactored version usually,
        // but this function identifies the click target.
        return;
    }
    if (loadBtn.shape.getGlobalBounds().contains(uiPos)) {
        return;
    }

    // Check Text (using World Coordinates / Text View)
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, textView);

    int bestIndex = -1;
    float bestDistance = std::numeric_limits<float>::max();

    for (size_t i = 0; i <= text.getString().getSize(); i++) {
        sf::Vector2f charPos = text.findCharacterPos(i);
        float charHeight = text.getCharacterSize();

        if (worldPos.y >= charPos.y && worldPos.y < charPos.y + charHeight) {
            float dist = std::abs(worldPos.x - charPos.x);
            if (dist < bestDistance) {
                bestDistance = dist;
                bestIndex = static_cast<int>(i);
            }
        }
    }

    if (bestIndex != -1) {
        buffer.moveTo(bestIndex);
    }
}