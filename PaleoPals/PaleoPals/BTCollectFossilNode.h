#pragma once
#include "BTNode.h"
#include <map>
class NPC;
class Map;
struct Collectible;

class BTCollectFossilNode : public BTNode
{
public:

	BTCollectFossilNode(NPC& npc, Map& map);
	BTStatus tick(float dt) override;

private:

	NPC& m_npc;
	Map& m_map;

	Collectible* m_currentTarget = nullptr;
	float m_targetTimeout = 0.0f;	// tracking time spent on target

	std::map<Collectible*, int> m_failedAttempts; 
	static const int MAX_ATTEMPTS_PER_FOSSIL = 3;
};