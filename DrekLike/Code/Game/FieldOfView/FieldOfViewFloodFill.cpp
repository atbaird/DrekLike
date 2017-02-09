#include "Game/FieldOfView/FieldOfViewFloodFill.hpp"
#include "Game/Items/Item.hpp"
#include "Game/Features/Feature.hpp"

//---------------------------------------------------------------
//constructors
FieldOfViewFloodFill::FieldOfViewFloodFill()
{
}
FieldOfViewFloodFill::~FieldOfViewFloodFill()
{
}

//---------------------------------------------------------------
//private functions
const bool FieldOfViewFloodFill::IsTileAlreadyInCurTiles(std::vector<Tile*>& curTiles, Tile* tile)
{
	for (size_t i = 0; i < curTiles.size(); i++)
	{
		if (curTiles.at(i) == tile)
		{
			return true;
		}
	}
	return false;
}

void FieldOfViewFloodFill::AddTileInSingleDir(std::vector<Tile*>& curTiles, std::vector<IntVector2>& dirs, std::map<Tile_Coord, Tile*>*& mapTiles, Tile_Coord coordsTest, const IntVector2& dirToAddIfTrue)
{
	std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(coordsTest);
	if (it != mapTiles->end() && it->second != nullptr)
	{
		if (FieldOfViewFloodFill::IsTileAlreadyInCurTiles(curTiles, it->second) == false)
		{
			curTiles.push_back(it->second);
			dirs.push_back(dirToAddIfTrue);
		}
	}
}

void FieldOfViewFloodFill::AddTilesInDir(std::vector<Tile*>& curTiles, std::vector<IntVector2>& dirs, std::map<Tile_Coord, Tile*>*& mapTiles, Tile* curTile, const IntVector2& dirTest)
{
	if (dirTest.x > 0)
	{
		FieldOfViewFloodFill::AddTileInSingleDir(curTiles, dirs, mapTiles, curTile->GetEastTileCoords(), IntVector2(1, 0));
	}
	else if (dirTest.x < 0)
	{
		FieldOfViewFloodFill::AddTileInSingleDir(curTiles, dirs, mapTiles, curTile->GetWestTileCoords(), IntVector2(-1, 0));
	}

	if (dirTest.y > 0)
	{
		FieldOfViewFloodFill::AddTileInSingleDir(curTiles, dirs, mapTiles, curTile->GetNorthTileCoords(), IntVector2(0, 1));
	}
	else if (dirTest.y < 0)
	{
		FieldOfViewFloodFill::AddTileInSingleDir(curTiles, dirs, mapTiles, curTile->GetSouthTileCoords(), IntVector2(0, -1));
	}

	if (dirTest.x > 0 && dirTest.y > 0)
	{
		FieldOfViewFloodFill::AddTileInSingleDir(curTiles, dirs, mapTiles, curTile->GetNorthEastTileCoords(), IntVector2(1, 1));
	}
	else if (dirTest.x > 0 && dirTest.y < 0)
	{
		FieldOfViewFloodFill::AddTileInSingleDir(curTiles, dirs, mapTiles, curTile->GetSouthEastTileCoords(), IntVector2(1, -1));
	}
	else if (dirTest.x < 0 && dirTest.y > 0)
	{
		FieldOfViewFloodFill::AddTileInSingleDir(curTiles, dirs, mapTiles, curTile->GetNorthWestTileCoords(), IntVector2(-1, 1));
	}
	else if (dirTest.x < 0 && dirTest.y < 0)
	{
		FieldOfViewFloodFill::AddTileInSingleDir(curTiles, dirs, mapTiles, curTile->GetSouthWestTileCoords(), IntVector2(-1, -1));
	}
}

//--------------------------------------------------------------
//public function
void FieldOfViewFloodFill::CalculateFieldOfViewForAgent(Agent* age, const float& viewDist, Map* map, std::vector<Agent*>& visibleAgentsFromAgent, const bool& useSeen)
{
	//actually permissive.
	if (age == nullptr)
	{
		return;
	}
	std::map<Tile_Coord, Tile*>* mapTiles = map->GetMapTiles();
	std::vector<Tile*> currentTiles;
	std::vector<IntVector2> directionsPerTile;
	std::vector<Tile*> newTiles;
	std::vector<IntVector2> newDirections;

	//get bl and tr corner of collision
	IntVector2 bl = age->GetBlTilePos();
	IntVector2 size = age->GetSize();
	IntVector2 tr = bl + size - IntVector2(1, 1);

	//First see if starting tile is solid
	//set up for Full size
	std::vector<Item*> visibleItems;
	std::vector<Feature*> visibleFeatures;


	for (int x = bl.x; x <= tr.x; x++)
	{
		for (int y = bl.y; y <= tr.y; y++)
		{
			std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(IntVector2(x, y));
			if (it != mapTiles->end() && it->second != nullptr)
			{
				TileType type = it->second->GetTileType();
				bool exists = false;
				bool solid = TileDefinition::GetTypeSolid(type, exists);
				AddItemsAtPositionToVector(visibleItems, IntVector2(x,y), useSeen);
				Feature* feat = Feature::GetFeatureAtPosition(IntVector2(x, y));
				if (feat != nullptr)
				{
					visibleFeatures.push_back(feat);
				}

				it->second->SetVisible(true);
				if (useSeen == true)
				{
					it->second->SetSeen(true);
				}

				if (exists == false || solid == true || (feat != nullptr && feat->GetCurrentlySolid() == true))
				{
					return;
				}
				if ((feat != nullptr && feat->GetCurrentlyBlockingLineOfSight() == true))
				{
					continue;
				}

				if (x == bl.x && y == bl.y)
				{
					FieldOfViewFloodFill::AddTilesInDir(currentTiles, directionsPerTile, mapTiles, it->second, IntVector2(-1, -1));
				}
				if (x == bl.x && y == (tr.y))
				{
					FieldOfViewFloodFill::AddTilesInDir(currentTiles, directionsPerTile, mapTiles, it->second, IntVector2(-1, 1));
				}
				if (x == (tr.x) && y == bl.y)
				{
					FieldOfViewFloodFill::AddTilesInDir(currentTiles, directionsPerTile, mapTiles, it->second, IntVector2(1, -1));
				}
				if (x == (tr.x) && y == (tr.y))
				{
					FieldOfViewFloodFill::AddTilesInDir(currentTiles, directionsPerTile, mapTiles, it->second, IntVector2(1, 1));
				}
				if (x == bl.x)
				{
					FieldOfViewFloodFill::AddTilesInDir(currentTiles, directionsPerTile, mapTiles, it->second, IntVector2(-1, 0));
				}
				if (x == (tr.x))
				{
					FieldOfViewFloodFill::AddTilesInDir(currentTiles, directionsPerTile, mapTiles, it->second, IntVector2(1, 0));
				}
				if (y == bl.y)
				{
					FieldOfViewFloodFill::AddTilesInDir(currentTiles, directionsPerTile, mapTiles, it->second, IntVector2(0, -1));
				}
				if (y == (tr.y))
				{
					FieldOfViewFloodFill::AddTilesInDir(currentTiles, directionsPerTile, mapTiles, it->second, IntVector2(0, 1));
				}
			}
			else
			{
				return;
			}
		}
	}

	float currentRadiusOut = 0.f;


	while (currentRadiusOut <= viewDist)
	{
		for (size_t i = 0; i < currentTiles.size(); i++)
		{
			TileType type = currentTiles.at(i)->GetTileType();
			bool exists = false;
			bool solid = TileDefinition::GetTypeSolid(type, exists);
			IntVector2 tileCoord = currentTiles.at(i)->GetTileCoords();
			Feature* feat = Feature::GetFeatureAtPosition(tileCoord);
			visibleFeatures.push_back(feat);

			AddAgentsAtPositionToVector(visibleAgentsFromAgent, tileCoord, useSeen);

			AddItemsAtPositionToVector(visibleItems, tileCoord, useSeen);

			currentTiles.at(i)->SetVisible(true);
			if (useSeen)
			{
				currentTiles.at(i)->SetSeen(true);
			}

			if (exists == false || solid == true || (feat != nullptr && feat->GetCurrentlyBlockingLineOfSight() == true))
			{
				continue;
			}
			else
			{
				FieldOfViewFloodFill::AddTilesInDir(newTiles, newDirections, mapTiles, currentTiles.at(i), directionsPerTile.at(i));
			}
		}

		currentTiles.clear();
		directionsPerTile.clear();
		currentTiles.insert(currentTiles.end(), newTiles.begin(), newTiles.end());
		directionsPerTile.insert(directionsPerTile.begin(), newDirections.begin(), newDirections.end());
		newTiles.clear();
		newDirections.clear();
		if (currentTiles.size() == 0)
		{
			break;
		}

		currentRadiusOut += 1.0f;
	}
	age->SetItemsVisible(visibleItems);
	age->SetVisibleFeatures(visibleFeatures);
}
void FieldOfViewFloodFill::AddAgentsAtPositionToVector(std::vector<Agent*>& visibleAgentsFromAgent, const IntVector2& tileCoord, const bool& useSeen)
{
	for (size_t ageIndex = 0; ageIndex < Agent::s_Agents.size(); ageIndex++)
	{
		Agent* agent = Agent::s_Agents.at(ageIndex);
		if (agent != nullptr)
		{
			IntVector2 bltest = agent->GetBlTilePos();
			IntVector2 trtest = bltest + agent->GetSize() - IntVector2(1, 1);

			if (tileCoord.x >= bltest.x && tileCoord.x <= trtest.x
				&& tileCoord.y >= bltest.y && tileCoord.y <= trtest.y)
			{
				bool alreadyExists = false;
				for (size_t ind = 0; ind < visibleAgentsFromAgent.size(); ind++)
				{
					if (agent == visibleAgentsFromAgent.at(ind))
					{
						alreadyExists = true;
						break;
					}
				}
				if (alreadyExists == false)
				{
					visibleAgentsFromAgent.push_back(agent);
				}
				if (useSeen == true)
				{
					agent->SetVisibleToPlayer(true);
				}
			}
			else
			{
				if (useSeen == true)
				{
					agent->SetVisibleToPlayer(false);
				}
			}
		}
	}
}
void FieldOfViewFloodFill::AddItemsAtPositionToVector(std::vector<Item*>& visibleItems, const IntVector2& tileCoord, const bool& useSeen)
{
	for (size_t itemIdx = 0; itemIdx < Item::s_ItemsOnTheGround.size(); itemIdx++)
	{
		Item* item = Item::s_ItemsOnTheGround.at(itemIdx);
		if (item != nullptr)
		{
			IntVector2 bltest = item->GetBlTilePos();
			IntVector2 trtest = bltest + item->GetSize() - IntVector2(1, 1);

			if (tileCoord.x >= bltest.x && tileCoord.x <= trtest.x
				&& tileCoord.y >= bltest.y && tileCoord.y <= trtest.y)
			{
				bool alreadyExists = false;
				for (size_t ind = 0; ind < visibleItems.size(); ind++)
				{
					if (item == visibleItems.at(ind))
					{
						alreadyExists = true;
						break;
					}
				}
				if (alreadyExists == false)
				{
					visibleItems.push_back(item);
				}
				if (useSeen == true)
				{
					item->SetVisibleToPlayer(true);
				}
			}
			else
			{
				if (useSeen == true)
				{
					item->SetVisibleToPlayer(false);
				}
			}
		}
	}
}