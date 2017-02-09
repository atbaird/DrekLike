#pragma once

#ifndef FIELDOFVIEWRADIUSAROUND_HPP
#define FIELDOFVIEWRADIUSAROUND_HPP

#include "Game/GameEntity/Agent.hpp"
#include "Game/Map/Map.hpp"

class FieldOfViewRadiusAround
{
private:
public:
	FieldOfViewRadiusAround();
	~FieldOfViewRadiusAround();

	static void CalculateFieldOfViewForAgent(Agent* age, const float& viewDist, Map* map);
};
#endif