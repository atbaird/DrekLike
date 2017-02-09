#pragma once

#ifndef FROMDATAGENERATOR_HPP
#define FROMDATAGENERATOR_HPP

#include "Game/Generators/MapGenerators/MapGenerator.hpp"
#include "Game/Map/TileData.hpp"


Generator* CreateFromDataGenerator(const std::string& name);
EnvironmentGenerationProcess* FromDataGenerationProcessCreateFunc(const XMLNode& node);

class FromDataGenerator : public MapGenerator
{
private:
	static const GeneratorRegistration FromDataGeneratorRegistration;
	std::vector<TileData> m_data;
public:

	FromDataGenerator();
	~FromDataGenerator();

	void SetData(const std::vector<TileData>& data) { m_data = data; };

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