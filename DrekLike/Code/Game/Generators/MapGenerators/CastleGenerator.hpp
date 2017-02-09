#pragma once

#ifndef CASTLEGENERATOR_HPP
#define CASTLEGENERATOR_HPP
#include "Game/Generators/MapGenerators/MapGenerator.hpp"


Generator* CreateCastleGenerator(const std::string& name);
EnvironmentGenerationProcess* CastleGenerationProcessCreateFunc(const XMLNode& node);

class CastleGenerator : public MapGenerator
{
private:
	static const GeneratorRegistration CastleGeneratorRegistration;
public:
	//Constructors
	CastleGenerator();
	~CastleGenerator();

	//overrides
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