#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>

class Scrollbar {
public:
    Scrollbar(float padding);

    void handleMousePress(sf::Vector2i mousePos, sf::Vector2u windowSize, 
                          const sf::FloatRect& textBounds, float topMargin);
    void handleMouseMove(sf::Vector2i mousePos, sf::Vector2u windowSize, 
                         const sf::FloatRect& textBounds);
    void handleMouseRelease();
    
    void draw(sf::RenderWindow& window, const sf::FloatRect& textBounds, float topMargin);
    void applyTheme(bool isDark);

    // Getters and Setters
    float getScrollOffset() const;
    void setScrollOffset(float offset);
    void clampScroll(sf::Vector2u windowSize, const sf::FloatRect& textBounds);

private:
    float scrollOffset;
    float padding;
    bool isDragging;
    sf::Vector2i mousePressPos;
    bool darkTheme = true;
};