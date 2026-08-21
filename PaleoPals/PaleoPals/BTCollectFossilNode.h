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

	Collectible* m_currentTarget = nullptr;
	NPC& m_npc;
	Map& m_map;
};