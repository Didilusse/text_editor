#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

struct Theme {
    bool isDark = true;

    // Background
    sf::Color windowBg()   const { return isDark ? sf::Color(0,0,0)       : sf::Color(255,255,255); }
    sf::Color headerBg()   const { return isDark ? sf::Color(30,30,30)    : sf::Color(220,220,220); }

    // Buttons / panels
    sf::Color btnNormal()  const { return isDark ? sf::Color(50,50,50)    : sf::Color(180,180,180); }
    sf::Color btnHover()   const { return isDark ? sf::Color(70,70,70)    : sf::Color(160,160,160); }
    sf::Color panelBg()    const { return isDark ? sf::Color(35,35,35)    : sf::Color(230,230,230); }
    sf::Color panelOut()   const { return isDark ? sf::Color(80,80,80)    : sf::Color(150,150,150); }
    sf::Color rowHighlight()const{ return isDark ? sf::Color(70,70,70)    : sf::Color(190,190,190); }

    // Text
    sf::Color textColor()  const { return isDark ? sf::Color::White        : sf::Color::Black; }
    sf::Color dimText()    const { return isDark ? sf::Color(160,160,160)  : sf::Color(80,80,80); }

    // Cursor & selection
    sf::Color cursorColor()const { return isDark ? sf::Color::White        : sf::Color::Black; }
    sf::Color selectionColor()const{ return isDark ? sf::Color(100,149,237,150) : sf::Color(100,149,237,120); }

    // Scrollbar
    sf::Color scrollbarTrack() const { return isDark ? sf::Color(30,30,30)  : sf::Color(210,210,210); }
    sf::Color scrollbarThumb() const { return isDark ? sf::Color(80,80,80)  : sf::Color(140,140,140); }
};

struct Button {
    sf::RectangleShape shape;
    sf::Text text;

    Button(const sf::Font& font) : text(font) {}
};

struct DropdownItem {
    std::string label;
    std::string shortcut;   // e.g. "Ctrl+S", shown right-aligned
};

class DropdownMenu {
public:
    DropdownMenu(const sf::Font& font,
                 const std::string& title,
                 sf::Vector2f pos,
                 const std::vector<DropdownItem>& items);

    void setPosition(sf::Vector2f pos);

    // Returns index of item clicked (-1 if none), and toggles open/close
    int handleClick(sf::Vector2f mousePos);

    // Call every frame with current mouse position to update hover state
    void handleHover(sf::Vector2f mousePos);

    // Close the dropdown (e.g. when clicking elsewhere)
    void close();

    bool isOpen() const { return open; }

    // Returns true if mousePos is within the menu button OR the open dropdown panel
    bool containsPoint(sf::Vector2f mousePos) const;

    void applyTheme(const Theme& theme);
    void draw(sf::RenderWindow& window) const;

private:
    sf::Font font;

    // The clickable "File" button in the header bar
    sf::RectangleShape menuBtn;
    sf::Text menuBtnText;

    // The dropdown panel
    sf::RectangleShape panel;
    sf::RectangleShape separator;

    struct ItemRow {
        sf::RectangleShape highlight;
        sf::Text labelText;
        sf::Text shortcutText;
        ItemRow(const sf::Font& font)
        : labelText(font), shortcutText(font) {}
    };

    std::vector<ItemRow> rows;
    std::vector<DropdownItem> items;

    bool open = false;
    int hoveredIndex = -1;

    static constexpr float BTN_W     = 60.f;
    static constexpr float BTN_H     = 30.f;
    static constexpr float ITEM_H    = 32.f;
    static constexpr float PANEL_W   = 200.f;
    static constexpr float PADDING_X = 12.f;

    sf::Vector2f position;
    void buildRows();
};

Button createButton(const sf::Font& font, const std::string& label, sf::Vector2f pos);
void updateCursorSize(sf::RectangleShape& cursor, const sf::Font& font, unsigned int charSize);