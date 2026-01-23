#include "TraderMenu.h"
#include <SFML/Graphics.hpp>
#include <iostream>

TraderMenu::TraderMenu()
{
    // Hardcoded simple layout sizes/colors
    m_background.setSize(sf::Vector2f(240.0f, 120.0f));
    m_background.setFillColor(sf::Color(30, 30, 30, 220));
    m_background.setOutlineColor(sf::Color::White);
    m_background.setOutlineThickness(2.0f);

    m_hireButton.setSize(sf::Vector2f(200.0f, 40.0f));
    m_hireButton.setFillColor(sf::Color(70,130,180));
    m_hireButton.setOutlineColor(sf::Color::White);
    m_hireButton.setOutlineThickness(1.0f);

    m_closeButton.setSize(sf::Vector2f(24.0f, 24.0f));
    m_closeButton.setFillColor(sf::Color(200, 50, 50));
    m_closeButton.setOutlineColor(sf::Color::White);
    m_closeButton.setOutlineThickness(1.0f);
}

void TraderMenu::openAt(const sf::Vector2f& worldPos)
{
    m_open = true;
    m_position = worldPos;
    // Hardcode positions relative to background
    m_background.setPosition(m_position - sf::Vector2f(m_background.getSize().x / 2.f, m_background.getSize().y + 10.f));
    m_hireButton.setPosition(m_background.getPosition() + sf::Vector2f(20.f, 45.f));
    m_closeButton.setPosition(m_background.getPosition() + sf::Vector2f(m_background.getSize().x - 28.f, 4.f));
}

void TraderMenu::close()
{
    m_open = false;
}

bool TraderMenu::handleClick(const sf::Vector2f& worldPos)
{
    if (!m_open) return false;

    // If click is outside the background, close the menu and indicate click handled (no hire)
    sf::FloatRect bgBounds = m_background.getGlobalBounds();
    if (!bgBounds.contains(worldPos))
    {
        m_open = false;
        return false;
    }

    // If click is inside the close button, close the menu
    sf::FloatRect closeBounds = m_closeButton.getGlobalBounds();
    if (closeBounds.contains(worldPos))
    {
        m_open = false;
        return false;
    }

    // If click is inside the hire button, report hire
    sf::FloatRect hireBounds = m_hireButton.getGlobalBounds();
    if (hireBounds.contains(worldPos))
    {
        // We return true to indicate the hire button was clicked
        return true;
    }

    // Click was inside the menu but not on a button: do nothing but consume the click
    return false;
}

void TraderMenu::draw(sf::RenderWindow& window)
{
    if (!m_open) return;

    window.draw(m_background);
    window.draw(m_hireButton);
    window.draw(m_closeButton);

    // simple visual markers for text (no fonts): draw small rectangles/lines to indicate title/button labels
    // Title bar rectangle
    sf::RectangleShape titleBar(sf::Vector2f(120.0f, 20.0f));
    titleBar.setFillColor(sf::Color(0,0,0,0));
    titleBar.setOutlineThickness(0);
    titleBar.setPosition(m_background.getPosition() + sf::Vector2f(10.f, 6.f));
    window.draw(titleBar);

    // Hire text placeholder: a small light rectangle to indicate where text would be
    sf::RectangleShape hireLabel(sf::Vector2f(140.0f, 10.0f));
    hireLabel.setFillColor(sf::Color(200,200,200,200));
    hireLabel.setPosition(m_hireButton.getPosition() + sf::Vector2f(20.f, 12.f));
    window.draw(hireLabel);
}
