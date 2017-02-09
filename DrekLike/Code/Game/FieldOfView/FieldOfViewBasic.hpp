#pragma once

#ifndef FIELDOFVIEWBASIC_HPP
#define FIELDOFVIEWBASIC_HPP
#include "Game/FieldOfView/FieldOfView.hpp"

class FieldOfViewBasic : public FieldOfView
{
private:
public:
	//constructors
	FieldOfViewBasic();
	~FieldOfViewBasic();

	//public functions
	static void CalculateFieldOfViewForAgent(Agent* age, const float& viewDist, Map* map);
};
#endif