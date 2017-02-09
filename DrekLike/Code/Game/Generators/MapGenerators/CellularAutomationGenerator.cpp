#include "Game/Generators/MapGenerators/CellularAutomationGenerator.hpp"
#include "Engine/Math/Noise.hpp"


const GeneratorRegistration CellularAutomationGenerator::CellularAutomationGeneratorRegistration = GeneratorRegistration("CellularAutomationGenerator", CreateCellularAutomationGenerator, CellularAutomationGenerationProcessCreateFunc);


Generator* CreateCellularAutomationGenerator(const std::string& name)
{
	CellularAutomationGenerator* generator = new CellularAutomationGenerator();
	generator->SetName(name);
	return (Generator*)generator;
}

EnvironmentGenerationProcess* CellularAutomationGenerationProcessCreateFunc(const XMLNode& node)
{
	EnvironmentGenerationProcess* newEGP = new EnvironmentGenerationProcess();
	newEGP->SetName("CurrentCellularAutomationGenerationProcess");

	for (int i = 0; i < node.nAttribute(); i++)
	{
		XMLAttribute attr = node.getAttribute(i);
		std::string name = attr.lpszName;
		std::string val = attr.lpszValue;
		newEGP->AddAttributeData(attr);
	}
	return newEGP;
}


CellularAutomationGenerator::CellularAutomationGenerator()
	: MapGenerator(),
	m_stonePercentage(0.6f)
{

}
CellularAutomationGenerator::~CellularAutomationGenerator()
{

}

const void CellularAutomationGenerator::GenerateBasedOnDataInput(EnvironmentGenerationProcess* data, Map*& map)
{
	std::string defHolder = "";
	TileType surface = TILE_STONE;
	int tempInt = 0;
	int numSteps = 0;
	int iterations = 1;
	int quantity = 1;
	float stonePercent = 0.6f;
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
		if (strcmp(defHolder.c_str(), "stone") == 0)
		{
			surface = TILE_STONE;
		}
		else if (strcmp(defHolder.c_str(), "air") == 0)
		{
			surface = TILE_AIR;
		}
		else if (strcmp(defHolder.c_str(), "water") == 0)
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
		defHolder = "";
	}
	defHolder = data->GetAttrValueByName("stonePercentage");
	if (defHolder != "")
	{
		stonePercent = (float)atof(defHolder.c_str());
		m_stonePercentage = stonePercent;
		defHolder = "";
	}

	
	for (int it = 0; it < iterations; it++)
	{
		FastGenerateMap(map, map->GetMapSize(), rangeMins, rangeMaxs);
	}

	for (int st = 0; st < numSteps; st++)
	{
		int step = st;
		GenerateStep(map, 0.f, step);
	}
}

void CellularAutomationGenerator::InitializeExtraVariables(const IntVector2& mapSize)
{
	mapSize;
}
Map* CellularAutomationGenerator::CreateInitializedMap(const std::string& mapName, IntVector2& mapSize)
{
	Map* newMap = new Map(mapSize, MAP_MODE_CELLULAR_AUTOMATION);
	newMap->PreInitializationSetMapName(mapName);
	newMap->InitializeMap();
	newMap->ChangeRenderLines(true);

	return newMap;
}
Map* CellularAutomationGenerator::GenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	if (map == nullptr)
	{
		return nullptr;
	}
	if ((lastVisitedCell.x < size.x && lastVisitedCell.x < endCoords.x) && (lastVisitedCell.y < size.y && lastVisitedCell.y < endCoords.y))
	{
		std::map<Tile_Coord, Tile*>* mapTiles = map->GetMapTiles();
		std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(lastVisitedCell);
		if (it != mapTiles->end() && it->second != nullptr)
		{
			float noise = (float)(rand() % 10 + 1) / 10.f;
			if (noise <= m_stonePercentage)
			{
				it->second->SetTileType(TILE_STONE);
				it->second->SetNextTileType(TILE_STONE);
			}
			else //if (noise <= (trueRelAirPercentCap))
			{
				it->second->SetTileType(TILE_AIR);
				it->second->SetNextTileType(TILE_AIR);
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
Map* CellularAutomationGenerator::FastGenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	if (map == nullptr)
	{
		return nullptr;
	}
	if ((lastVisitedCell.x >= size.x || lastVisitedCell.x >= startCoords.x) && (lastVisitedCell.y >= size.y || lastVisitedCell.y >= size.y))
	{
		return map;
	}
	while ((lastVisitedCell.x < size.x && lastVisitedCell.x < endCoords.x) && (lastVisitedCell.y < size.y && lastVisitedCell.y < endCoords.y))
	{
		std::map<Tile_Coord, Tile*>* mapTiles = map->GetMapTiles();
		std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(lastVisitedCell);
		if (it != mapTiles->end() && it->second != nullptr)
		{
			float noise = (float)(rand() % 10 + 1) / 10.f;
			if (noise <= m_stonePercentage)
			{
				it->second->SetTileType(TILE_STONE);
				it->second->SetNextTileType(TILE_STONE);
			}
			else //if (noise <= (trueRelAirPercentCap))
			{
				it->second->SetTileType(TILE_AIR);
				it->second->SetNextTileType(TILE_AIR);
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

const bool CellularAutomationGenerator::GenerateStep(Map* map, const float& deltaSeconds, int& currentStepNumber)
{
	if (map == nullptr)
	{
		return false;
	}
	//map->Update(deltaSeconds, currentStepNumber);
	IntVector2 mapSize = map->GetMapSize();
	std::map<Tile_Coord, Tile*>* mapTiles = map->GetMapTiles();
	for (std::map<Tile_Coord, Tile*>::iterator it = mapTiles->begin(); it != mapTiles->end(); it++)
	{
		if (it->second != nullptr)
		{
			TileType type = it->second->GetTileType();
			std::vector<IntVector2> tiles;
			int count = 0;
			if (type == TILE_STONE)
			{
				//"Living" Cell
				map->GetTileCoordsOfTypeAndCountAboutRadius(it->first, TILE_STONE, 1, tiles, count, true, true);
				count--;
				if (count < 2 || count > 3)
				{
					//Each cell with one or no neighbors dies, as if by solitude.
					//Each cell with four or more neighbors dies, as if by overpopulation.
					it->second->SetNextTileType(TILE_AIR);
				}
			}
			else if (type == TILE_AIR)
			{
				//"Dead" Cell
				map->GetTileCoordsOfTypeAndCountAboutRadius(it->first, TILE_STONE, 1, tiles, count, true, true);
				if (count == 3)
				{
					//Each cell with three neighbors becomes populated.
					it->second->SetNextTileType(TILE_STONE);
				}
			}
		}
	}
	map->UpdateTiles(deltaSeconds, currentStepNumber);
	map->ChangeTiles(deltaSeconds, currentStepNumber);

	return true;
}