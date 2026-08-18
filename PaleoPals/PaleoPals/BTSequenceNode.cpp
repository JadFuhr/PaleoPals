#include "BTSequenceNode.h"

void SequenceNode::addChildNode(BTNode* child)
{
	m_children.push_back(child);
}

BTStatus SequenceNode::tick(float dt)
{
	while (m_index < m_children.size())
	{
		BTStatus status = m_children[m_index]->tick(dt);	//store running, failuure or success in status
		//std::cout << "Sequence child " << m_index << " returned " << (int)status << std::endl;

		if (status == BTStatus::Running)	// return running if still busy
		{
			return BTStatus::Running;
		}

		if (status == BTStatus::Failure)	// return fail if one child doesnt succeed
		{
			m_index = 0;

			return BTStatus::Failure;
		}

		m_index++;	// go to next child
	}

	m_index = 0;
	return BTStatus::Success;	// every child succeeded

}

