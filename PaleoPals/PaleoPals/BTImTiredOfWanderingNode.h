#pragma once
#include "BTNode.h"
class NPC;

class BTImTiredOfWanderingNode : public BTNode
{
public:
	BTImTiredOfWanderingNode(NPC& npc);

	BTStatus tick(float dt) override;

private:

	NPC& m_npc;
	float m_timer = 0.0f;
};