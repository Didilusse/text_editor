//
// Search Dialog Component
//

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <SFML/Graphics.hpp>
#include <string>

class SearchDialog {
private:
    sf::RectangleShape background;
    sf::RectangleShape dialogBox;
    sf::RectangleShape inputBox;
    sf::Text titleText;
    sf::Text searchText;
    sf::Text resultText;
    sf::RectangleShape cursor;

    std::string searchQuery;
    bool isVisible;
    bool cursorVisible;
    sf::Clock cursorBlinkClock;

    // Search results
    std::vector<size_t> matchPositions;
    int currentMatchIndex;

public:
    SearchDialog(const sf::Font& font);

    void show();
    void hide();
    bool getIsVisible() const;

    void handleTextInput(char c);
    void handleBackspace();
    void handleKeyPress(sf::Keyboard::Key key);

    void updateSearch(const std::string& text);
    void nextMatch();
    void previousMatch();

    size_t getCurrentMatchPosition() const;
    size_t getMatchLength() const;
    bool hasMatches() const;
    int getCurrentMatchIndex() const;
    int getTotalMatches() const;

    void update();
    void draw(sf::RenderWindow& window);

    void setPosition(sf::Vector2f windowSize);
};

#endif //SEARCHDIALOG_H