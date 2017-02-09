#include "Game/Generators/MapGenerators/DungeonMap/DungeonMapGen_Room.hpp"
#include "Game/Map/TileDefinition.hpp"

std::vector<DungeonMapGen_Room*> DungeonMapGen_Room::g_rooms;

const IntVector2 DungeonMapGen_Room::minRoomSize = IntVector2(3, 3);
const IntVector2 DungeonMapGen_Room::randomPossibleIncreasedRoomSize = IntVector2(5,5);

DungeonMapGen_Room::DungeonMapGen_Room()
	: m_startTile(0,0),
	m_size(0,0),
	m_roomTileType(TILE_AIR)
{
}
DungeonMapGen_Room::DungeonMapGen_Room(const IntVector2& start, const IntVector2& size, const IntVector2& mapSize, const TileType& type)
	: m_startTile(start),
	m_size(0,0),
	m_roomTileType(type)
{
	IntVector2 endLoc = m_startTile + size;
	if (endLoc.x < mapSize.x || endLoc.x < (start.x + size.x))
	{
		m_size.x = size.x;
	}
	else
	{
		int difference = (endLoc.x - mapSize.x) + 1;
		m_size.x = size.x - difference;
	}
	if (endLoc.y < mapSize.y || endLoc.y < (start.y + size.y))
	{
		m_size.y = size.y;
	}
	else
	{
		int difference = (endLoc.y - mapSize.y) + 1;
		m_size.y = size.y - difference;
	}
}
DungeonMapGen_Room::DungeonMapGen_Room(const DungeonMapGen_Room& other)
	: m_startTile(other.m_startTile),
	m_size(other.m_size),
	m_roomTileType(other.m_roomTileType)
{
}
DungeonMapGen_Room::~DungeonMapGen_Room()
{
}

const IntVector2 DungeonMapGen_Room::GetStartTile() const
{
	return m_startTile;
}
const IntVector2 DungeonMapGen_Room::GetSize() const
{
	return m_size;
}
void DungeonMapGen_Room::SetRoomTileType(const TileType& type)
{
	m_roomTileType = type;
}

const IntVector2 DungeonMapGen_Room::GetCenter() const
{
	return m_startTile + (m_size / 2.f);
}
const TileType DungeonMapGen_Room::GetRoomTileType() const
{
	return m_roomTileType;
}
const bool DungeonMapGen_Room::IsTileLocInRoom(const IntVector2& tileLoc) const
{
	const IntVector2 endLoc = m_startTile + m_size;
	if (tileLoc.x >= m_startTile.x
		&& tileLoc.y >= m_startTile.y
		&& tileLoc.x <= endLoc.x
		&& tileLoc.y <= endLoc.y)
	{
		return true;
	}
	return false;
}

const void DungeonMapGen_Room::ClearRooms()
{
	for (size_t i = 0; i < g_rooms.size(); i++)
	{
		if (g_rooms.at(i) != nullptr)
		{
			delete g_rooms.at(i);
			g_rooms[i] = nullptr;
		}
	}
	g_rooms.clear();
}

const bool DungeonMapGen_Room::IsTileInARoom(const IntVector2& tileCoord, TileType& tileTypeOfRoom)
{
	for (size_t i = 0; i < g_rooms.size(); i++)
	{
		if (g_rooms.at(i)->IsTileLocInRoom(tileCoord) == true)
		{
			tileTypeOfRoom = g_rooms.at(i)->GetRoomTileType();
			return true;
		}
	}
	return false;
}

const float DungeonMapGen_Room::GetNearestPositionFromPointToRoom(const IntVector2& point, IntVector2& nearestPos) const
{
	IntVector2 maxPosition = m_startTile + m_size;
	bool testedOnce = false;
	float nearestDistance = 99999999.f;
	for (int x = m_startTile.x; x < maxPosition.x; x++)
	{
		for (int y = m_startTile.y; y < maxPosition.y; y++)
		{
			float distance = (IntVector2(x, y) - point).getLength();
			if (testedOnce == false || (abs(distance) < abs(nearestDistance)))
			{
				testedOnce = true;
				nearestDistance = distance;
				nearestPos = IntVector2(x, y);
			}
		}
	}
	return (float)nearestDistance;
}
const bool DungeonMapGen_Room::VerifyTileCoordIsAlreadyExistingStartCoord(const IntVector2& tileCoord)
{
	for (size_t i = 0; i < g_rooms.size(); i++)
	{
		if (g_rooms.at(i)->GetStartTile() == tileCoord)
		{
			return true;
		}
	}
	return false;
}
const void DungeonMapGen_Room::GenerateRooms(const IntVector2& start, const IntVector2& quadrantSize, const int& numberOfRooms, const TileType& type)
{
	if (numberOfRooms > (quadrantSize.x - start.x) * (quadrantSize.y - start.y))
	{
		return;
	}

	for (int i = 0; i < numberOfRooms; i++)
	{
		IntVector2 startPosition = start + IntVector2(rand() % quadrantSize.x, rand() % quadrantSize.y);
		if (VerifyTileCoordIsAlreadyExistingStartCoord(startPosition) == false)
		{
			IntVector2 roomSize = IntVector2(minRoomSize.x + (rand() % randomPossibleIncreasedRoomSize.x), minRoomSize.y + (rand() % randomPossibleIncreasedRoomSize.y));
			//int index = rand() % g_TileDefinitions.size()- 1;
			DungeonMapGen_Room* newRoom = new DungeonMapGen_Room(startPosition, roomSize, quadrantSize, type);
			g_rooms.push_back(newRoom);
		}
		else
		{
			i--;
		}
	}
}