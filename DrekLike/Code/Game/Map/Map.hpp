#pragma once

#include "Game/CommonDefinitions.hpp"
#include "Game/Map/TileDefinition.hpp"
#include "Game/Map/Tile.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/Vertex_PCT.hpp"
#include "Game/GameEntity/Agent.hpp"
#include <vector>
#include <map>

#ifndef MAP_HPP
#define MAP_HPP

enum MAP_MODE
{
	MAP_MODE_INVALID = -1,
	MAP_MODE_TEST,
	MAP_MODE_CELLULAR_AUTOMATION,
	MAP_MODE_DUNGEON,
	MAP_MODE_XML,
	NUMBER_OF_MAP_MODES
};

class Map;
extern Map* g_map;

class Map
{
private:
	static const float s_straightPathFindCost;
	static const float s_diagPathFindCost;

	IntVector2 m_mapSize;
	std::string m_mapName;
	std::map<Tile_Coord, Tile*>* mapTiles;
	Vector2 m_TileRenderSize;
	Rgba m_tileLineColor;
	int lastStepNumber;
	bool m_RenderLines;
	bool m_finalizedOnce;
	bool m_viewFullMapRegardlessOfVisible;
	MAP_MODE m_mapMode;
public:
	
	//Constructors
	Map(const IntVector2& mapSize = IntVector2(20, 20), const MAP_MODE& mode = MAP_MODE_TEST, const Vector2& tileRenderSize = Vector2(1.f,1.f), const bool& renderLines = false);
	virtual ~Map();

	//Functions
	void InitializeMap(const TileType& type = TILE_AIR);
	void ClearMap();
	void WriteToXMLNode(XMLNode& node);
	void ReadFromXMLNode(GenerationProcessData* data);
	const std::vector<Tile_Coord> GetOpenCoordsAboutEntityBLAndTR(const IntVector2& bl, const IntVector2& tr) const;
	const std::vector<Tile_Coord> GetOpenCoordsAboutEntityBLAndTRWithRespectToActors(const IntVector2& bl, const IntVector2& tr, Agent* ignoreMe) const;
	const std::vector<Tile_Coord> FindAdjacentValidPositions(const Tile_Coord& pos);
	const std::vector<Tile_Coord> FindAdjacentValidPositionsThatMeetMask(const Tile_Coord& pos, const int& mask);
	const std::vector<Tile_Coord> FindAdjacentValidPositionsThatDontHurtSlowOrBlock(const Tile_Coord& pos, const int& mask);
	const std::vector<Tile_Coord> FindAdjacentValidPositionsThatDontBlockOrHurt(const Tile_Coord& pos, const int& mask);
	const float ComputeLocalG(const Tile_Coord& position, const Tile_Coord& originalPos, const int& mask) const;
	const float ComputeEstimatedHCostFromPosToPos(const Tile_Coord& pos, const Tile_Coord& goal) const;
	const int GetNumberOfTilesOfTypeAboutRadius(const Tile_Coord& location, const TileType& type, const int& radius, const bool& useCorners = false,
		const bool& useNSEW = true);
	const std::vector<Tile_Coord> GetTileCoordsOfTypeAboutRadius(const Tile_Coord& location, const TileType& type, const int& radius,
		const bool& useCorners = false, const bool& useNSEW = true);
	const void GetTileCoordsOfTypeAndCountAboutRadius(const Tile_Coord& location, const TileType& type, const int& radius, std::vector<Tile_Coord>& coords,
		int& count, const bool& useCorners = false, const bool& useNSEW = true);
private:
	void GetNumberOfTilesOfTypeInRadius(const Tile_Coord& currentCell, const TileType& typeToTest, int& count, std::vector<Tile_Coord>& coords,
		const int& radius, const int& radiusOut, const bool& useCorners = false, const bool& useNSEW = true);

public:
	//Operators
	void ToggleMapVisibleAlways();
	void SetAllUnseenAndUnvisible();
	void SetAllUnvisible();
	virtual void Update(const float& deltaSeconds, const int& currentStepNumber);
	virtual void UpdateTiles(const float& deltaSeconds, const int& currentStepNumber);
	virtual void ChangeTiles(const float& deltaSeconds, const int& currentStepNumber);
	virtual void Render(const SpriteSheet* spriteSheet = nullptr) const;
	virtual void FinalizeMap(Agent* player, std::vector<Agent*>& visibleAgents);
	void PreInitializationSetMapSize(const IntVector2& size);
	void PreInitializationSetMapName(const std::string& name) { m_mapName = name; };
	std::vector<Tile*> GetNeighborTilesAll(const Tile_Coord& coords, const bool& getNSEW = true, const bool& getCorners = true) const;
	std::vector<Tile*> GetAllUnoccupiedTilesOfType(const TileType& type);
	std::vector<Tile_Coord> GetNeighborTileCoordsAll(const Tile_Coord& coords, const bool& getNSEW = true, const bool& getCorners = true) const;
	std::vector<Tile_Coord> GetAllUnoccupiedTileCoordsOfType(const TileType& type);


	//Getters
	const Vector2 GetTileRenderSize() const;
	const bool GetViewFullMapRegardlessOfVisibility() const;
	const bool GetIsTileCoordSolid(Tile_Coord coord, Tile*& tile, const int& mask) const;
	const std::vector<Vertex_PCT> GetVerticesForPosition(const Tile_Coord& tileCoord, const Rgba& colorToRender, const Vector2& bl, const Vector2& tr) const;
	const std::vector<Tile_Coord> GetAllNonSolidValidTileCoords() const;
	const std::vector<Tile_Coord> GetAllNonSolidAndNonOccupiedTileCoords() const;
	const MAP_MODE GetMapMode() const { return m_mapMode; };
	const void GetTileCoordsOfTileType(const TileType& type, std::vector<Tile_Coord>& coords, std::vector<Tile_Coord>& everywhereElse);
	const IntVector2 GetMapSize() const { return m_mapSize; };
	std::map<Tile_Coord, Tile*>* GetMapTiles() const { return mapTiles; };

	//Setters
	void ChangeTileRenderSize(const Vector2& tileRenderSize) { m_TileRenderSize = tileRenderSize; };
	void ChangeRenderLines(const bool& renderLines) { m_RenderLines = renderLines; };
	void SetTileTypeAtCoords(const IntVector2& tile, const TileType& type, const bool& setNextAsWell = false);
private:
	void SetAllTilesUnvisited();
};

#endif