//
// StatusBar.cpp - Implementation of status bar
//

#include "StatusBar.h"
#include <sstream>
#include <iomanip>

StatusBar::StatusBar(const sf::Font& font, float windowWidth)
    : width(windowWidth),
      lineColText(font),
      charCountText(font),
      wordCountText(font),
      fileSizeText(font),
      modifiedIndicator(font),
      fontSizeText(font) {
    
    // Background - positioned at bottom will be done in draw()
    background.setSize(sf::Vector2f(width, HEIGHT));
    
    // Border (top border) - positioned at bottom will be done in draw()
    border.setSize(sf::Vector2f(width, 1.0f));
    
    // Initialize all text objects - Y positions will be set relative to bottom
    lineColText.setCharacterSize(12);
    lineColText.setPosition(sf::Vector2f(10, 0)); // Y will be set dynamically
    
    charCountText.setCharacterSize(12);
    charCountText.setPosition(sf::Vector2f(150, 0)); // Y will be set dynamically
    
    wordCountText.setCharacterSize(12);
    wordCountText.setPosition(sf::Vector2f(280, 0)); // Y will be set dynamically
    
    fileSizeText.setCharacterSize(12);
    fileSizeText.setPosition(sf::Vector2f(400, 0)); // Y will be set dynamically
    
    fontSizeText.setCharacterSize(12);
    fontSizeText.setPosition(sf::Vector2f(width - 180, 0)); // Y will be set dynamically
    
    modifiedIndicator.setCharacterSize(14);
    modifiedIndicator.setPosition(sf::Vector2f(width - 30, 0)); // Y will be set dynamically
}

StatusMetrics StatusBar::calculateMetrics(const GapBuffer& buffer, bool unsavedChanges,
                                          int selectionAnchor, unsigned int fontSize) {
    std::string content = buffer.getString();
    size_t cursorPos = buffer.getGapStart();
    
    // Calculate line and column
    size_t line = 1, column = 0;
    for (size_t i = 0; i < cursorPos && i < content.length(); i++) {
        if (content[i] == '\n') {
            line++;
            column = 0;
        } else {
            column++;
        }
    }
    
    // Character count
    size_t charCount = content.length();
    
    // Word count
    size_t wordCount = countWords(content);
    
    // Line count
    size_t lineCount = 1;
    for (char c : content) {
        if (c == '\n') lineCount++;
    }
    
    return {line, column, charCount, wordCount, lineCount, unsavedChanges, fontSize};
}

std::string StatusBar::formatFileSize(size_t bytes) {
    if (bytes == 0) return "0 B";
    
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unitIndex < 3) {
        size /= 1024.0;
        unitIndex++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << size << " " << units[unitIndex];
    return oss.str();
}

size_t StatusBar::countWords(const std::string& text) {
    if (text.empty()) return 0;
    
    size_t wordCount = 0;
    bool inWord = false;
    
    for (char c : text) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            inWord = false;
        } else if (!inWord) {
            inWord = true;
            wordCount++;
        }
    }
    
    return wordCount;
}

void StatusBar::update(const GapBuffer& buffer, bool unsavedChanges,
                       int selectionAnchor, unsigned int fontSize) {
    StatusMetrics metrics = calculateMetrics(buffer, unsavedChanges, selectionAnchor, fontSize);
    
    // Update line and column
    std::ostringstream lineColStream;
    lineColStream << "Ln " << metrics.line << ", Col " << metrics.column;
    lineColText.setString(lineColStream.str());
    
    // Update character count
    std::ostringstream charStream;
    charStream << metrics.charCount << " chars";
    charCountText.setString(charStream.str());
    
    // Update word count
    std::ostringstream wordStream;
    wordStream << metrics.wordCount << " words";
    wordCountText.setString(wordStream.str());
    
    // Update file size
    fileSizeText.setString(formatFileSize(metrics.charCount));
    
    // Update font size
    std::ostringstream fontStream;
    fontStream << "Text Size: " << metrics.fontSize << "pt";
    fontSizeText.setString(fontStream.str());
    
    // Update modified indicator
    modifiedIndicator.setString(metrics.isModified ? "●" : "");
}

void StatusBar::draw(sf::RenderWindow& window, const Theme& theme) {
    // Get window dimensions to position at bottom
    float windowHeight = static_cast<float>(window.getSize().y);
    float bottomY = windowHeight - HEIGHT;
    
    // Position background at bottom
    background.setPosition(sf::Vector2f(0, bottomY));
    background.setFillColor(theme.headerBg());
    window.draw(background);
    
    // Position border at top of status bar (just below the text area)
    border.setPosition(sf::Vector2f(0, bottomY));
    border.setFillColor(theme.isDark ? sf::Color(80, 80, 80) : sf::Color(200, 200, 200));
    window.draw(border);
    
    // Set text colors
    lineColText.setFillColor(theme.textColor());
    charCountText.setFillColor(theme.textColor());
    wordCountText.setFillColor(theme.textColor());
    fileSizeText.setFillColor(theme.textColor());
    fontSizeText.setFillColor(theme.textColor());
    modifiedIndicator.setFillColor(theme.isDark ? sf::Color::Yellow : sf::Color(200, 100, 0));
    
    // Update Y positions for all text
    float textY = bottomY + 8.0f;
    lineColText.setPosition(sf::Vector2f(10, textY));
    charCountText.setPosition(sf::Vector2f(150, textY));
    wordCountText.setPosition(sf::Vector2f(280, textY));
    fileSizeText.setPosition(sf::Vector2f(400, textY));
    fontSizeText.setPosition(sf::Vector2f(width - 180, textY));
    modifiedIndicator.setPosition(sf::Vector2f(width - 30, textY - 2.0f));
    
    // Draw all text
    window.draw(lineColText);
    window.draw(charCountText);
    window.draw(wordCountText);
    window.draw(fileSizeText);
    window.draw(fontSizeText);
    window.draw(modifiedIndicator);
}

void StatusBar::setWidth(float newWidth) {
    width = newWidth;
    background.setSize(sf::Vector2f(width, HEIGHT));
    border.setSize(sf::Vector2f(width, 1.0f));
    fontSizeText.setPosition(sf::Vector2f(width - 100, 8));
    modifiedIndicator.setPosition(sf::Vector2f(width - 30, 6));
}
