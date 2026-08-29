#include "BTWanderSurfaceNode.h"
#include "NPC.h"
#include "Map.h"

BTWanderSurfaceNode::BTWanderSurfaceNode(NPC& npc, Map& map) :m_npc(npc), m_map(map) {}

BTStatus BTWanderSurfaceNode::tick(float dt)
{
	if (m_npc.m_returningToSurface)
	{
		return BTStatus::Failure;	// dont trigger mining while returning
	}

	m_npc.updateSurfaceWandering(sf::seconds(dt), m_map);

	m_timer += dt;

	if (m_timer > 1.0f)
	{
		m_timer = 0.0f;

		int roll = std::rand() % 100;

		if (roll< 30)
		{
			return BTStatus::Success;
		}
	}

	return BTStatus::Running;
}


