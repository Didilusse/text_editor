//
// Created by Adil Rahmani on 2/5/26.
//
#pragma once
#include <SFML/Graphics.hpp>
#include "GapBuffer.h"
#include "UI.h"

enum class MouseState {
    Idle,
    Pressed,
    Dragging,
    ScrollbarDragging
};

void handleMouseClick(sf::Vector2i mousePos, GapBuffer& buffer, const sf::Text& text,
                      const sf::RenderWindow& window, const sf::View& textView);