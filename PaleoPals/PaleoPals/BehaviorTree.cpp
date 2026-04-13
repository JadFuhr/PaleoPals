#include "BehaviorTree.h"
#include "Paleontologist.h"
#include "Map.h"
#include <SFML/System/Time.hpp>

BTStatus SequenceNode::tick(Paleontologist* agent, sf::Time dt, Map& map)
{
    while (m_current < m_children.size())
    {
        BTStatus status = m_children[m_current]->tick(agent, dt, map);

        if (status == BTStatus::Running)
        {
            return BTStatus::Running;
        }
        if (status == BTStatus::Failure)
        {
            m_current = 0;
            return BTStatus::Failure;
        }
        ++m_current;
    }
    m_current = 0;
    return BTStatus::Success;
}

BTStatus SelectorNode::tick(Paleontologist* agent, sf::Time dt, Map& map)
{
    while (m_current < m_children.size())
    {
        BTStatus status = m_children[m_current]->tick(agent, dt, map);

        if (status == BTStatus::Running)
        {
            return BTStatus::Running;
        }
        if (status == BTStatus::Success)
        {
            m_current = 0;
            return BTStatus::Success;
        }
        ++m_current;
    }
    m_current = 0;

    return BTStatus::Failure;
}

void BehaviorTree::tick(Paleontologist* agent, sf::Time dt, Map& map)
{
    if (m_root)
    {
        m_root->tick(agent, dt, map);
    }
}
