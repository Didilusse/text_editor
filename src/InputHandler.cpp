//
// Input Handler
//

#include "InputHandler.h"
#include <cmath>
#include <limits>

void handleMouseClick(sf::Vector2i mousePos, GapBuffer& buffer, const sf::Text& text,
                      const sf::RenderWindow& window, const sf::View& textView) {

    // Map mouse pixel coords → world coords in the text view
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, textView);

    int bestIndex = -1;
    float bestDistance = std::numeric_limits<float>::max();

    for (size_t i = 0; i <= text.getString().getSize(); i++) {
        sf::Vector2f charPos = text.findCharacterPos(i);
        float charHeight = static_cast<float>(text.getCharacterSize());

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