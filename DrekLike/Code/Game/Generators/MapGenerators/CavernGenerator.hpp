#pragma once

#ifndef CAVERNGENERATOR_HPP
#define CAVERNGENERATOR_HPP
#include "Game/Generators/MapGenerators/MapGenerator.hpp"


Generator* CreateCavernGenerator(const std::string& name);
EnvironmentGenerationProcess* CavernGenerationProcessCreateFunc(const XMLNode& node);

class CavernGenerator : public MapGenerator
{
private:
	static const GeneratorRegistration CavernGeneratorRegistration;
public:
	CavernGenerator();
	~CavernGenerator();

	virtual void SetName(const std::string& name) override { MapGenerator::SetName(name); };
	virtual const IntVector2 GetLastVisited() const override { return MapGenerator::GetLastVisited(); };
	virtual const void SetLastVisited(const IntVector2& lastVisited) override { MapGenerator::SetLastVisited(lastVisited); };
	virtual const void ResetLastVisited() override { MapGenerator::ResetLastVisited(); };
	virtual const void GenerateBasedOnDataInput(EnvironmentGenerationProcess* data, Map*& map) override;
	virtual void InitializeExtraVariables(const IntVector2& mapSize) override;
	virtual Map* CreateInitializedMap(const std::string& mapName, IntVector2& mapSize) override;
	virtual Map* GenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoord, const IntVector2& endCoord) override;
	virtual	Map* FastGenerateMap(Map* map, const IntVector2& size, const IntVector2& startCoord, const IntVector2& endCoord) override;
	virtual const bool GenerateStep(Map* map, const float& deltaSeconds, int& currentStepNumber) override;
};
#endif