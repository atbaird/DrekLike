#include "Game/FieldOfView/FieldOfViewRayCast.hpp"
#include "Engine/Math/ExtendedMath.hpp"

//-------------------------------------------------------------
//constructors
FieldOfViewRayCast::FieldOfViewRayCast()
{
}
FieldOfViewRayCast::~FieldOfViewRayCast()
{
}

//---------------------------------------------------------------
//public functions
void FieldOfViewRayCast::RaycastFromAgent(Agent* age, const float& viewDist, Map* map, const Vector2& direction, const int& numIncrements)
{
	if (age == nullptr)
	{
		return;
	}
	std::map<Tile_Coord, Tile*>* mapTiles = map->GetMapTiles();

	Tile_Coord blPos = age->GetBlTilePos();
	IntVector2 size = age->GetSize();
	Tile_Coord trPos = blPos + size - IntVector2(1, 1);
	Tile_Coord lastTested = blPos;
	IntVector2 cTile = blPos;
	Vector2 current = Vector2((float)blPos.x, (float)blPos.y);
	Vector2 end = current + (direction * viewDist);
	IntVector2 endTile = IntVector2((int)floor(end.x), (int)floor(end.y));
	IntVector2 lastVisited;
	bool firstRun = true;
	float increment = viewDist / (float)numIncrements;
	while ((direction.x != 0 || direction.y != 0)
		&& ((direction.x < 0 && cTile.x >= endTile.x) || (direction.x < 0 && cTile.x <= endTile.x))
		&& ((direction.y < 0 && cTile.y >= endTile.y) || (direction.y > 0 && cTile.y <= endTile.y)))
	{
		if (firstRun == true || lastVisited != cTile)
		{
			std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(cTile);
			if (it == mapTiles->end() || it->second == nullptr)
			{
				break;
			}
			TileType type = it->second->GetTileType();
			bool exists = false;
			bool solid = TileDefinition::GetTypeSolid(type, exists);
			if (exists == false || solid == true)
			{
				it->second->SetVisible(true);
				it->second->SetSeen(true);
				break;
			}
			it->second->SetVisible(true);
			it->second->SetSeen(true);
			
		}

		current += (direction * increment);
		lastVisited = cTile;
		cTile = IntVector2((int)current.x, (int)current.y);
		firstRun = false;
	}

}
void FieldOfViewRayCast::CalculateFieldOfViewForAgentAround(Agent* age, const float& viewDist, Map* map, const float& degreesToTestAbout)
{
	for (float deg = 0.f; deg < 360.f; deg += degreesToTestAbout)
	{
		Vector2 dir = Vector2(CosDegrees(deg), SinDegrees(deg));
		FieldOfViewRayCast::RaycastFromAgent(age, viewDist, map, dir);
	}
}