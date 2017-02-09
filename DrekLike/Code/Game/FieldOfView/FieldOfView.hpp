#pragma once

#ifndef FIELDOFVIEW_HPP
#define FIELDOFVIEW_HPP

#include "Game/GameEntity/Agent.hpp"
#include "Game/Map/Map.hpp"

class FieldOfView
{
private:
public:
	//constructors
	FieldOfView();
	~FieldOfView();

	//public functions
	static void RaycastFromAgent(Agent* age, const float& viewDist, Map* map);
};
#endif