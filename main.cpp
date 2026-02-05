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
enum class MouseState {
    Idle,
    Pressed,
    Dragging,
    ScrollbarDragging
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

void handleMouseClick(sf::Vector2i mousePos, GapBuffer& buffer, const sf::Text& text,
                      const Button& saveBtn, const Button& loadBtn,
                      const sf::RenderWindow& window, const sf::View& textView) {

    // Check Buttons (using Screen Coordinates / UI View)
    sf::Vector2f uiPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    if (saveBtn.shape.getGlobalBounds().contains(uiPos)) {
        saveToFile(buffer);
        return;
    }
    if (loadBtn.shape.getGlobalBounds().contains(uiPos)) {
        loadFromFile(buffer);
        return;
    }

    // Check Text (using World Coordinates / Text View)
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, textView);

    int bestIndex = -1;
    float bestDistance = std::numeric_limits<float>::max();

    for (size_t i = 0; i <= text.getString().getSize(); i++) {
        sf::Vector2f charPos = text.findCharacterPos(i);
        float charHeight = text.getCharacterSize();

        if (worldPos.y >= charPos.y &&
            worldPos.y < charPos.y + charHeight) {

            float dist = std::abs(worldPos.x - charPos.x);

            if (dist < bestDistance) {
                bestDistance = dist;
                bestIndex = static_cast<int>(i);
            }
            }
    }

    if (bestIndex != -1) {
        buffer.moveTo(bestIndex);
    }
}


int main() {
    const float TOP_MARGIN = 50.0f;
    const float SCROLL_PADDING = 10.f;
    const sf::Time CURSOR_BLINK_INTERVAL = sf::milliseconds(500);

    sf::RenderWindow window(sf::VideoMode({800, 600}), "Text Editor");
    sf::View uiView = window.getDefaultView();
    sf::View textView = window.getDefaultView();
    window.setFramerateLimit(60);
    sf::Font font;
    if (!font.openFromFile("fonts/Roboto.ttf")) return 1;

    sf::Text text(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setPosition({0, TOP_MARGIN});

    sf::RectangleShape cursor(sf::Vector2f(2, 24));
    updateCursorSize(cursor, font, text.getCharacterSize());
    GapBuffer gapBuffer;

    Button saveBtn = createButton(font, "Save", {10, 10});
    Button loadBtn = createButton(font, "Load", {100, 10});
    Button textSize = createButton(font, " ", {190, 10});
    textSize.text.setCharacterSize(12);
    textSize.text.setPosition(sf::Vector2f(195, 17));
    float scrollOffsetY = 0.f;
    bool cursorMovedThisFrame = false;

    int selectionAnchor = -1;
    sf::Clock cursorBlinkClock;
    bool cursorVisible = true;

    MouseState mouseState = MouseState::Idle;
    sf::Vector2i mousePressPos;
    const float DRAG_THRESHOLD = 4.f; // pixels

    while (window.isOpen()) {
        // Update display text BEFORE processing events so cursor movement has accurate positions
        DisplayState state = wrapText(gapBuffer, text, static_cast<float>(window.getSize().x) - 10.0f);
        text.setString(state.content);

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* resizeEvent = event->getIf<sf::Event::Resized>()) {
                // Resize both views
                sf::Vector2f newSize(static_cast<float>(resizeEvent->size.x), static_cast<float>(resizeEvent->size.y));
                uiView.setSize(newSize);
                uiView.setCenter(newSize / 2.f);

                textView.setSize(newSize);
                textView.setCenter(newSize / 2.f);  // Reset center on resize
            }

            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                const float scrollSpeed = 40.f;
                scrollOffsetY -= wheel->delta * scrollSpeed;
                cursorMovedThisFrame = false;
            }


            if (const auto *textEvent = event->getIf<sf::Event::TextEntered>()) {
                if (textEvent->unicode == 8) {
                    if (selectionAnchor != -1 && selectionAnchor != gapBuffer.getGapStart()) {
                        size_t start = std::min((size_t)selectionAnchor, gapBuffer.getGapStart());
                        size_t end   = std::max((size_t)selectionAnchor, gapBuffer.getGapStart());

                        gapBuffer.moveTo(end);
                        for (size_t i = 0; i < (end - start); ++i) {
                            gapBuffer.backspace();
                        }
                        selectionAnchor = -1;
                    }
                    else {
                        gapBuffer.backspace();
                    }
                }else if (textEvent->unicode == 13 || textEvent->unicode == 10) {
                    if (selectionAnchor != -1 && selectionAnchor != gapBuffer.getGapStart()) {
                        size_t start = std::min((size_t)selectionAnchor, gapBuffer.getGapStart());
                        size_t end   = std::max((size_t)selectionAnchor, gapBuffer.getGapStart());
                        gapBuffer.moveTo(end);
                        for (size_t i = 0; i < (end - start); ++i) gapBuffer.backspace();
                        selectionAnchor = -1;
                    }
                    gapBuffer.insert('\n');
                } else if (textEvent->unicode >= 32 && textEvent->unicode < 128) {
                    if (selectionAnchor != -1 && selectionAnchor != gapBuffer.getGapStart()) {
                        size_t start = std::min((size_t)selectionAnchor, gapBuffer.getGapStart());
                        size_t end   = std::max((size_t)selectionAnchor, gapBuffer.getGapStart());

                        gapBuffer.moveTo(end);
                        for (size_t i = 0; i < (end - start); ++i) {
                            gapBuffer.backspace();
                        }
                        selectionAnchor = -1;
                    }
                    gapBuffer.insert(static_cast<char>(textEvent->unicode));
                }

                cursorMovedThisFrame = true;
            }


            if (const auto *keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                bool ctrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                   sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl) ||
                   sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LSystem);
                if (keyEvent->code == sf::Keyboard::Key::Left)  {
                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) && selectionAnchor == -1) {
                        selectionAnchor = gapBuffer.getGapStart();
                    }
                    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)){
                        selectionAnchor = -1;
                    }
                    gapBuffer.moveLeft();
                    cursorMovedThisFrame = true;
                }
                if (keyEvent->code == sf::Keyboard::Key::Right) {
                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) && selectionAnchor == -1) {
                        selectionAnchor = gapBuffer.getGapStart();
                    }
                    else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)){
                        selectionAnchor = -1;
                    }
                    gapBuffer.moveRight();
                    cursorMovedThisFrame = true;
                }
                //Copy
                if (keyEvent->code == sf::Keyboard::Key::C && ctrlPressed) {
                    if (selectionAnchor != -1 && selectionAnchor != gapBuffer.getGapStart()) {
                        size_t start = std::min((size_t)selectionAnchor, gapBuffer.getGapStart());
                        size_t end   = std::max((size_t)selectionAnchor, gapBuffer.getGapStart());

                        std::string content = gapBuffer.getString();
                        std::string selection = content.substr(start, end - start);

                        sf::Clipboard::setString(selection);
                    }
                }
                //Cut
                if (keyEvent->code == sf::Keyboard::Key::X && ctrlPressed) {
                    if (selectionAnchor != -1 && selectionAnchor != gapBuffer.getGapStart()) {
                        size_t start = std::min((size_t)selectionAnchor, gapBuffer.getGapStart());
                        size_t end   = std::max((size_t)selectionAnchor, gapBuffer.getGapStart());


                        std::string content = gapBuffer.getString();
                        std::string selection = content.substr(start, end - start);
                        sf::Clipboard::setString(selection);


                        // move cursor to the END of selection and backspace until START
                        gapBuffer.moveTo(end);

                        for (size_t i = 0; i < (end - start); ++i) {
                            gapBuffer.backspace();
                        }

                        selectionAnchor = -1;
                        cursorMovedThisFrame = true;
                    }
                }
                //Select All
                if (keyEvent->code == sf::Keyboard::Key::A && ctrlPressed) {
                    selectionAnchor = 0;
                    gapBuffer.moveTo(text.getString().getSize());
                    cursorMovedThisFrame = true;
                }

                if (keyEvent->code == sf::Keyboard::Key::V && ctrlPressed) {

                    if (selectionAnchor != -1 && selectionAnchor != gapBuffer.getGapStart()) {
                        size_t start = std::min((size_t)selectionAnchor, gapBuffer.getGapStart());
                        size_t end   = std::max((size_t)selectionAnchor, gapBuffer.getGapStart());


                        gapBuffer.moveTo(end);
                        for (size_t i = 0; i < (end - start); ++i) {
                            gapBuffer.backspace();
                        }


                        selectionAnchor = -1;
                    }

                    // perform the actual Paste
                    std::string pasted = sf::Clipboard::getString();
                    for (char c : pasted) {
                        if (c != '\r') {
                            gapBuffer.insert(c);
                        }
                    }
                    cursorMovedThisFrame = true;
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
                //Increase text
                if (keyEvent->code == sf::Keyboard::Key::Equal &&
                    (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LSystem))) {
                        text.setCharacterSize(text.getCharacterSize() + 1);
                        updateCursorSize(cursor, font, text.getCharacterSize());
                    }
                //Decrease text
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
                    float windowWidth = static_cast<float>(window.getSize().x);
                    float windowH = static_cast<float>(window.getSize().y);

                    // Check if clicking on the scrollbar area
                    if (mouseEvent->position.x >= windowWidth - 12) {
                        mouseState = MouseState::ScrollbarDragging;

                        // Calculate scrollbar properties
                        sf::FloatRect textBounds = text.getGlobalBounds();
                        float totalContentHeight = std::max(windowH, textBounds.position.y + textBounds.size.y + SCROLL_PADDING);
                        float maxScroll = std::max(0.f, totalContentHeight - windowH);

                        // Calculate thumb height and position
                        float thumbHeight = (windowH / totalContentHeight) * windowH;
                        if (thumbHeight < 20.f) thumbHeight = 20.f;

                        float thumbTravel = windowH - thumbHeight;
                        float currentThumbY = 0.f;
                        if (maxScroll > 0.f) {
                            float ratio = scrollOffsetY / maxScroll;
                            currentThumbY = ratio * thumbTravel;
                        }

                        float mouseY = static_cast<float>(mouseEvent->position.y);

                        // Check if clicking on the thumb itself or on the track
                        if (mouseY >= currentThumbY && mouseY <= currentThumbY + thumbHeight) {
                            // Clicking on the thumb - will drag from current position
                            mousePressPos = mouseEvent->position;
                        } else {
                            // Clicking on the track - jump to that position
                            float targetThumbY = mouseY - (thumbHeight / 2.f);
                            targetThumbY = std::clamp(targetThumbY, 0.f, thumbTravel);

                            if (thumbTravel > 0.f) {
                                float ratio = targetThumbY / thumbTravel;
                                scrollOffsetY = ratio * maxScroll;
                                scrollOffsetY = std::clamp(scrollOffsetY, 0.f, maxScroll);
                            }

                            mousePressPos = mouseEvent->position;
                        }
                    } else {
                        // Not clicking on scrollbar
                        mouseState = MouseState::Pressed;
                        mousePressPos = mouseEvent->position;

                        handleMouseClick(
                            mouseEvent->position,
                            gapBuffer,
                            text,
                            saveBtn,
                            loadBtn,
                            window,
                            textView
                        );
                        selectionAnchor = gapBuffer.getGapStart();
                    }
                }
            }

            if (const auto* moveEvent = event->getIf<sf::Event::MouseMoved>()) {
                if (mouseState == MouseState::ScrollbarDragging) {
                    float windowH = static_cast<float>(window.getSize().y);

                    // Calculate total content height and max scroll
                    sf::FloatRect textBounds = text.getGlobalBounds();
                    float totalContentHeight = std::max(windowH, textBounds.position.y + textBounds.size.y + SCROLL_PADDING);
                    float maxScroll = std::max(0.f, totalContentHeight - windowH);

                    // Calculate thumb height
                    float thumbHeight = (windowH / totalContentHeight) * windowH;
                    if (thumbHeight < 20.f) thumbHeight = 20.f;

                    // Calculate the available space for the thumb to move
                    float thumbTravel = windowH - thumbHeight;

                    // Get mouse Y position and clamp it to the scrollbar track
                    float mouseY = static_cast<float>(moveEvent->position.y);
                    mouseY = std::clamp(mouseY, 0.f, windowH);

                    // Calculate the new thumb position (center the thumb on the mouse)
                    float thumbY = mouseY - (thumbHeight / 2.f);
                    thumbY = std::clamp(thumbY, 0.f, thumbTravel);

                    // Map thumb position to scroll offset
                    if (thumbTravel > 0.f) {
                        float ratio = thumbY / thumbTravel;
                        scrollOffsetY = ratio * maxScroll;
                    }

                    scrollOffsetY = std::clamp(scrollOffsetY, 0.f, maxScroll);
                }
                else if (mouseState == MouseState::Pressed) {
                    sf::Vector2f delta(
                        moveEvent->position.x - mousePressPos.x,
                        moveEvent->position.y - mousePressPos.y
                    );

                    if (std::hypot(delta.x, delta.y) > DRAG_THRESHOLD) {
                        mouseState = MouseState::Dragging;
                    }
                }
                else if (mouseState == MouseState::Dragging) {
                    // Convert mouse → text coords
                    sf::Vector2f worldPos = window.mapPixelToCoords(moveEvent->position, textView);

                    int bestIndex = -1;
                    float bestDistance = std::numeric_limits<float>::max();

                    // Iterate through all characters to find the closest one on the correct line
                    for (size_t i = 0; i <= text.getString().getSize(); i++) {
                        sf::Vector2f charPos = text.findCharacterPos(i);
                        float charHeight = text.getCharacterSize();

                        // Check if this character is on the same line (Y-level) as the mouse
                        if (worldPos.y >= charPos.y && worldPos.y < charPos.y + charHeight) {

                            // Calculate horizontal distance
                            float dist = std::abs(worldPos.x - charPos.x);

                            // Keep track of the closest match found so far
                            if (dist < bestDistance) {
                                bestDistance = dist;
                                bestIndex = static_cast<int>(i);
                            }
                        }
                    }


                    if (bestIndex != -1) {
                        gapBuffer.moveTo(bestIndex);
                    }

                    cursorMovedThisFrame = true;
                }
            }

            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {

                    if (mouseState == MouseState::Pressed) {

                        selectionAnchor = -1;
                    }


                    mouseState = MouseState::Idle;
                }
            }
        }


        static sf::Clock verticalMoveClock;
        static bool verticalKeyHeld = false;

        const sf::Time initialDelay = sf::milliseconds(250);
        const sf::Time repeatDelay  = sf::milliseconds(60);

        bool upHeld   = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
        bool downHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);

        if (upHeld || downHeld) {
            if (!verticalKeyHeld) {
                // First press: move immediately
                moveCursorVertical(gapBuffer, text, font, downHeld);
                cursorMovedThisFrame = true;
                verticalMoveClock.restart();
                verticalKeyHeld = true;
            } else {
                // Key held: repeat after delay
                sf::Time elapsed = verticalMoveClock.getElapsedTime();
                sf::Time needed  = (elapsed < initialDelay) ? initialDelay : repeatDelay;

                if (elapsed >= needed) {
                    moveCursorVertical(gapBuffer, text, font, downHeld);
                    cursorMovedThisFrame = true;
                    verticalMoveClock.restart();
                }
            }
        } else {
            verticalKeyHeld = false;
        }
        // Cursor blinking logic
        if (cursorMovedThisFrame) {
            cursorVisible = true;
            cursorBlinkClock.restart();
        }

        if (cursorBlinkClock.getElapsedTime() >= CURSOR_BLINK_INTERVAL) {
            cursorVisible = !cursorVisible;
            cursorBlinkClock.restart();
        }

        // Calculate bounds for clamping
        sf::FloatRect textBounds = text.getGlobalBounds();
        float textHeight = textBounds.position.y + textBounds.size.y;
        float windowHeight = static_cast<float>(window.getSize().y);

        float maxScroll = std::max(0.f, textHeight - windowHeight + SCROLL_PADDING);


        cursor.setPosition(text.findCharacterPos(state.cursorIndex));
        sf::Vector2f cursorPos = cursor.getPosition();
        float cursorHeight = cursor.getSize().y;

        // If the cursor moved, we force the screen to scroll to it
        if (cursorMovedThisFrame) {
            float topVisible = scrollOffsetY + TOP_MARGIN;
            float bottomVisible = scrollOffsetY + windowHeight - SCROLL_PADDING;

            if (cursorPos.y < topVisible) {
                scrollOffsetY = cursorPos.y - TOP_MARGIN;
            } else if (cursorPos.y + cursorHeight > bottomVisible) {
                scrollOffsetY = (cursorPos.y + cursorHeight) - windowHeight + SCROLL_PADDING;
            }
        }


        scrollOffsetY = std::clamp(scrollOffsetY, 0.f, maxScroll);

        textSize.text.setString("Font Size: " + std::to_string(text.getCharacterSize()));
        window.clear(sf::Color::Black);

        textView.setCenter(
            sf::Vector2f(
                static_cast<float>(window.getSize().x) / 2.f,
                (static_cast<float>(window.getSize().y) / 2.f) + scrollOffsetY
            )
        );
        window.setView(textView);

        if (selectionAnchor != -1 && selectionAnchor != gapBuffer.getGapStart()) {
            size_t start = std::min((size_t)selectionAnchor, gapBuffer.getGapStart());
            size_t end   = std::max((size_t)selectionAnchor, gapBuffer.getGapStart());

            for (size_t i = start; i < end; ++i) {
                sf::Vector2f charPos = text.findCharacterPos(i);
                sf::Vector2f nextCharPos = text.findCharacterPos(i + 1);

                // Calculate width: distance to next char
                float width = nextCharPos.x - charPos.x;
                float height = font.getLineSpacing(text.getCharacterSize());


                if (nextCharPos.y != charPos.y || width <= 0) {
                    width = 10.f; // Arbitrary width to show newline selection
                }

                sf::RectangleShape selectionRect({width, height});
                selectionRect.setFillColor(sf::Color(100, 100, 255, 128)); // Semi-transparent blue
                selectionRect.setPosition(charPos);
                window.draw(selectionRect);
            }
        }

        window.draw(text);
        if (cursorVisible) {
            window.draw(cursor);
        }

        window.setView(uiView);

        float windowW = static_cast<float>(window.getSize().x);
        float windowH = static_cast<float>(window.getSize().y);
        float totalContentHeight =
            std::max(windowH, textBounds.position.y + textBounds.size.y + SCROLL_PADDING);
        float contentH = std::max(windowH, text.getGlobalBounds().position.y + TOP_MARGIN + SCROLL_PADDING);
        float thumbHeight = (windowH / totalContentHeight) * windowH;
        if (thumbHeight < 20.f) thumbHeight = 20.f;
        sf::RectangleShape scrollTrack({12.f, windowH});
        scrollTrack.setFillColor(sf::Color(40, 40, 40)); // Dark grey background
        scrollTrack.setPosition(sf::Vector2f(windowW - 12.f, 0.f));
        window.draw(scrollTrack);

        if (totalContentHeight > windowH) {
            float thumbHeight = (windowH / totalContentHeight) * windowH;
            if (thumbHeight < 20.f) thumbHeight = 20.f;

            float maxScrollY = totalContentHeight - windowH;
            float ratio = scrollOffsetY / maxScrollY;
            float thumbY = ratio * (windowH - thumbHeight);

            sf::RectangleShape scrollThumb({10.f, thumbHeight});
            scrollThumb.setFillColor(sf::Color(150, 150, 150));
            scrollThumb.setPosition(sf::Vector2f(windowW - 11.f, thumbY));
            scrollThumb.setOutlineColor(sf::Color(80, 80, 80));
            scrollThumb.setOutlineThickness(1.f);

            window.draw(scrollThumb);
        }

        sf::RectangleShape headerBg(sf::Vector2f(static_cast<float>(window.getSize().x), TOP_MARGIN));
        headerBg.setFillColor(sf::Color(30, 30, 30));
        window.draw(headerBg);
        std::cout << selectionAnchor << std::endl;
        window.draw(textSize.shape);
        window.draw(textSize.text);
        window.draw(saveBtn.shape);
        window.draw(saveBtn.text);
        window.draw(loadBtn.shape);
        window.draw(loadBtn.text);
        window.display();

        cursorMovedThisFrame = false;
    }
}