#include "BTMiningNode.h"
#include "NPC.h"
#include "Map.h"

BTMiningNode::BTMiningNode(NPC& npc, Map& map) : m_npc(npc), m_map(map) {}

BTStatus BTMiningNode::tick(float dt)
{
	std::cout << "ticking mining node" << std::endl;

	m_npc.updateMining(sf::seconds(dt), m_map);

	if (m_npc.m_returningToSurface)
	{
		std::cout << "Mining returning SUCCESS (switching to return)" << std::endl;
		return BTStatus::Success;
	}

	std::cout << "Mining returning RUNNING" << std::endl;
	return BTStatus::Running;
}

