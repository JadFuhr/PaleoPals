#pragma once
#ifndef TRADERMENU_H
#define TRADERMENU_H

#include <SFML/Graphics.hpp>

enum class HireAction
{
    None,
    HirePaleontologist,
    HireResearcher,
	Upgrade1,
	Upgrade2,
    Upgrade3,
    Upgrade4

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

    int upgrade1Level = 0;  // pickaxe radius 
    int upgrade2Level = 0;  // dmg to tiles
    int upgrade3Level = 0; // pickup radius
    int upgrade4Level = 0; // jump height

    void markUpgrade1Purchased() { m_upgrade1Purchased = true; }
    void markUpgrade2Purchased() { m_upgrade2Purchased = true; }

    int getUpgrade1Cost() const { return 100 + upgrade1Level * 50; }
    int getUpgrade2Cost() const { return 200 + upgrade2Level * 75; }
    int getUpgrade3Cost() const { return 150 + upgrade3Level * 60; }
    int getUpgrade4Cost() const { return 175 + upgrade4Level * 70; }

private:
    // Helper to update all button positions
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
    sf::RectangleShape m_upgrade3Button;
    sf::RectangleShape m_upgrade4Button;

    // Close button
    sf::RectangleShape m_closeButton;

    sf::Font m_font;
    sf::Text m_hirePaleoText{ m_font };
    sf::Text m_hireResearcherText{ m_font };
    sf::Text m_upgrade1Text{ m_font };
    sf::Text m_upgrade2Text{ m_font };
    sf::Text m_upgrade3Text{ m_font };
    sf::Text m_upgrade4Text{ m_font };
    sf::Text m_hiringTabText{ m_font };
    sf::Text m_upgradesTabText{ m_font };

    // upgrade state
    bool m_upgrade1Purchased = false;
    bool m_upgrade2Purchased = false;
   

};

#endif
