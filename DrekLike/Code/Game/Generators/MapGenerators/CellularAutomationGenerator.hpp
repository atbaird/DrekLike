#pragma once

#include "Game/Generators/MapGenerators/MapGenerator.hpp"

/*
For a space that is 'populated':
	Each cell with one or no neighbors dies, as if by solitude.
	Each cell with four or more neighbors dies, as if by overpopulation.
	Each cell with two or three neighbors survives.

For a space that is 'empty' or 'unpopulated'
	Each cell with three neighbors becomes populated.
*/

#ifndef CELLULARAUTOMATIONGENERATOR_HPP
#define CELLULARAUTOMATIONGENERATOR_HPP

Generator* CreateCellularAutomationGenerator(const std::string& name);
EnvironmentGenerationProcess* CellularAutomationGenerationProcessCreateFunc(const XMLNode& node);

class CellularAutomationGenerator : public MapGenerator
{
private:
	static const GeneratorRegistration CellularAutomationGeneratorRegistration;
	float m_stonePercentage;
public:
	CellularAutomationGenerator();
	~CellularAutomationGenerator();

	virtual void SetName(const std::string& name) override { MapGenerator::SetName(name); };
	virtual const void SetLastVisited(const IntVector2& lastVisited) override { MapGenerator::SetLastVisited(lastVisited); };
	virtual const void ResetLastVisited() override { MapGenerator::ResetLastVisited(); };
	virtual const IntVector2 GetLastVisited() const  override { return MapGenerator::GetLastVisited(); };
	virtual const void GenerateBasedOnDataInput(EnvironmentGenerationProcess* data, Map*& map) override;
	virtual void InitializeExtraVariables(const IntVector2& mapSize) override;
	virtual Map* CreateInitializedMap(const std::string& mapName, IntVector2& mapSize) override;
	virtual Map* GenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords) override;
	virtual Map* FastGenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoords, const IntVector2& endCoords) override;
	virtual const bool GenerateStep(Map* map, const float& deltaSeconds, int& currentStepNumber) override;
};
#endif
