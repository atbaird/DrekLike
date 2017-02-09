#pragma once

#ifndef DUNGEONMAPGEN_ROOM_HPP
#define DUNGEONMAPGEN_ROOM_HPP

#include <vector>
#include "Engine/Math/IntVector2.hpp"
#include "Game/Map/TileType.hpp"

class DungeonMapGen_Room;


class DungeonMapGen_Room
{
private:
	static const IntVector2 minRoomSize;
	static const IntVector2 randomPossibleIncreasedRoomSize;

	IntVector2 m_startTile;
	IntVector2 m_size;
	TileType m_roomTileType;

public:
	DungeonMapGen_Room();
	DungeonMapGen_Room(const IntVector2& start, const IntVector2& size, const IntVector2& mapSize, const TileType& type);
	DungeonMapGen_Room(const DungeonMapGen_Room& other);
	~DungeonMapGen_Room();

	void SetRoomTileType(const TileType& type);
	const IntVector2 GetStartTile() const;
	const IntVector2 GetSize() const;
	const IntVector2 GetCenter() const;
	const TileType GetRoomTileType() const;
	const bool IsTileLocInRoom(const IntVector2& tileLoc) const;
	const float GetNearestPositionFromPointToRoom(const IntVector2& point, IntVector2& nearestPos) const;

	static const void ClearRooms();
	static const bool IsTileInARoom(const IntVector2& tileCoord, TileType& tileTypeOfRoom);
	static const bool VerifyTileCoordIsAlreadyExistingStartCoord(const IntVector2& tileCoord);
	static const void GenerateRooms(const IntVector2& start, const IntVector2& quadrantSize, const int& numberOfRooms, const TileType& type = TILE_AIR);

	static std::vector<DungeonMapGen_Room*> g_rooms;
};
#endif