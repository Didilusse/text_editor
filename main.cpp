#include <SFML/Graphics.hpp>
#include "src/GapBuffer.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <limits>
#include <optional>
#include "libs/nativefiledialog/src/include/nfd.h"

struct Button {
    sf::RectangleShape shape;
    sf::Text text;

    Button(const sf::Font& font) : text(font) {}
};

struct DisplayState {
    std::string content;
    size_t cursorIndex;
};
void updateCursorSize(sf::RectangleShape& cursor,
                      const sf::Font& font,
                      unsigned int charSize) {
    float lineHeight = font.getLineSpacing(charSize);
    cursor.setSize({2.f, lineHeight});
}
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

void saveToFile(const GapBuffer& buffer) {
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_SaveDialog(nullptr, nullptr, &outPath);

    if (result == NFD_OKAY) {
        std::string path(outPath);
        if (path.size() < 4 || path.substr(path.size() - 4) != ".txt") {
            path += ".txt";
        }
        std::ofstream outputFile(path);
        if (outputFile.is_open()) {
            outputFile << buffer.getString() << std::endl;
            outputFile.close();
        }
        free(outPath);
    } else if (result == NFD_CANCEL) {
        // Cancelled
    } else {
        free(outPath);
    }
}

void loadFromFile(GapBuffer& buffer) {
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &outPath);

    if (result == NFD_OKAY) {
        std::ifstream inputFile(outPath);
        if (inputFile.is_open()) {
            buffer.clear();
            char ch;
            while (inputFile.get(ch)) {
                buffer.insert(ch);
            }
            inputFile.close();
        }
        free(outPath);
    } else if (result == NFD_CANCEL) {
        // Cancelled
    } else {
        free(outPath);
    }
}

void moveCursorVertical(GapBuffer& buffer, const sf::Text& text, const sf::Font& font, bool down) {
    int currentY = text.findCharacterPos(buffer.getGapStart()).y;
    int currentX = text.findCharacterPos(buffer.getGapStart()).x;
    int spacing = font.getLineSpacing(text.getCharacterSize());
    int targetY = down ? currentY + spacing : currentY - spacing;

    int bestIndex = -1;
    float bestDistance = std::numeric_limits<float>::max();

    for (size_t i = 0; i < text.getString().getSize(); i++) {
        sf::Vector2f charPos = text.findCharacterPos(i);
        if (static_cast<int>(charPos.y) == targetY) {
            float dist = std::abs(charPos.x - static_cast<float>(currentX));
            if (dist < bestDistance) {
                bestDistance = dist;
                bestIndex = i;
            }
        }
    }

    if (bestIndex != -1) {
        buffer.moveTo(bestIndex);
    }
}

DisplayState wrapText(const GapBuffer& buffer, sf::Text& textObj, float maxWidth) {
    std::string raw = buffer.getString();
    std::string displayString;
    std::string currentLine;
    std::string wordBuffer;

    size_t rawCursorIndex = buffer.getGapStart();
    size_t displayCursorIndex = 0;
    bool cursorFound = false;

    for (size_t i = 0; i < raw.size(); i++) {
        char c = raw[i];

        if (i == rawCursorIndex) {
            displayCursorIndex = displayString.size() + currentLine.size() + wordBuffer.size();
            cursorFound = true;
        }

        if (c == '\n') {
            currentLine += wordBuffer;
            displayString += currentLine + "\n";
            currentLine.clear();
            wordBuffer.clear();
            continue;
        }

        wordBuffer += c;

        if (c == ' ' || i == raw.size() - 1) {
            sf::Text temp = textObj;
            temp.setString(currentLine + wordBuffer);

            if (temp.getLocalBounds().size.x > maxWidth) {
                displayString += currentLine + "\n";
                currentLine.clear();
            }

            currentLine += wordBuffer;
            wordBuffer.clear();
        }
    }

    if (!cursorFound) {
        displayCursorIndex = displayString.size() + currentLine.size() + wordBuffer.size();
    }

    displayString += currentLine + wordBuffer;
    return {displayString, displayCursorIndex};
}

void handleMouseClick(sf::Vector2i mousePos, GapBuffer& buffer, const sf::Text& text, const Button& saveBtn, const Button& loadBtn) {
    sf::Vector2f mPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (saveBtn.shape.getGlobalBounds().contains(mPos)) {
        saveToFile(buffer);
        return;
    }

    if (loadBtn.shape.getGlobalBounds().contains(mPos)) {
        loadFromFile(buffer);
        return;
    }

    for (size_t i = 0; i <= text.getString().getSize(); i++) {
        sf::Vector2f charPos = text.findCharacterPos(i);
        float charHeight = static_cast<float>(text.getCharacterSize());

        if (mPos.y >= charPos.y && mPos.y < charPos.y + charHeight) {
            float nextX = text.findCharacterPos(i + 1).x;
            if (mPos.x >= charPos.x && mPos.x < nextX) {
                buffer.moveTo(i);
                break;
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Text Editor");
    sf::Font font;
    if (!font.openFromFile("fonts/Roboto.ttf")) return 1;

    sf::Text text(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setPosition({0, 50});

    sf::RectangleShape cursor(sf::Vector2f(2, 24));
    updateCursorSize(cursor, font, text.getCharacterSize());
    GapBuffer gapBuffer;

    Button saveBtn = createButton(font, "Save", {10, 10});
    Button loadBtn = createButton(font, "Load", {100, 10});
    Button textSize = createButton(font, " ", {190, 10});
    textSize.text.setCharacterSize(12);
    textSize.text.setPosition(sf::Vector2f(195, 17));


    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* resizeEvent = event->getIf<sf::Event::Resized>()) {
                sf::FloatRect visibleArea({0, 0}, {static_cast<float>(resizeEvent->size.x), static_cast<float>(resizeEvent->size.y)});
                window.setView(sf::View(visibleArea));
            }

            if (const auto *textEvent = event->getIf<sf::Event::TextEntered>()) {
                if (textEvent->unicode == 8) {
                    gapBuffer.backspace();
                } else if (textEvent->unicode == 13) {
                    gapBuffer.insert('\n');
                } else {
                    gapBuffer.insert(static_cast<char>(textEvent->unicode));
                }
            }

            if (const auto *keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::Left) gapBuffer.moveLeft();
                if (keyEvent->code == sf::Keyboard::Key::Right) gapBuffer.moveRight();
                if (keyEvent->code == sf::Keyboard::Key::Up) moveCursorVertical(gapBuffer, text, font, false);
                if (keyEvent->code == sf::Keyboard::Key::Down) moveCursorVertical(gapBuffer, text, font, true);
                //Paste
                if (keyEvent->code == sf::Keyboard::Key::V &&
                    (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LSystem))) {
                    std::string pasted = sf::Clipboard::getString();
                    for (char c : pasted) {
                        gapBuffer.insert(c);
                    }
                }
                //Save
                if (keyEvent->code == sf::Keyboard::Key::S &&
                    (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LSystem))) {
                        saveToFile(gapBuffer);
                    }
                //Open file
                if (keyEvent->code == sf::Keyboard::Key::O &&
                    (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LSystem))) {
                        loadFromFile(gapBuffer);
                    }
                if (keyEvent->code == sf::Keyboard::Key::Equal &&
                    (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LSystem))) {
                        text.setCharacterSize(text.getCharacterSize() + 1);
                        updateCursorSize(cursor, font, text.getCharacterSize());
                    }
                if (keyEvent->code == sf::Keyboard::Key::Hyphen &&
                    (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LSystem))) {
                        if (text.getCharacterSize() >= 6) {
                            text.setCharacterSize(text.getCharacterSize() - 1);
                            updateCursorSize(cursor, font, text.getCharacterSize());
                        }
                    }
            }

            if (const auto *mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    handleMouseClick(mouseEvent->position, gapBuffer, text, saveBtn, loadBtn);
                }
            }
        }

        DisplayState state = wrapText(gapBuffer, text, static_cast<float>(window.getSize().x) - 10.0f);
        text.setString(state.content);

        cursor.setPosition(text.findCharacterPos(state.cursorIndex));
        textSize.text.setString("Font Size: " + std::to_string(text.getCharacterSize()));
        window.clear(sf::Color::Black);
        window.draw(textSize.shape);
        window.draw(textSize.text);
        window.draw(saveBtn.shape);
        window.draw(saveBtn.text);
        window.draw(loadBtn.shape);
        window.draw(loadBtn.text);
        window.draw(text);
        window.draw(cursor);
        window.display();
    }
}