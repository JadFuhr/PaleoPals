#include "BTMiningNode.h"
#include "NPC.h"
#include "Map.h"

BTMiningNode::BTMiningNode(NPC& npc, Map& map) : m_npc(npc), m_map(map) {}

BTStatus BTMiningNode::tick(float dt)
{
	if (m_npc.m_miningPath.empty() && !m_npc.m_returningToSurface)
	{
		m_npc.generateMiningPath(m_map);
	}

	m_npc.updateMining(sf::seconds(dt), m_map);

	if (m_npc.m_miningIndex < m_npc.m_miningPath.size())
	{
		return BTStatus::Running;
	}

	if (m_npc.m_returningToSurface)
	{
		if (m_npc.m_returnPath.empty())
		{
			m_npc.generateReturnPath(m_map);

			if (m_npc.m_returnPath.empty())
			{
				std::cout << "Mining node: return path empty, FAILING to reset tree\n";
				m_npc.m_returningToSurface = false;
				m_npc.m_miningPath.clear();
				m_npc.m_miningIndex = 0;
				m_npc.m_returnIndex = 0;
				return BTStatus::Failure;  // Force tree to reset
			}
		}
		return BTStatus::Success;
	}

	return BTStatus::Running;
}

