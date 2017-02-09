#pragma once

#ifndef TUNNEL_HPP 
#define TUNNEL_HPP
#include "Engine/Math/IntVector2.hpp"
#include "Game/Generators/MapGenerators/DungeonMap/DungeonMapGen_Room.hpp"
#include "Game/Map/TileType.hpp"
#include <vector>


struct TunnelLine
{
public:
	IntVector2 startPoint;
	IntVector2 endPoint;
	float m_startTunnelSize;
	float m_endTunnelSize;

	TunnelLine();
	TunnelLine(const IntVector2& start, const IntVector2& end, const float& size);
	TunnelLine(const IntVector2& start, const IntVector2& end, const float& startSize, const float& endSize);
	TunnelLine(const TunnelLine& other);

	const bool IsTileCoordInTunnel(const IntVector2& point, float& distanceAway, IntVector2& nearestPoint) const;
	const bool operator==(const TunnelLine& other) const;
private:
};

class Tunnel
{
private:
	std::vector<TunnelLine> m_TunnelLines;
	TileType m_tileType;
public:
	Tunnel(const TileType& tileType = TILE_INVALID);
	~Tunnel();

	void SetTileType(const TileType& tileType);
	const TileType GetTileType();
	void AddTunnelLine(const IntVector2& start, const IntVector2& end, const float& size);
	void AddTunnelLine(const IntVector2& start, const IntVector2& end, const float& startSize, const float& endSize);
	void AddSquareTunnelLine(const IntVector2& start, const IntVector2& end, const float& size);
	const bool DetectTileInATunnel(const IntVector2& tileCoords, IntVector2& nearestPoint, float& nearestDistance);
	const std::vector<TunnelLine> GetTunnelLines() const;


	static const void ClearTunnels();
	static const bool TileIsInATunnel(const IntVector2& tileCoords, TileType& tileTypeOfTunnel, IntVector2& nearestTunnelPoint, float& nearestTunnelDistance, Tunnel*& nearestTunnel);
	static const bool DetectTunnelConnectsRoom(DungeonMapGen_Room* room, IntVector2& nearestPoint, IntVector2& nearestEndPoint, float& nearestDistance, Tunnel*& nearestTunnel);
	static const bool isThereATunnelThatHasThisTunnelLineOfSameTypeAlready(const TunnelLine& line, const TileType& typ);

	static std::vector<Tunnel*> g_tunnels;
};
#endif