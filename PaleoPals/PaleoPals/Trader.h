#pragma once
#ifndef TRADER_H
#define TRADER_H

#include <json.hpp>
#include <SFML/Graphics.hpp>


class Trader {
public:
	Trader();

	bool loadTraderFromConfig(const nlohmann::json& data);
	void updateTraderHover(const sf::RenderWindow& window);
	void drawTrader(sf::RenderWindow& window);

private:

	sf::Texture m_texture;
	sf::Sprite m_sprite{ m_texture };
	sf::Vector2f m_position;
	int m_frameWidth = 0;
	int m_frameHeight = 0;
};

#endif 
