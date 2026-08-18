#pragma once
#include <SFML/System.hpp>
#include <iostream>

enum class BTStatus
{
	Success,
	Failure,
	Running
};

class BTNode
{
public:
	virtual ~BTNode() = default;
	virtual BTStatus tick(float dt) = 0;
};