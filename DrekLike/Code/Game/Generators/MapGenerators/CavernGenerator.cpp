#include "Game/Generators/MapGenerators/CavernGenerator.hpp"
#include "Game/Generators/MapGenerators/CavernMap/Cavern.hpp"


const GeneratorRegistration CavernGenerator::CavernGeneratorRegistration = GeneratorRegistration("CavernGenerator", CreateCavernGenerator, CavernGenerationProcessCreateFunc);


Generator* CreateCavernGenerator(const std::string& name)
{
	CavernGenerator* generator = new CavernGenerator();
	generator->SetName(name);
	return (Generator*)generator;
}
EnvironmentGenerationProcess* CavernGenerationProcessCreateFunc(const XMLNode& node)
{
	EnvironmentGenerationProcess* newEGP = new EnvironmentGenerationProcess(node);
	newEGP->SetName("CurrentCavernGenerationProcess");

	for (int i = 0; i < node.nAttribute(); i++)
	{
		XMLAttribute attr = node.getAttribute(i);
		std::string name = attr.lpszName;
		std::string val = attr.lpszValue;
		newEGP->AddAttributeData(attr);
	}
	return newEGP;
}


CavernGenerator::CavernGenerator()
	: MapGenerator()
{

}
CavernGenerator::~CavernGenerator()
{
	Cavern::ClearCaverns();
}
const void CavernGenerator::GenerateBasedOnDataInput(EnvironmentGenerationProcess* data, Map*& map)
{
	std::string defHolder = "";
	TileType surface = TILE_AIR;
	int tempInt = 0;
	int numSteps = 0;
	int iterations = 1;
	int quantity = 1;
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
		std::vector<std::string> parsed = ParseString(defHolder, ',');
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
		std::vector<std::string> parsed = ParseString(defHolder, ',');
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
		std::vector<std::string> parsed = ParseString(defHolder, '~');
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
		std::vector<std::string> parsed = ParseString(defHolder, '~');
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

	for (int it = 0; it < iterations; it++)
	{
		Cavern::ClearCaverns();
		for (int cavernNum = 0; cavernNum < quantity; cavernNum++)
		{
			Cavern::GenerateCavern(rangeMins, rangeMaxs - rangeMins, surface);
		}
	}
	FastGenerateMap(map, map->GetMapSize(), rangeMins, rangeMaxs);

	for (int st = 0; st < numSteps; st++)
	{
		int step = st;
		GenerateStep(map, 0.f, step);
	}
}


void CavernGenerator::InitializeExtraVariables(const IntVector2& mapSize)
{
	int numberOfCaverns = Cavern::minNumberOfCaverns + (rand() % Cavern::addRandNumOfCaverns);
	for (int i = 0; i < numberOfCaverns; i++)
	{
		Cavern::GenerateCavern(IntVector2(0,0), mapSize);
	}
}
Map* CavernGenerator::CreateInitializedMap(const std::string& mapName, IntVector2& mapSize)
{
	Map* newMap = new Map(mapSize, MAP_MODE_DUNGEON);
	newMap->PreInitializationSetMapName(mapName);
	newMap->InitializeMap(TILE_STONE);
	newMap->ChangeRenderLines(true);

	return newMap;
}
Map* CavernGenerator::GenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	if (map == nullptr)
	{
		return nullptr;
	}

	if ((lastVisitedCell.x < size.x && lastVisitedCell.x < endCoords.x) && (lastVisitedCell.y < size.y && lastVisitedCell.y < endCoords.y))
	{
		Cavern* cavern = nullptr;
		if (Cavern::IsTileCoordInACavern(cavern, lastVisitedCell))
		{
			map->SetTileTypeAtCoords(lastVisitedCell, cavern->GetTileType());
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
Map* CavernGenerator::FastGenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	if (map == nullptr)
	{
		return nullptr;
	}

	while ((lastVisitedCell.x < size.x && lastVisitedCell.x < endCoords.x) && (lastVisitedCell.y < size.y && lastVisitedCell.y < endCoords.y))
	{
		Cavern* cavern = nullptr;
		if (Cavern::IsTileCoordInACavern(cavern, lastVisitedCell))
		{
			map->SetTileTypeAtCoords(lastVisitedCell, cavern->GetTileType(), true);
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
const bool CavernGenerator::GenerateStep(Map* map, const float& deltaSeconds, int& currentStepNumber)
{
	if (map == nullptr)
	{
		return false;
	}
	map;
	deltaSeconds;
	currentStepNumber;
	return false;
}