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

	// test whether a world-coordinate point hits the trader sprite
	bool containsPoint(const sf::Vector2f& point) const;

	// Get the sprite for frustum culling
	const sf::Sprite& getSprite() const { return m_sprite; }

private:

	sf::Texture m_texture;
	sf::Sprite m_sprite{ m_texture };
	sf::Vector2f m_position;
	int m_frameWidth = 0;
	int m_frameHeight = 0;
};

#endif
