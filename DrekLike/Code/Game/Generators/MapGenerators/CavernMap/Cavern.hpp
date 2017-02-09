#pragma once

#ifndef CAVERN_HPP
#define CAVERN_HPP
#include "Game/Generators/MapGenerators/DungeonMap/Tunnel.hpp"

class Cavern
{
private:
	Tunnel* m_tunnel;
	IntVector2 startPos;
	IntVector2 m_quadrantSize;
	TileType m_tileType;
public:

	Cavern(const TileType& type = TILE_AIR, const IntVector2& startPos = IntVector2(0,0), const IntVector2& quadrantSize = IntVector2(20,20));
	~Cavern();

	void GenerateNTunnels(const int& numberOfTunnels);
	const bool IsTileCoordInCavern(const IntVector2& tileCoord) const;
	const TileType GetTileType() const;

	static const void GenerateCavern(const IntVector2& startCoord, const IntVector2& quadrantSize, const TileType& type = TILE_AIR, int numberOfTileLines = -1);
	static const bool IsTileCoordInACavern(Cavern*& cavern, const IntVector2& TileCoord);
	static const void ClearCaverns();

	static const int minNumberOfTunnelLines;
	static const int addRandNumOfTunnelLines;
	static const float minCavernSize;
	static const float addRandCavernSize;
	static const int minNumberOfCaverns;
	static const int addRandNumOfCaverns;
	static std::vector<Cavern*> g_Caverns;
};
#endif