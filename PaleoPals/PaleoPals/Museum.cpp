#include "Museum.h"
#include <SFML/Graphics/Rect.hpp>

bool Museum::loadFromConfig(const nlohmann::json& data)
{
	return false;
}

void Museum::updateHover(const sf::RenderWindow& window)
{
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel);

    if (m_sprite.getGlobalBounds().contains(mouseWorld))
    {
        // move to second frame when hovered 
        m_sprite.setTextureRect(sf::IntRect({ m_frameWidth, 0 }, { m_frameWidth , m_frameHeight }));
    }
    else
    {
        m_sprite.setTextureRect(sf::IntRect({ 0,0 }, { m_frameWidth,m_frameHeight }));
    }
 
}



void Museum::drawMuseum(sf::RenderWindow& window)
{
	window.draw(m_sprite);
}
