#include "TraderMenu.h"
#include <SFML/Graphics.hpp>
#include <iostream>

TraderMenu::TraderMenu()
{
    // Full-screen semi-transparent overlay
    m_overlay.setSize(sf::Vector2f(10000.0f, 10000.0f)); // Large enough to cover screen
    m_overlay.setFillColor(sf::Color(0, 0, 0, 100));
    m_overlay.setPosition(sf::Vector2f(0.0f, 0.0f));

    // Main menu background - centered, large
    m_background.setSize(sf::Vector2f(600.0f, 450.0f));
    m_background.setFillColor(sf::Color(40, 40, 50, 240));
    m_background.setOutlineColor(sf::Color(200, 200, 200));
    m_background.setOutlineThickness(3.0f);

    // Tab buttons
    m_hiringTabButton.setSize(sf::Vector2f(150.0f, 40.0f));
    m_hiringTabButton.setFillColor(sf::Color(70, 130, 180));
    m_hiringTabButton.setOutlineColor(sf::Color(200, 200, 200));
    m_hiringTabButton.setOutlineThickness(2.0f);

    m_upgradesTabButton.setSize(sf::Vector2f(150.0f, 40.0f));
    m_upgradesTabButton.setFillColor(sf::Color(60, 60, 70));
    m_upgradesTabButton.setOutlineColor(sf::Color(100, 100, 100));
    m_upgradesTabButton.setOutlineThickness(2.0f);

    // Tab underlines (active indicator)
    m_hiringTabUnderline.setSize(sf::Vector2f(150.0f, 4.0f));
    m_hiringTabUnderline.setFillColor(sf::Color(100, 200, 255));

    m_upgradesTabUnderline.setSize(sf::Vector2f(150.0f, 4.0f));
    m_upgradesTabUnderline.setFillColor(sf::Color(100, 200, 255));

    // Hire buttons
    m_hirePaleontologistButton.setSize(sf::Vector2f(250.0f, 60.0f));
    m_hirePaleontologistButton.setFillColor(sf::Color(100, 180, 100));
    m_hirePaleontologistButton.setOutlineColor(sf::Color(200, 200, 200));
    m_hirePaleontologistButton.setOutlineThickness(2.0f);

    m_hireResearcherButton.setSize(sf::Vector2f(250.0f, 60.0f));
    m_hireResearcherButton.setFillColor(sf::Color(150, 120, 180));
    m_hireResearcherButton.setOutlineColor(sf::Color(200, 200, 200));
    m_hireResearcherButton.setOutlineThickness(2.0f);

    // Placeholder upgrade buttons
    m_upgrade1Button.setSize(sf::Vector2f(250.0f, 60.0f));
    m_upgrade1Button.setFillColor(sf::Color(180, 150, 100));
    m_upgrade1Button.setOutlineColor(sf::Color(200, 200, 200));
    m_upgrade1Button.setOutlineThickness(2.0f);

    m_upgrade2Button.setSize(sf::Vector2f(250.0f, 60.0f));
    m_upgrade2Button.setFillColor(sf::Color(180, 150, 100));
    m_upgrade2Button.setOutlineColor(sf::Color(200, 200, 200));
    m_upgrade2Button.setOutlineThickness(2.0f);

    // Close button (X in top right)
    m_closeButton.setSize(sf::Vector2f(30.0f, 30.0f));
    m_closeButton.setFillColor(sf::Color(200, 80, 80));
    m_closeButton.setOutlineColor(sf::Color(255, 255, 255));
    m_closeButton.setOutlineThickness(2.0f);
}

void TraderMenu::openAt(const sf::Vector2f& worldPos)
{
    m_open = true;
    m_worldPosition = worldPos;
    m_activeTab = ActiveTab::Hiring; // Default to hiring tab

    // Center menu on screen (will position in draw() relative to window)
}

void TraderMenu::close()
{
    m_open = false;
}

bool TraderMenu::containsPoint(const sf::RectangleShape& shape, const sf::Vector2f& point) const
{
    return shape.getGlobalBounds().contains(point);
}

void TraderMenu::updateButtonPositions(const sf::RenderWindow& window)
{
    // Get window size to center menu
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f windowCenter(windowSize.x / 2.0f, windowSize.y / 2.0f);

    // Position background in center of screen (in screen coordinates)
    m_background.setPosition(windowCenter - sf::Vector2f(m_background.getSize().x / 2.0f, m_background.getSize().y / 2.0f));

    sf::Vector2f bgPos = m_background.getPosition();
    float bgX = bgPos.x;
    float bgY = bgPos.y;
    float bgWidth = m_background.getSize().x;

    // Position tab buttons
    m_hiringTabButton.setPosition(sf::Vector2f(bgX + 20.0f, bgY + 15.0f));
    m_upgradesTabButton.setPosition(sf::Vector2f(bgX + 190.0f, bgY + 15.0f));

    // Position tab underlines
    m_hiringTabUnderline.setPosition(m_hiringTabButton.getPosition() + sf::Vector2f(0.0f, m_hiringTabButton.getSize().y));
    m_upgradesTabUnderline.setPosition(m_upgradesTabButton.getPosition() + sf::Vector2f(0.0f, m_upgradesTabButton.getSize().y));

    // Position close button (top right)
    m_closeButton.setPosition(sf::Vector2f(bgX + bgWidth - 40.0f, bgY + 10.0f));

    // Position content buttons based on active tab
    m_hirePaleontologistButton.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 100.0f));
    m_hireResearcherButton.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 190.0f));

    m_upgrade1Button.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 100.0f));
    m_upgrade2Button.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 190.0f));
}

HireAction TraderMenu::handleClick(const sf::Vector2f& screenPos, const sf::RenderWindow& window)
{
    if (!m_open) return HireAction::None;

    // Update button positions before checking clicks
    updateButtonPositions(window);

    // Check close button first
    if (containsPoint(m_closeButton, screenPos))
    {
        m_open = false;
        return HireAction::None;
    }

    // Check tab buttons
    if (containsPoint(m_hiringTabButton, screenPos))
    {
        m_activeTab = ActiveTab::Hiring;
        return HireAction::None;
    }

    if (containsPoint(m_upgradesTabButton, screenPos))
    {
        m_activeTab = ActiveTab::Upgrades;
        return HireAction::None;
    }

    // Check content buttons based on active tab
    if (m_activeTab == ActiveTab::Hiring)
    {
        if (containsPoint(m_hirePaleontologistButton, screenPos))
        {
            return HireAction::HirePaleontologist;
        }
        if (containsPoint(m_hireResearcherButton, screenPos))
        {
            return HireAction::HireResearcher;
        }
    }
    else if (m_activeTab == ActiveTab::Upgrades)
    {
        // Upgrades not implemented yet, just return None
        if (containsPoint(m_upgrade1Button, screenPos) || containsPoint(m_upgrade2Button, screenPos))
        {
            return HireAction::None;
        }
    }

    // Click outside menu = close it
    if (!containsPoint(m_background, screenPos))
    {
        m_open = false;
        return HireAction::None;
    }

    return HireAction::None;
}

void TraderMenu::draw(sf::RenderWindow& window)
{
    if (!m_open) return;

    // Update button positions (same positions used for both draw and click detection)
    updateButtonPositions(window);

    // Draw overlay first (darkens everything behind)
    window.draw(m_overlay);

    // Draw main background
    window.draw(m_background);

    sf::Vector2f bgPos = m_background.getPosition();
    float bgX = bgPos.x;
    float bgY = bgPos.y;

    // Draw tab buttons
    window.draw(m_hiringTabButton);
    window.draw(m_upgradesTabButton);

    // Position and draw tab text labels
    // Hiring tab label
    sf::RectangleShape hiringLabel(sf::Vector2f(60.0f, 12.0f));
    hiringLabel.setFillColor(sf::Color(255, 255, 255));
    hiringLabel.setPosition(sf::Vector2f(bgX + 45.0f, bgY + 21.0f));
    window.draw(hiringLabel);

    // Upgrades tab label
    sf::RectangleShape upgradesLabel(sf::Vector2f(75.0f, 12.0f));
    upgradesLabel.setFillColor(sf::Color(255, 255, 255));
    upgradesLabel.setPosition(sf::Vector2f(bgX + 200.0f, bgY + 21.0f));
    window.draw(upgradesLabel);

    // Draw active tab underline
    if (m_activeTab == ActiveTab::Hiring)
    {
        window.draw(m_hiringTabUnderline);
    }
    else
    {
        window.draw(m_upgradesTabUnderline);
    }

    // Draw close button (top right)
    window.draw(m_closeButton);

    // Draw X on close button - using two diagonal lines represented as thin rectangles
    sf::RectangleShape closeX1(sf::Vector2f(20.0f, 3.0f));
    closeX1.setFillColor(sf::Color::White);
    closeX1.setRotation(sf::degrees(45.0f));
    closeX1.setPosition(sf::Vector2f(bgX + m_background.getSize().x - 30.0f, bgY + 18.0f));
    window.draw(closeX1);

    sf::RectangleShape closeX2(sf::Vector2f(20.0f, 3.0f));
    closeX2.setFillColor(sf::Color::White);
    closeX2.setRotation(sf::degrees(-45.0f));
    closeX2.setPosition(sf::Vector2f(bgX + m_background.getSize().x - 30.0f, bgY + 32.0f));
    window.draw(closeX2);

    // Draw content based on active tab
    if (m_activeTab == ActiveTab::Hiring)
    {
        window.draw(m_hirePaleontologistButton);
        window.draw(m_hireResearcherButton);

        // Label: "Hire Paleontologist"
        sf::RectangleShape paleLabel1(sf::Vector2f(35.0f, 10.0f));
        paleLabel1.setFillColor(sf::Color(255, 255, 255));
        paleLabel1.setPosition(sf::Vector2f(bgX + 63.0f, bgY + 107.0f));
        window.draw(paleLabel1);

        sf::RectangleShape paleLabel2(sf::Vector2f(100.0f, 10.0f));
        paleLabel2.setFillColor(sf::Color(255, 255, 255));
        paleLabel2.setPosition(sf::Vector2f(bgX + 100.0f, bgY + 107.0f));
        window.draw(paleLabel2);

        // Label: "Hire Researcher"
        sf::RectangleShape reseLabel1(sf::Vector2f(35.0f, 10.0f));
        reseLabel1.setFillColor(sf::Color(255, 255, 255));
        reseLabel1.setPosition(sf::Vector2f(bgX + 63.0f, bgY + 197.0f));
        window.draw(reseLabel1);

        sf::RectangleShape reseLabel2(sf::Vector2f(75.0f, 10.0f));
        reseLabel2.setFillColor(sf::Color(255, 255, 255));
        reseLabel2.setPosition(sf::Vector2f(bgX + 100.0f, bgY + 197.0f));
        window.draw(reseLabel2);
    }
    else if (m_activeTab == ActiveTab::Upgrades)
    {
        // Upgrades tab content (placeholder)
        m_upgrade1Button.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 100.0f));
        m_upgrade2Button.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 190.0f));

        window.draw(m_upgrade1Button);
        window.draw(m_upgrade2Button);

        // Placeholder labels for upgrades
        sf::RectangleShape upgradeLabel1(sf::Vector2f(70.0f, 10.0f));
        upgradeLabel1.setFillColor(sf::Color(255, 255, 255));
        upgradeLabel1.setPosition(sf::Vector2f(bgX + 100.0f, bgY + 107.0f));
        window.draw(upgradeLabel1);

        sf::RectangleShape upgradeLabel2(sf::Vector2f(70.0f, 10.0f));
        upgradeLabel2.setFillColor(sf::Color(255, 255, 255));
        upgradeLabel2.setPosition(sf::Vector2f(bgX + 100.0f, bgY + 197.0f));
        window.draw(upgradeLabel2);
    }
}
