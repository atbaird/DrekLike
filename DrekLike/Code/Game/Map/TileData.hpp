#pragma once

#ifndef TILEDATA_HPP
#define TILEDATA_HPP
#include "Engine/Math/IntVector2.hpp"
#include "Game/Map/TileType.hpp"

class TileData
{
public:
	TileData(const IntVector2& min = IntVector2(0,0), const IntVector2& max = IntVector2(20,20), const TileType& typ = TILE_AIR);

	IntVector2 m_mins;
	IntVector2 m_maxs;
	TileType m_type;
};
#endif