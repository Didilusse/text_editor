//
// Search Dialog Implementation
//

#include "SearchDialog.h"
#include <algorithm>

SearchDialog::SearchDialog(const sf::Font& font)
    : titleText(font),
      searchText(font),
      resultText(font),
      isVisible(false),
      currentMatchIndex(-1),
      cursorVisible(true) {

    // Semi-transparent background overlay
    background.setSize(sf::Vector2f(10000, 10000));
    background.setFillColor(sf::Color(0, 0, 0, 100));

    // Dialog box
    dialogBox.setSize(sf::Vector2f(400, 120));
    dialogBox.setFillColor(sf::Color(40, 40, 40));
    dialogBox.setOutlineColor(sf::Color(100, 100, 100));
    dialogBox.setOutlineThickness(2);

    // Input box
    inputBox.setSize(sf::Vector2f(360, 35));
    inputBox.setFillColor(sf::Color(60, 60, 60));
    inputBox.setOutlineColor(sf::Color(120, 120, 120));
    inputBox.setOutlineThickness(1);

    // Title text
    titleText.setFont(font);
    titleText.setString("Search");
    titleText.setCharacterSize(18);
    titleText.setFillColor(sf::Color::White);

    // Search text
    searchText.setFont(font);
    searchText.setString("");
    searchText.setCharacterSize(16);
    searchText.setFillColor(sf::Color::White);

    // Result text
    resultText.setFont(font);
    resultText.setString("");
    resultText.setCharacterSize(14);
    resultText.setFillColor(sf::Color(200, 200, 200));

    // Cursor
    cursor.setSize(sf::Vector2f(2, 20));
    cursor.setFillColor(sf::Color::White);
}

void SearchDialog::show() {
    isVisible = true;
    cursorBlinkClock.restart();
    cursorVisible = true;
}

void SearchDialog::hide() {
    isVisible = false;
}

bool SearchDialog::getIsVisible() const {
    return isVisible;
}

void SearchDialog::handleTextInput(char c) {
    // Exclude Enter (13) and other control characters
    if (c >= 32 && c < 127) {  // Printable characters only
        searchQuery += c;
    }
}

void SearchDialog::handleBackspace() {
    if (!searchQuery.empty()) {
        searchQuery.pop_back();
    }
}

void SearchDialog::handleKeyPress(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Escape) {
        hide();
    } else if (key == sf::Keyboard::Key::Enter) {
        nextMatch();
    } else if (key == sf::Keyboard::Key::F3) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
            previousMatch();
        } else {
            nextMatch();
        }
    }
}

void SearchDialog::updateSearch(const std::string& text) {
    matchPositions.clear();
    currentMatchIndex = -1;

    if (searchQuery.empty()) {
        return;
    }

    // Case-insensitive search
    std::string lowerText = text;
    std::string lowerQuery = searchQuery;

    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    // Find all matches
    size_t pos = 0;
    while ((pos = lowerText.find(lowerQuery, pos)) != std::string::npos) {
        matchPositions.push_back(pos);
        pos += 1;  // Continue searching from next character
    }

    // Set to first match if any found
    if (!matchPositions.empty()) {
        currentMatchIndex = 0;
    }
}

void SearchDialog::nextMatch() {
    if (matchPositions.empty()) return;

    currentMatchIndex = (currentMatchIndex + 1) % matchPositions.size();
}

void SearchDialog::previousMatch() {
    if (matchPositions.empty()) return;

    currentMatchIndex--;
    if (currentMatchIndex < 0) {
        currentMatchIndex = matchPositions.size() - 1;
    }
}

size_t SearchDialog::getCurrentMatchPosition() const {
    if (currentMatchIndex >= 0 && currentMatchIndex < static_cast<int>(matchPositions.size())) {
        return matchPositions[currentMatchIndex];
    }
    return 0;
}

size_t SearchDialog::getMatchLength() const {
    return searchQuery.length();
}

bool SearchDialog::hasMatches() const {
    return !matchPositions.empty() && currentMatchIndex >= 0;
}

int SearchDialog::getCurrentMatchIndex() const {
    return currentMatchIndex;
}

int SearchDialog::getTotalMatches() const {
    return matchPositions.size();
}

void SearchDialog::update() {
    // Update cursor blinking
    if (cursorBlinkClock.getElapsedTime() >= sf::milliseconds(500)) {
        cursorVisible = !cursorVisible;
        cursorBlinkClock.restart();
    }

    // Update search text display
    searchText.setString(searchQuery);

    // Update result text
    if (!searchQuery.empty()) {
        if (matchPositions.empty()) {
            resultText.setString("No matches found");
            resultText.setFillColor(sf::Color(255, 100, 100));
        } else {
            resultText.setString("Match " + std::to_string(currentMatchIndex + 1) +
                                " of " + std::to_string(matchPositions.size()) +
                                " (Enter/F3: next, Shift+F3: prev, Esc: close)");
            resultText.setFillColor(sf::Color(100, 255, 100));
        }
    } else {
        resultText.setString("Type to search... (Esc to close)");
        resultText.setFillColor(sf::Color(200, 200, 200));
    }
}

void SearchDialog::draw(sf::RenderWindow& window) {
    if (!isVisible) return;
    sf::FloatRect textBounds = searchText.getLocalBounds();
    sf::Vector2f textPos = searchText.getPosition();
    cursor.setPosition(sf::Vector2f(
        textPos.x + textBounds.size.x,
        textPos.y - 2
    ));
    // Draw semi-transparent background
    window.draw(background);

    // Draw dialog box
    window.draw(dialogBox);

    // Draw title
    window.draw(titleText);

    // Draw input box
    window.draw(inputBox);

    // Draw search text
    window.draw(searchText);

    // Draw cursor
    if (cursorVisible) {
        window.draw(cursor);
    }

    // Draw result text
    window.draw(resultText);
}

void SearchDialog::setPosition(sf::Vector2f windowSize) {
    // Center the dialog
    float dialogX = (windowSize.x - dialogBox.getSize().x) / 2.f;
    float dialogY = (windowSize.y - dialogBox.getSize().y) / 2.f;

    dialogBox.setPosition(sf::Vector2f(dialogX, dialogY));

    titleText.setPosition(sf::Vector2f(dialogX + 20, dialogY + 10));

    inputBox.setPosition(sf::Vector2f(dialogX + 20, dialogY + 40));

    searchText.setPosition(sf::Vector2f(dialogX + 25, dialogY + 45));

    // Position cursor at end of text
    sf::FloatRect textBounds = searchText.getLocalBounds();
    cursor.setPosition(sf::Vector2f(
        dialogX + 25 + textBounds.size.x,
        dialogY + 43
    ));

    resultText.setPosition(sf::Vector2f(dialogX + 20, dialogY + 85));
}