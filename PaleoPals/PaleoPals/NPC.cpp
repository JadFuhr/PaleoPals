#include "NPC.h"
#include <iostream>
#include <cmath>

NPC::NPC()
{

	if (!m_texture.loadFromFile("ASSETS/IMAGES/Sprites/Characters/paleontologist_walk.png"))
	{
		std::cerr << "failed to load npc sprite/ \n";
	}
	else
	{
		std::cout << "npc loaded \n";
	}

	m_sprite.setTexture(m_texture);
	m_sprite.setTextureRect(sf::IntRect({ 0,0 }, { m_frameWidth,m_frameHeight }));
	m_sprite.setOrigin(sf::Vector2f(m_frameWidth / 2.0f, m_frameHeight));
	m_sprite.setScale(sf::Vector2f(0.2f, 0.2f));
	m_sprite.setColor(sf::Color::Cyan);

	m_sprite.setPosition(sf::Vector2f(WINDOW_X / 2.0f - 150.0f, WINDOW_Y / 2.0f));
}

void NPC::updateNPC(sf::Time dt, Map& map)
{



}

void NPC::drawNPC(sf::RenderWindow& window)
{

	window.draw(m_sprite);

}

void NPC::updateSurfaceWandering(sf::Time dt, Map& map)
{



}

void NPC::updateNPCAnimation(sf::Time dt)
{



}

void NPC::setNPCFrames(int frame)
{


}
