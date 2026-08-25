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
#include "BTCollectFossilNode.h"

class Player;

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

    void setRoot(BTNode* root) { m_root = root; }
    BTNode* getRoot() const { return m_root; }

	void setPlayer(Player* player) { m_player = player; }

    void updateNPC(sf::Time dt, Map& map);
	void drawNPC(sf::RenderWindow& window, Map& map);
    void updateReturn(sf::Time dt, Map& map);

    std::vector<sf::Vector2i> m_miningPath;   // DFS result
    sf::Vector2i m_miningStartTile;     // npc rememberrs where it starts 
    std::vector<sf::Vector2i> m_returnPath;
	std::vector<sf::Vector2i> m_fossilPath; // path to fossil (for Astar)
	int m_fossilIndex = 0;  

    int m_miningIndex = 0;
    int m_returnIndex = 0;  

    float npcMiningDamageCooldown = 0.0f;
    float npcMiningTickDelay = 0.12f;
    int m_npcMiningDamage = 5;
	int m_miningDepthLevel = 0; // for npc mining upgrade 

    bool m_useDFS = true;
    bool m_returningToSurface = false; 

    void updateMining(sf::Time dt, Map& map);
    void mineTile(Map& map, sf::Vector2i tile);
    void generateMiningPath(Map& map);  //DFS
    void generateReturnPath(Map& map);
    void generateFossilPath(Map& map, sf::Vector2i goal);   // Astar
	void updateFossilPath(sf::Time dt, Map& map);
    void updateSurfaceWandering(sf::Time dt, Map& map);

    void drawPath(sf::RenderWindow& window, Map& map);

    sf::Vector2i worldToTile(sf::Vector2f pos, Map& map);
    sf::Vector2f tileToWorld(sf::Vector2i tile, Map& map);
	sf::Vector2f getNPCPosition() const { return m_sprite.getPosition(); }

	Player* m_player = nullptr; // Pointer to the player object

private:
    BTNode* m_root = nullptr;

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

    void updateNPCAnimation(sf::Time dt);
    void setNPCFrames(int frame);
};

#endif