#pragma once

#ifndef ENVIRONMENTBLUEPRINT_HPP
#define ENVIRONMENTBLUEPRINT_HPP
#include <map>
#include <vector>
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/EngineXMLParser.hpp"

class EnvironmentGenerationProcess;

typedef EnvironmentGenerationProcess* GenerationProcessCreationFunc(const XMLNode& node);


//GeneratationProcessData
class GenerationProcessData
{
private:
	StrXMLAttr m_name;
	XMLNode m_originalNode;
	std::vector<StrXMLAttr> m_attributes;
	std::vector<GenerationProcessData*> m_children;
public:
	GenerationProcessData();
	GenerationProcessData(StrXMLAttr name);
	~GenerationProcessData();
	StrXMLAttr GetName() const { return m_name; };
	const std::vector<StrXMLAttr> GetAttributes() const;
	void SetName(StrXMLAttr genName);
	void SetOriginalNode(const XMLNode& node) { m_originalNode = node; };
	void AddAttribute(const StrXMLAttr&);
	void RemoveAttributeByIndex(const int& index);
	void RemoveAttributeByName(const std::string& name);
	const std::string GetAttributeByIndexString(const int& index) const;
	const std::string GetAttributeByNameString(const std::string& name) const;
	const StrXMLAttr GetAttributeByName(const std::string& name) const;
	const XMLNode GetOriginalNode() const { return m_originalNode; };


	void AddChild(GenerationProcessData* child);
	GenerationProcessData* GetChildByIndex(const int& index);
	GenerationProcessData* GetChildByName(const std::string& name);
	const std::vector<GenerationProcessData*> GetChildren() const { return m_children; };
};



//EnvironmentGenerationprocess
class EnvironmentGenerationProcess
{
private:
public:
	EnvironmentGenerationProcess();
	EnvironmentGenerationProcess(const XMLNode& blueprintNode);
	EnvironmentGenerationProcess(const EnvironmentGenerationProcess& other);
	~EnvironmentGenerationProcess();

	static void LoadAllBlueprintFiles();
	static void LoadEnvironmentBlueprints();
	static void LoadNPCFactoryBlueprints();
	static void LoadItemBlueprints();
	static void LoadFactionBlueprints();
	static void LoadFeatureBlueprints();
	static void LoadSaveFiles();
	static const std::vector<std::string> GetGeneratorLocations();
	static const std::vector<std::string> GetNPCFactoryLocations();
	static const std::vector<std::string> GetFactionLocations();
	static const std::vector<std::string> GetItemFactoryLocations();
	static const std::vector<std::string> GetFeaturesLocations();
	static const std::vector<std::string> GetSaveFileLocations();
	const std::vector<StrXMLAttr> GetAttributes() const;
	void AddGenerationProcessData(GenerationProcessData* data);
	void AddAttributeData(StrXMLAttr attr);
	void SetName(const std::string& name);
	const std::string GetName() const { return m_name; };
	const std::string GetAttrValueByName(const std::string& entered_name) const;
	const std::vector<GenerationProcessData*> GetChildren() const;

	EnvironmentGenerationProcess* Copy();

	void PopulateFromXMLNode(const XMLNode& blueprintNode);
private:
	void PopulateFromXMLNode(const XMLNode& node, GenerationProcessData* data);
protected:
	static std::map<std::string, EnvironmentGenerationProcess*> s_loadedEnvironmentBlueprints;
	bool m_running;

	//parent attributes
	std::string m_name;
	std::vector<StrXMLAttr> attrs;

	std::vector<GenerationProcessData*> m_children;

	static const std::string s_RegularEnvironmentFolderLoc;
	static const std::string s_RegularNPCFactoryLoc;
	static const std::string s_RegularItemFactorLoc;
	static const std::string s_RegularFactionFactoryLoc;
	static const std::string s_RegularSaveFileLoc;
	static const std::string s_RegularFeaturesFileLoc;
	static std::vector<std::string> s_GeneratorLocs;
	static std::vector<std::string> s_NPCFactoryLocs;
	static std::vector<std::string> s_ItemLocs;
	static std::vector<std::string> s_FactionLocs;
	static std::vector<std::string> s_SaveFileLocs;
	static std::vector<std::string> s_featuresLocs;
};
#endif