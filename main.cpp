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

    int cursorPosX = -1, cursorPosY = -1;
    int mousePosX, mousePosY;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            //Check for entered text
            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>())
            {
                if (textEvent->unicode == 8) {
                    //backspace
                    gapBuffer.backspace();
                }
                else if (textEvent->unicode == 13) {
                    //enter key
                    gapBuffer.insert('\n');
                }
                else {
                    gapBuffer.insert(static_cast<char>(textEvent->unicode));
                }

            }

            //Check for key event
            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyEvent->code == sf::Keyboard::Key::Left) {
                    gapBuffer.moveLeft();
                }
                if (keyEvent->code == sf::Keyboard::Key::Right) {
                    gapBuffer.moveRight();
                }
            }

            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    mousePosX = mouseEvent->position.x;
                    mousePosY = mouseEvent->position.y;
                }
            }



        }

        if (mousePosX != -1 && mousePosY != -1) {
            for (int i = 0; i < text.getString().getSize(); i++) {
                //get pos for every character
                int x = text.findCharacterPos(i).x;
                int y = text.findCharacterPos(i).y;
                if (mousePosY >= y && mousePosY < y + text.getCharacterSize()){
                    float x1 = text.findCharacterPos(i).x;
                    float x2 = text.findCharacterPos(i + 1).x;
                    if (mousePosX >= x1 && mousePosX < x2)
                    {
                        gapBuffer.moveTo(i);
                        //reset mouse pos
                        mousePosX = -1;
                        mousePosY = -1;
                        break;
                    }
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