//
// Created by Adil Rahmani on 2/5/26.
//

#include "UI.h"

Button createButton(const sf::Font& font, const std::string& label, sf::Vector2f pos) {
    Button btn(font);
    btn.shape.setSize(sf::Vector2f(80, 30));
    btn.shape.setFillColor(sf::Color(50, 50, 50));
    btn.shape.setPosition(pos);

    btn.text.setFont(font);
    btn.text.setString(label);
    btn.text.setCharacterSize(18);
    btn.text.setFillColor(sf::Color::White);
    btn.text.setPosition(sf::Vector2f(pos.x + 20, pos.y + 2));

    return btn;
}

void updateCursorSize(sf::RectangleShape& cursor, const sf::Font& font, unsigned int charSize) {
    float lineHeight = font.getLineSpacing(charSize);
    cursor.setSize({2.f, lineHeight});
}