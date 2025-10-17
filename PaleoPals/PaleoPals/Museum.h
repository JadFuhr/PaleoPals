#pragma once
#ifndef MUSEUM_H
#define MUSEUM_H

#include <SFML/Graphics.hpp>
#include <json.hpp>

class Museum {
public:

	bool loadFromConfig(const nlohmann::json& data);
	void updateHover(const sf::RenderWindow& window);
	void draw(sf::RenderWindow& window);

private:

	sf::Texture m_texture;
	sf::Sprite m_sprite;
	sf::Vector2f m_position;
	int m_frameWidth = 0;
	int m_frameHeight = 0;
};
#endif