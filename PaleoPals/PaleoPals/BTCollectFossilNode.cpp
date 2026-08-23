#include "BTCollectFossilNode.h"
#include "NPC.h"
#include "Map.h""
#include "Player.h"

BTCollectFossilNode::BTCollectFossilNode(NPC& npc, Map& map) : m_npc(npc), m_map(map) {}

BTStatus BTCollectFossilNode::tick(float dt)
{
    auto& fossils = m_map.getFossilManager().getAllCollectibles();

    // Pick a target if none or if the old one was collected
    if (!m_currentTarget || m_currentTarget->isPickedUp)
    {
        m_currentTarget = nullptr;

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
        }
    }

    // No fossils left
    if (!m_currentTarget)
    {
		m_npc.m_returningToSurface = true; // Start returning to surface
		m_npc.generateReturnPath(m_map);
        return BTStatus::Success;
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
        // Reached fossil
        m_currentTarget->isPickedUp = true;
        m_currentTarget->sprite.setPosition(sf::Vector2f(-10000.f, -10000.f));
        std::cout << "NPC collected fossil: " << m_currentTarget->collectibleIndex << std::endl;

        // Reset path, next tick generates another
        m_npc.m_fossilPath.clear();
        m_npc.m_fossilIndex = 0;
        m_currentTarget = nullptr;
    }

    return BTStatus::Running; // keep running until all fossils are gone
}


