#pragma once

#ifndef NPCFACTORY_HPP
#define NPCFACTORY_HPP
#include "Game/Generators/Generator.hpp"
#include "Game/GameEntity/AgentType/NPC/NPCTemplate.hpp"
#include "Game/Map/TileType.hpp"

class NPCFactory : public Generator
{
private:
	NPCTemplate m_template;
	std::string m_name;
	std::vector<TileType> m_preferredSpawnTileType;
protected:
	static std::map<std::string, NPCFactory*> s_factories;
	static std::vector<std::string> s_factoryNames;
public:
	NPCFactory();
	NPCFactory(EnvironmentGenerationProcess* data);
	~NPCFactory();

	static const int GenerateNMonsters(const int& n, SpriteSheet* spriteSheet);
	static EnvironmentGenerationProcess* NPCGenerationProcCreateFunc(const XMLNode& node);
	static Generator* NPCGeneratorCreationFunc(const std::string& name);

	//Setters
	void SetTemplate(NPCTemplate temp);
	static void GetColorOutOfData(const std::string& val, float& val1, float& val2);
	//Getters
	const std::vector<TileType> GetPreferredSpawnTileType() const;
	const std::string GetName();
	const IntVector2 GetMonsterSize() const;

	const NPCTemplate GetTemplate() const;

	static void ClearAllNPCFactories();
	static void LoadAllNPCS();

};
#endif