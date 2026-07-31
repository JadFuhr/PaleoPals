#include "NPC.h"
#include <iostream>
#include <cmath>

NPC::NPC()
{

	if (!m_texture.loadFromFile("ASSETS/IMAGES/Sprites/Characters/paleontologist_walk.png"))
	{
		std::cerr << "failed to load npc sprite \n";
	}
	else
	{
		std::cout << "npc loaded from file \n";
	}

	m_sprite.setTexture(m_texture);
	m_sprite.setTextureRect(sf::IntRect({ 0,0 }, { m_frameWidth,m_frameHeight }));
	m_sprite.setOrigin(sf::Vector2f(m_frameWidth / 2.0f, m_frameHeight));
	m_sprite.setScale(sf::Vector2f(0.12f, 0.12f));
	m_sprite.setColor(sf::Color::Cyan);

	m_sprite.setPosition(sf::Vector2f(WINDOW_X / 2.0f - 150.0f, WINDOW_Y / 2.0f));
}

void NPC::updateNPC(sf::Time dt, Map& map)
{
	switch (m_state)
	{
	case NPCState::WANDERTHESURFACE:
		updateSurfaceWandering(dt, map);
		break;
	}

	updateNPCAnimation(dt);


}

void NPC::drawNPC(sf::RenderWindow& window)
{

	window.draw(m_sprite);

}

void NPC::updateSurfaceWandering(sf::Time dt, Map& map)
{

	float tileSize = map.getTileSize();
	int cols = map.getColumnCount();
	int rows = map.getRowCount();

	// same offset logic as Player
	float offsetX = (WINDOW_X - cols * tileSize) / 2.0f;
	float offsetY = WINDOW_Y / 2.0f;

	sf::Vector2f pos = m_sprite.getPosition();

	// horizontal wandering: move left/right
	m_velocity.x = m_facingRight ? m_moveSpeed : -m_moveSpeed;

	pos += m_velocity * dt.asSeconds();

	// clamp to surface horizontally
	const float halfW = tileSize * 0.35f;
	float minX = offsetX + halfW;
	float maxX = offsetX + cols * tileSize - halfW;

	if (pos.x < minX)
	{
		pos.x = minX;
		m_facingRight = true;
	}
	else if (pos.x > maxX)
	{
		pos.x = maxX;
		m_facingRight = false;
	}

	float surfaceY = offsetY + tileSize * 0.0f; // row 0
	pos.y = surfaceY;

	m_sprite.setPosition(pos);
}

void NPC::updateNPCAnimation(sf::Time dt)
{

	m_animationTimer += dt.asSeconds();

	if (m_animationTimer >= m_frameTime)
	{
		m_animationTimer -= m_frameTime;
		m_currentFrame = (m_currentFrame + 1) % m_totalFrames;
		setNPCFrames(m_currentFrame);
	}

}

void NPC::setNPCFrames(int frame)
{
	if (frame < 0 || frame >= m_totalFrames)
	{
		frame = 0;
	}

	m_currentFrame = frame;

	if (m_texture.getSize().x == 0 || m_texture.getSize().y == 0)
	{
		return;
	}

	int xOffset = frame * m_frameWidth;
	m_sprite.setTextureRect(sf::IntRect({ xOffset, 0 }, { m_frameWidth, m_frameHeight }));

	sf::Vector2f scale = m_sprite.getScale();
	scale.x = m_facingRight ? std::abs(scale.x) : -std::abs(scale.x);
	m_sprite.setScale(scale);

}
