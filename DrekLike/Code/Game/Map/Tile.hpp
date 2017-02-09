#pragma once

#ifndef TILE_HPP
#define TILE_HPP

#include "Game/CommonDefinitions.hpp"
#include "Game/Map/TileType.hpp"

class Tile
{
private:
	//Fast reference:
	//SW corner = GetTileCoords()
	//NW corenr = GetNorthTileCoords()
	//NE corner = GetNorthEastTileCoords();
	//SE corner = GetEastTileCoords()

	Tile_Coord m_tileCoord;
	TileType m_tileType;
	TileType m_nextType;
	bool m_visited;
	bool m_visible;
	bool m_seen;
public:
	Tile(const Tile_Coord& coords = Tile_Coord(0,0), const TileType& type = TILE_INVALID);
	Tile(const Tile& tile);
	~Tile();

	void SetVisited(const bool& visited) { m_visited = visited; };
	void SetSeen(const bool& seen) { m_seen = seen; };
	void SetVisible(const bool& visible) { m_visible = visible; };
	void SetTileCoord(const Tile_Coord& coords);
	void SetTileType(const TileType& type);
	void SetNextTileType(const TileType& type);
	void ChangeToNextTileType();
	void ClearNextType();

	inline const bool GetVisited() const { return m_visited; };
	inline const Tile_Coord GetTileCoords() const;
	inline const TileType GetTileType() const;
	const TileType GetNextTileType() const;
	const bool GetVisible() const { return m_visible; };
	const bool GetSeen() const { return m_seen; };
	inline const Tile_Coord GetNorthTileCoords() const { return Tile_Coord(m_tileCoord.x, m_tileCoord.y + 1); }; //nw corner
	inline const Tile_Coord GetSouthTileCoords() const { return Tile_Coord(m_tileCoord.x, m_tileCoord.y - 1); };
	inline const Tile_Coord GetWestTileCoords() const { return Tile_Coord(m_tileCoord.x - 1, m_tileCoord.y); };
	inline const Tile_Coord GetEastTileCoords() const { return Tile_Coord(m_tileCoord.x + 1, m_tileCoord.y); }; //se corner
	inline const Tile_Coord GetNorthEastTileCoords() const { return Tile_Coord(m_tileCoord.x + 1, m_tileCoord.y + 1); }; //ne corner
	inline const Tile_Coord GetNorthWestTileCoords() const { return Tile_Coord(m_tileCoord.x - 1, m_tileCoord.y + 1); }; 
	inline const Tile_Coord GetSouthEastTileCoords() const { return Tile_Coord(m_tileCoord.x + 1, m_tileCoord.y - 1); };
	inline const Tile_Coord GetSouthWestTileCoords() const { return Tile_Coord(m_tileCoord.x - 1, m_tileCoord.y - 1); };
};
#endif