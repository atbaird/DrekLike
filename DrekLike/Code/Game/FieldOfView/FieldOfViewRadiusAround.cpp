#include "Game/FieldOfView/FieldOfViewRadiusAround.hpp"


FieldOfViewRadiusAround::FieldOfViewRadiusAround()
{

}
FieldOfViewRadiusAround::~FieldOfViewRadiusAround()
{

}

void FieldOfViewRadiusAround::CalculateFieldOfViewForAgent(Agent* age, const float& viewDist, Map* map)
{
	if (age == nullptr)
	{
		return;
	}
	IntVector2 bl = age->GetBlTilePos();
	std::map<Tile_Coord, Tile*>* mapTiles = map->GetMapTiles();
	for (std::map<Tile_Coord, Tile*>::iterator it = mapTiles->begin(); it != mapTiles->end(); it++)
	{
		IntVector2 dif = bl - it->first;
		if (it->second != nullptr && dif.getLength() <= viewDist)
		{
			it->second->SetVisible(true);
			it->second->SetSeen(true);
		}
		else
		{
			it->second->SetVisible(false);
		}
	}
}