#pragma once
#include <vector>
#include "BTNode.h"

class SequenceNode : public BTNode
{
public:

	void addChildNode(BTNode* child) { m_children.push_back(child); }	//add new child node to sequence

	BTStatus tick(float dt) override
	{
		while (m_index < m_children.size())
		{
			BTStatus status = m_children[m_index]->tick(dt);	//store running, failuure or success in status

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

private:

	std::vector<BTNode*> m_children;
	size_t m_index = 0;
};