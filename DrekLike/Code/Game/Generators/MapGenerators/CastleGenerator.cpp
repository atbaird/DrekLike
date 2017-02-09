#include "Game/Generators/MapGenerators/CastleGenerator.hpp"
#include "Game/Features/FeatureFactory.hpp"


Generator* CreateCastleGenerator(const std::string& name)
{
	CastleGenerator* generator = new CastleGenerator();
	generator->SetName(name);
	return generator;
}
EnvironmentGenerationProcess* CastleGenerationProcessCreateFunc(const XMLNode& node)
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

const GeneratorRegistration CastleGenerator::CastleGeneratorRegistration("CastleGenerator", CreateCastleGenerator, CastleGenerationProcessCreateFunc);

//Constructors
CastleGenerator::CastleGenerator()
	: MapGenerator()
{

}
CastleGenerator::~CastleGenerator()
{
	MapGenerator::~MapGenerator();
}

//overrides
const void CastleGenerator::GenerateBasedOnDataInput(EnvironmentGenerationProcess* data, Map*& map)
{

	std::string defHolder = "";
	TileType surface = TILE_LAVA;
	int tempInt = 0;
	int numSteps = 0;
	int iterations = 1;
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

	FastGenerateMap(map, map->GetMapSize(), rangeMins, rangeMaxs);
}
void CastleGenerator::InitializeExtraVariables(const IntVector2& mapSize)
{
	mapSize;
}
Map* CastleGenerator::CreateInitializedMap(const std::string& mapName, IntVector2& mapSize)
{
	Map* newMap = new Map();
	newMap->PreInitializationSetMapSize(mapSize);
	newMap->PreInitializationSetMapName(mapName);
	newMap->ChangeRenderLines(true);
	newMap->InitializeMap(TILE_LAVA);
	return newMap;
}
Map* CastleGenerator::GenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	size;
	startCoords;
	endCoords;
	return map;
}
Map* CastleGenerator::FastGenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords)
{
	if (map == nullptr)
	{
		return map;
	}
	const IntVector2 defaultMapSize(40, 26);
	Vector2 scale = Vector2((float)size.x / (float)defaultMapSize.x, (float)size.y / (float)defaultMapSize.y);
	//default the map first.
	Tile_Coord mapSize = size;
	Tile_Coord midMap = mapSize / 2;

	//Entrance Coordinates
	Tile_Coord entranceSize = Tile_Coord((int)((float)(3 + (rand() % 3))  * scale.x), (int)((float)(3 + (rand() % 3))  * scale.y));
	int halfY = entranceSize.y / 2;
	Tile_Coord entranceStart = Tile_Coord(0, (midMap.y - halfY));
	Tile_Coord entranceEnd = entranceStart + entranceSize;
	Tile_Coord entranceCenter = (entranceStart + entranceEnd) / 2;

	//Castle Coordinates
	Tile_Coord blCastleStart = Tile_Coord(entranceEnd.x + (int)((float)(2 + (rand() % 2)) * scale.x), 2);
	Tile_Coord trCastleEnd = Tile_Coord(mapSize.x - 4, mapSize.y - 3);
	Tile_Coord castleSize = (trCastleEnd - blCastleStart);
	Tile_Coord castleCenter = blCastleStart + (castleSize / 2);

	//Gatehouse
	Tile_Coord gateHouseSize = Tile_Coord((int)((float)(6 + (rand() % 3)) * scale.x), (int)((float)(3 + (rand() % 3)) * scale.y));
	Tile_Coord blGateHouse = Tile_Coord(blCastleStart.x + 1, castleCenter.y - (gateHouseSize.y / 2));
	Tile_Coord trGateHouse = blGateHouse + gateHouseSize;

	//Top left Tower
	Tile_Coord towerTLSize = Tile_Coord((int)((float)(2 + (rand() % 2)) * scale.x), (int)((float)(2 + (rand() % 2)) * scale.y));
	Tile_Coord blTowerTL = Tile_Coord(blCastleStart.x, trCastleEnd.y) - Tile_Coord(0, towerTLSize.y);
	Tile_Coord trTowerTL = blTowerTL + towerTLSize;

	//Bottom Left Tower
	Tile_Coord towerBLSize = Tile_Coord((int)((float)(2 + (rand() % 2)) * scale.x), (int)((float)(2 + (rand() % 2)) * scale.y));
	Tile_Coord blTowerBL = Tile_Coord(blCastleStart.x, blCastleStart.y);
	Tile_Coord trTowerBL = blTowerBL + towerBLSize;

	//Rooms
	Tile_Coord blRooms = Tile_Coord(blCastleStart.x + 5, blCastleStart.y + 1);
	Tile_Coord trRooms = trCastleEnd - Tile_Coord(1,1);


	while ((lastVisitedCell.x < size.x && lastVisitedCell.x < endCoords.x) && (lastVisitedCell.y < size.y && lastVisitedCell.y < endCoords.y))
	{
		if ((lastVisitedCell.x >= entranceStart.x && lastVisitedCell.x < (entranceEnd.x) //Entrance vars
			&& lastVisitedCell.y >= entranceStart.y && lastVisitedCell.y <= (entranceEnd.y))

			|| (lastVisitedCell.x >= blGateHouse.x && lastVisitedCell.x <= trGateHouse.x //Gate House
				&& lastVisitedCell.y >= blGateHouse.y && lastVisitedCell.y <= trGateHouse.y)

			|| (lastVisitedCell.x >= blTowerTL.x && lastVisitedCell.x <= trTowerTL.x //TL Tower
				&& lastVisitedCell.y >= blTowerTL.y && lastVisitedCell.y <= trTowerTL.y)

			|| (lastVisitedCell.x >= blTowerBL.x && lastVisitedCell.x <= trTowerBL.x //BL Tower
				&& lastVisitedCell.y >= blTowerBL.y && lastVisitedCell.y <= trTowerBL.y)

			|| (lastVisitedCell.x == (blTowerTL.x + 1) //TL Tower Hallway
				&& lastVisitedCell.y <= blTowerTL.y && lastVisitedCell.y >= blGateHouse.y)

			|| (lastVisitedCell.x == (blTowerBL.x + 1) //BL Tower Hallway
				&& lastVisitedCell.y >= blTowerBL.y && lastVisitedCell.y <= blGateHouse.y)

			|| ( lastVisitedCell.x >= entranceEnd.x && lastVisitedCell.x <= blGateHouse.x
				&& lastVisitedCell.y == entranceCenter.y)//Solid DrawBridge
			)
		{
			map->SetTileTypeAtCoords(lastVisitedCell, TILE_AIR, true);
		}
		else if ((lastVisitedCell.x == 0 || lastVisitedCell.y == 0 //Edges of map
			|| lastVisitedCell.x == endCoords.x - 1
			|| lastVisitedCell.y == endCoords.y - 1)

			|| (lastVisitedCell.x >= blCastleStart.x && lastVisitedCell.x <= trCastleEnd.x // castle vars
				&& lastVisitedCell.y >= blCastleStart.y && lastVisitedCell.y <= trCastleEnd.y))
		{

			map->SetTileTypeAtCoords(lastVisitedCell, TILE_STONE, true);
		}
		else
		{
			map->SetTileTypeAtCoords(lastVisitedCell, TILE_LAVA, true);
		}


		if ((lastVisitedCell.x >= blRooms.x && lastVisitedCell.x <= trRooms.x
			&& lastVisitedCell.y >= blRooms.y && lastVisitedCell.y <= trRooms.y))
		{
			float ran = (float)(rand() % 100) / 100.f;
			if (ran <= 0.85f)
			{
				map->SetTileTypeAtCoords(lastVisitedCell, TILE_AIR, true);
			}
			else
			{
				map->SetTileTypeAtCoords(lastVisitedCell, TILE_STONE, true);
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

	Feature* tlSiege = FeatureFactory::GenerateFeatureByType(FEATURETYPE_SIEGE);
	Feature* blSiege = FeatureFactory::GenerateFeatureByType(FEATURETYPE_SIEGE);
	tlSiege->SetBLPosition(Tile_Coord(blTowerTL.x, trTowerTL.y));
	blSiege->SetBLPosition(blTowerBL);

	return map;
}
const bool CastleGenerator::GenerateStep(Map* map, const float& deltaSeconds, int& currentStepNumber)
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