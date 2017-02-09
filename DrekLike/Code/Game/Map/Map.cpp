#include "Game/Map/Map.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/FieldOfView/FieldOfViewFloodFill.hpp"

Map* g_map = nullptr;
const float Map::s_straightPathFindCost = 1.0f;
const float Map::s_diagPathFindCost = 1.4f;


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Constructors
//!----------------------------------------------------------------------
//Map
Map::Map(const IntVector2& mapSize, const MAP_MODE& mode, const Vector2& tileRenderSize, const bool& renderLines)
	:m_mapSize(mapSize),
	m_mapMode(mode),
	lastStepNumber(0),
	m_TileRenderSize(tileRenderSize),
	m_RenderLines(renderLines),
	m_tileLineColor(0.f,0.f,0.f,1.f),
	m_viewFullMapRegardlessOfVisible(false)
{
	mapTiles = new std::map<Tile_Coord, Tile*>();
}
//!----------------------------------------------------------------------
//~Map
Map::~Map()
{
	ClearMap();
	delete mapTiles;
	mapTiles = nullptr;
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Functions
//!----------------------------------------------------------------------
//InitializeMap
void Map::InitializeMap(const TileType& type)
{
	for (int y = 0; y < m_mapSize.y; y++)
	{
		for (int x = 0; x < m_mapSize.x; x++)
		{
			Tile_Coord coords = IntVector2(x, y);
			Tile* newTile = new Tile(coords, type);
			mapTiles->insert(std::pair<Tile_Coord, Tile*>(coords, newTile));
		}
	}
}
//!----------------------------------------------------------------------
//ClearMap
void Map::ClearMap()
{
	for (int y = 0; y < m_mapSize.y; y++)
	{
		for (int x = 0; x < m_mapSize.x; x++)
		{
			Tile_Coord coords = IntVector2(x, y);
			std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(coords);
			if (it != mapTiles->end() && it->second != nullptr)
			{
				Tile* temp = it->second;
				it->second = nullptr;
				delete temp;
			}
		}
	}
}
void Map::WriteToXMLNode(XMLNode& node)
{
	XMLNode xMap = node.addChild("Map");
	xMap.addAttribute("size", (std::to_string(m_mapSize.x) + "," + std::to_string(m_mapSize.y)).c_str());

	XMLNode fullMap = xMap.addChild("fullMap");
	XMLNode seenMap = xMap.addChild("seenMap");
	for (int y = 0; y < m_mapSize.y; y++)
	{
		std::string row = "";
		std::string rowVis = "";
		for (int x = 0; x < m_mapSize.x; x++)
		{
			IntVector2 tileCoord(x, y);
			std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(tileCoord);
			if (it != mapTiles->end() && it->second != nullptr)
			{
				TileType type = it->second->GetTileType();
				bool seen = it->second->GetSeen();
				std::string seenstr = std::to_string(seen);
				row += std::to_string(type) + " ";
				rowVis += seenstr + " ";
			}
		}
		std::string childName = "row_" + std::to_string(y);
		XMLNode rowX = fullMap.addChild(childName.c_str());
		XMLNode rowVisX = seenMap.addChild(childName.c_str());
		rowX.addAttribute("data", row.c_str());
		rowVisX.addAttribute("data", rowVis.c_str());
	}
}
void Map::ReadFromXMLNode(GenerationProcessData* data)
{
	std::vector<StrXMLAttr> attrs = data->GetAttributes();
	for (size_t i = 0; i < attrs.size(); i++)
	{
		StrXMLAttr attr = attrs.at(i);
		if (SimpleStrCmp(attr.name, "size"))
		{
			std::string val = ReplaceCharInString(attr.val, ',', ' ');
			std::vector<std::string> vals = ParseString(val);
			m_mapSize = IntVector2(atoi(vals.at(0).c_str()), atoi(vals.at(1).c_str()));
		}
	}
	GenerationProcessData* fullMap = data->GetChildByName("fullMap");
	GenerationProcessData* visibilityMap = data->GetChildByName("seenMap");
	for (int row = 0; row < m_mapSize.y; row++)
	{
		std::string mapRow = fullMap->GetChildByIndex(row)->GetAttributeByName("data").val;
		std::string visRow = visibilityMap->GetChildByIndex(row)->GetAttributeByName("data").val;
		std::vector<std::string> tileEnums = ParseString(mapRow);
		std::vector<std::string> visData = ParseString(visRow);
		for (int col = 0; col < m_mapSize.x; col++)
		{
			IntVector2 coord(col, row);
			Tile* newTile = new Tile(coord);
			newTile->SetTileType((TileType)atoi(tileEnums.at(col).c_str()));
			newTile->SetSeen(atoi(visData.at(col).c_str()) == 1);
			mapTiles->insert(std::pair<IntVector2, Tile*>(coord, newTile));
		}
	}
}
//!----------------------------------------------------------------------
//GetOpenCoordsAboutEntityBLAndTR
const std::vector<Tile_Coord> Map::GetOpenCoordsAboutEntityBLAndTR(const IntVector2& bl, const IntVector2& tr) const
{
	std::vector<Tile_Coord> tileCoords;
	for (int y = bl.y - 1; y <= tr.y + 1; y++)
	{
		for (int x = bl.x - 1; x <= tr.x + 1; x++)
		{
			if (x < bl.x || x > tr.x || y < bl.y || y > tr.y)
			{
				Tile_Coord coord(x, y);
				Tile_Coord size((tr.x - bl.x), (tr.y - bl.y));
				Tile_Coord newTR(x + size.x, y + size.y);
				Tile_Coord current = coord;
				bool validSpot = true;
				while (current <= newTR)
				{
					std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->find(current);
					if (it == mapTiles->end() && it->second == nullptr)
					{
						validSpot = false;
						break;
					}
					TileType type = it->second->GetTileType();
					bool exists = TileDefinition::TypeExists(type);
					if (exists == false)
					{
						validSpot = false;
						break;
					}
					current.x += 1;
					if (current.x > newTR.x && current.y < newTR.y)
					{
						current.x = coord.x;
						current.y += 1;
					}
				}
				if (validSpot == true)
				{
					tileCoords.push_back(coord);
				}
			}

		}
	}
	return tileCoords;
}
//!----------------------------------------------------------------------
//GetOpenCoordsAboutEntityBLAndTRWithRespectToActors
const std::vector<Tile_Coord> Map::GetOpenCoordsAboutEntityBLAndTRWithRespectToActors(const IntVector2& bl, const IntVector2& tr, Agent* ignoreMe) const
{
	std::vector<Tile_Coord> tileCoords = GetOpenCoordsAboutEntityBLAndTR(bl, tr);
	size_t numAgents = Agent::s_Agents.size();
	for (size_t i = 0; i < numAgents; i++)
	{
		Agent* currentAgent = Agent::s_Agents.at(i);

		if (currentAgent != ignoreMe)
		{
			Tile_Coord blAg = currentAgent->GetBlTilePos();
			Tile_Coord blWantedAg = currentAgent->GetWantedBLTilePos();
			Tile_Coord size = currentAgent->GetSize();
			Tile_Coord trAg;

			if (blAg != blWantedAg)
			{
				blAg = blWantedAg;
				trAg = blWantedAg + size - IntVector2(1, 1);
			}
			else
			{
				trAg = blAg + size - IntVector2(1, 1);
			}

			for (size_t j = 0; j < tileCoords.size(); j++)
			{
				Tile_Coord tileCoord = tileCoords.at(j);
				if (tileCoord.x >= blAg.x && tileCoord.y >= blAg.y
					&& tileCoord.x <= trAg.x && tileCoord.y <= trAg.y)
				{
					tileCoords.erase(tileCoords.begin() + j);
					j--;
				}
			}
		}
	}
	return tileCoords;

}
//!----------------------------------------------------------------------
//FindAdjacentValidPositions
const std::vector<Tile_Coord> Map::FindAdjacentValidPositions(const Tile_Coord& pos)
{
	std::vector<Tile_Coord> coords;

	Tile_Coord bl = pos - IntVector2(1, 1);
	Tile_Coord tr = pos + IntVector2(1, 1);

	for (int y = bl.y; y <= tr.y; y++)
	{
		for (int x = bl.x; x <= tr.x; x++)
		{
			if (x == pos.x && y == pos.y)
			{
				continue;
			}
			Tile_Coord coord = Tile_Coord(x, y);
			std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->find(coord);
			if (it != mapTiles->end() && it->second != nullptr)
			{
				TileType type = it->second->GetTileType();
				bool exists = false;
				bool solid = TileDefinition::GetTypeSolid(type, exists);
				if (exists == true && solid == false)
				{
					coords.push_back(coord);
				}

			}
		}
	}


	return coords;
}

const std::vector<Tile_Coord> Map::FindAdjacentValidPositionsThatMeetMask(const Tile_Coord& pos, const int& mask)
{
	std::vector<Tile_Coord> coords;

	Tile_Coord bl = pos - IntVector2(1, 1);
	Tile_Coord tr = pos + IntVector2(1, 1);

	for (int y = bl.y; y <= tr.y; y++)
	{
		for (int x = bl.x; x <= tr.x; x++)
		{
			if (x == pos.x && y == pos.y)
			{
				continue;
			}
			Tile_Coord coord = Tile_Coord(x, y);
			std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->find(coord);
			if (it != mapTiles->end() && it->second != nullptr)
			{
				TileType type = it->second->GetTileType();
				std::map<TileType, TileDefinition>::iterator itD = g_TileDefinitions.find(type);
				if (itD != g_TileDefinitions.end())
				{
					bool solid = false;
					if ((type == TILE_AIR && Agent::GetBlockedByAir(mask))
						|| (itD->second.GetIsFire() && Agent::GetBlockedByFire(mask))
						|| (itD->second.GetIsWall() && Agent::GetBlockedByWalls(mask))
						|| (itD->second.GetIsLiquid() && Agent::GetBlockedByWater(mask)))
					{
						solid = true;
					}
					if (solid == false)
					{
						coords.push_back(coord);
					}
				}

			}
		}
	}


	return coords;
}

const std::vector<Tile_Coord> Map::FindAdjacentValidPositionsThatDontHurtSlowOrBlock(const Tile_Coord& pos, const int& mask)
{
	std::vector<Tile_Coord> coords;

	Tile_Coord bl = pos - IntVector2(1, 1);
	Tile_Coord tr = pos + IntVector2(1, 1);

	for (int y = bl.y; y <= tr.y; y++)
	{
		for (int x = bl.x; x <= tr.x; x++)
		{
			if (x == pos.x && y == pos.y)
			{
				continue;
			}
			Tile_Coord coord = Tile_Coord(x, y);
			std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->find(coord);
			if (it != mapTiles->end() && it->second != nullptr)
			{
				TileType type = it->second->GetTileType();
				std::map<TileType, TileDefinition>::iterator itD = g_TileDefinitions.find(type);
				if (itD != g_TileDefinitions.end())
				{
					bool solid = false;
					if ((type == TILE_AIR && Agent::GetBlockedByAir(mask))
						|| ((itD->second.GetIsFire()) && (Agent::GetBlockedByFire(mask) || Agent::GetSlowedByFire(mask) || Agent::GetHurtByFire(mask)))
						|| (itD->second.GetIsWall() && (Agent::GetBlockedByWalls(mask) || Agent::GetSlowedByWalls(mask)))
						|| (itD->second.GetIsLiquid() && (Agent::GetBlockedByWater(mask) || Agent::GetSlowedByWater(mask))))
					{
						solid = true;
					}
					if (solid == false)
					{
						coords.push_back(coord);
					}
				}

			}
		}
	}


	return coords;
}

const std::vector<Tile_Coord> Map::FindAdjacentValidPositionsThatDontBlockOrHurt(const Tile_Coord& pos, const int& mask)
{
	std::vector<Tile_Coord> coords;

	Tile_Coord bl = pos - IntVector2(1, 1);
	Tile_Coord tr = pos + IntVector2(1, 1);

	for (int y = bl.y; y <= tr.y; y++)
	{
		for (int x = bl.x; x <= tr.x; x++)
		{
			if (x == pos.x && y == pos.y)
			{
				continue;
			}
			Tile_Coord coord = Tile_Coord(x, y);
			std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->find(coord);
			if (it != mapTiles->end() && it->second != nullptr)
			{
				TileType type = it->second->GetTileType();
				std::map<TileType, TileDefinition>::iterator itD = g_TileDefinitions.find(type);
				if (itD != g_TileDefinitions.end())
				{
					bool solid = false;
					if ((type == TILE_AIR && Agent::GetBlockedByAir(mask))
						|| ((itD->second.GetIsFire()) && (Agent::GetBlockedByFire(mask) || (Agent::GetHurtByFire(mask) && Agent::GetFlying(mask) == false)))
						|| (itD->second.GetIsWall() && (Agent::GetBlockedByWalls(mask)))
						|| (itD->second.GetIsLiquid() && (Agent::GetBlockedByWater(mask))))
					{
						solid = true;
					}
					if (solid == false)
					{
						coords.push_back(coord);
					}
				}

			}
		}
	}


	return coords;
}
//!----------------------------------------------------------------------
//ComputeLocalG
const float Map::ComputeLocalG(const Tile_Coord& position, const Tile_Coord& originalPos, const int& mask) const
{
	std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->find(position);
	if (it == mapTiles->end() || it->second == nullptr)
	{
		return FLT_MAX;
	}
	TileType type = it->second->GetTileType();
	if (!TileDefinition::TypeExists(type))
	{
		return FLT_MAX;
	}
	std::map<TileType, TileDefinition>::const_iterator itD = g_TileDefinitions.find(type);
	Tile_Coord absDiff = Tile_Coord(abs(position.x - originalPos.x), abs(position.y - originalPos.y));
	bool diag = false;
	if (absDiff.x >= 1 && absDiff.y >= 1)
	{
		diag = true;
	}

	float cost = s_straightPathFindCost;
	if (diag == true)
	{
		cost = s_diagPathFindCost;
	}

	if (((Agent::GetHurtByFire(mask) || Agent::GetSlowedByFire(mask)) && itD->second.GetIsFire())
		|| (Agent::GetSlowedByWater(mask) && itD->second.GetIsLiquid())
		|| (Agent::GetSlowedByWalls(mask) && itD->second.GetIsWall()))
	{
		cost *= 2.f;
	}
	

	return cost;
}
//!----------------------------------------------------------------------
//ComputerEstimatedHCostFromPosToPos
const float Map::ComputeEstimatedHCostFromPosToPos(const Tile_Coord& pos, const Tile_Coord& goal) const
{
	Tile_Coord diff = pos - goal;
	return (float)-(abs(diff.x) + abs(diff.y));
}
//!----------------------------------------------------------------------
//GetNumberOfTilesOfTypeAboutRadius
const int Map::GetNumberOfTilesOfTypeAboutRadius(const Tile_Coord& location, const TileType& type, const int& radius, const bool& useCorners, const bool& useNSEW)
{
	std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->find(location);
	int count = 0;
	if (it == mapTiles->end())
	{
		return count;
	}
	SetAllTilesUnvisited();
	std::vector<Tile_Coord> coords;
	GetNumberOfTilesOfTypeInRadius(location, type, count, coords, radius, 0, useCorners, useNSEW);
	return count;
}
//!----------------------------------------------------------------------
//GetTileCoordsOfTypeAboutRadius
const std::vector<Tile_Coord> Map::GetTileCoordsOfTypeAboutRadius(const Tile_Coord& location, const TileType& type, const int& radius, const bool& useCorners, const bool& useNSEW)
{
	std::vector<Tile_Coord> coords;
	std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->find(location);
	if (it == mapTiles->end())
	{
		return coords;
	}
	SetAllTilesUnvisited();
	int count = 0;
	GetNumberOfTilesOfTypeInRadius(location, type, count, coords, radius, 0, useCorners, useNSEW);
	return coords;
}
//!----------------------------------------------------------------------
//GetTileCoordsOfTypeAndCountAboutRadius
const void Map::GetTileCoordsOfTypeAndCountAboutRadius(const Tile_Coord& location, const TileType& type, const int& radius,
	std::vector<Tile_Coord>& coords, int& count, const bool& useCorners, const bool& useNSEW)
{
	std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->find(location);
	count = 0;
	coords.clear();
	if (it == mapTiles->end())
	{
		return;
	}
	SetAllTilesUnvisited();
	GetNumberOfTilesOfTypeInRadius(location, type, count, coords, radius, 0, useCorners, useNSEW);
}
//!----------------------------------------------------------------------
//GetNumberOfTilesOfTypeInRadius
void Map::GetNumberOfTilesOfTypeInRadius(const Tile_Coord& currentCell, const TileType& typeToTest, int& count, std::vector<Tile_Coord>& coords,
	const int& radius, const int& radiusOut, const bool& useCorners, const bool& useNSEW)
{
	if (useCorners == false && useNSEW == false)
	{
		return;
	}
	std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->find(currentCell);
	if (radiusOut > radius || it == mapTiles->end() || it->second->GetVisited() == true)
	{
		return;
	}
	it->second->SetVisited(true);
	if (it->second->GetTileType() == typeToTest)
	{
		count++;
		coords.push_back(currentCell);
	}
	if (useNSEW == true)
	{
		GetNumberOfTilesOfTypeInRadius(it->second->GetNorthTileCoords(), typeToTest, count, coords, radius, radiusOut + 1);
		GetNumberOfTilesOfTypeInRadius(it->second->GetSouthTileCoords(), typeToTest, count, coords, radius, radiusOut + 1);
		GetNumberOfTilesOfTypeInRadius(it->second->GetEastTileCoords(), typeToTest, count, coords, radius, radiusOut + 1);
		GetNumberOfTilesOfTypeInRadius(it->second->GetWestTileCoords(), typeToTest, count, coords, radius, radiusOut + 1);
	}
	if (useCorners == true)
	{
		GetNumberOfTilesOfTypeInRadius(it->second->GetNorthEastTileCoords(), typeToTest, count, coords, radius, radiusOut + 1);
		GetNumberOfTilesOfTypeInRadius(it->second->GetNorthWestTileCoords(), typeToTest, count, coords, radius, radiusOut + 1);
		GetNumberOfTilesOfTypeInRadius(it->second->GetSouthEastTileCoords(), typeToTest, count, coords, radius, radiusOut + 1);
		GetNumberOfTilesOfTypeInRadius(it->second->GetSouthWestTileCoords(), typeToTest, count, coords, radius, radiusOut + 1);
	}
}



//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Operators
//!----------------------------------------------------------------------
//ToggleMapVisibleAlways
void Map::ToggleMapVisibleAlways()
{
	m_viewFullMapRegardlessOfVisible = !m_viewFullMapRegardlessOfVisible;
}
//!----------------------------------------------------------------------
//SetAllUnseenAndUnvisible
void Map::SetAllUnseenAndUnvisible()
{
	for (std::map<Tile_Coord, Tile*>::iterator it = mapTiles->begin(); it != mapTiles->end(); it++)
	{
		if (it->second != nullptr)
		{
			it->second->SetSeen(false);
			it->second->SetVisible(false);
		}
	}
}
//!----------------------------------------------------------------------
//SetAllUnvisible
void Map::SetAllUnvisible()
{
	for (std::map<Tile_Coord, Tile*>::iterator it = mapTiles->begin(); it != mapTiles->end(); it++)
	{
		if (it->second != nullptr)
		{
			it->second->SetVisible(false);
		}
	}
}
//!----------------------------------------------------------------------
//Update
void Map::Update(const float& deltaSeconds, const int& currentStepNumber)
{
	int stepDifference = currentStepNumber - lastStepNumber;
	for (int step = 0; step < stepDifference; step++)
	{
		UpdateTiles(deltaSeconds, currentStepNumber);
		ChangeTiles(deltaSeconds, currentStepNumber);
	}
	lastStepNumber = currentStepNumber;
}
//!----------------------------------------------------------------------
//UpdateTiles
void Map::UpdateTiles(const float& deltaSeconds, const int& currentStepNumber)
{
	//assume One Step per run
	deltaSeconds;
	currentStepNumber;
}
//!----------------------------------------------------------------------
//ChangeTiles
void Map::ChangeTiles(const float& deltaSeconds, const int& currentStepNumber)
{
	//assume One Step per run
	deltaSeconds;
	currentStepNumber;
	for (std::map<Tile_Coord, Tile*>::iterator it = mapTiles->begin(); it != mapTiles->end(); it++)
	{
		if (it->second != nullptr)
		{
			if (it->second->GetTileType() != it->second->GetNextTileType())
			{
				if (it->second->GetNextTileType() == TILE_INVALID)
				{
					it->second->SetNextTileType(it->second->GetTileType());
					continue;
				}
				it->second->SetTileType(it->second->GetNextTileType());
			}
		}
	}
}
//!----------------------------------------------------------------------
//Render
void Map::Render(const SpriteSheet* spriteSheet) const
{
	if (g_Renderer == nullptr)
	{
		return;
	}
	std::vector<Vertex_PCT> quadVertices;
	std::vector<Vertex_PCT> lineVertices;
	for (int y = 0; y < m_mapSize.y; y++)
	{
		for (int x = 0; x < m_mapSize.x; x++)
		{
			IntVector2 tileCoord(x, y);
			std::map<Tile_Coord, Tile*>::const_iterator tile_it = mapTiles->find(tileCoord);
			if (tile_it != mapTiles->end())
			{
				TileType type = tile_it->second->GetTileType();
				bool visible = tile_it->second->GetVisible();
				bool seen = tile_it->second->GetSeen();
				Rgba colorToRender = Rgba(0.f, 2.f, 9.f, 1.f);
				unsigned char car = TileDefinition::GetTypeChar(type);
				if (TileDefinition::TypeExists(type))
				{
					colorToRender = TileDefinition::GetTypeColor(type);
					if (visible == false && seen == true && m_viewFullMapRegardlessOfVisible == false && m_viewFullMapRegardlessOfVisible == false)
					{
						colorToRender = Rgba(colorToRender.Red / 2.f, colorToRender.Green / 2.f, colorToRender.Blue / 2.f, colorToRender.Alpha);
					}
					else if(visible == false && seen == false && m_viewFullMapRegardlessOfVisible == false)
					{
						colorToRender = Rgba(0.f, 0.f, 0.f, 1.f);
					}
				}
				else
				{
					colorToRender = Rgba(1.f, 1.f, 1.f, 1.f);
				}
				Vector2 bl;
				Vector2 tr;
				if (car != ' ' && spriteSheet != nullptr)
				{
					AABB2 sprVal = spriteSheet->GetTexCoordsForSpriteIndex(car);
					bl = Vector2(sprVal.mins.x, sprVal.maxs.y);
					tr = Vector2(sprVal.maxs.x, sprVal.mins.y);
				}
				else if (car == ' ' && spriteSheet != nullptr)
				{
					AABB2 sprVal = spriteSheet->GetTexCoordsForSpriteIndex(1);
					bl = Vector2(sprVal.mins.x, sprVal.maxs.y);
					tr = Vector2(sprVal.maxs.x, sprVal.mins.y);
				}
				Vector2 realTileLocation = Vector2(tileCoord.x * m_TileRenderSize.x, tileCoord.y * m_TileRenderSize.y);
				const std::vector<Vertex_PCT> singleQuad = GetVerticesForPosition(tileCoord, colorToRender, bl, tr);

				quadVertices.insert(quadVertices.end(), singleQuad.begin(), singleQuad.end());

				if (m_RenderLines == true)
				{
					lineVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x, realTileLocation.y, 0.f), m_tileLineColor));
					lineVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x + m_TileRenderSize.x, realTileLocation.y, 0.f), m_tileLineColor));

					lineVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x + m_TileRenderSize.x, realTileLocation.y, 0.f), m_tileLineColor));
					lineVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x + m_TileRenderSize.x, realTileLocation.y + m_TileRenderSize.y, 0.f), m_tileLineColor));

					lineVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x + m_TileRenderSize.x, realTileLocation.y + m_TileRenderSize.y, 0.f), m_tileLineColor));
					lineVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x, realTileLocation.y + m_TileRenderSize.y, 0.f), m_tileLineColor));

					lineVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x, realTileLocation.y + m_TileRenderSize.y, 0.f), m_tileLineColor));
					lineVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x, realTileLocation.y, 0.f), m_tileLineColor));
				}
			}
		}
	}
	g_Renderer->DrawVertexArray(quadVertices, PRIMITIVE_QUADS, spriteSheet->getSpriteSheetTexture());
	if (m_RenderLines == true)
	{
		g_Renderer->DrawVertexArray(lineVertices, PRIMITIVE_LINES, nullptr);
	}
}
//!----------------------------------------------------------------------
//FinalizeMap
void Map::FinalizeMap(Agent* player, std::vector<Agent*>& visibleAgents)
{
	if ((player == nullptr && m_finalizedOnce == true) && (player != nullptr && (player->GetMovementDirection() == Direction_None || player->GetMovementDirection() == Direction_Not_picked)))
	{
		return;
	}
	if (player != nullptr)
	{
		SetAllUnvisible();
		FieldOfViewFloodFill::CalculateFieldOfViewForAgent(player, player->GetSightRadius(), this, visibleAgents, true);
		Agent::SetListActorsVisibleToPlayer(player->GetVisibleAgents());
		Item::s_itemsVisibleToPlayer = player->GetVisibleItems();
	}
	else if (player == nullptr)
	{
		for (std::map<Tile_Coord, Tile*>::iterator it = mapTiles->begin(); it != mapTiles->end(); it++)
		{
			if (it != mapTiles->end() && it->second != nullptr)
			{
				Tile_Coord dirs[8];
				dirs[0] = it->second->GetNorthWestTileCoords();
				dirs[1] = it->second->GetNorthTileCoords();
				dirs[2] = it->second->GetNorthEastTileCoords();
				dirs[3] = it->second->GetEastTileCoords();
				dirs[4] = it->second->GetSouthEastTileCoords();
				dirs[5] = it->second->GetSouthTileCoords();
				dirs[6] = it->second->GetSouthWestTileCoords();
				dirs[7] = it->second->GetWestTileCoords();
				bool solids[8];
				for (size_t i = 0; i < 8; i++)
				{
					solids[i] = false;
				}

				for (size_t i = 0; i < 8; i++)
				{
					Tile_Coord coord = dirs[i];
					if (coord.x < 0 || coord.x >= m_mapSize.x
						|| coord.y < 0 || coord.y >= m_mapSize.y)
					{
						solids[i] = true;
					}
					else
					{
						std::map<Tile_Coord, Tile*>::iterator itNow = mapTiles->find(coord);
						if (itNow == mapTiles->end() || itNow->second == nullptr)
						{
							solids[i] = true;
						}
						else
						{
							TileType type = itNow->second->GetTileType();
							bool exists = false;
							bool solid = TileDefinition::GetTypeSolid(type, exists);
							if (exists == true)
							{
								solids[i] = solid;
							}
						}
					}
				}

				if (solids[0] == true
					&& solids[1] == true
					&& solids[2] == true
					&& solids[3] == true
					&& solids[4] == true
					&& solids[5] == true
					&& solids[6] == true
					&& solids[7] == true)
				{
					it->second->SetVisible(false);
				}
				else
				{
					it->second->SetVisible(true);
				}

			}
		}
		m_finalizedOnce = true;
	}

}
//!----------------------------------------------------------------------
//PreInitializationSetMapSize
void Map::PreInitializationSetMapSize(const IntVector2& size)
{
	for (int x = 0; x < m_mapSize.x; x++)
	{
		for (int y = 0; y < m_mapSize.y; y++)
		{
			std::map<IntVector2, Tile*>::iterator it = mapTiles->find(IntVector2(x, y));
			if (it != mapTiles->end() && it->second != nullptr)
			{
				delete it->second;
				it->second = nullptr;
			}
		}
	}

	m_mapSize = size;
}
//!----------------------------------------------------------------------
//GetNeighborTilesAll
std::vector<Tile*> Map::GetNeighborTilesAll(const Tile_Coord& coords, const bool& getNSEW, const bool& getCorners) const
{
	//Order is always 0:: N, 1:: NE, 2:: E, 3::SE, 4:: S, 5:: SW, 6:: W, 7:: NW
	std::vector<Tile*> neighbors;
	neighbors.resize(8);
	std::map<Tile_Coord, Tile*>::iterator centerIt = mapTiles->find(coords);
	if ((getNSEW == false && getCorners == false) || centerIt == mapTiles->end() || centerIt->second == nullptr)
	{
		for (size_t i = 0; i < 8; i++)
		{
			neighbors[i] = nullptr;
		}
		return neighbors;
	}


	//~!-------------------------------------------------------------------
	//NSEW locations
	if (getNSEW == true)
	{
		Tile_Coord northCoord = centerIt->second->GetNorthTileCoords();
		Tile_Coord southCoord = centerIt->second->GetSouthTileCoords();
		Tile_Coord eastCoord = centerIt->second->GetEastTileCoords();
		Tile_Coord westCoord = centerIt->second->GetWestTileCoords();
		std::map<Tile_Coord, Tile*>::iterator northIt = mapTiles->find(northCoord);
		std::map<Tile_Coord, Tile*>::iterator southIt = mapTiles->find(southCoord);
		std::map<Tile_Coord, Tile*>::iterator eastIt = mapTiles->find(eastCoord);
		std::map<Tile_Coord, Tile*>::iterator westIt = mapTiles->find(westCoord);

		//North
		if (northIt != mapTiles->end() && northIt->second != nullptr)
		{
			neighbors[0] = northIt->second;
		}
		else
		{
			neighbors[0] = nullptr;
		}

		//East
		if (eastIt != mapTiles->end() && eastIt->second != nullptr)
		{
			neighbors[2] = (eastIt->second);
		}
		else
		{
			neighbors[2] = (nullptr);
		}

		//South
		if (southIt != mapTiles->end() && southIt->second != nullptr)
		{
			neighbors[4] = (southIt->second);
		}
		else
		{
			neighbors[4] = (nullptr);
		}

		//West
		if (westIt != mapTiles->end() && westIt->second != nullptr)
		{
			neighbors[6] = (westIt->second);
		}
		else
		{
			neighbors[6] = (nullptr);
		}
	}
	else
	{
		neighbors[0] = nullptr;
		neighbors[2] = nullptr;
		neighbors[4] = nullptr;
		neighbors[6] = nullptr;
	}

	//~!-------------------------------------------------------------------
	//Corner locations
	if (getCorners == true)
	{
		Tile_Coord northEastCoord = centerIt->second->GetNorthEastTileCoords();
		Tile_Coord northWestCoord = centerIt->second->GetNorthWestTileCoords();
		Tile_Coord southEastCoord = centerIt->second->GetSouthEastTileCoords();
		Tile_Coord southWestCoord = centerIt->second->GetSouthWestTileCoords();
		std::map<Tile_Coord, Tile*>::iterator northEastIt = mapTiles->find(northEastCoord);
		std::map<Tile_Coord, Tile*>::iterator northWestIt = mapTiles->find(northWestCoord);
		std::map<Tile_Coord, Tile*>::iterator southEastIt = mapTiles->find(southEastCoord);
		std::map<Tile_Coord, Tile*>::iterator southWestIt = mapTiles->find(southWestCoord);

		//North East
		if (northEastIt != mapTiles->end() && northEastIt->second != nullptr)
		{
			neighbors[1] = (northEastIt->second);
		}
		else
		{
			neighbors[1] = (nullptr);
		}

		//South East
		if (southEastIt != mapTiles->end() && southEastIt->second != nullptr)
		{
			neighbors[3] = (southEastIt->second);
		}
		else
		{
			neighbors[3] = (nullptr);
		}

		//South West
		if (southWestIt != mapTiles->end() && southWestIt->second != nullptr)
		{
			neighbors[5] = (southWestIt->second);
		}
		else
		{
			neighbors[5] = (nullptr);
		}

		//North West
		if (northWestIt != mapTiles->end() && northWestIt->second != nullptr)
		{
			neighbors[7] = (northWestIt->second);
		}
		else
		{
			neighbors[7] = (nullptr);
		}
	}
	else
	{
		neighbors[1] = nullptr;
		neighbors[3] = nullptr;
		neighbors[5] = nullptr;
		neighbors[7] = nullptr;
	}
	return neighbors;
}
std::vector<Tile*> Map::GetAllUnoccupiedTilesOfType(const TileType& type)
{
	std::vector<Tile*> tiles;

	for (std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->begin(); it != mapTiles->end(); it++)
	{
		if (it->second != nullptr)
		{
			TileType typ = it->second->GetTileType();
			if (typ == type)
			{
				Tile_Coord bl = it->second->GetTileCoords();
				bool occupied = false;
				for (size_t i = 0; i < Agent::s_Agents.size(); i++)
				{
					Agent* age = Agent::s_Agents.at(i);
					if (age->GetBlTilePos() == bl)
					{
						occupied = true;
						break;
					}
				}
				if (occupied == true)
				{
					continue;
				}
				tiles.push_back(it->second);
			}
		}
	}

	return tiles;
}
std::vector<Tile_Coord> Map::GetNeighborTileCoordsAll(const Tile_Coord& coords, const bool& getNSEW, const bool& getCorners) const
{
	//Order is always 0:: N, 1:: NE, 2:: E, 3::SE, 4:: S, 5:: SW, 6:: W, 7:: NW
	std::vector<Tile_Coord> neighbors;
	std::map<Tile_Coord, Tile*>::iterator centerIt = mapTiles->find(coords);


	//~!-------------------------------------------------------------------
	//NSEW locations
	if (getNSEW == true)
	{
		Tile_Coord northCoord = centerIt->second->GetNorthTileCoords();
		Tile_Coord southCoord = centerIt->second->GetSouthTileCoords();
		Tile_Coord eastCoord = centerIt->second->GetEastTileCoords();
		Tile_Coord westCoord = centerIt->second->GetWestTileCoords();
		std::map<Tile_Coord, Tile*>::iterator northIt = mapTiles->find(northCoord);
		std::map<Tile_Coord, Tile*>::iterator southIt = mapTiles->find(southCoord);
		std::map<Tile_Coord, Tile*>::iterator eastIt = mapTiles->find(eastCoord);
		std::map<Tile_Coord, Tile*>::iterator westIt = mapTiles->find(westCoord);

		//North
		if (northIt != mapTiles->end() && northIt->second != nullptr)
		{
			neighbors.push_back(northCoord);
		}

		//East
		if (eastIt != mapTiles->end() && eastIt->second != nullptr)
		{
			neighbors.push_back(eastCoord);
		}

		//South
		if (southIt != mapTiles->end() && southIt->second != nullptr)
		{
			neighbors.push_back(southCoord);
		}

		//West
		if (westIt != mapTiles->end() && westIt->second != nullptr)
		{
			neighbors.push_back(westCoord);
		}
	}

	//~!-------------------------------------------------------------------
	//Corner locations
	if (getCorners == true)
	{
		Tile_Coord northEastCoord = centerIt->second->GetNorthEastTileCoords();
		Tile_Coord northWestCoord = centerIt->second->GetNorthWestTileCoords();
		Tile_Coord southEastCoord = centerIt->second->GetSouthEastTileCoords();
		Tile_Coord southWestCoord = centerIt->second->GetSouthWestTileCoords();
		std::map<Tile_Coord, Tile*>::iterator northEastIt = mapTiles->find(northEastCoord);
		std::map<Tile_Coord, Tile*>::iterator northWestIt = mapTiles->find(northWestCoord);
		std::map<Tile_Coord, Tile*>::iterator southEastIt = mapTiles->find(southEastCoord);
		std::map<Tile_Coord, Tile*>::iterator southWestIt = mapTiles->find(southWestCoord);

		//North East
		if (northEastIt != mapTiles->end() && northEastIt->second != nullptr)
		{
			neighbors.push_back(northEastCoord);
		}

		//South East
		if (southEastIt != mapTiles->end() && southEastIt->second != nullptr)
		{
			neighbors.push_back(southEastCoord);
		}

		//South West
		if (southWestIt != mapTiles->end() && southWestIt->second != nullptr)
		{
			neighbors.push_back(southWestCoord);
		}

		//North West
		if (northWestIt != mapTiles->end() && northWestIt->second != nullptr)
		{
			neighbors.push_back(northWestCoord);
		}
	}
	return neighbors;
}
std::vector<Tile_Coord> Map::GetAllUnoccupiedTileCoordsOfType(const TileType& type)
{
	std::vector<Tile_Coord> tiles;

	for (std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->begin(); it != mapTiles->end(); it++)
	{
		if (it->second != nullptr)
		{
			TileType typ = it->second->GetTileType();
			if (typ == type)
			{
				Tile_Coord bl = it->second->GetTileCoords();
				bool occupied = false;
				for (size_t i = 0; i < Agent::s_Agents.size(); i++)
				{
					Agent* age = Agent::s_Agents.at(i);
					if (age->GetBlTilePos() == bl)
					{
						occupied = true;
						break;
					}
				}
				if (occupied == true)
				{
					continue;
				}
				tiles.push_back(it->second->GetTileCoords());
			}
		}
	}

	return tiles;
}





//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Getters

//!----------------------------------------------------------------------
//GetIsTileCoordSolid
const Vector2 Map::GetTileRenderSize() const
{
	return m_TileRenderSize; 
}
const bool Map::GetViewFullMapRegardlessOfVisibility() const
{ 
	return m_viewFullMapRegardlessOfVisible; 
}
const bool Map::GetIsTileCoordSolid(Tile_Coord coord, Tile*& tile, const int& mask) const
{
	tile = nullptr;
	std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(coord);
	if (it == mapTiles->end() || it->second == nullptr)
	{
		tile = nullptr;
		return true;
	}
	else
	{
		TileType type = it->second->GetTileType();
		tile = it->second;
		bool exists = false;
		bool solid = false;
		if ((type == TILE_AIR && Agent::GetBlockedByAir(mask))
			|| (TileDefinition::GetTypeSolid(type,exists) && Agent::GetBlockedByWalls(mask))
			|| (TileDefinition::GetTypeIsFire(type, exists) && Agent::GetBlockedByFire(mask))
			|| (TileDefinition::GetTypeIsLiquid(type, exists) && Agent::GetBlockedByWater(mask)))
		{
			solid = true;
		}
		if (exists == true && solid == true)
		{
			return true;
		}
		return false;
	}
}
//!----------------------------------------------------------------------
//GetVerticesForPosition
const std::vector<Vertex_PCT> Map::GetVerticesForPosition(const Tile_Coord& tileCoord, const Rgba& colorToRender, const Vector2& bl, const Vector2& tr) const
{
	std::vector<Vertex_PCT> quadVertices;
	Vector2 br = Vector2(tr.x, bl.y);
	Vector2 tl = Vector2(bl.x, tr.y);
	Vector2 realTileLocation = Vector2(tileCoord.x * m_TileRenderSize.x, tileCoord.y * m_TileRenderSize.y);
	quadVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x, realTileLocation.y, 0.f), colorToRender, bl));
	quadVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x + m_TileRenderSize.x, realTileLocation.y, 0.f), colorToRender, br));
	quadVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x + m_TileRenderSize.x, realTileLocation.y + m_TileRenderSize.y, 0.f), colorToRender, tr));
	quadVertices.push_back(Vertex_PCT(Vector3(realTileLocation.x, realTileLocation.y + m_TileRenderSize.y, 0.f), colorToRender, tl));
	return quadVertices;
}
//!----------------------------------------------------------------------
//GetAllNonSolidValidTileCoords
const std::vector<Tile_Coord> Map::GetAllNonSolidValidTileCoords() const
{
	std::vector<Tile_Coord> coords;
	for (std::map<Tile_Coord, Tile*>::const_iterator it = mapTiles->begin(); it != mapTiles->end(); it++)
	{
		if (it->second == nullptr)
		{
			continue;
		}
		TileType type = it->second->GetTileType();
		bool exists = false;
		bool solid = TileDefinition::GetTypeSolid(type, exists);
		if (exists == false || solid == true)
		{
			continue;
		}
		else
		{
			coords.push_back(it->first);
		}
	}
	return coords;
}
//!----------------------------------------------------------------------
//getAllNonSolidAndNonOccupiedTileCoords
const std::vector<Tile_Coord> Map::GetAllNonSolidAndNonOccupiedTileCoords() const
{
	std::vector<Tile_Coord> coords = GetAllNonSolidValidTileCoords();
	for (size_t i = 0; i < coords.size(); i++)
	{
		Tile_Coord coord = coords.at(i);
		for (size_t j = 0; j < Agent::s_Agents.size(); j++)
		{
			Agent* curAge = Agent::s_Agents.at(j);
			Tile_Coord blPosition = curAge->GetBlTilePos();
			IntVector2 size = curAge->GetSize();
			Tile_Coord trPosition = blPosition + size - IntVector2(1, 1);
			if (coord.x >= blPosition.x && coord.x <= trPosition.x
				&& coord.y >= blPosition.y && coord.y <= trPosition.y)
			{
				coords.erase(coords.begin() + i);
				i--;
				break;
			}
		}
	}
	return coords;
}
//!----------------------------------------------------------------------
//GetTileCoordsOfTileType
const void Map::GetTileCoordsOfTileType(const TileType& type, std::vector<Tile_Coord>& coords, std::vector<Tile_Coord>& everywhereElse)
{
	coords.clear();
	for (std::map<Tile_Coord, Tile*>::iterator it = mapTiles->begin(); it != mapTiles->end(); it++)
	{
		if (it->second != nullptr && it->second->GetTileType() == type)
		{
			coords.push_back(it->first);
		}
		else if (it->second != nullptr && it->second->GetTileType() != type)
		{
			everywhereElse.push_back(it->first);
		}
	}
}





//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//Setters
//!----------------------------------------------------------------------
//SetTileTypeAtCoords
void Map::SetTileTypeAtCoords(const IntVector2& tile, const TileType& type, const bool& setNextAsWell)
{
	std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(tile);
	if (it != mapTiles->end() && it->second != nullptr)
	{
		it->second->SetTileType(type);
		if (setNextAsWell)
		{
			it->second->SetNextTileType(type);
		}
	}
}
//!----------------------------------------------------------------------
//SetAllTilesUnvisited
void Map::SetAllTilesUnvisited()
{
	for (int y = 0; y < m_mapSize.y; y++)
	{
		for (int x = 0; x < m_mapSize.x; x++)
		{
			Tile_Coord tileCoord(x, y);
			std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(tileCoord);
			if (it != mapTiles->end() && it->second != nullptr)
			{
				it->second->SetVisited(false);
			}
		}
	}
}