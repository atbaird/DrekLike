#include "Game/Generators/MapGenerators/FromDataGenerator.hpp"



const GeneratorRegistration FromDataGenerator::FromDataGeneratorRegistration = GeneratorRegistration("FromDataGenerator", CreateFromDataGenerator, FromDataGenerationProcessCreateFunc);

Generator* CreateFromDataGenerator(const std::string& name)
{

	FromDataGenerator* generator = new FromDataGenerator();
	generator->SetName(name);
	return (Generator*)generator;
}
EnvironmentGenerationProcess* FromDataGenerationProcessCreateFunc(const XMLNode& node)
{

	EnvironmentGenerationProcess* newEGP = new EnvironmentGenerationProcess();
	newEGP->SetName("CurrentDungeonGenerationProcess");

	for (int i = 0; i < node.nAttribute(); i++)
	{
		XMLAttribute attr = node.getAttribute(i);
		newEGP->AddAttributeData(attr);
	}

	for (int i = 0; i < node.nChildNode(); i++)
	{
		XMLNode child = node.getChildNode(i);
		std::string cName = child.getName();
		{
			GenerationProcessData* proc = new GenerationProcessData(StrXMLAttr("Name", "Tiles"));
			bool tileTypeSet = false;
			if (cName != "Tiles")
			{
				tileTypeSet = true;
			}
			for (int j = 0; j < child.nAttribute(); j++)
			{
				XMLAttribute attr = child.getAttribute(j);
				std::string name = attr.lpszName;
				std::string val = attr.lpszValue;
				proc->AddAttribute(StrXMLAttr(name, val));
				if (name == "tileType")
				{
					tileTypeSet = true;
				}
			}
			if (tileTypeSet == false)
			{
				proc->AddAttribute(StrXMLAttr("tileType", "air"));
			}
			newEGP->AddGenerationProcessData(proc);
		}
	}
	return newEGP;
}

FromDataGenerator::FromDataGenerator()
	: MapGenerator()
{

}
FromDataGenerator::~FromDataGenerator()
{

}

const void FromDataGenerator::GenerateBasedOnDataInput(EnvironmentGenerationProcess* data, Map*& map)
{
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
		if (defHolder == "stone")
		{
			surface = TILE_STONE;
		}
		else if (defHolder == "air")
		{
			surface = TILE_AIR;
		}
		else if (defHolder == "water")
		{
			surface = TILE_WATER;
		}
		else if (defHolder == "lava")
		{
			surface = TILE_LAVA;
		}
		defHolder = "";
	}
	defHolder = data->GetAttrValueByName("tileType");
	if (defHolder != "")
	{
		if (defHolder == "stone")
		{
			surface = TILE_STONE;
		}
		else if (defHolder == "air")
		{
			surface = TILE_AIR;
		}
		else if (defHolder == "water")
		{
			surface = TILE_WATER;
		}
		else if (defHolder == "lava")
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

	std::vector<GenerationProcessData*> children = data->GetChildren();
	std::vector<TileData> data2;
	for (size_t i = 0; i < children.size(); i++)
	{
		GenerationProcessData* proc = children.at(i);
		if (proc->GetName().val == "Tiles")
		{
			TileData newDat;
			std::vector<StrXMLAttr> attrs = proc->GetAttributes();
			for (size_t j = 0; j < attrs.size(); j++)
			{
				StrXMLAttr cur = attrs.at(j);
				if (cur.name == "tilePosition")
				{
					cur.val = ReplaceCharInString(cur.val, ',', ' ');
					cur.val = ReplaceCharInString(cur.val, '~', ' ');
					std::vector<std::string> parsed = ParseString(cur.val);
					tempInt = atoi(parsed.at(0).c_str());
					newDat.m_mins.x = tempInt;
					newDat.m_maxs.x = tempInt;
					tempInt = atoi(parsed.at(0).c_str());
					newDat.m_mins.y = tempInt;
					newDat.m_maxs.y = tempInt;
					tempInt = 0;
				}
				else if (cur.name == "rangeMins")
				{
					cur.val = ReplaceCharInString(cur.val, ',', ' ');
					cur.val = ReplaceCharInString(cur.val, '~', ' ');
					std::vector<std::string> parsed = ParseString(cur.val);
					tempInt = atoi(parsed.at(0).c_str());
					newDat.m_mins.x = tempInt;
					tempInt = atoi(parsed.at(0).c_str());
					newDat.m_mins.y = tempInt;
					tempInt = 0;
				}
				else if (cur.name == "rangeMaxs")
				{
					cur.val = ReplaceCharInString(cur.val, ',', ' ');
					cur.val = ReplaceCharInString(cur.val, '~', ' ');
					std::vector<std::string> parsed = ParseString(cur.val);
					tempInt = atoi(parsed.at(0).c_str());
					newDat.m_maxs.x = tempInt;
					tempInt = atoi(parsed.at(0).c_str());
					newDat.m_maxs.y = tempInt;
					tempInt = 0;
				}
				else if (cur.name == "xRange")
				{
					cur.val = ReplaceCharInString(cur.val, ',', ' ');
					cur.val = ReplaceCharInString(cur.val, '~', ' ');
					std::vector<std::string> parsed = ParseString(cur.val);
					tempInt = atoi(parsed.at(0).c_str());
					newDat.m_mins.x = tempInt;
					tempInt = atoi(parsed.at(0).c_str());
					newDat.m_maxs.x = tempInt;
					tempInt = 0;
				}
				else if (cur.name == "yRange")
				{
					cur.val = ReplaceCharInString(cur.val, ',', ' ');
					cur.val = ReplaceCharInString(cur.val, '~', ' ');
					std::vector<std::string> parsed = ParseString(cur.val);
					tempInt = atoi(parsed.at(0).c_str());
					newDat.m_mins.y = tempInt;
					tempInt = atoi(parsed.at(0).c_str());
					newDat.m_maxs.y = tempInt;
					tempInt = 0;
				}
				else if (cur.name == "tileType")
				{
					/*
					TILE_INVALID = -1,
					TILE_AIR,
					TILE_STONE,
					TILE_WATER,
					*/
					if (strcmp(cur.val.c_str(), "air") == 0)
					{
						newDat.m_type = TILE_AIR;
					}
					else if (strcmp(cur.val.c_str(), "stone") == 0)
					{
						newDat.m_type = TILE_STONE;
					}
					else if (strcmp(cur.val.c_str(), "water") == 0)
					{
						newDat.m_type = TILE_WATER;
					}
					else if (strcmp(cur.val.c_str(), "lava") == 0)
					{
						newDat.m_type = TILE_LAVA;
					}
					else
					{
						newDat.m_type = TILE_INVALID;
					}
				}
			}
			data2.push_back(newDat);
		}
		proc = nullptr;
	}
	SetData(data2);

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
void FromDataGenerator::InitializeExtraVariables(const IntVector2& mapSize)
{
	mapSize;
}
Map* FromDataGenerator::CreateInitializedMap(const std::string& mapName, IntVector2& mapSize)
{
	Map* newMap = new Map(mapSize, MAP_MODE_CELLULAR_AUTOMATION);
	newMap->PreInitializationSetMapName(mapName);
	newMap->InitializeMap();
	newMap->ChangeRenderLines(true);

	return newMap;
}
Map* FromDataGenerator::GenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	return FastGenerateMap(map, size, startCoords, endCoords);
}
Map* FromDataGenerator::FastGenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	size;
	std::map<Tile_Coord, Tile*>* mapTiles = map->GetMapTiles();
	for (size_t i = 0; i < m_data.size(); i++)
	{
		TileData data = m_data.at(i);
		for (int y = data.m_mins.y; y <= data.m_maxs.y; y++)
		{
			if (y >= startCoords.y && y <= endCoords.x)
			{
				for (int x = data.m_mins.x; x <= data.m_maxs.x; x++)
				{
					if (x >= startCoords.x && x <= endCoords.x)
					{
						IntVector2 tile(x, y);
						std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(tile);
						if (it != mapTiles->end() && it->second != nullptr)
						{
							it->second->SetTileType(data.m_type);
							it->second->SetNextTileType(data.m_type);
						}
					}
				}
			}
		}
	}
	lastVisitedCell = map->GetMapSize();
	return map;
}
const bool FromDataGenerator::GenerateStep(Map* map, const float& deltaSeconds, int& currentStepNumber)
{
	map;
	deltaSeconds;
	currentStepNumber;
	return true;

}