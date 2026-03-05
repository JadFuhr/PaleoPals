#pragma once
#ifndef TRADERMENU_H
#define TRADERMENU_H

#include <SFML/Graphics.hpp>

enum class HireAction
{
    None,
    HirePaleontologist,
    HireResearcher
};

// A larger tabbed UI for the trader interaction
class TraderMenu
{
public:
    TraderMenu();

    // position the menu at a point in world coordinates (menu will be centered on screen in screen coords)
    void openAt(const sf::Vector2f& worldPos);
    void close();
    bool isOpen() const { return m_open; }

    // handle a click in screen coordinates, returns the action taken
    HireAction handleClick(const sf::Vector2f& screenPos, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);

private:
    // Helper to update all button positions (used by both draw and click detection)
    void updateButtonPositions(const sf::RenderWindow& window);

    // Helper to check if a point is inside a rectangle shape
    bool containsPoint(const sf::RectangleShape& shape, const sf::Vector2f& point) const;
    enum class ActiveTab
    {
        Hiring,
        Upgrades
    };

    // State
    bool m_open = false;
    sf::Vector2f m_worldPosition; // original trader position (not used for UI layout)
    ActiveTab m_activeTab = ActiveTab::Hiring;

    // Background and main containers
    sf::RectangleShape m_background; // large semi-transparent background
    sf::RectangleShape m_overlay; // full-screen semi-transparent overlay

    // Tab buttons
    sf::RectangleShape m_hiringTabButton;
    sf::RectangleShape m_upgradesTabButton;
    sf::RectangleShape m_hiringTabUnderline;
    sf::RectangleShape m_upgradesTabUnderline;

    // Hire buttons (in Hiring tab)
    sf::RectangleShape m_hirePaleontologistButton;
    sf::RectangleShape m_hireResearcherButton;

    // Upgrade buttons (in Upgrades tab) - placeholder for now
    sf::RectangleShape m_upgrade1Button;
    sf::RectangleShape m_upgrade2Button;

    // Close button
    sf::RectangleShape m_closeButton;


};

#endif
