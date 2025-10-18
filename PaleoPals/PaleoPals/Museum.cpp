#include "Museum.h"
#include <iostream>
#include <SFML/Graphics/Rect.hpp>

Museum::Museum() = default;

bool Museum::loadFromConfig(const nlohmann::json& data)
{

    if (!m_texture.loadFromFile(data["texture"].get<std::string>()))
    {
        std::cerr << "Failed to load museum texture" << std::endl;
        return false;
    }

    m_frameWidth = data["frameWidth"].get<int>();
    m_frameHeight = data["frameHeight"].get<int>();

    m_position = sf::Vector2f(data["position"]["x"].get<float>(), data["position"]["y"].get<float>());

    m_sprite.setTexture(m_texture);
    m_sprite.setTextureRect(sf::IntRect({ 0,0 }, { m_frameWidth,m_frameHeight }));
    m_sprite.setPosition(m_position);


	return true;
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
