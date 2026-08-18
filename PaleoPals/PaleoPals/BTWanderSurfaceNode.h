#pragma once
#include "BTNode.h"
class NPC;
class Map;

class BTWanderSurfaceNode : public BTNode
{
public:

	BTWanderSurfaceNode(NPC& npc, Map& map);
	BTStatus tick(float dt) override;

private:

	NPC& m_npc;
	Map& m_map;
	float m_timer = 0.0f;
};