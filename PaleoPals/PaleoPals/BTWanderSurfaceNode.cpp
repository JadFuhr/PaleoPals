#include "BTWanderSurfaceNode.h"
#include "NPC.h"
#include "Map.h"

BTWanderSurfaceNode::BTWanderSurfaceNode(NPC& npc, Map& map) :m_npc(npc), m_map(map) {}

BTStatus BTWanderSurfaceNode::tick(float dt)
{
		m_npc.updateSurfaceWandering(sf::seconds(dt), m_map);

		if (m_npc.m_returningToSurface)
		{
			return BTStatus::Success;
		}

		return BTStatus::Running;
	}


