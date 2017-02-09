#pragma once

#ifndef TILETYPE_HPP
#define TILETYPE_HPP
#include <vector>
#include <string>

enum TileType
{
	TILE_INVALID = -1,
	TILE_AIR,
	TILE_STONE,
	TILE_WATER,
	TILE_LAVA,
	NUM_TILE_TYPES
};

//#define TileType std::string
//#define STATIC
//extern STATIC std::vector<TileType> g_TileTypes;

#endif // !TILETYPE_HPP
