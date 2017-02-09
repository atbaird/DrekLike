#pragma once

#include "Game/Generators/MapGenerators/MapGenerator.hpp"

#ifndef DUNGEONMAPGENERATOR_HPP
#define DUNGOENMAPGENERATOR_HPP

Generator* CreateDungeonMapGenerator(const std::string& name);
EnvironmentGenerationProcess* DungeonGenerationProcessCreateFunc(const XMLNode& node);

class DungeonMapGenerator : public MapGenerator
{
private:
	static const IntVector2 rangeOutForPerlinWorms;
	static const IntVector2 minRoomSize;
	static const IntVector2 maxRoomSize;
	static const GeneratorRegistration DungeonMapRegistration;
	bool m_makeDoors;
	bool m_loadedRooms;
public:
	DungeonMapGenerator();
	virtual ~DungeonMapGenerator();

	virtual void SetName(const std::string& name) override { MapGenerator::SetName(name); };
	virtual const void ResetLastVisited() override;
	virtual const void SetLastVisited(const IntVector2& lastVisited) override { MapGenerator::SetLastVisited(lastVisited); };
	virtual const IntVector2 GetLastVisited() const override { return MapGenerator::GetLastVisited(); };
	virtual const void GenerateBasedOnDataInput(EnvironmentGenerationProcess* data, Map*& map) override;
	virtual void InitializeExtraVariables(const IntVector2& mapSize) override;
	virtual Map* CreateInitializedMap(const std::string& mapName, IntVector2& mapSize) override;
	virtual Map* GenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords) override;
	virtual Map* FastGenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords) override;
	virtual const bool GenerateStep(Map* map, const float& deltaSeconds, int& currentStepNumber) override;
private:
	static void GenerateDoors();
};
#endif