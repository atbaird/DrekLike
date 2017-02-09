#include "Game/Generators/MapGenerators/DungeonMapGenerator.hpp"
#include "Game/Generators/MapGenerators/DungeonMap/DungeonMapGen_Room.hpp"
#include "Game/Generators/MapGenerators/DungeonMap/Tunnel.hpp"
#include "Game/Features/FeatureFactory.hpp"
#include <algorithm>
#include "Engine/Math/Noise.hpp"

const GeneratorRegistration DungeonMapGenerator::DungeonMapRegistration = GeneratorRegistration("DungeonMapGenerator", CreateDungeonMapGenerator, DungeonGenerationProcessCreateFunc);

const IntVector2 DungeonMapGenerator::rangeOutForPerlinWorms = IntVector2(10,10);
const IntVector2 DungeonMapGenerator::minRoomSize = IntVector2(2,2);
const IntVector2 DungeonMapGenerator::maxRoomSize = IntVector2(8,8);

Generator* CreateDungeonMapGenerator(const std::string& name)
{
	DungeonMapGenerator* generator = new DungeonMapGenerator();
	generator->SetName(name);
	return (Generator*)generator;
}
EnvironmentGenerationProcess* DungeonGenerationProcessCreateFunc(const XMLNode& node)
{
	EnvironmentGenerationProcess* newEGP = new EnvironmentGenerationProcess();
	newEGP->SetName("CurrentDungeonGenerationProcess");

	for (int i = 0; i < node.nAttribute(); i++)
	{
		XMLAttribute attr = node.getAttribute(i);
		std::string name = attr.lpszName;
		std::string val = attr.lpszValue;
		newEGP->AddAttributeData(attr);
	}
	return newEGP;
}

DungeonMapGenerator::DungeonMapGenerator()
	: MapGenerator(),
	m_makeDoors(false)
{
}

DungeonMapGenerator::~DungeonMapGenerator()
{
	Tunnel::ClearTunnels();
	DungeonMapGen_Room::ClearRooms();
}

const void DungeonMapGenerator::GenerateBasedOnDataInput(EnvironmentGenerationProcess* data, Map*& map)
{
	m_makeDoors = false;
	std::string defHolder = "";
	TileType surface = TILE_AIR;
	int tempInt = 0;
	int numSteps = 0;
	int iterations = 1;
	int quantity = 5;
	IntVector2 rangeMins;
	IntVector2 rangeMaxs = map->GetMapSize();
	defHolder = data->GetAttrValueByName("steps");
	if (defHolder != "")
	{
		tempInt = atoi(defHolder.c_str());
		if (tempInt >= 0)
		{
			numSteps = tempInt;
		}
		tempInt = 0;
		defHolder = "";
	}

	defHolder = data->GetAttrValueByName("surface");
	if (defHolder != "")
	{
		if (strcmp(defHolder.c_str(),"stone") == 0)
		{
			surface = TILE_STONE;
		}
		else if (strcmp(defHolder.c_str(),"air") == 0)
		{
			surface = TILE_AIR;
		}
		else if (strcmp(defHolder.c_str(),"water") == 0)
		{
			surface = TILE_WATER;
		}
		else if (strcmp(defHolder.c_str(), "lava") == 0)
		{
			surface = TILE_LAVA;
		}
		defHolder = "";
	}

	defHolder = data->GetAttrValueByName("iterations");
	if (defHolder != "")
	{
		tempInt = atoi(defHolder.c_str());
		if (tempInt >= 0)
		{
			iterations = tempInt;
		}
		tempInt = 0;
		defHolder = "";
	}

	defHolder = data->GetAttrValueByName("rangeMins");
	if (defHolder != "")
	{
		defHolder = ReplaceCharInString(defHolder, ',', ' ');
		defHolder = ReplaceCharInString(defHolder, '~', ' ');
		std::vector<std::string> parsed = ParseString(defHolder);
		tempInt = atoi(parsed.at(0).c_str());
		rangeMins.x = tempInt;
		tempInt = 0;
		tempInt = atoi(parsed.at(1).c_str());
		rangeMins.y = tempInt;
		defHolder = "";
		tempInt = 0;
	}

	defHolder = data->GetAttrValueByName("rangeMaxs");
	if (defHolder != "")
	{
		defHolder = ReplaceCharInString(defHolder, ',', ' ');
		defHolder = ReplaceCharInString(defHolder, '~', ' ');
		std::vector<std::string> parsed = ParseString(defHolder);
		tempInt = atoi(parsed.at(0).c_str());
		rangeMaxs.x = tempInt;
		tempInt = 0;
		tempInt = atoi(parsed.at(1).c_str());
		rangeMaxs.y = tempInt;
		defHolder = "";
		tempInt = 0;
	}

	defHolder = data->GetAttrValueByName("xRange");
	if (defHolder != "")
	{
		defHolder = ReplaceCharInString(defHolder, ',', ' ');
		defHolder = ReplaceCharInString(defHolder, '~', ' ');
		std::vector<std::string> parsed = ParseString(defHolder);
		tempInt = atoi(parsed.at(0).c_str());
		rangeMins.x = tempInt;
		tempInt = 0;
		tempInt = atoi(parsed.at(1).c_str());
		rangeMaxs.x = tempInt;
		defHolder = "";
		tempInt = 0;
	}
	defHolder = data->GetAttrValueByName("yRange");
	if (defHolder != "")
	{
		defHolder = ReplaceCharInString(defHolder, ',', ' ');
		defHolder = ReplaceCharInString(defHolder, '~', ' ');
		std::vector<std::string> parsed = ParseString(defHolder);
		tempInt = atoi(parsed.at(0).c_str());
		rangeMins.y = tempInt;
		tempInt = 0;
		tempInt = atoi(parsed.at(1).c_str());
		rangeMaxs.y = tempInt;
		defHolder = "";
		tempInt = 0;
	}

	defHolder = data->GetAttrValueByName("quantity");
	if (defHolder != "")
	{
		quantity = atoi(defHolder.c_str());
	}
	defHolder = data->GetAttrValueByName("makeDoors");
	if (defHolder != "")
	{
		std::transform(defHolder.begin(), defHolder.end(), defHolder.begin(), ::tolower);
		if (SimpleStrCmp(defHolder, "t") || SimpleStrCmp(defHolder, "true"))
		{
			m_makeDoors = true;
		}
	}

	for (int it = 0; it < iterations; it++)
	{
		DungeonMapGen_Room::ClearRooms();
		Tunnel::ClearTunnels();
		DungeonMapGen_Room::GenerateRooms(rangeMins, rangeMaxs - rangeMins, quantity, surface);
		
		Tunnel::g_tunnels.push_back(new Tunnel(surface));

		for (size_t roomIdx1 = 0; roomIdx1 < DungeonMapGen_Room::g_rooms.size(); roomIdx1++)
		{
			for (size_t roomIdx2 = 0; roomIdx2 < DungeonMapGen_Room::g_rooms.size(); roomIdx2++)
			{
				if (roomIdx1 != roomIdx2)
				{
					Tunnel::g_tunnels.at(0)->AddSquareTunnelLine(DungeonMapGen_Room::g_rooms.at(roomIdx1)->GetCenter(), DungeonMapGen_Room::g_rooms.at(roomIdx2)->GetCenter(), 0.5f);
					if (m_makeDoors)
					{
						GenerateDoors();
					}
				}
			}
		}
	}
	FastGenerateMap(map, map->GetMapSize(), rangeMins, rangeMaxs);

	for (int st = 0; st < numSteps; st++)
	{
		int step = st;
		GenerateStep(map, 0.f, step);
	}
}
const void DungeonMapGenerator::ResetLastVisited()
{
	MapGenerator::ResetLastVisited();

	Tunnel::ClearTunnels();
	m_loadedRooms = false;
}

void DungeonMapGenerator::InitializeExtraVariables(const IntVector2& mapSize)
{
	int numRooms = 2 + (rand() % 5);
	Tunnel::g_tunnels.push_back(new Tunnel(TILE_AIR));

	DungeonMapGen_Room::GenerateRooms(IntVector2(0,0), mapSize, numRooms);

	for (size_t i = 0; i < DungeonMapGen_Room::g_rooms.size(); i++)
	{

		for (size_t j = 0; j < DungeonMapGen_Room::g_rooms.size(); j++)
		{
			if (i != j)
			{
				Tunnel::g_tunnels.at(0)->AddSquareTunnelLine(DungeonMapGen_Room::g_rooms.at(i)->GetCenter(), DungeonMapGen_Room::g_rooms.at(j)->GetCenter(), 0.5f);
			}
		}
	}
}
Map* DungeonMapGenerator::CreateInitializedMap(const std::string& mapName, IntVector2& mapSize)
{
	Map* newMap = new Map(mapSize, MAP_MODE_DUNGEON);
	newMap->PreInitializationSetMapName(mapName);
	newMap->InitializeMap(TILE_STONE);
	newMap->ChangeRenderLines(true);

	//for (size_t i = 0; i < DungeonMapGen_Room::g_rooms.size(); i++)
	//{
	//	IntVector2 nearestP;
	//	IntVector2 nearestRoomP;
	//	float nearDist;
	//	Tunnel* nearestTunnel = nullptr;
	//	if (Tunnel::DetectTunnelConnectsRoom(DungeonMapGen_Room::g_rooms.at(i), nearestP, nearestRoomP, nearDist, nearestTunnel) == false)
	//	{
	//		nearestTunnel->AddTunnelLine(nearestP, nearestRoomP, 0.5f);
	//	}
	//}

	return newMap;
}
Map* DungeonMapGenerator::GenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	if (map == nullptr)
	{
		return nullptr;
	}

	if ((lastVisitedCell.x < size.x && lastVisitedCell.x < endCoords.x) && (lastVisitedCell.y < size.y && lastVisitedCell.y < endCoords.y))
	{
		const IntVector2 minRoomS = DungeonMapGenerator::minRoomSize;
		const IntVector2 roomSizeVariance = DungeonMapGenerator::maxRoomSize - minRoomS;

		std::map<Tile_Coord, Tile*>* tiles = map->GetMapTiles();

		for (size_t i = 0; i < Tunnel::g_tunnels.size(); i++)
		{
			IntVector2 nearP;
			float nearD;
			if (Tunnel::g_tunnels.at(i)->DetectTileInATunnel(lastVisitedCell, nearP, nearD))
			{
				std::map<Tile_Coord, Tile*>::iterator it = tiles->find(lastVisitedCell);
				if (it != tiles->end() && it->second != nullptr)
				{
					it->second->SetTileType(Tunnel::g_tunnels.at(i)->GetTileType());
					break;
				}
			}
		}

		for (size_t i = 0; i < DungeonMapGen_Room::g_rooms.size(); i++)
		{
			if (DungeonMapGen_Room::g_rooms.at(i)->IsTileLocInRoom(lastVisitedCell))
			{
				std::map<Tile_Coord, Tile*>::iterator it = tiles->find(lastVisitedCell);
				if (it != tiles->end() && it->second != nullptr)
				{
					it->second->SetTileType(DungeonMapGen_Room::g_rooms.at(i)->GetRoomTileType());
					break;
				}
			}
		}
	}
	lastVisitedCell.x++;
	if ((lastVisitedCell.x >= size.x || lastVisitedCell.x >= endCoords.x) && (lastVisitedCell.y < size.y && lastVisitedCell.y < endCoords.y))
	{
		lastVisitedCell.x = startCoords.x;
		lastVisitedCell.y++;
	}

	return map;
}
Map* DungeonMapGenerator::FastGenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	if (map == nullptr)
	{
		return nullptr;
	}

	const IntVector2 minRoomS = DungeonMapGenerator::minRoomSize;
	const IntVector2 roomSizeVariance = DungeonMapGenerator::maxRoomSize - minRoomS;
	std::map<Tile_Coord, Tile*>* tiles = map->GetMapTiles();
	while ((lastVisitedCell.x < size.x && lastVisitedCell.x < endCoords.x) && (lastVisitedCell.y < size.y && lastVisitedCell.y < endCoords.y))
	{

		for (size_t i = 0; i < Tunnel::g_tunnels.size(); i++)
		{
			IntVector2 nearP;
			float nearD;
			if (Tunnel::g_tunnels.at(i)->DetectTileInATunnel(lastVisitedCell, nearP, nearD))
			{
				std::map<Tile_Coord, Tile*>::iterator it = tiles->find(lastVisitedCell);
				if (it != tiles->end() && it->second != nullptr)
				{
					it->second->SetTileType(Tunnel::g_tunnels.at(i)->GetTileType());
					it->second->SetNextTileType(Tunnel::g_tunnels.at(i)->GetTileType());
					break;
				}
			}
		}

		for (size_t i = 0; i < DungeonMapGen_Room::g_rooms.size(); i++)
		{
			if (DungeonMapGen_Room::g_rooms.at(i)->IsTileLocInRoom(lastVisitedCell))
			{
				std::map<Tile_Coord, Tile*>::iterator it = tiles->find(lastVisitedCell);
				if (it != tiles->end() && it->second != nullptr)
				{
					it->second->SetTileType(DungeonMapGen_Room::g_rooms.at(i)->GetRoomTileType());
					it->second->SetNextTileType(DungeonMapGen_Room::g_rooms.at(i)->GetRoomTileType());
					break;
				}
			}
		}

		lastVisitedCell.x++;
		if ((lastVisitedCell.x >= size.x || lastVisitedCell.x >= endCoords.x) && (lastVisitedCell.y < size.y && lastVisitedCell.y < endCoords.y))
		{
			lastVisitedCell.x = startCoords.x;
			lastVisitedCell.y++;
		}
	}
	lastVisitedCell.x = endCoords.x;


	return map;
}
const bool DungeonMapGenerator::GenerateStep(Map* map, const float& deltaSeconds, int& currentStepNumber)
{
	if (map == nullptr)
	{
		return false;
	}
	map;
	deltaSeconds;
	currentStepNumber;
	return true;
}
void DungeonMapGenerator::GenerateDoors()
{
	for (size_t i = 0; i < Tunnel::g_tunnels.size(); i++)
	{
		Tunnel* tunnel = Tunnel::g_tunnels.at(i);
		std::vector<TunnelLine> tunnelLines = tunnel->GetTunnelLines();
		for (size_t LineIdx = 0; LineIdx < tunnelLines.size(); LineIdx++)
		{
			TunnelLine tLine = tunnelLines.at(LineIdx);
			IntVector2 bl = tLine.startPoint;
			IntVector2 tr = tLine.endPoint;
			IntVector2 firstEncounter = IntVector2(-1, -1);
			bool encounteredOpenTile = false;
			bool encounteredRoom = false;
			//Guarantee only go one direction.
			if (tr.x < bl.x)
			{
				int temp = tr.x;
				tr.x = bl.x;
				bl.x = temp;
			}
			if (tr.y < bl.y)
			{
				int temp = tr.y;
				tr.y = bl.y;
				bl.y = temp;
			}

			for (int x = bl.x; x <= tr.x; x++)
			{
				for (int y = bl.y; y <= tr.y; y++)
				{
					bool inARoom = false;
					for (size_t roomIdx = 0; roomIdx < DungeonMapGen_Room::g_rooms.size(); roomIdx++)
					{
						DungeonMapGen_Room* room = DungeonMapGen_Room::g_rooms.at(roomIdx);
						if (room == nullptr)
						{
							continue;
						}
						IntVector2 tile(x, y);
						if (room->IsTileLocInRoom(tile) == true)
						{
							inARoom = true;
							encounteredRoom = true;
						}
					}
					if ((inARoom == false) && (encounteredOpenTile == false || encounteredRoom == false))
					{
						encounteredOpenTile = true;
						firstEncounter = IntVector2(x, y);
					}
				}
			}
			if (encounteredOpenTile == true && encounteredRoom == true)
			{
				if(Feature::IsPositionTakenByAFeature(firstEncounter) == false)
				{
					Feature* fet = FeatureFactory::GenerateFeatureByType(FEATURETYPE_DOOR);
					if (fet != nullptr)
					{
						fet->SetBLPosition(firstEncounter);
					}
				}
			}
		}
	}
}