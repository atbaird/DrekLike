#include "Game/Generators/MapGenerators/MapGenerator.hpp"
#include "Game/Generators/MapGenerators/CellularAutomationGenerator.hpp"
#include "Game/Generators/MapGenerators/DungeonMapGenerator.hpp"
#include "Game/Generators/MapGenerators/CavernGenerator.hpp"
#include "Game/Generators/MapGenerators/FromDataGenerator.hpp"

void MapGenerator::RunGenerationInstructions(const std::string& xmlFileLocation, Map*& m_map, const Vector2& windowSize, const float& messageBoxSize)
{
	bool couldNotLoad = false;
	XMLNode root = EngineXMLParser::ParseXMLFile(xmlFileLocation, "EnvironmentBlueprint", couldNotLoad);
	if (couldNotLoad == true)
	{
		return;
	}
	IntVector2 mapSize = IntVector2(20,20);
	std::string mapName = "";
	TileType tileType = TILE_STONE;
	for (int i = 0; i < root.nAttribute(); i++)
	{
		XMLAttribute attr = root.getAttribute(i);
		std::string name = attr.lpszName;
		std::string val = attr.lpszValue;
		if (name == "size")
		{
			std::vector<std::string> vals = ParseString(val, ',');
			mapSize.x = atoi(vals.at(0).c_str());
			mapSize.y = atoi(vals.at(1).c_str());
		}
		else if (name == "name")
		{
			mapName = val;
		}
		else if (name == "surface")
		{
			if (val == "air")
			{
				tileType = TILE_AIR;
			}
			else if (val == "stone")
			{
				tileType = TILE_STONE;
			}
			else if (val == "water")
			{
				tileType = TILE_WATER;
			}
			else if (val == "lava")
			{
				tileType = TILE_LAVA;
			}
		}
	}

	if (mapName == "")
	{
		int periodsEncountered = 0;
		for (int i = (int)xmlFileLocation.length() - 1; i >= 0; i--)
		{
			char ch = xmlFileLocation.at(i);
			if (ch == '.')
			{
				periodsEncountered++;
			}
			else if (ch == '\\')
			{
				break;
			}
			else if (periodsEncountered == 2)
			{
				mapName = ch + mapName;
			}
		}
	}

	m_map->PreInitializationSetMapSize(mapSize);
	m_map->PreInitializationSetMapName(mapName);
	m_map->InitializeMap(tileType);
	m_map->ChangeRenderLines(true);
	m_map->ChangeTileRenderSize(Vector2(windowSize.x / (float)mapSize.x, (windowSize.y - messageBoxSize) / (float)mapSize.y));

	for (int i = 0; i < root.nChildNode(); i++)
	{
		XMLNode child = root.getChildNode(i);
		std::string childName = child.getName();
		if (childName == "GenerationProcessData")
		{
			std::string generatorType = "";
			for (int j = 0; j < child.nAttribute(); j++)
			{
				XMLAttribute attr = child.getAttribute(j);
				std::string attrNam = attr.lpszName;
				std::string attrVal = attr.lpszValue;
				if (attrNam == "generator")
				{
					generatorType = attrVal;
					break;
				}
			}
			EnvironmentGenerationProcess* data = GeneratorRegistration::GetGenerationProcessData(generatorType, child);
			if (data != nullptr)
			{
				MapGenerator* generator = (MapGenerator*)GeneratorRegistration::CreateGeneratorByname(generatorType);
				if (generator != nullptr)
				{
					generator->GenerateBasedOnDataInput(data, m_map);
					delete generator;
				}
				delete data;
			}
		}
	}
}

MapGenerator::MapGenerator()
	: Generator(GENERATOR_TYPE_MAP),
	lastVisitedCell(0,0)
{

}
MapGenerator::~MapGenerator()
{

}
void MapGenerator::InitializeExtraVariables(const IntVector2& mapSize)
{
	mapSize;
}
void MapGenerator::RegisterAllMapGenerators(int& numberOfMapGenerators)
{
	numberOfMapGenerators = 4;
}
Map* MapGenerator::CreateInitializedMap(const std::string& mapName, IntVector2& mapSize)
{
	mapName;
	mapSize;
	return nullptr;
}
const void MapGenerator::GenerateBasedOnDataInput(EnvironmentGenerationProcess* data, Map*& map)
{
	data;
	map;
}
Map* MapGenerator::GenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	endCoords;
	startCoords;
	if (map == nullptr)
	{
		return nullptr;
	}
	const std::map<Tile_Coord, Tile*>* tiles = map->GetMapTiles();
	for (int y = 0; y < size.y; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			Tile_Coord tileCoord(x, y);
			std::map<Tile_Coord, Tile*>::const_iterator it = tiles->find(tileCoord);
			if (it != tiles->end())
			{
				it->second->SetTileType(TILE_STONE);
			}
		}
	}
	return map;
}
Map* MapGenerator::FastGenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	endCoords;
	startCoords;
	if (map == nullptr)
	{
		return nullptr;
	}
	const std::map<Tile_Coord, Tile*>* tiles = map->GetMapTiles();
	for (int y = 0; y < size.y; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			Tile_Coord tileCoord(x, y);
			std::map<Tile_Coord, Tile*>::const_iterator it = tiles->find(tileCoord);
			if (it != tiles->end())
			{
				it->second->SetTileType(TILE_STONE);
			}
		}
	}
	return map;
}
const bool MapGenerator::GenerateStep(Map* map, const float& deltaSeconds, int& currentStepNumber)
{
	map->Update(deltaSeconds, currentStepNumber);
	return true;
}