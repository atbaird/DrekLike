#pragma once

#ifndef FIELDOFVIEWPERMISSIVE_HPP
#define FIELDOFVIEWPERMISSIVE_HPP
#include "Game/FieldOfView/FieldOfView.hpp"

struct PermissiveCone
{
	PermissiveCone();
	PermissiveCone(const float& start, const float& end, const IntVector2& center);
	float startDeg;
	float endDeg;
	IntVector2 centerPosition;
};

class FieldOfViewPermissive : public FieldOfView
{
private:
public:

	//Fast reference:
	//SW corner = GetTileCoords()
	//NW corenr = GetNorthTileCoords()
	//NE corner = GetNorthEastTileCoords();
	//SE corner = GetEastTileCoords()

	FieldOfViewPermissive();
	~FieldOfViewPermissive();


	static void RaycastFromAgent(Agent* age, const float& viewDist, Map* map);
};
#endif // !FIELDOFVIEWPERMISSIVE_HPP
