#pragma once
#ifndef TRADERMENU_H
#define TRADERMENU_H

#include <SFML/Graphics.hpp>

// A very small UI for the trader interaction. Presents a simple "Hire Paleontologist" and a close button
class TraderMenu
{
public:
    TraderMenu();

    // position the menu near a point in world coordinates
    void openAt(const sf::Vector2f& worldPos);
    void close();
    bool isOpen() const { return m_open; }

    // handle a click, returns true if the "Hire" button was clicked
    // clicking close will close the menu and return false
    bool handleClick(const sf::Vector2f& worldPos);
    void draw(sf::RenderWindow& window);

private:
    bool m_open = false;
    sf::RectangleShape m_background;
    sf::RectangleShape m_hireButton;
    sf::RectangleShape m_closeButton;
    sf::Vector2f m_position;
};

#endif
