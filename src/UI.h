#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct Button {
    sf::RectangleShape shape;
    sf::Text text;

    Button(const sf::Font& font) : text(font) {}
};

Button createButton(const sf::Font& font, const std::string& label, sf::Vector2f pos);
void updateCursorSize(sf::RectangleShape& cursor, const sf::Font& font, unsigned int charSize);