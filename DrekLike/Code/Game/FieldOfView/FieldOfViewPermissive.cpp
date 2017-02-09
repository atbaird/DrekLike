#include "Game/FieldOfView/FieldOfViewPermissive.hpp"

PermissiveCone::PermissiveCone()
{

}
PermissiveCone::PermissiveCone(const float& start, const float& end, const IntVector2& center)
	: startDeg(start),
	endDeg(end),
	centerPosition(center)
{

}

FieldOfViewPermissive::FieldOfViewPermissive()
{

}
FieldOfViewPermissive::~FieldOfViewPermissive()
{

}


void FieldOfViewPermissive::RaycastFromAgent(Agent* age, const float& viewDist, Map* map)
{
	if (age == nullptr)
	{
		return;
	}
	//Fast reference:
	//SW corner = GetTileCoords()
	//NW corenr = GetNorthTileCoords()
	//NE corner = GetNorthEastTileCoords();
	//SE corner = GetEastTileCoords()
	std::vector<PermissiveCone> cones;
	IntVector2 startPos = age->GetBlTilePos();
	Tile* tile = nullptr;
	if (map->GetIsTileCoordSolid(startPos, tile, age->GetMask()) == true)
	{
		return;
	}
	tile = nullptr;

	cones.push_back(PermissiveCone(0.f, 360.f, startPos));
	float startDistance = 1.f;
	while (startDistance <= viewDist)
	{
		for (size_t i = 0; i < cones.size(); i++)
		{

		}


		startDistance += 1.f;
	}


}