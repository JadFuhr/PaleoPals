#pragma once
#include <vector>
#include "BTNode.h"

class SequenceNode : public BTNode
{
public:

	void addChildNode(BTNode* child);
	BTStatus tick(float dt) override;

private:

	std::vector<BTNode*> m_children;
	size_t m_index = 0;
};