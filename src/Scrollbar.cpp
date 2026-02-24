#include "Scrollbar.h"

Scrollbar::Scrollbar(float padding) 
    : scrollOffset(0.f), padding(padding), isDragging(false) {}

void Scrollbar::handleMousePress(sf::Vector2i mousePos, sf::Vector2u windowSize, 
                                 const sf::FloatRect& textBounds, float topMargin) {
    float windowH = static_cast<float>(windowSize.y);
    float totalContentHeight = std::max(windowH, textBounds.position.y + textBounds.size.y + padding);
    float maxScroll = std::max(0.f, totalContentHeight - windowH);

    float thumbHeight = (windowH / totalContentHeight) * windowH;
    if (thumbHeight < 20.f) thumbHeight = 20.f;

    float thumbTravel = windowH - thumbHeight;
    float currentThumbY = 0.f;
    
    if (maxScroll > 0.f) {
        float ratio = scrollOffset / maxScroll;
        currentThumbY = ratio * thumbTravel;
    }

    float mouseY = static_cast<float>(mousePos.y);

    // Check if clicking on the thumb or track
    if (mouseY >= currentThumbY && mouseY <= currentThumbY + thumbHeight) {
        // Clicking thumb
        mousePressPos = mousePos;
        isDragging = true;
    } else {
        // Clicking track - jump to position
        float targetThumbY = mouseY - (thumbHeight / 2.f);
        targetThumbY = std::clamp(targetThumbY, 0.f, thumbTravel);

        if (thumbTravel > 0.f) {
            float ratio = targetThumbY / thumbTravel;
            scrollOffset = ratio * maxScroll;
            scrollOffset = std::clamp(scrollOffset, 0.f, maxScroll);
        }
        mousePressPos = mousePos;
        isDragging = true; // Treat jump as start of drag
    }
}

void Scrollbar::handleMouseMove(sf::Vector2i mousePos, sf::Vector2u windowSize, const sf::FloatRect& textBounds) {
    if (!isDragging) return;

    float windowH = static_cast<float>(windowSize.y);
    float totalContentHeight = std::max(windowH, textBounds.position.y + textBounds.size.y + padding);
    float maxScroll = std::max(0.f, totalContentHeight - windowH);
    float thumbHeight = (windowH / totalContentHeight) * windowH;
    if (thumbHeight < 20.f) thumbHeight = 20.f;

    float thumbTravel = windowH - thumbHeight;
    float mouseY = static_cast<float>(mousePos.y);
    mouseY = std::clamp(mouseY, 0.f, windowH);

    float thumbY = mouseY - (thumbHeight / 2.f);
    thumbY = std::clamp(thumbY, 0.f, thumbTravel);

    if (thumbTravel > 0.f) {
        float ratio = thumbY / thumbTravel;
        scrollOffset = ratio * maxScroll;
    }
    scrollOffset = std::clamp(scrollOffset, 0.f, maxScroll);
}

void Scrollbar::handleMouseRelease() {
    isDragging = false;
}

void Scrollbar::clampScroll(sf::Vector2u windowSize, const sf::FloatRect& textBounds) {
    float windowHeight = static_cast<float>(windowSize.y);
    float textHeight = textBounds.position.y + textBounds.size.y;
    float maxScroll = std::max(0.f, textHeight - windowHeight + padding);
    scrollOffset = std::clamp(scrollOffset, 0.f, maxScroll);
}

void Scrollbar::draw(sf::RenderWindow& window, const sf::FloatRect& textBounds, float topMargin) {
    float windowW = static_cast<float>(window.getSize().x);
    float windowH = static_cast<float>(window.getSize().y);
    
    // Draw Track
    sf::RectangleShape scrollTrack({12.f, windowH});
    scrollTrack.setFillColor(darkTheme ? sf::Color(40, 40, 40) : sf::Color(210, 210, 210));
    scrollTrack.setPosition(sf::Vector2f(windowW - 12.f, 0.f));
    window.draw(scrollTrack);

    // Calculate Thumbs
    float totalContentHeight = std::max(windowH, textBounds.position.y + textBounds.size.y + padding);

    if (totalContentHeight > windowH) {
        float thumbHeight = (windowH / totalContentHeight) * windowH;
        if (thumbHeight < 20.f) thumbHeight = 20.f;

        float maxScrollY = totalContentHeight - windowH;
        float ratio = scrollOffset / maxScrollY;
        float thumbY = ratio * (windowH - thumbHeight);

        sf::RectangleShape scrollThumb({10.f, thumbHeight});
        scrollThumb.setFillColor(darkTheme ? sf::Color(150, 150, 150) : sf::Color(120, 120, 120));
        scrollThumb.setPosition(sf::Vector2f(windowW - 11.f, thumbY));
        scrollThumb.setOutlineColor(darkTheme ? sf::Color(80, 80, 80) : sf::Color(160, 160, 160));
        scrollThumb.setOutlineThickness(1.f);
        window.draw(scrollThumb);
    }
}

float Scrollbar::getScrollOffset() const { return scrollOffset; }
void Scrollbar::setScrollOffset(float offset) { scrollOffset = offset; }
void Scrollbar::applyTheme(bool isDark) { darkTheme = isDark; }