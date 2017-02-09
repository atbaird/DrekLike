#include "Game/Generators/Generator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/Generators/NPCFactories/NPCFactory.hpp"
#include "Game/Items/ItemFactory.hpp"
#include "Game/Features/FeatureFactory.hpp"
#include "Game/Generators/MapGenerators/MapGenerator.hpp"

std::map<std::string, GeneratorRegistration>* GeneratorRegistration::s_generatorRegistrationMap = nullptr;



GeneratorRegistration::GeneratorRegistration(const std::string& name, GeneratorCreationFunc* creationFunc, GenerationProcessCreateFunc* processCreationFunc)
	: m_name(name),
	m_creationFunc(creationFunc),
	m_processCreationFunc(processCreationFunc)
{
	if (s_generatorRegistrationMap == nullptr)
	{
		s_generatorRegistrationMap = new std::map<std::string, GeneratorRegistration>();
	}
	//find verify not in map under that name
	//insert
	std::map<std::string, GeneratorRegistration>::iterator it = s_generatorRegistrationMap->find(name);
	if (it == s_generatorRegistrationMap->end())
	{
		s_generatorRegistrationMap->insert(std::pair<std::string, GeneratorRegistration>(m_name, *this));
	}
	else
	{
		//Complain
		GUARANTEE_RECOVERABLE(false, name + " GeneratorRegistration already exists!");
	}
}
Generator* GeneratorRegistration::CreateGeneratorByname(const std::string& name)
{
	if (s_generatorRegistrationMap == nullptr)
	{
		s_generatorRegistrationMap = new std::map<std::string, GeneratorRegistration>();
	}
	Generator* generator = nullptr;
	std::map<std::string, GeneratorRegistration>::iterator gRegIter = s_generatorRegistrationMap->find(name);
	if (gRegIter != s_generatorRegistrationMap->end())
	{
		GeneratorRegistration reg = gRegIter->second;
		if (reg.m_creationFunc != nullptr)
		{
			generator = (reg.m_creationFunc)(reg.m_name);
		}
	}
	else
	{
		//complain
		GUARANTEE_RECOVERABLE(false, name + " GeneratorRegistration does not exist!");
	}
	return generator;
}
EnvironmentGenerationProcess* GeneratorRegistration::GetGenerationProcessData(const std::string& name, const XMLNode& node)
{
	if (s_generatorRegistrationMap == nullptr)
	{
		return nullptr;
	}
	std::map<std::string, GeneratorRegistration>::iterator gRegIter = s_generatorRegistrationMap->find(name);
	if (gRegIter != s_generatorRegistrationMap->end())
	{
		return gRegIter->second.m_processCreationFunc(node);
	}
	return nullptr;

}

void GeneratorRegistration::ClearAllGeneratorRegistrations()
{
	NPCFactory::ClearAllNPCFactories();
	ItemFactory::ClearAllItemFactories();
	FeatureFactory::ClearAllFeatureFactories();
	delete s_generatorRegistrationMap;
}
void GeneratorRegistration::RemoveRegistration(const std::string& regName)
{
	std::map<std::string, GeneratorRegistration>::iterator it = s_generatorRegistrationMap->find(regName);
	if (it != s_generatorRegistrationMap->end())
	{
		s_generatorRegistrationMap->erase(it);
	}
}

Generator::Generator(const GeneratorType& type)
	: m_genType(type)
{
}
Generator::~Generator()
{

}
void Generator::LoadAllFeatures()
{
	FeatureFactory::LoadAllFeatures();
}
void Generator::LoadAllNPCS()
{
	NPCFactory::LoadAllNPCS();
}

const int Generator::GenerateNMonsters(const int& n, SpriteSheet* spriteSheet)
{
	return NPCFactory::GenerateNMonsters(n, spriteSheet);
}
const int Generator::GenerateNItems(const int& n, SpriteSheet* spriteSheet)
{
	return ItemFactory::GenerateNItems(n, spriteSheet);
}