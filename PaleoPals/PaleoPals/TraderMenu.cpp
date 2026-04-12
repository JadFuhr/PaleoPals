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

    m_upgrade3Button.setSize(sf::Vector2f(250.f, 60.f));
    m_upgrade3Button.setFillColor(sf::Color(180, 150, 100));
    m_upgrade3Button.setOutlineColor(sf::Color(200, 200, 200));
    m_upgrade3Button.setOutlineThickness(2.f);

    m_upgrade4Button.setSize(sf::Vector2f(250.f, 60.f));
    m_upgrade4Button.setFillColor(sf::Color(180, 150, 100));
    m_upgrade4Button.setOutlineColor(sf::Color(200, 200, 200));
    m_upgrade4Button.setOutlineThickness(2.f);

    // Close button (X in top right)
    m_closeButton.setSize(sf::Vector2f(30.0f, 30.0f));
    m_closeButton.setFillColor(sf::Color(200, 80, 80));
    m_closeButton.setOutlineColor(sf::Color(255, 255, 255));
    m_closeButton.setOutlineThickness(2.0f);

    if (!m_font.openFromFile("ASSETS/FONTS/Jersey20-Regular.ttf"))
    {
        std::cout << "TraderMenu: failed to load font\n";
    }

    auto setupText = [&](sf::Text& text, const std::string& str)
        {
            text.setFont(m_font);
            text.setString(str);
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::White);
        };

    setupText(m_hirePaleoText, "Hire Paleontologist");
    setupText(m_hireResearcherText, "Hire Researcher");
    setupText(m_upgrade1Text, "Upgrade 1");
    setupText(m_upgrade2Text, "Upgrade 2");
    setupText(m_upgrade3Text, "Pickup Radius");
    setupText(m_upgrade4Text, "Jump Height");
    setupText(m_hiringTabText, "Hiring");
    setupText(m_upgradesTabText, "Upgrades");


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
    m_upgrade3Button.setPosition(sf::Vector2f(bgX + 50.f, bgY + 280.f));
    m_upgrade4Button.setPosition(sf::Vector2f(bgX + 50.f, bgY + 370.f));

    m_hirePaleoText.setPosition(m_hirePaleontologistButton.getPosition() + sf::Vector2f(20.f, 15.f));
    m_hireResearcherText.setPosition(m_hireResearcherButton.getPosition() + sf::Vector2f(20.f, 15.f));

    m_upgrade1Text.setPosition(m_upgrade1Button.getPosition() + sf::Vector2f(20.f, 15.f));
    m_upgrade2Text.setPosition(m_upgrade2Button.getPosition() + sf::Vector2f(20.f, 15.f));
    m_upgrade3Text.setPosition(m_upgrade3Button.getPosition() + sf::Vector2f(20.f, 15.f));
    m_upgrade4Text.setPosition(m_upgrade4Button.getPosition() + sf::Vector2f(20.f, 15.f));


    m_hiringTabText.setPosition(m_hiringTabButton.getPosition() + sf::Vector2f(20.f, 10.f));
    m_upgradesTabText.setPosition(m_upgradesTabButton.getPosition() + sf::Vector2f(20.f, 10.f));

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
        // Upgrades 
        if (containsPoint(m_upgrade1Button, screenPos) && !m_upgrade1Purchased)
        {
            return HireAction::Upgrade1;
        }
        if (containsPoint(m_upgrade2Button, screenPos) && !m_upgrade2Purchased)
        {
            return HireAction::Upgrade2;
        }
        if (containsPoint(m_upgrade3Button, screenPos))
        {
            return HireAction::Upgrade3;
        }
        if (containsPoint(m_upgrade4Button, screenPos))
        {
            return HireAction::Upgrade4;
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

    m_upgrade1Text.setString("Radius +" + std::to_string(upgrade1Level) +" ($" + std::to_string(getUpgrade1Cost()) + ")");

    m_upgrade2Text.setString("Damage +" + std::to_string(upgrade2Level) +" ($" + std::to_string(getUpgrade2Cost()) + ")");

    m_upgrade3Text.setString("Pickup +" + std::to_string(upgrade3Level) + " ($" + std::to_string(getUpgrade3Cost()) + ")");

    m_upgrade4Text.setString("Jump +" + std::to_string(upgrade4Level) + " ($" + std::to_string(getUpgrade4Cost()) + ")");

    if (!m_open) return;

    // Update button positions
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

    window.draw(m_hiringTabText);
    window.draw(m_upgradesTabText);

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

        window.draw(m_hirePaleoText);
        window.draw(m_hireResearcherText);


    }
    else if (m_activeTab == ActiveTab::Upgrades)
    {
        m_upgrade1Button.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 100.0f));
        m_upgrade2Button.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 190.0f));


        window.draw(m_upgrade1Button);
        window.draw(m_upgrade2Button);
        window.draw(m_upgrade3Button);
        window.draw(m_upgrade4Button);

        window.draw(m_upgrade1Text);
        window.draw(m_upgrade2Text);
        window.draw(m_upgrade3Text);
        window.draw(m_upgrade4Text);


    }
}
