#include <SFML/Graphics.hpp>
#include "src/GapBuffer.h"
#include <fstream>
#include <iostream>
#include "libs/nativefiledialog/src/include/nfd.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({600, 600}), "Text Editor");
    sf::Font font;
    if (!font.openFromFile("fonts/Roboto.ttf")) {
        return 1;
    }
    sf::Text text(font);
    sf::RectangleShape cursor(sf::Vector2f(2, 24));

    GapBuffer gapBuffer;

    // set the character size
    text.setCharacterSize(24);

    // set the color
    text.setFillColor(sf::Color::White);

    sf::RectangleShape saveBtn(sf::Vector2f(80, 30));
    saveBtn.setFillColor(sf::Color(50, 50, 50));
    saveBtn.setPosition(sf::Vector2f(10, 10));

    sf::Text saveText(font);
    saveText.setString("Save");
    saveText.setCharacterSize(18);
    saveText.setFillColor(sf::Color::White);
    saveText.setPosition(sf::Vector2f(30, 12));

    // Load Button
    sf::RectangleShape loadBtn(sf::Vector2f(80, 30));
    loadBtn.setFillColor(sf::Color(50, 50, 50));
    loadBtn.setPosition(sf::Vector2f(100, 10));

    sf::Text loadText(font);
    loadText.setString("Load");
    loadText.setCharacterSize(18);
    loadText.setFillColor(sf::Color::White);
    loadText.setPosition(sf::Vector2f(120, 12));

    text.setPosition(sf::Vector2f(0, 50));


    int cursorPosX = -1, cursorPosY = -1;
    int mousePosX = -1, mousePosY = -1;
    while (window.isOpen()) {


        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* resizeEvent = event->getIf<sf::Event::Resized>()) {
                unsigned int newWidth = resizeEvent->size.x;
                unsigned int newHeight = resizeEvent->size.y;

                sf::FloatRect visibleArea(sf::Vector2f(0,0), sf::Vector2f(newWidth, newHeight));
                window.setView(sf::View(visibleArea));
            }
            //Check for entered text
            if (const auto *textEvent = event->getIf<sf::Event::TextEntered>()) {
                if (textEvent->unicode == 8) {
                    //backspace
                    gapBuffer.backspace();
                } else if (textEvent->unicode == 13) {
                    //enter key
                    gapBuffer.insert('\n');
                } else {
                    gapBuffer.insert(static_cast<char>(textEvent->unicode));
                }
            }

            //Check for key event
            if (const auto *keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Left) {
                    gapBuffer.moveLeft();
                }
                if (keyEvent->code == sf::Keyboard::Key::Right) {
                    gapBuffer.moveRight();
                }
                if (keyEvent->code == sf::Keyboard::Key::Up) {
                    int currentY = text.findCharacterPos(gapBuffer.getGapStart()).y;
                    int currentX = text.findCharacterPos(gapBuffer.getGapStart()).x;
                    //calculate new Y direction
                    int targetY = currentY - font.getLineSpacing(text.getCharacterSize());

                    int bestIndex = -1;
                    float bestDistance = std::numeric_limits<float>::max();

                    for (int i = 0; i < text.getString().getSize(); i++) {
                        int y = text.findCharacterPos(i).y;
                        if (y == targetY) {
                            float x = text.findCharacterPos(i).x;
                            float dist = std::abs(x - currentX);
                            if (dist < bestDistance) {
                                bestDistance = dist;
                                bestIndex = i;
                            }
                        }
                    }
                    if (bestIndex != -1) {
                        gapBuffer.moveTo(bestIndex);
                    }
                }
                if (keyEvent->code == sf::Keyboard::Key::Down) {
                    int currentY = text.findCharacterPos(gapBuffer.getGapStart()).y;
                    int currentX = text.findCharacterPos(gapBuffer.getGapStart()).x;

                    int targetY = currentY + font.getLineSpacing(text.getCharacterSize());

                    int bestIndex = -1;
                    float bestDistance = std::numeric_limits<float>::max();

                    for (int i = 0; i < text.getString().getSize(); i++) {
                        int y = text.findCharacterPos(i).y;
                        if (y == targetY) {
                            float x = text.findCharacterPos(i).x;
                            float dist = std::abs(x - currentX);
                            if (dist < bestDistance) {
                                bestDistance = dist;
                                bestIndex = i;
                            }
                        }
                    }
                    if (bestIndex != -1) {
                        gapBuffer.moveTo(bestIndex);
                    }
                }
            }

            if (const auto *mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    mousePosX = mouseEvent->position.x;
                    mousePosY = mouseEvent->position.y;
                }
            }
        }

        if (mousePosX != -1 && mousePosY != -1) {
            //check for button presses
            if (saveBtn.getGlobalBounds().contains(sf::Vector2f(mousePosX, mousePosY))) {
                nfdchar_t *outPath = nullptr;
                nfdresult_t result = NFD_SaveDialog(nullptr, nullptr, &outPath);

                if (result == NFD_OKAY) {
                    std::string path(outPath);
                    if (path.size() < 4 || path.substr(path.size() - 4) != ".txt") {
                        path += ".txt";
                    }
                    std::ofstream outputFile(path);

                    if (outputFile.is_open()) {
                        // Write data to the file using the insertion operator
                        outputFile << gapBuffer.getString() << std::endl;
                        outputFile.close();
                    } else {
                        std::cerr << "Error opening the file." << std::endl;
                    }
                    puts(outPath);
                } else if (result == NFD_CANCEL) {
                    puts("User pressed cancel.");
                } else {
                    printf("Error: %s\n", NFD_GetError());
                }


                free(outPath);
                mousePosX = -1;
                mousePosY = -1;
            }


            if (loadBtn.getGlobalBounds().contains(sf::Vector2f(mousePosX, mousePosY))) {
                nfdchar_t *outPath = nullptr;
                nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

                if (result == NFD_OKAY) {
                    std::ifstream inputFile(outPath);
                    char ch;
                    if (inputFile.is_open()) {
                        gapBuffer.clear();
                        while (inputFile.get(ch)) {
                            gapBuffer.insert(ch);
                        }
                        inputFile.close();
                    } else {
                        std::cerr << "Error opening the file." << std::endl;
                    }
                    puts(outPath);
                } else if (result == NFD_CANCEL) {
                    puts("User pressed cancel.");
                } else {
                    printf("Error: %s\n", NFD_GetError());
                }


                free(outPath);
                mousePosX = -1;
                mousePosY = -1;
            }

            for (int i = 0; i <= text.getString().getSize(); i++) {
                //get pos for every character
                int y = text.findCharacterPos(i).y;
                if (mousePosY >= y && mousePosY < y + text.getCharacterSize()) {
                    float x1 = text.findCharacterPos(i).x;
                    float x2 = text.findCharacterPos(i + 1).x;
                    if (mousePosX >= x1 && mousePosX < x2) {
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
        window.draw(saveBtn);
        window.draw(saveText);
        window.draw(loadBtn);
        window.draw(loadText);
        text.setString(gapBuffer.getString());
        window.draw(cursor);
        window.draw(text);


        window.display();
    }
}
