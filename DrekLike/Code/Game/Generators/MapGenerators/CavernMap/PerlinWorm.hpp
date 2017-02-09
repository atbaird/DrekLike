#pragma once

#ifndef PERLINWORM_HPP
#define PERLINWORM_HPP
#include "Engine/Math/IntVector2.hpp"

class PerlinWorm
{
private:
	IntVector2 CurrentPoint;
	float Yaw;
public:
	PerlinWorm(IntVector2 startPoint, float startYaw);
	~PerlinWorm();

	const IntVector2 GetNextPoint();

	static const float minDistanceToTravel;
	static const float addDistanceToTravel;
	static const float yawChangeRange;
};
#endif