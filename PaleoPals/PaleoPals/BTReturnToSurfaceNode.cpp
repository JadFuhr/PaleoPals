#include "BTReturnToSurfaceNode.h"
#include "NPC.h"
#include "Map.h"

BTReturnToSurfaceNode::BTReturnToSurfaceNode(NPC& npc, Map& map) : m_npc(npc), m_map(map) {}

BTStatus BTReturnToSurfaceNode::tick(float dt)
{
	// not in return state, fail to move to next behaviour
	if (!m_npc.m_returningToSurface)
	{
		m_timeoutTimer = 0.0f;
		return BTStatus::Failure;
	}

	m_timeoutTimer += dt;

	if (m_timeoutTimer > 10.0f && m_npc.m_returnPath.empty())
	{
		std::cout << "Return node TIMEOUT - NPC trapped underground for 10+ seconds!\n";

		// Reset NPC state completely
		m_npc.m_returningToSurface = false;
		m_npc.m_miningPath.clear();
		m_npc.m_returnPath.clear();
		m_npc.m_miningIndex = 0;
		m_npc.m_returnIndex = 0;
		m_timeoutTimer = 0.f;

		return BTStatus::Failure;  // Force tree to reset then try again
	}

	m_npc.updateReturn(sf::seconds(dt), m_map);

	if (m_npc.m_returnIndex >= m_npc.m_returnPath.size())
	{
		m_npc.m_returningToSurface = false; 
		m_npc.m_miningPath.clear();
		m_npc.m_returnPath.clear();
		m_npc.m_miningIndex = 0;
		m_npc.m_returnIndex = 0;
		m_timeoutTimer = 0.f;
		return BTStatus::Success;
	}
	return BTStatus::Running;

}