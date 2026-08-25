#pragma once
#include "BTNode.h"
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

	int m_pathfindingFailedCount = 0;		// tracking how many times pathfinding has failed 
	float m_currentTargetTimeout = 0.0f;	// tracking time spent on current target
};