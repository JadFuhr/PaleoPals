#include "BTImTiredOfWanderingNode.h"

BTImTiredOfWanderingNode::BTImTiredOfWanderingNode(NPC& npc) : m_npc(npc){}

BTStatus BTImTiredOfWanderingNode::tick(float dt)
{
	std::cout << "ticking tired of wandering node" << std::endl;

	//time spent wandering surface
	m_timer += dt;

	//roll rand chance
	if (m_timer > 5.0f)
	{
		m_timer = 0.0f;

		int roll = std::rand() % 100;

		if (roll < 100) // always succeed
		{
			/*std::cout << "NPC: Im tired of wandering, lemme dig a hole" << std::endl;
			std::cout << "Condition SUCCESS (roll=" << roll << ")" << std::endl;*/

			return BTStatus::Success;
		}
	/*	std::cout << "Condition FAILURE (roll=" << roll << ")" << std::endl;*/
	}

	return BTStatus::Failure;
}
