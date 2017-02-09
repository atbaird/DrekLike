#include "Game/Map/Tile.hpp"


Tile::Tile(const Tile_Coord& coords, const TileType& type)
	:m_tileCoord(coords),
	m_tileType(type),
	m_nextType(type),
	m_visible(false),
	m_seen(false)
{

}
Tile::Tile(const Tile& tile)
	: m_tileCoord(tile.GetTileCoords()),
	m_tileType(tile.GetTileType()),
	m_nextType(tile.GetNextTileType()),
	m_visible(tile.m_visible),
	m_seen(tile.m_seen)
{
}
Tile::~Tile()
{

}

void Tile::SetTileCoord(const Tile_Coord& coords)
{
	m_tileCoord = coords;
}
void Tile::SetTileType(const TileType& type)
{
	m_tileType = type;
}
void Tile::SetNextTileType(const TileType& type)
{
	m_nextType = type;
}
void Tile::ChangeToNextTileType()
{
	if (m_nextType == TILE_INVALID)
	{
		return;
	}
	m_tileType = m_nextType;
	m_nextType = TILE_INVALID;
}
void Tile::ClearNextType()
{
	m_nextType = TILE_INVALID;
}

const Tile_Coord Tile::GetTileCoords() const
{
	return m_tileCoord;
}
const TileType Tile::GetTileType() const
{
	return m_tileType;
}
const TileType Tile::GetNextTileType() const
{
	return m_nextType;
}