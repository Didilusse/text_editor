#include <SFML/Graphics.hpp>
#include "src/GapBuffer.h"
int main()
{
    sf::RenderWindow window(sf::VideoMode({600, 600}), "Text Editor");
    sf::Font font;
    if (!font.openFromFile("fonts/Roboto.ttf"))
    {
        return 1;
    }
    sf::Text text(font);
    sf::RectangleShape cursor(sf::Vector2f(2, 24));

    GapBuffer gapBuffer;
    // set the string to display
    text.setString("Hello world!");

    // set the character size
    text.setCharacterSize(24);

    // set the color
    text.setFillColor(sf::Color::Red);

    // set the text style
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);

    int cursorPosX, cursorPosY;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>())
            {
                if (textEvent->unicode == 8) {
                    gapBuffer.backspace();
                }
                else {
                    gapBuffer.insert(static_cast<char>(textEvent->unicode));
                }

            }
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyEvent->code == sf::Keyboard::Key::Left) {
                    gapBuffer.moveLeft();
                }
                if (keyEvent->code == sf::Keyboard::Key::Right) {
                    gapBuffer.moveRight();
                }

            }

        }
        cursorPosX = text.findCharacterPos(gapBuffer.getGapStart()).x;
        cursorPosY = text.findCharacterPos(gapBuffer.getGapStart()).y;

        cursor.setPosition(sf::Vector2f(cursorPosX, cursorPosY));

        window.clear(sf::Color::Black);
        //draw stuff here
        text.setString(gapBuffer.getString());
        window.draw(cursor);
        window.draw(text);


        window.display();
    }
}