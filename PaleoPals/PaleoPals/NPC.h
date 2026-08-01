#pragma once
#ifndef NPC_H
#define NPC_H

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "Map.h"

enum class NPCState
{
	WANDERTHESURFACE,
	MINING,
	SEARCHING
};

class NPC
{
public:
	NPC();

    void updateNPC(sf::Time dt, Map& map);
	void drawNPC(sf::RenderWindow& window);

    std::vector<sf::Vector2i> m_miningPath;   // DFS/BFS result
    int m_miningIndex = 0;
    bool m_useDFS = true;

    void updateMining(sf::Time dt, Map& map);
    void generateMiningPath(Map& map);

    sf::Vector2i worldToTile(sf::Vector2f pos, Map& map);
    sf::Vector2f tileToWorld(sf::Vector2i tile, Map& map);
	sf::Vector2f getNPCPosition() const { return m_sprite.getPosition(); }
private:

    sf::Texture m_texture;
    sf::Sprite  m_sprite{ m_texture };

    NPCState m_state = NPCState::WANDERTHESURFACE;

    sf::Vector2f m_velocity;
    float m_moveSpeed = 80.0f; // slower speed

    bool m_facingRight = true;

    int m_currentFrame = 0;
    float m_animationTimer = 0.0f;
    float m_frameTime = 0.15f;
    const int m_frameWidth = 192;
    const int m_frameHeight = 192;
    const int m_totalFrames = 4;

    void updateSurfaceWandering(sf::Time dt, Map& map);
    void updateNPCAnimation(sf::Time dt);
    void setNPCFrames(int frame);
};

#endif