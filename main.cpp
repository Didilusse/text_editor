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
    GapBuffer gapBuffer;
    // set the string to display
    text.setString("Hello world!");

    // set the character size
    text.setCharacterSize(24);

    // set the color
    text.setFillColor(sf::Color::Red);

    // set the text style
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>())
            {
                gapBuffer.insert(textEvent->unicode);
            }

        }



        window.clear(sf::Color::Black);
        //draw stuff here
        text.setString(gapBuffer.getString());
        window.draw(text);


        window.display();
    }
}