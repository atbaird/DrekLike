#pragma once

#ifndef FIELDOFVIEWPERMISSIVE_HPP
#define FIELDOFVIEWPERMISSIVE_HPP
#include "Game/FieldOfView/FieldOfView.hpp"

class FieldOfViewFloodFill : public FieldOfView
{
private:
public:
	//constructors
	FieldOfViewFloodFill();
	~FieldOfViewFloodFill();

	//public function
	static void CalculateFieldOfViewForAgent(Agent* age, const float& viewDist, Map* map, std::vector<Agent*>& visibleAgentsFromAgent, const bool& useSeen = false);
private:

	//private functions
	static const bool IsTileAlreadyInCurTiles(std::vector<Tile*>& curTiles, Tile* tile);
	static void AddTileInSingleDir(std::vector<Tile*>& curTiles, std::vector<IntVector2>& dirs, std::map<Tile_Coord, Tile*>*& mapTiles,
		Tile_Coord coordsTest, const IntVector2& dirToAddIfTrue);
	static void AddTilesInDir(std::vector<Tile*>& curTiles, std::vector<IntVector2>& dirs, std::map<Tile_Coord, Tile*>*& mapTiles, Tile* curTile, const IntVector2& dirTest);

	static void AddAgentsAtPositionToVector(std::vector<Agent*>& visibleAgentsFromAgent, const IntVector2& tileCoord, const bool& useSeen);
	static void AddItemsAtPositionToVector(std::vector<Item*>& visibleItems, const IntVector2& tileCoord, const bool& useSeen);
};
#endif