#include "BTCollectFossilNode.h"
#include "NPC.h"
#include "Map.h"

BTCollectFossilNode::BTCollectFossilNode(NPC& npc, Map& map) : m_npc(npc), m_map(map) {}

BTStatus BTCollectFossilNode::tick(float dt)
{
	//std::cout << "collect fossil ticked" << std::endl;

	auto& fossils = m_map.getFossilManager().getAllCollectibles();

	for (auto& c : fossils)
	{
		m_npc.generateFossilPath(m_map, sf::Vector2i(c.gridCol, c.gridRow));
		if (!c.isPickedUp)
		{
			//std::cout << "looking for collectible" << c.collectibleIndex << " at tile: << " << c.gridRow << ", " << c.gridCol << "with value of: " << c.monetaryValue << std::endl;
			
			return BTStatus::Running;
		}
	}

	return BTStatus::Success;
}
