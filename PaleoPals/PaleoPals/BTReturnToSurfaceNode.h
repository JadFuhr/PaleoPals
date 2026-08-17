#pragma once
#include "BTNode.h"
#include "NPC.h"
#include "Map.h"

class BTReturnToSurfaceNode : public BTNode
{
public:

	BTReturnToSurfaceNode(NPC& npc, Map& map): m_npc(npc), m_map(map){}

	BTStatus tick(float dt) override
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

private:

	NPC& m_npc;
	Map& m_map;
};