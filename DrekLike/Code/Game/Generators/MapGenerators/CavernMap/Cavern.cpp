#include "Game/Generators/MapGenerators/CavernMap/Cavern.hpp"
#include "Game/Generators/MapGenerators/CavernMap/PerlinWorm.hpp"

const int Cavern::minNumberOfTunnelLines = 4;
const int Cavern::addRandNumOfTunnelLines = 3;
const float Cavern::minCavernSize = 1.0f;
const float Cavern::addRandCavernSize = 5.0f;
const int Cavern::minNumberOfCaverns = 1;
const int Cavern::addRandNumOfCaverns = 6;
std::vector<Cavern*> Cavern::g_Caverns;


Cavern::Cavern(const TileType& type, const IntVector2& startPos, const IntVector2& mapSize)
	: m_tileType(type),
	startPos(startPos),
	m_quadrantSize(mapSize),
	m_tunnel(nullptr)
{
}
Cavern::~Cavern()
{
	if (m_tunnel != nullptr)
	{
		delete m_tunnel;
		m_tunnel = nullptr;
	}
}

void Cavern::GenerateNTunnels(const int& numberOfTunnels)
{
	IntVector2 start = startPos +  IntVector2( (rand() % m_quadrantSize.x), (rand() % m_quadrantSize.y));
	float startTunnelSize = minCavernSize + (float)(rand() % (int)addRandCavernSize);
	float startYaw = (float)(rand() % 360);
	m_tunnel = new Tunnel();

	PerlinWorm worm(start, startYaw);

	for (int i = 0; i < numberOfTunnels; i++)
	{
		IntVector2 nextPoint = worm.GetNextPoint();
		float newTunnelSize = minCavernSize + (float)(rand() % (int)addRandCavernSize);
		m_tunnel->AddTunnelLine(start, nextPoint, startTunnelSize, newTunnelSize);
		start = nextPoint;
		startTunnelSize = newTunnelSize;
	}


}
const bool Cavern::IsTileCoordInCavern(const IntVector2& tileCoord) const
{
	IntVector2 realCoord = tileCoord - startPos;
	if (realCoord.x < 0 || realCoord.y < 0
		|| realCoord.x >= m_quadrantSize.x
		|| realCoord.y >= m_quadrantSize.y)
	{
		return false;
	}
	std::string type;
	IntVector2 nearestTunnel;
	float dist;
	if (m_tunnel->DetectTileInATunnel(tileCoord, nearestTunnel, dist))
	{
		return true;
	}
	return false;
}
const TileType Cavern::GetTileType() const
{
	return m_tileType;
}

const void Cavern::GenerateCavern(const IntVector2& start, const IntVector2& quadrantSize, const TileType& type, int numberOfTileLines)
{
	Cavern* cavern = new Cavern(type, start, quadrantSize);

	if (numberOfTileLines < 0)
	{
		numberOfTileLines = (rand() % addRandNumOfTunnelLines) + minNumberOfTunnelLines;
	}
	cavern->GenerateNTunnels(numberOfTileLines);
	g_Caverns.push_back(cavern);
}
const bool Cavern::IsTileCoordInACavern(Cavern*& cavern, const IntVector2& tileCoord)
{
	cavern = nullptr;
	for (size_t i = 0; i < g_Caverns.size(); i++)
	{
		if (g_Caverns.at(i) != nullptr && g_Caverns.at(i)->IsTileCoordInCavern(tileCoord))
		{
			cavern = g_Caverns.at(i);
			return true;
		}
	}
	return false;
}

const void Cavern::ClearCaverns()
{
	for (size_t i = 0;i < g_Caverns.size(); i++)
	{
		if (g_Caverns.at(i) != nullptr)
		{
			delete g_Caverns.at(i);
			g_Caverns[i] = nullptr;
		}
	}
}