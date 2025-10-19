#pragma once
#ifndef MUSEUM_H
#define MUSEUM_H

#include <SFML/Graphics.hpp>
#include <json.hpp>

class Museum {
public:
	Museum();

	bool loadMuseumFromConfig(const nlohmann::json& data);
	void updateMuseumHover(const sf::RenderWindow& window);
	void drawMuseum(sf::RenderWindow& window);

private:

	sf::Texture m_texture;
	sf::Sprite m_sprite{ m_texture };
	sf::Vector2f m_position;
	int m_frameWidth = 0;
	int m_frameHeight = 0;
};
#endif