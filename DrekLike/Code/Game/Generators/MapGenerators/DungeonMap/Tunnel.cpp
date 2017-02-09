#include "Game/Generators/MapGenerators/DungeonMap/Tunnel.hpp"
#include "Game/GameCommonFunctions.hpp"
#include "Engine/Math/Vector2.hpp"

//-----------------------------------------------------------------
//TunnelLine

TunnelLine::TunnelLine()
	: startPoint(0,0),
	endPoint(0,0),
	m_startTunnelSize(0.f),
	m_endTunnelSize(0.f)
{

}
TunnelLine::TunnelLine(const IntVector2& start, const IntVector2& end, const float& size)
	: startPoint(start),
	endPoint(end),
	m_startTunnelSize(size),
	m_endTunnelSize(size)
{
}

TunnelLine::TunnelLine(const IntVector2& start, const IntVector2& end, const float& startSize, const float& endSize)
	: startPoint(start),
	endPoint(end),
	m_startTunnelSize(startSize),
	m_endTunnelSize(endSize)
{

}
TunnelLine::TunnelLine(const TunnelLine& other)
	: startPoint(other.startPoint),
	endPoint(other.endPoint),
	m_startTunnelSize(other.m_startTunnelSize),
	m_endTunnelSize(other.m_endTunnelSize)
{
}

const bool TunnelLine::IsTileCoordInTunnel(const IntVector2& point, float& distanceAway, IntVector2& nearestPoint) const
{
	bool isInTunnel = false;
	LineTraceStruct lineTrace = LoadLineTraceStruct(startPoint, endPoint);
	bool hasTestedOnce = false;
	IntVector2 tCurrent = startPoint;
	float radiusDiff = m_endTunnelSize - m_startTunnelSize;
	float startDistance = (point - startPoint).getLength();

	while (tCurrent != endPoint)
	{
		if (abs(lineTrace.tOfNextCrossing.x < abs(lineTrace.tOfNextCrossing.y)))
		{
			if (abs(lineTrace.tOfNextCrossing.x) > 1.0f)
			{
				break;
			}
			tCurrent.x += lineTrace.tileStep.x;
			float currentDist = (point - startPoint).getLength();
			float percentageDist = currentDist / startDistance;
			if (CircleTest(tCurrent, point, (m_startTunnelSize + (radiusDiff * percentageDist))))
			{
				isInTunnel = true;
				if (currentDist < distanceAway)
				{
					nearestPoint = tCurrent;
					distanceAway = currentDist;
				}
				break;
			}
			else if (hasTestedOnce == false || (currentDist < distanceAway))
			{
				hasTestedOnce = true;
				if (currentDist < distanceAway)
				{
					nearestPoint = tCurrent;
					distanceAway = currentDist;
				}
			}
			lineTrace.tOfNextCrossing.x += lineTrace.tDelta.x;
		}
		else
		{
			if (abs(lineTrace.tOfNextCrossing.y) > 1.0f)
			{
				break;
			}
			tCurrent.y += lineTrace.tileStep.y;
			float currentDist = (point - startPoint).getLength();
			float percentageDist = currentDist / startDistance;
			if (CircleTest(tCurrent, point, (m_startTunnelSize + (radiusDiff * percentageDist))))
			{
				isInTunnel = true;
				if (currentDist < distanceAway)
				{
					nearestPoint = tCurrent;
					distanceAway = currentDist;
				}
				break;
			}
			else if(hasTestedOnce == false || ( currentDist < distanceAway))
			{
				hasTestedOnce = true;
				if (currentDist < distanceAway)
				{
					nearestPoint = tCurrent;
					distanceAway = currentDist;
				}
			}
			lineTrace.tOfNextCrossing.y += lineTrace.tDelta.y;
		}
	}

	return isInTunnel;
}
const bool TunnelLine::operator==(const TunnelLine& other) const
{
	if (other.m_endTunnelSize == m_endTunnelSize
		&& other.m_startTunnelSize == m_startTunnelSize
		&& other.startPoint == startPoint
		&& other.endPoint == endPoint)
	{
		return true;
	}
	return false;
}


std::vector<Tunnel*> Tunnel::g_tunnels;

Tunnel::Tunnel(const TileType& tileType)
	: m_tileType(tileType)
{

}
Tunnel::~Tunnel()
{

}

void Tunnel::SetTileType(const TileType& tileType)
{
	m_tileType = tileType;
}

const TileType Tunnel::GetTileType()
{
	return m_tileType;
}
void Tunnel::AddTunnelLine(const IntVector2& start, const IntVector2& end, const float& size)
{
	TunnelLine line(start, end, size);
	m_TunnelLines.push_back(line);
}
void  Tunnel::AddTunnelLine(const IntVector2& start, const IntVector2& end, const float& startSize, const float& endSize)
{
	TunnelLine line(start, end, startSize, endSize);
	m_TunnelLines.push_back(line);
}
void Tunnel::AddSquareTunnelLine(const IntVector2& start, const IntVector2& end, const float& size)
{
	int xDiff = end.x - start.x;
	int yDiff = end.y - start.y;
	if (abs(yDiff) > xDiff)
	{
		TunnelLine line(start, IntVector2(start.x, end.y), size);
		TunnelLine line1(IntVector2(start.x, end.y), end, size);
		if (isThereATunnelThatHasThisTunnelLineOfSameTypeAlready(line, m_tileType) == false)
		{
			m_TunnelLines.push_back(line);
		}
		if (isThereATunnelThatHasThisTunnelLineOfSameTypeAlready(line1, m_tileType) == false)
		{
			m_TunnelLines.push_back(line1);
		}
	}
	else
	{
		TunnelLine line(start, IntVector2(end.x, start.y), size);
		TunnelLine line1(IntVector2(end.x, start.y), end, size);
		if (isThereATunnelThatHasThisTunnelLineOfSameTypeAlready(line, m_tileType) == false)
		{
			m_TunnelLines.push_back(line);
		}
		if (isThereATunnelThatHasThisTunnelLineOfSameTypeAlready(line1, m_tileType) == false)
		{
			m_TunnelLines.push_back(line1);
		}
	}

}
const bool Tunnel::DetectTileInATunnel(const IntVector2& tileCoords, IntVector2& nearestPoint, float& nearestDistance)
{
	TunnelLine nearestTunnelLine;
	nearestDistance = 9999999999999999.f;
	nearestPoint;
	for (size_t i = 0; i < m_TunnelLines.size(); i++)
	{
		IntVector2 point;
		float nearDist = 999999999999999999.f;
		if (m_TunnelLines.at(i).IsTileCoordInTunnel(tileCoords, nearDist, point) == true)
		{
			if (nearDist < nearestDistance)
			{
				nearestDistance = nearDist;
				nearestPoint = point;
			}
			return true;
		}
		else if (nearDist < nearestDistance)
		{
			nearestDistance = nearDist;
			nearestPoint = point;
		}
	}
	return false;
}
const std::vector<TunnelLine> Tunnel::GetTunnelLines() const
{
	return m_TunnelLines;
}


const void Tunnel::ClearTunnels()
{
	for (size_t i = 0; i < g_tunnels.size(); i++)
	{
		if (g_tunnels.at(i) != nullptr)
		{
			delete g_tunnels.at(i);
			g_tunnels[i] = nullptr;
		}
	}
	g_tunnels.clear();
}
const bool Tunnel::TileIsInATunnel(const IntVector2& tileCoords, TileType& tileTypeOfTunnel, IntVector2& nearestTunnelPoint, float& nearestTunnelDistance, Tunnel*& nearestTunnel)
{
	nearestTunnelDistance = 99999999999999999999.f;
	for (size_t i = 0; i < g_tunnels.size(); i++)
	{
		IntVector2 nearestPoint;
		float nearestDist = 999999999999999999999.f;
		if (g_tunnels.at(i)->DetectTileInATunnel(tileCoords, nearestPoint, nearestDist))
		{
			tileTypeOfTunnel = g_tunnels.at(i)->GetTileType();
			if (nearestDist < nearestTunnelDistance)
			{
				nearestTunnelDistance = nearestDist;
				nearestTunnelPoint = nearestPoint;
				nearestTunnel = nullptr;
				nearestTunnel = g_tunnels.at(i);
			}
			return true;
		}
		else if (nearestDist < nearestTunnelDistance)
		{
			nearestTunnelDistance = nearestDist;
			nearestTunnelPoint = nearestPoint;
			nearestTunnel = nullptr;
			nearestTunnel = g_tunnels.at(i);
		}
	}
	return false;
}

const bool Tunnel::DetectTunnelConnectsRoom(DungeonMapGen_Room* room, IntVector2& nearestPoint, IntVector2& nearestEndPoint, float& nearestDistance, Tunnel*& nearestTunnel)
{
	if (room == nullptr)
	{
		return false;
	}
	IntVector2 startRoom = room->GetStartTile();
	IntVector2 endRoom = startRoom + room->GetSize();
	Tunnel* tunTest = nullptr;
	nearestDistance = 9999999999999999999999.f;
	nearestPoint = IntVector2(0, 0);
	TileType type;

	//
	for (int x = startRoom.x; x < endRoom.x; x++)
	{
		IntVector2 southSide = IntVector2(x, startRoom.y);
		IntVector2 northSide = IntVector2(x, endRoom.y);
		float distance = 999999999999999999999.f;
		IntVector2 nearPoint;
		if (Tunnel::TileIsInATunnel(southSide, type, nearPoint, distance, tunTest))
		{
			return true;
		}
		else if (distance < nearestDistance)
		{
			nearestDistance = distance;
			nearestPoint = nearPoint;
			nearestEndPoint = southSide;
			nearestTunnel = nullptr;
			nearestTunnel = tunTest;
		}
		if (Tunnel::TileIsInATunnel(northSide, type, nearPoint, distance, tunTest))
		{
			return true;
		}
		else if (distance < nearestDistance)
		{
			nearestDistance = distance;
			nearestPoint = nearPoint;
			nearestEndPoint = northSide;
			nearestTunnel = nullptr;
			nearestTunnel = tunTest;
		}
	}
	for (int y = startRoom.y; y < endRoom.y; y++)
	{
		IntVector2 westSide = IntVector2(startRoom.x, y);
		IntVector2 eastSide = IntVector2(endRoom.x, y);
		float distance = 999999999999999999999.f;
		IntVector2 nearPoint;
		if (Tunnel::TileIsInATunnel(westSide, type, nearPoint, distance, tunTest))
		{
			return true;
		}
		else if (distance < nearestDistance)
		{
			nearestDistance = distance;
			nearestPoint = nearPoint;
			nearestEndPoint = westSide;
			nearestTunnel = nullptr;
			nearestTunnel = tunTest;
		}
		if (Tunnel::TileIsInATunnel(eastSide, type, nearPoint, distance, tunTest))
		{
			return true;
		}
		else if (distance < nearestDistance)
		{
			nearestDistance = distance;
			nearestPoint = nearPoint;
			nearestEndPoint = eastSide;
			nearestTunnel = nullptr;
			nearestTunnel = tunTest;
		}
	}
	return false;
}
const bool Tunnel::isThereATunnelThatHasThisTunnelLineOfSameTypeAlready(const TunnelLine& line, const TileType& typ)
{
	for (size_t tunnelIdx = 0; tunnelIdx < g_tunnels.size(); tunnelIdx++)
	{
		Tunnel* tun = g_tunnels.at(tunnelIdx);
		if (tun != nullptr)
		{
			TileType type = tun->GetTileType();
			std::vector<TunnelLine> lines = tun->GetTunnelLines();
			for (size_t lineIdx = 0; lineIdx < lines.size(); lineIdx++)
			{
				if (lines.at(lineIdx) == line && type == typ)
				{
					return true;
				}
			}
		}
	}

	return false;
}