#pragma once
#ifndef NPC_H
#define NPC_H

#include <SFML/Graphics.hpp>
#include "constants.h"
#include "Map.h"
#include "BTNode.h"
#include "BTSelectorNode.h"
#include "BTSequenceNode.h"
#include "BTWanderSurfaceNode.h"
#include "BTMiningNode.h"
#include "BTReturnToSurfaceNode.h"

BTNode* m_root = nullptr;

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

    std::vector<sf::Vector2i> m_miningPath;   // DFS result
    sf::Vector2i m_miningStartTile;     // npc rememberrs where it starts 
    std::vector<sf::Vector2i> m_returnPath;

    int m_miningIndex = 0;
    int m_returnIndex = 0;  

    float npcMiningDamageCooldown = 0.0f;
    float npcMiningTickDelay = 0.12f;
    int m_npcMiningDamage = 1;

    bool m_useDFS = true;
    bool m_returningToSurface = false; 

    void updateMining(sf::Time dt, Map& map);
    void mineTile(Map& map, sf::Vector2i tile);
    void generateMiningPath(Map& map);
    void generateReturnPath(Map& map);

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