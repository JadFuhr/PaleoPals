#include "TraderMenu.h"
#include <SFML/Graphics.hpp>
#include <iostream>

TraderMenu::TraderMenu()
{
    m_overlay.setSize(sf::Vector2f(10000.0f, 10000.0f)); 
    m_overlay.setFillColor(sf::Color(0, 0, 0, 100));
    m_overlay.setPosition(sf::Vector2f(0.0f, 0.0f));

    m_background.setSize(sf::Vector2f(600.0f, 550.0f));
    m_background.setFillColor(sf::Color(40, 40, 50, 240));
    m_background.setOutlineColor(sf::Color(200, 200, 200));
    m_background.setOutlineThickness(3.0f);

    m_hiringTabButton.setSize(sf::Vector2f(150.0f, 40.0f));
    m_hiringTabButton.setFillColor(sf::Color(70, 130, 180));
    m_hiringTabButton.setOutlineColor(sf::Color(200, 200, 200));
    m_hiringTabButton.setOutlineThickness(2.0f);

    m_upgradesTabButton.setSize(sf::Vector2f(150.0f, 40.0f));
    m_upgradesTabButton.setFillColor(sf::Color(60, 60, 70));
    m_upgradesTabButton.setOutlineColor(sf::Color(100, 100, 100));
    m_upgradesTabButton.setOutlineThickness(2.0f);

    m_hiringTabUnderline.setSize(sf::Vector2f(150.0f, 4.0f));
    m_hiringTabUnderline.setFillColor(sf::Color(100, 200, 255));

    m_upgradesTabUnderline.setSize(sf::Vector2f(150.0f, 4.0f));
    m_upgradesTabUnderline.setFillColor(sf::Color(100, 200, 255));

    m_hirePaleontologistButton.setSize(sf::Vector2f(250.0f, 60.0f));
    m_hirePaleontologistButton.setFillColor(sf::Color(100, 180, 100));
    m_hirePaleontologistButton.setOutlineColor(sf::Color(200, 200, 200));
    m_hirePaleontologistButton.setOutlineThickness(2.0f);

    m_upgrade1_RayLengthButton.setSize(sf::Vector2f(250.0f, 60.0f));
    m_upgrade1_RayLengthButton.setFillColor(sf::Color(180, 150, 100));
    m_upgrade1_RayLengthButton.setOutlineColor(sf::Color(200, 200, 200));
    m_upgrade1_RayLengthButton.setOutlineThickness(2.0f);

    m_upgrade2_RayDamageButton.setSize(sf::Vector2f(250.0f, 60.0f));
    m_upgrade2_RayDamageButton.setFillColor(sf::Color(180, 150, 100));
    m_upgrade2_RayDamageButton.setOutlineColor(sf::Color(200, 200, 200));
    m_upgrade2_RayDamageButton.setOutlineThickness(2.0f);

    m_upgrade3_PickupRadiusButton.setSize(sf::Vector2f(250.f, 60.f));
    m_upgrade3_PickupRadiusButton.setFillColor(sf::Color(180, 150, 100));
    m_upgrade3_PickupRadiusButton.setOutlineColor(sf::Color(200, 200, 200));
    m_upgrade3_PickupRadiusButton.setOutlineThickness(2.f);

    m_upgrade4_JumpHeightButton.setSize(sf::Vector2f(250.f, 60.f));
    m_upgrade4_JumpHeightButton.setFillColor(sf::Color(180, 150, 100));
    m_upgrade4_JumpHeightButton.setOutlineColor(sf::Color(200, 200, 200));
    m_upgrade4_JumpHeightButton.setOutlineThickness(2.f);

    m_upgradeNPCButton.setSize(sf::Vector2f(250.f, 60.f));
    m_upgradeNPCButton.setFillColor(sf::Color(180, 150, 100));
    m_upgradeNPCButton.setOutlineColor(sf::Color(200, 200, 200));
    m_upgradeNPCButton.setOutlineThickness(2.f);

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
    setupText(m_upgrade1_RayLengthText, "Upgrade 1");
    setupText(m_upgrade2_RayDamageText, "Upgrade 2");
    setupText(m_upgrade3_PickupRadiusText, "Pickup Radius");
    setupText(m_upgrade4_JumpHeightText, "Jump Height");
    setupText(m_hiringTabText, "Hiring");
    setupText(m_upgradesTabText, "Upgrades");
	setupText(m_upgradeNPCText, "Upgrade NPC");


}

void TraderMenu::openAt(const sf::Vector2f& worldPos)
{
    m_open = true;
    m_worldPosition = worldPos;
    m_activeTab = ActiveTab::Hiring; 

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
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f windowCenter(windowSize.x / 2.0f, windowSize.y / 2.0f);

    m_background.setPosition(windowCenter - sf::Vector2f(m_background.getSize().x / 2.0f, m_background.getSize().y / 2.0f));

    sf::Vector2f bgPos = m_background.getPosition();
    float bgX = bgPos.x;
    float bgY = bgPos.y;
    float bgWidth = m_background.getSize().x;

    m_hiringTabButton.setPosition(sf::Vector2f(bgX + 20.0f, bgY + 15.0f));
    m_upgradesTabButton.setPosition(sf::Vector2f(bgX + 190.0f, bgY + 15.0f));

    m_hiringTabUnderline.setPosition(m_hiringTabButton.getPosition() + sf::Vector2f(0.0f, m_hiringTabButton.getSize().y));
    m_upgradesTabUnderline.setPosition(m_upgradesTabButton.getPosition() + sf::Vector2f(0.0f, m_upgradesTabButton.getSize().y));

    m_closeButton.setPosition(sf::Vector2f(bgX + bgWidth - 40.0f, bgY + 10.0f));

    m_hirePaleontologistButton.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 100.0f));

    m_upgrade1_RayLengthButton.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 100.0f));
    m_upgrade2_RayDamageButton.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 190.0f));
    m_upgrade3_PickupRadiusButton.setPosition(sf::Vector2f(bgX + 50.f, bgY + 280.f));
    m_upgrade4_JumpHeightButton.setPosition(sf::Vector2f(bgX + 50.f, bgY + 370.f));
	m_upgradeNPCButton.setPosition(sf::Vector2f(bgX + 50.f, bgY + 460.f));
    m_hirePaleoText.setPosition(m_hirePaleontologistButton.getPosition() + sf::Vector2f(20.f, 15.f));

    m_upgrade1_RayLengthText.setPosition(m_upgrade1_RayLengthButton.getPosition() + sf::Vector2f(20.f, 15.f));
    m_upgrade2_RayDamageText.setPosition(m_upgrade2_RayDamageButton.getPosition() + sf::Vector2f(20.f, 15.f));
    m_upgrade3_PickupRadiusText.setPosition(m_upgrade3_PickupRadiusButton.getPosition() + sf::Vector2f(20.f, 15.f));
    m_upgrade4_JumpHeightText.setPosition(m_upgrade4_JumpHeightButton.getPosition() + sf::Vector2f(20.f, 15.f));
	m_upgradeNPCText.setPosition(m_upgradeNPCButton.getPosition() + sf::Vector2f(20.f, 15.f));

    m_hiringTabText.setPosition(m_hiringTabButton.getPosition() + sf::Vector2f(20.f, 10.f));
    m_upgradesTabText.setPosition(m_upgradesTabButton.getPosition() + sf::Vector2f(20.f, 10.f));

}

HireAction TraderMenu::handleClick(const sf::Vector2f& screenPos, const sf::RenderWindow& window)
{
    if (!m_open) return HireAction::None;

    updateButtonPositions(window);

    if (containsPoint(m_closeButton, screenPos))
    {
        m_open = false;
        return HireAction::None;
    }

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

    if (m_activeTab == ActiveTab::Hiring)
    {
        if (containsPoint(m_hirePaleontologistButton, screenPos))
        {
            return HireAction::HirePaleontologist;
        }
    }
    else if (m_activeTab == ActiveTab::Upgrades)
    {
        // Upgrades 
        if (containsPoint(m_upgrade1_RayLengthButton, screenPos))
        {
            return HireAction::UpgradeRayLength;
        }
        if (containsPoint(m_upgrade2_RayDamageButton, screenPos))
        {
            return HireAction::UpgradeRayDamage;
        }
        if (containsPoint(m_upgrade3_PickupRadiusButton, screenPos))
        {
            return HireAction::UpgradePickupRadius;
        }
        if (containsPoint(m_upgrade4_JumpHeightButton, screenPos))
        {
            return HireAction::UpgradeJumpHeight;
        }
        if (containsPoint(m_upgradeNPCButton, screenPos))
        {
			return HireAction::UpgradeNPC;
        }
    }

    if (!containsPoint(m_background, screenPos))
    {
        m_open = false;
        return HireAction::None;
    }

    return HireAction::None;
}

void TraderMenu::draw(sf::RenderWindow& window)
{
    sf::View prev = window.getView();
    window.setView(window.getDefaultView());

    m_upgrade1_RayLengthText.setString("Length +" + std::to_string(upgrade1Level) +" ($" + std::to_string(getUpgrade1Cost()) + ")");

    m_upgrade2_RayDamageText.setString("Damage +" + std::to_string(upgrade2Level) +" ($" + std::to_string(getUpgrade2Cost()) + ")");

    m_upgrade3_PickupRadiusText.setString("Pickup +" + std::to_string(upgrade3Level) + " ($" + std::to_string(getUpgrade3Cost()) + ")");

    m_upgrade4_JumpHeightText.setString("Jump +" + std::to_string(upgrade4Level) + " ($" + std::to_string(getUpgrade4Cost()) + ")");

    m_upgradeNPCText.setString("Upgrade NPC ($" + std::to_string(getUpgradeNPCCost()) + ")");

    if (!m_open) return;

    updateButtonPositions(window);

    window.draw(m_overlay);

    window.draw(m_background);

    sf::Vector2f bgPos = m_background.getPosition();
    float bgX = bgPos.x;
    float bgY = bgPos.y;

    window.draw(m_hiringTabButton);
    window.draw(m_upgradesTabButton);

    window.draw(m_hiringTabText);
    window.draw(m_upgradesTabText);

    if (m_activeTab == ActiveTab::Hiring)
    {
        window.draw(m_hiringTabUnderline);

    }
    else
    {
        window.draw(m_upgradesTabUnderline);

    }

    window.draw(m_closeButton);

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

    if (m_activeTab == ActiveTab::Hiring)
    {
        window.draw(m_hirePaleontologistButton);
        window.draw(m_hirePaleoText);


    }
    else if (m_activeTab == ActiveTab::Upgrades)
    {
        m_upgrade1_RayLengthButton.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 100.0f));
        m_upgrade2_RayDamageButton.setPosition(sf::Vector2f(bgX + 50.0f, bgY + 190.0f));


        window.draw(m_upgrade1_RayLengthButton);
        window.draw(m_upgrade2_RayDamageButton);
        window.draw(m_upgrade3_PickupRadiusButton);
        window.draw(m_upgrade4_JumpHeightButton);
        window.draw(m_upgradeNPCButton);

        window.draw(m_upgrade1_RayLengthText);
        window.draw(m_upgrade2_RayDamageText);
        window.draw(m_upgrade3_PickupRadiusText);
        window.draw(m_upgrade4_JumpHeightText);
		window.draw(m_upgradeNPCText);

    }


    window.setView(prev);

}
