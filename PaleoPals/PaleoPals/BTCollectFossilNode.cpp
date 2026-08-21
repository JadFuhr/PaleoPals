#include "BTCollectFossilNode.h"
#include "NPC.h"
#include "Map.h"

BTCollectFossilNode::BTCollectFossilNode(NPC& npc, Map& map) : m_npc(npc), m_map(map) {}

BTStatus BTCollectFossilNode::tick(float dt)
{
	auto& fossils = m_map.getFossilManager().getAllCollectibles();

	for (auto& c : fossils)
	{
		if (!c.isPickedUp)
		{
			if (m_npc.m_fossilPath.empty())
			{
				m_npc.generateFossilPath(m_map, { c.gridCol, c.gridRow });
			}

			//m_npc.updateFossilPath() function to be added

			if(m_npc.m_fossilIndex >= m_npc.m_fossilPath.size())
			{
				//reached fossil

				c.isPickedUp = true;

				std::cout << "npc chose to go for fossil: " << c.collectibleIndex << " at tile: " << c.gridRow << ", " << c.gridCol << std::endl;

				return BTStatus::Success;
			}
			
			return BTStatus::Running;
		}
	}

	return BTStatus::Success;
}
