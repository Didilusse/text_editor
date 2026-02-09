#include <SFML/Graphics.hpp>
#include "src/GapBuffer.h"
#include "src/UI.h"
#include "src/Scrollbar.h"
#include "src/TextRenderer.h"
#include "src/FileOperations.h"
#include "src/InputHandler.h"
#include "src/SearchDialog.h"
#include <iostream>
#include <cmath>

int main() {
    const float TOP_MARGIN = 50.0f;
    const float SCROLL_PADDING = 10.f;
    const float DRAG_THRESHOLD = 5.0f;
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
    cursor.setFillColor(sf::Color::White);

    GapBuffer gapBuffer;
    Scrollbar scrollbar(SCROLL_PADDING);
    SearchDialog searchDialog(font);

    Button saveBtn = createButton(font, "Save", sf::Vector2f(10, 10));
    Button loadBtn = createButton(font, "Load", sf::Vector2f(100, 10));

    // Text size button - will be positioned dynamically
    Button textSize(font);
    textSize.shape.setSize(sf::Vector2f(140, 30));
    textSize.shape.setFillColor(sf::Color(50, 50, 50));
    textSize.text.setFont(font);
    textSize.text.setCharacterSize(18);
    textSize.text.setFillColor(sf::Color::White);

    // Helper function to update text size button position
    auto updateTextSizeButtonPosition = [&]() {
        float xPos = static_cast<float>(window.getSize().x) - 152.f; // 140 width + 12 scrollbar
        textSize.shape.setPosition(sf::Vector2f(xPos, 10));
        textSize.text.setPosition(sf::Vector2f(xPos + 10, 12));
    };
    updateTextSizeButtonPosition();

    bool cursorVisible = true;
    sf::Clock cursorBlinkClock;

    MouseState mouseState = MouseState::Idle;
    sf::Vector2i mousePressPos;
    int selectionAnchor = -1;
    std::string clipboard = ""; // Internal clipboard storage

    // Search highlight
    sf::RectangleShape searchHighlight;
    searchHighlight.setFillColor(sf::Color(255, 255, 0, 100)); // Yellow highlight

    while (window.isOpen()) {
        bool cursorMovedThisFrame = false;

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* resizeEvent = event->getIf<sf::Event::Resized>()) {
                // Resize both views
                sf::Vector2f newSize(static_cast<float>(resizeEvent->size.x),
                                   static_cast<float>(resizeEvent->size.y));
                uiView.setSize(newSize);
                uiView.setCenter(newSize / 2.f);

                textView.setSize(newSize);
                textView.setCenter(newSize / 2.f);

                // Update text size button position to stay anchored to right
                updateTextSizeButtonPosition();

                // Update search dialog position
                searchDialog.setPosition(newSize);
            }

            // Handle search dialog input when it's visible
            if (searchDialog.getIsVisible()) {
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    if (textEvent->unicode < 128 && textEvent->unicode != '\b' &&
                        textEvent->unicode != 127 && textEvent->unicode != 27) {
                        searchDialog.handleTextInput(static_cast<char>(textEvent->unicode));
                        searchDialog.updateSearch(gapBuffer.getString());
                        cursorMovedThisFrame = true;
                    }
                }

                if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyEvent->code == sf::Keyboard::Key::Backspace) {
                        searchDialog.handleBackspace();
                        searchDialog.updateSearch(gapBuffer.getString());
                    }
                    else if (keyEvent->code == sf::Keyboard::Key::Enter) {
                        // Treat Enter the same as F3 (next match)
                        searchDialog.handleKeyPress(sf::Keyboard::Key::F3);
                    }
                    else {
                        searchDialog.handleKeyPress(keyEvent->code);
                    }

                    // Move cursor to current match
                    if (searchDialog.hasMatches()) {
                        gapBuffer.moveTo(searchDialog.getCurrentMatchPosition());
                        cursorMovedThisFrame = true;
                    }
                }

                // Skip normal text input handling when search is open
                continue;
            }

            if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                if (textEvent->unicode < 128 && textEvent->unicode != '\b' &&
                    textEvent->unicode != 127) {
                    // If there's a selection, delete it first before inserting
                    if (selectionAnchor != -1) {
                        int cursorPos = static_cast<int>(gapBuffer.getGapStart());
                        int start = std::min(selectionAnchor, cursorPos);
                        int end = std::max(selectionAnchor, cursorPos);
                        gapBuffer.deleteRange(start, end);
                        selectionAnchor = -1;
                    }
                    gapBuffer.insert(static_cast<char>(textEvent->unicode));
                    cursorMovedThisFrame = true;
                }
            }

            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                bool ctrlOrCmd = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl) ||
                                 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LSystem);

                bool shiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
                                   sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

                if (keyEvent->code == sf::Keyboard::Key::Left) {
                    if (shiftPressed) {
                        // Start selection if not already active
                        if (selectionAnchor == -1) {
                            selectionAnchor = gapBuffer.getGapStart();
                        }
                    } else {
                        // Clear selection if not holding shift
                        selectionAnchor = -1;
                    }
                    gapBuffer.moveLeft();
                    cursorMovedThisFrame = true;
                }
                if (keyEvent->code == sf::Keyboard::Key::Right) {
                    if (shiftPressed) {
                        // Start selection if not already active
                        if (selectionAnchor == -1) {
                            selectionAnchor = gapBuffer.getGapStart();
                        }
                    } else {
                        // Clear selection if not holding shift
                        selectionAnchor = -1;
                    }
                    gapBuffer.moveRight();
                    cursorMovedThisFrame = true;
                }
                if (keyEvent->code == sf::Keyboard::Key::Backspace) {
                    if (selectionAnchor != -1) {
                        // Delete the selection
                        int cursorPos = static_cast<int>(gapBuffer.getGapStart());
                        int start = std::min(selectionAnchor, cursorPos);
                        int end = std::max(selectionAnchor, cursorPos);
                        gapBuffer.deleteRange(start, end);
                        selectionAnchor = -1;
                    } else {
                        // Normal backspace
                        gapBuffer.backspace();
                    }
                    cursorMovedThisFrame = true;
                }
                if (keyEvent->code == sf::Keyboard::Key::Delete) {
                    if (selectionAnchor != -1) {
                        // Delete the selection
                        int cursorPos = static_cast<int>(gapBuffer.getGapStart());
                        int start = std::min(selectionAnchor, cursorPos);
                        int end = std::max(selectionAnchor, cursorPos);
                        gapBuffer.deleteRange(start, end);
                        selectionAnchor = -1;
                    } else {
                        // Delete character at cursor position
                        int cursorPos = gapBuffer.getGapStart();
                        std::string currentText = gapBuffer.getString();
                        if (cursorPos < static_cast<int>(currentText.length())) {
                            gapBuffer.deleteRange(cursorPos, cursorPos + 1);
                        }
                    }
                    cursorMovedThisFrame = true;
                }
                if (keyEvent->code == sf::Keyboard::Key::S && ctrlOrCmd) {
                    saveToFile(gapBuffer);
                }
                if (keyEvent->code == sf::Keyboard::Key::O && ctrlOrCmd) {
                    loadFromFile(gapBuffer);
                }
                if (keyEvent->code == sf::Keyboard::Key::Equal && ctrlOrCmd) {
                    text.setCharacterSize(text.getCharacterSize() + 1);
                    updateCursorSize(cursor, font, text.getCharacterSize());
                }
                if (keyEvent->code == sf::Keyboard::Key::Hyphen && ctrlOrCmd) {
                    if (text.getCharacterSize() >= 6) {
                        text.setCharacterSize(text.getCharacterSize() - 1);
                        updateCursorSize(cursor, font, text.getCharacterSize());
                    }
                }

                // Search
                if (keyEvent->code == sf::Keyboard::Key::F && ctrlOrCmd) {
                    searchDialog.show();
                    searchDialog.setPosition(sf::Vector2f(window.getSize().x, window.getSize().y));
                }

                // Clipboard operations
                if (keyEvent->code == sf::Keyboard::Key::A && ctrlOrCmd) {
                    // Select All
                    selectionAnchor = 0;
                    gapBuffer.moveTo(gapBuffer.getString().length());
                    cursorMovedThisFrame = true;
                }
                if (keyEvent->code == sf::Keyboard::Key::C && ctrlOrCmd) {
                    // Copy
                    if (selectionAnchor != -1) {
                        int cursorPos = static_cast<int>(gapBuffer.getGapStart());
                        int start = std::min(selectionAnchor, cursorPos);
                        int end = std::max(selectionAnchor, cursorPos);
                        clipboard = gapBuffer.getRange(start, end);
                        sf::Clipboard::setString(clipboard); // Also set system clipboard
                    }
                }
                if (keyEvent->code == sf::Keyboard::Key::X && ctrlOrCmd) {
                    // Cut
                    if (selectionAnchor != -1) {
                        int cursorPos = static_cast<int>(gapBuffer.getGapStart());
                        int start = std::min(selectionAnchor, cursorPos);
                        int end = std::max(selectionAnchor, cursorPos);
                        clipboard = gapBuffer.getRange(start, end);
                        sf::Clipboard::setString(clipboard); // Also set system clipboard
                        gapBuffer.deleteRange(start, end);
                        selectionAnchor = -1;
                        cursorMovedThisFrame = true;
                    }
                }
                if (keyEvent->code == sf::Keyboard::Key::V && ctrlOrCmd) {
                    // Paste
                    // Try system clipboard first, fall back to internal clipboard
                    std::string textToPaste = sf::Clipboard::getString().toAnsiString();
                    if (textToPaste.empty()) {
                        textToPaste = clipboard;
                    }

                    if (!textToPaste.empty()) {
                        // If there's a selection, delete it first
                        if (selectionAnchor != -1) {
                            int cursorPos = static_cast<int>(gapBuffer.getGapStart());
                            int start = std::min(selectionAnchor, cursorPos);
                            int end = std::max(selectionAnchor, cursorPos);
                            gapBuffer.deleteRange(start, end);
                            selectionAnchor = -1;
                        }
                        gapBuffer.insertString(textToPaste);
                        cursorMovedThisFrame = true;
                    }
                }
            }

            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    float windowWidth = static_cast<float>(window.getSize().x);

                    // Check if clicking on the scrollbar area
                    if (mouseEvent->position.x >= windowWidth - 12) {
                        sf::FloatRect textBounds = text.getGlobalBounds();
                        scrollbar.handleMousePress(mouseEvent->position, window.getSize(),
                                                  textBounds, TOP_MARGIN);
                        mouseState = MouseState::ScrollbarDragging;
                    } else {
                        // Not clicking on scrollbar
                        mouseState = MouseState::Pressed;
                        mousePressPos = mouseEvent->position;

                        // Check if clicking on buttons
                        sf::Vector2f uiPos(static_cast<float>(mouseEvent->position.x),
                                         static_cast<float>(mouseEvent->position.y));

                        if (saveBtn.shape.getGlobalBounds().contains(uiPos)) {
                            saveToFile(gapBuffer);
                        } else if (loadBtn.shape.getGlobalBounds().contains(uiPos)) {
                            loadFromFile(gapBuffer);
                        } else {
                            // Clicking in text area
                            handleMouseClick(mouseEvent->position, gapBuffer, text,
                                           saveBtn, loadBtn, window, textView);
                            selectionAnchor = gapBuffer.getGapStart();
                        }
                    }
                }
            }

            if (const auto* moveEvent = event->getIf<sf::Event::MouseMoved>()) {
                if (mouseState == MouseState::ScrollbarDragging) {
                    sf::FloatRect textBounds = text.getGlobalBounds();
                    scrollbar.handleMouseMove(moveEvent->position, window.getSize(), textBounds);
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

                    for (size_t i = 0; i <= text.getString().getSize(); i++) {
                        sf::Vector2f charPos = text.findCharacterPos(i);
                        float charHeight = text.getCharacterSize();

                        if (worldPos.y >= charPos.y && worldPos.y < charPos.y + charHeight) {
                            float dist = std::abs(worldPos.x - charPos.x);

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
                    if (mouseState == MouseState::ScrollbarDragging) {
                        scrollbar.handleMouseRelease();
                    }
                    mouseState = MouseState::Idle;
                }
            }

            if (const auto* scrollEvent = event->getIf<sf::Event::MouseWheelScrolled>()) {
                if (scrollEvent->wheel == sf::Mouse::Wheel::Vertical) {
                    float delta = scrollEvent->delta * 30.f;
                    scrollbar.setScrollOffset(scrollbar.getScrollOffset() - delta);
                }
            }
        }

        // Vertical arrow key handling with repeat
        static sf::Clock verticalMoveClock;
        static bool verticalKeyHeld = false;

        const sf::Time initialDelay = sf::milliseconds(250);
        const sf::Time repeatDelay = sf::milliseconds(60);

        bool upHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
        bool downHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);

        if (upHeld || downHeld) {
            if (!verticalKeyHeld) {
                moveCursorVertical(gapBuffer, text, font, downHeld);
                cursorMovedThisFrame = true;
                verticalMoveClock.restart();
                verticalKeyHeld = true;
            } else {
                sf::Time elapsed = verticalMoveClock.getElapsedTime();
                sf::Time needed = (elapsed < initialDelay) ? initialDelay : repeatDelay;

                if (elapsed >= needed) {
                    moveCursorVertical(gapBuffer, text, font, downHeld);
                    cursorMovedThisFrame = true;
                    verticalMoveClock.restart();
                }
            }
        } else {
            verticalKeyHeld = false;
        }

        // Update search dialog
        searchDialog.update();

        // Cursor blinking logic
        if (cursorMovedThisFrame) {
            cursorVisible = true;
            cursorBlinkClock.restart();
        }

        if (cursorBlinkClock.getElapsedTime() >= CURSOR_BLINK_INTERVAL) {
            cursorVisible = !cursorVisible;
            cursorBlinkClock.restart();
        }

        // Update text display with word wrapping
        float textAreaWidth = static_cast<float>(window.getSize().x) - 25.f;
        DisplayState state = wrapText(gapBuffer, text, textAreaWidth);
        text.setString(state.content);

        // Update cursor position
        cursor.setPosition(text.findCharacterPos(state.cursorIndex));
        sf::Vector2f cursorPos = cursor.getPosition();
        float cursorHeight = cursor.getSize().y;

        // Auto-scroll to cursor
        if (cursorMovedThisFrame) {
            float windowHeight = static_cast<float>(window.getSize().y);
            float topVisible = scrollbar.getScrollOffset() + TOP_MARGIN;
            float bottomVisible = scrollbar.getScrollOffset() + windowHeight - SCROLL_PADDING;

            if (cursorPos.y < topVisible) {
                scrollbar.setScrollOffset(cursorPos.y - TOP_MARGIN);
            } else if (cursorPos.y + cursorHeight > bottomVisible) {
                scrollbar.setScrollOffset((cursorPos.y + cursorHeight) - windowHeight + SCROLL_PADDING);
            }
        }

        // Clamp scroll to valid range
        sf::FloatRect textBounds = text.getGlobalBounds();
        scrollbar.clampScroll(window.getSize(), textBounds);

        // Update UI
        textSize.text.setString("Font Size: " + std::to_string(text.getCharacterSize()));
        window.clear(sf::Color::Black);

        // Set text view with scroll offset
        textView.setCenter(
            sf::Vector2f(
                static_cast<float>(window.getSize().x) / 2.f,
                (static_cast<float>(window.getSize().y) / 2.f) + scrollbar.getScrollOffset()
            )
        );
        window.setView(textView);

        // Draw selection highlighting
        drawSelection(window, text, font, selectionAnchor, gapBuffer.getGapStart());


        // Draw text and cursor
        window.draw(text);
        if (cursorVisible && !searchDialog.getIsVisible()) {
            window.draw(cursor);
        }

        // Switch to UI view for drawing UI elements
        window.setView(uiView);

        // Draw scrollbar
        scrollbar.draw(window, textBounds, TOP_MARGIN);

        // Draw header background
        sf::RectangleShape headerBg(sf::Vector2f(static_cast<float>(window.getSize().x), TOP_MARGIN));
        headerBg.setFillColor(sf::Color(30, 30, 30));
        window.draw(headerBg);

        // Draw UI buttons
        window.draw(textSize.shape);
        window.draw(textSize.text);
        window.draw(saveBtn.shape);
        window.draw(saveBtn.text);
        window.draw(loadBtn.shape);
        window.draw(loadBtn.text);

        // Draw search dialog on top of everything
        searchDialog.draw(window);

        window.display();
        cursorMovedThisFrame = false;
    }

    return 0;
}