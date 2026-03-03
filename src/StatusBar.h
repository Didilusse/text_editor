//
// StatusBar.h - Status bar for text editor displaying metrics
//

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <SFML/Graphics.hpp>
#include "GapBuffer.h"
#include "UI.h"

struct StatusMetrics {
    size_t line;
    size_t column;
    size_t charCount;
    size_t wordCount;
    size_t lineCount;
    bool isModified;
    unsigned int fontSize;
};

class StatusBar {
private:
    sf::RectangleShape background;
    sf::RectangleShape border;
    
    sf::Text lineColText;
    sf::Text charCountText;
    sf::Text wordCountText;
    sf::Text fileSizeText;
    sf::Text modifiedIndicator;
    sf::Text fontSizeText;
    
    float width;
    
    // Helper functions
    StatusMetrics calculateMetrics(const GapBuffer& buffer, bool unsavedChanges, 
                                   int selectionAnchor, unsigned int fontSize);
    std::string formatFileSize(size_t bytes);
    size_t countWords(const std::string& text);

public:
    StatusBar(const sf::Font& font, float windowWidth);
    
    void update(const GapBuffer& buffer, bool unsavedChanges, 
                int selectionAnchor, unsigned int fontSize);
    void draw(sf::RenderWindow& window, const Theme& theme);
    void setWidth(float newWidth);
    
    static constexpr float HEIGHT = 30.0f;
};

#endif //STATUSBAR_H
