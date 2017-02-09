#pragma once
#include <string>
#include <map>
#include "Game/Generators/EnvironmentGenerationProcess.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

#ifndef GENERATOR_HPP
#define GENERATOR_HPP


//-------------------------------------------------------------------------
//GeneratorRegistration
class Generator;

typedef Generator* (GeneratorCreationFunc)(const std::string& name);
typedef EnvironmentGenerationProcess* GenerationProcessCreateFunc(const XMLNode& node);


class GeneratorRegistration
{
private:
	std::string m_name;
	GeneratorCreationFunc* m_creationFunc;
	GenerationProcessCreateFunc* m_processCreationFunc;
public:
	GeneratorRegistration(const std::string& name, GeneratorCreationFunc* creationFunc, GenerationProcessCreateFunc* processCreationFunc);

	static Generator* CreateGeneratorByname(const std::string& name);
	static EnvironmentGenerationProcess* GetGenerationProcessData(const std::string& name, const XMLNode& node);
	std::map<std::string, GeneratorRegistration>* GetGeneratorRegistrations() { return s_generatorRegistrationMap; };

	static void ClearAllGeneratorRegistrations();
	static void RemoveRegistration(const std::string& regName);
	protected:
		static std::map<std::string, GeneratorRegistration>* s_generatorRegistrationMap;
};


//-------------------------------------------------------------
//Generator
enum GeneratorType
{
	GENERATOR_TYPE_INVALID = -1,
	GENERATOR_TYPE_MAP,
	GENERATOR_TYPE_NPC,
	GENERATOR_TYPE_ITEM,
	GENERATOR_TYPE_FEATURE,
	NUMBER_OF_GENERATOR_TYPES
};


class Generator
{
private:
	GeneratorType m_genType;
	std::string m_name;
public:
	Generator(const GeneratorType& type = GENERATOR_TYPE_INVALID);
	~Generator();

	virtual void SetName(const std::string& name) { m_name = name; };
	void SetGeneratorType(const GeneratorType& type) { m_genType = type; };
	const GeneratorType GetGeneratorType() const { return m_genType; };

	static void LoadAllFeatures();
	static void LoadAllNPCS();
	static const int GenerateNMonsters(const int& n, SpriteSheet* spriteSheet);
	static const int GenerateNItems(const int& n, SpriteSheet* spriteSheet);
};
#endif // !GENERATOR_HPP
