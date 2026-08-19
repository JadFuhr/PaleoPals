#pragma once
#include "BTNode.h"
class NPC;
class Map;

class BTCollectFossilNode : public BTNode
{
public:

	BTCollectFossilNode(NPC& npc, Map& map);
	BTStatus tick(float dt) override;

private:

	NPC& m_npc;
	Map& m_map;
};