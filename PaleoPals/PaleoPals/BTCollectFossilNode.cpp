#include "BTCollectFossilNode.h"
#include "NPC.h"
#include "Map.h""
#include "Player.h"

BTCollectFossilNode::BTCollectFossilNode(NPC& npc, Map& map) : m_npc(npc), m_map(map), m_pathfindingFailedCount(0), m_currentTargetTimeout(0) {}

BTStatus BTCollectFossilNode::tick(float dt)
{
    auto& fossils = m_map.getFossilManager().getAllCollectibles();

    // Pick a target if none or if the old one was collected
    if (!m_currentTarget || m_currentTarget->isPickedUp)
    {
        m_currentTarget = nullptr;
		m_pathfindingFailedCount = 0;
		m_currentTargetTimeout = 0.0f;

        float bestDistanceSq = std::numeric_limits<float>::max();
        sf::Vector2f npcPos = m_npc.getNPCPosition();

        for (auto& c : fossils)
        {
            if (!c.isPickedUp)
            {
                sf::Vector2f difference = c.sprite.getPosition() - npcPos;
                float distSq = difference.x * difference.x + difference.y * difference.y;

                if (distSq < bestDistanceSq)
                {
                    bestDistanceSq = distSq;
                    m_currentTarget = &c;
                }
            }
        }

        // Generate path to new target
        if (m_currentTarget)
        {
            m_npc.generateFossilPath(m_map, { m_currentTarget->gridCol, m_currentTarget->gridRow });

            if (m_npc.m_fossilPath.empty())
            {
                std::cout << "npc failed to generate path to fossil \n";
                m_currentTarget = nullptr; // Reset target if pathfinding fails
                m_pathfindingFailedCount++;
            }
        }
    }

    // No fossils left
    if (!m_currentTarget)
    {
		m_npc.m_returningToSurface = true; // Start returning to surface
		m_npc.generateReturnPath(m_map);
        return BTStatus::Success;
    }


    // Track time on current target - timeout if stuck for 3 seconds
    m_currentTargetTimeout += dt;
    if (m_currentTargetTimeout > 3.0f)
    {
        std::cout << "NPC timeout on fossil target,  marking as unreachable\n";
        m_npc.m_fossilPath.clear();
        m_npc.m_fossilIndex = 0;
        m_currentTarget = nullptr;
        m_currentTargetTimeout = 0.0f;
        return BTStatus::Running; // Try next target next frame
    }

    // Walk path
    m_npc.updateFossilPath(sf::seconds(dt), m_map);

    // Check proximity to fossil
    sf::Vector2f npcPos = m_npc.getNPCPosition();
    sf::Vector2f fossilPos = m_currentTarget->sprite.getPosition();
    sf::Vector2f diff = fossilPos - npcPos;

    float distSq = diff.x * diff.x + diff.y * diff.y;

    if (distSq < 16.0f * 16.0f)
    {
        // create collected item like player does 
        if (m_npc.m_player)
        {
			CollectedItem item;
			item.collectibleIndex = m_currentTarget->collectibleIndex;
			item.monetaryValue = m_currentTarget->monetaryValue;

            if (m_currentTarget->collectibleIndex <= 6) //fossils
            {
                item.type = "fossil";
				item.dinosaurName = m_currentTarget->assignedDinosaurName;
                item.pieceId = m_currentTarget->assignedPieceId;
				item.category = m_currentTarget->assignedCategory;
				item.name = m_currentTarget->assignedPieceId + " of " + m_currentTarget->assignedDinosaurName;
				m_npc.m_player->addMoney(m_currentTarget->monetaryValue);
            }
            else if(m_currentTarget->collectibleIndex <= 8) //amber
            {
                item.type = "amber";
				item.name = (m_currentTarget->monetaryValue == 75) ? "Small Amber" : "Large Amber";
                m_npc.m_player->addMoney(m_currentTarget->monetaryValue);
            }
            else //trash
            {
                item.type = "trash";
                item.name = "Trash";
                m_npc.m_player->addMoney(m_currentTarget->monetaryValue);
            }

			//add to inventory

			m_npc.m_player->addCollectedItem(item);
        }

        // Reached fossil
        m_currentTarget->isPickedUp = true;
        m_currentTarget->sprite.setPosition(sf::Vector2f(-10000.f, -10000.f));
        std::cout << "npc pickup fossil index" << m_currentTarget->collectibleIndex << std::endl;

        // Reset path, next tick generates another
        m_npc.m_fossilPath.clear();
        m_npc.m_fossilIndex = 0;
        m_currentTarget = nullptr;
    }

    return BTStatus::Running; // keep running until all fossils are gone
}


