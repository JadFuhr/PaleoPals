#include "BTReturnToSurfaceNode.h"
#include "NPC.h"
#include "Map.h"

BTReturnToSurfaceNode::BTReturnToSurfaceNode(NPC& npc, Map& map) : m_npc(npc), m_map(map) {}

BTStatus BTReturnToSurfaceNode::tick(float dt)
{
	if (!m_npc.m_returningToSurface)
	{
		return BTStatus::Failure;
	}

	m_npc.updateMining(sf::seconds(dt), m_map);

	if (!m_npc.m_returningToSurface)
	{
		return BTStatus::Success;
	}

	return BTStatus::Running;

}