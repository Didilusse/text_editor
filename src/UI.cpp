#include "UI.h"

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

void updateCursorSize(sf::RectangleShape& cursor, const sf::Font& font, unsigned int charSize) {
    float lineHeight = font.getLineSpacing(charSize);
    cursor.setSize({2.f, lineHeight});
}

DropdownMenu::DropdownMenu(const sf::Font& font,
                           const std::string& title,
                           sf::Vector2f pos,
                           const std::vector<DropdownItem>& items)
    : font(font), menuBtnText(font), items(items), position(pos) {
    // ── Menu button (header bar) ──────────────────
    menuBtn.setSize(sf::Vector2f(BTN_W, BTN_H));
    menuBtn.setFillColor(sf::Color(50, 50, 50));

    menuBtnText = sf::Text(font);
    menuBtnText.setString(title);
    menuBtnText.setCharacterSize(18);
    menuBtnText.setFillColor(sf::Color::White);

    // ── Dropdown panel ────────────────────────────
    float panelH = static_cast<float>(items.size()) * ITEM_H + 8.f;
    panel.setSize(sf::Vector2f(PANEL_W, panelH));
    panel.setFillColor(sf::Color(35, 35, 35));
    panel.setOutlineColor(sf::Color(80, 80, 80));
    panel.setOutlineThickness(1.f);

    buildRows();
    setPosition(pos);
}

void DropdownMenu::buildRows() {
    rows.clear();
    for (size_t i = 0; i < items.size(); ++i) {
        ItemRow row(font);

        // Full-width highlight bar (shown on hover)
        row.highlight.setSize(sf::Vector2f(PANEL_W - 2.f, ITEM_H - 2.f));
        row.highlight.setFillColor(sf::Color(70, 70, 70));

        // Label on the left
        row.labelText = sf::Text(font);
        row.labelText.setString(items[i].label);
        row.labelText.setCharacterSize(16);
        row.labelText.setFillColor(sf::Color::White);

        // Shortcut on the right (dimmer)
        row.shortcutText = sf::Text(font);
        row.shortcutText.setString(items[i].shortcut);
        row.shortcutText.setCharacterSize(14);
        row.shortcutText.setFillColor(sf::Color(160, 160, 160));

        rows.push_back(std::move(row));
    }
}

void DropdownMenu::setPosition(sf::Vector2f pos) {
    position = pos;

    // Header button
    menuBtn.setPosition(pos);

    // Centre text vertically in the button
    sf::FloatRect tb = menuBtnText.getLocalBounds();
    menuBtnText.setPosition(sf::Vector2f(
        pos.x + (BTN_W - tb.size.x) / 2.f - tb.position.x,
        pos.y + (BTN_H - tb.size.y) / 2.f - tb.position.y
    ));

    // Panel appears just below the button
    float panelX = pos.x;
    float panelY = pos.y + BTN_H;
    panel.setPosition(sf::Vector2f(panelX, panelY));

    // Position each row
    for (size_t i = 0; i < rows.size(); ++i) {
        float rowY = panelY + 4.f + static_cast<float>(i) * ITEM_H;

        rows[i].highlight.setPosition(sf::Vector2f(panelX + 1.f, rowY + 1.f));

        // Label: left-aligned with padding
        sf::FloatRect lb = rows[i].labelText.getLocalBounds();
        rows[i].labelText.setPosition(sf::Vector2f(
            panelX + PADDING_X,
            rowY + (ITEM_H - lb.size.y) / 2.f - lb.position.y
        ));

        // Shortcut: right-aligned with padding
        sf::FloatRect sb = rows[i].shortcutText.getLocalBounds();
        rows[i].shortcutText.setPosition(sf::Vector2f(
            panelX + PANEL_W - sb.size.x - PADDING_X - sb.position.x,
            rowY + (ITEM_H - sb.size.y) / 2.f - sb.position.y
        ));
    }
}

bool DropdownMenu::containsPoint(sf::Vector2f p) const {
    if (menuBtn.getGlobalBounds().contains(p)) return true;
    if (open && panel.getGlobalBounds().contains(p)) return true;
    return false;
}

void DropdownMenu::close() {
    open = false;
    hoveredIndex = -1;
}

void DropdownMenu::handleHover(sf::Vector2f mousePos) {
    if (!open) {
        // Subtle hover tint on the menu button
        if (menuBtn.getGlobalBounds().contains(mousePos)) {
            menuBtn.setFillColor(sf::Color(70, 70, 70));
        } else {
            menuBtn.setFillColor(sf::Color(50, 50, 50));
        }
        return;
    }

    hoveredIndex = -1;
    float panelY = position.y + BTN_H + 4.f;
    float panelX = position.x;

    for (size_t i = 0; i < items.size(); ++i) {
        float rowY = panelY + static_cast<float>(i) * ITEM_H;
        sf::FloatRect rowBounds(sf::Vector2f(panelX, rowY),
                                sf::Vector2f(PANEL_W, ITEM_H));
        if (rowBounds.contains(mousePos)) {
            hoveredIndex = static_cast<int>(i);
            break;
        }
    }
}

int DropdownMenu::handleClick(sf::Vector2f mousePos) {
    // Click on the "File" button: toggle open/closed
    if (menuBtn.getGlobalBounds().contains(mousePos)) {
        open = !open;
        hoveredIndex = -1;
        return -1;
    }

    // Click inside the panel
    if (open && panel.getGlobalBounds().contains(mousePos)) {
        float panelY = position.y + BTN_H + 4.f;
        float panelX = position.x;

        for (size_t i = 0; i < items.size(); ++i) {
            float rowY = panelY + static_cast<float>(i) * ITEM_H;
            sf::FloatRect rowBounds(sf::Vector2f(panelX, rowY),
                                    sf::Vector2f(PANEL_W, ITEM_H));
            if (rowBounds.contains(mousePos)) {
                open = false;
                hoveredIndex = -1;
                return static_cast<int>(i);
            }
        }
    }

    return -1;
}

void DropdownMenu::applyTheme(const Theme& theme) {
    menuBtn.setFillColor(theme.btnNormal());
    menuBtnText.setFillColor(theme.textColor());
    panel.setFillColor(theme.panelBg());
    panel.setOutlineColor(theme.panelOut());
    for (size_t i = 0; i < rows.size(); ++i) {
        rows[i].highlight.setFillColor(theme.rowHighlight());
        rows[i].labelText.setFillColor(theme.textColor());
        rows[i].shortcutText.setFillColor(theme.dimText());
    }
}

void DropdownMenu::draw(sf::RenderWindow& window) const {
    window.draw(menuBtn);
    window.draw(menuBtnText);

    if (!open) return;

    window.draw(panel);

    for (size_t i = 0; i < rows.size(); ++i) {
        if (static_cast<int>(i) == hoveredIndex) {
            window.draw(rows[i].highlight);
        }
        window.draw(rows[i].labelText);
        if (!items[i].shortcut.empty()) {
            window.draw(rows[i].shortcutText);
        }
    }
}