#include "BTCollectFossilNode.h"
#include "NPC.h"
#include "Map.h"

BTCollectFossilNode::BTCollectFossilNode(NPC& npc, Map& map) : m_npc(npc), m_map(map) {}

BTStatus BTCollectFossilNode::tick(float dt)
{


	return BTStatus::Success;
}
