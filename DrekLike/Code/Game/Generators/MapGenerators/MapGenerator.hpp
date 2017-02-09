#pragma once

#include "Game/Generators/Generator.hpp"
#include "Game/Map/Map.hpp"
#include <string>

#ifndef MAPGENERATOR_HPP
#define MAPGENERATOR_HPP



class MapGenerator : public Generator
{
private:
public:
	IntVector2 lastVisitedCell;
	//Constructors
	MapGenerator();
	~MapGenerator();

	static void RegisterAllMapGenerators(int& numberOfMapGenerators);
	static void RunGenerationInstructions(const std::string& xmlFileLocation, Map*& m_map, const Vector2& windowSize = Vector2(16.f,9.f), const float& messageBoxSize = 2.5f);

	virtual void SetName(const std::string& name) override { Generator::SetName(name); };
	virtual const IntVector2 GetLastVisited() const { return lastVisitedCell; };
	virtual const void SetLastVisited(const IntVector2& lastVisited) { lastVisitedCell = lastVisited; };
	virtual const void ResetLastVisited() { lastVisitedCell = IntVector2(0, 0); };
	virtual const void GenerateBasedOnDataInput(EnvironmentGenerationProcess* data, Map*& map);
	virtual void InitializeExtraVariables(const IntVector2& mapSize);
	virtual Map* CreateInitializedMap(const std::string& mapName, IntVector2& mapSize);
	virtual Map* GenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords);
	virtual	Map* FastGenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords);
	virtual const bool GenerateStep(Map* map, const float& deltaSeconds, int& currentStepNumber);
};
#endif