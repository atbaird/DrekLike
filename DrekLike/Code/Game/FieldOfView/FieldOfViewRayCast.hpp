#pragma once

#ifndef FIELDOFVIEWRAYCAST_HPP
#define FIELDOFVIEWRAYCAST_HPP
#include "Game/FieldOfView/FieldOfView.hpp"

class FieldOfViewRayCast : public FieldOfView
{
private:

public:
	//constructors
	FieldOfViewRayCast();
	virtual ~FieldOfViewRayCast();

	//public functions
	static void RaycastFromAgent(Agent* age, const float& viewDist, Map* map, const Vector2& direction, const int& numIncrements = 100);
	static void CalculateFieldOfViewForAgentAround(Agent* age, const float& viewDist, Map* map, const float& degreesToTestAbout = 1.f);

};
#endif