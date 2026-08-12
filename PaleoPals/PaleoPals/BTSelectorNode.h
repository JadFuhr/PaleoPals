#pragma once
#include <vector>
#include "BTNode.h"

class SelectorNode : public BTNode
{
public:

	void addChildNode(BTNode* child) { m_children.push_back(child); }

	BTStatus tick(float dt) override
	{
		while (m_index < m_children.size())
		{
			BTStatus status = m_children[m_index]->tick(dt);

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

private:

	std::vector<BTNode*> m_children;
	size_t m_index = 0;
};