#include "BTSelectorNode.h"

void SelectorNode::addChildNode(BTNode* child)
{
	m_children.push_back(child);
}

BTStatus SelectorNode::tick(float dt)
{
	while (m_index < m_children.size())
	{
		BTStatus status = m_children[m_index]->tick(dt);
		//std::cout << "Selector child " << m_index << " returned " << (int)status << std::endl;

		if (status == BTStatus::Running)		// still busy
		{
			return BTStatus::Running;
		}

		if (status == BTStatus::Success)	// if succeeds, entire selector succeeds
		{
			m_index = 0;	// reset index, reevaluate from top

			return BTStatus::Success;
		}

		m_index++;
	}

	m_index = 0;
	return BTStatus::Failure;	// every child failed

}
