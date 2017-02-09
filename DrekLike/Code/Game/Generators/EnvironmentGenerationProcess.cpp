#include "Game/Generators/EnvironmentGenerationProcess.hpp"
#include "Game/Generators/Generator.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include <algorithm>



//~------------------------------------------------
//GeneratationProcessData

std::map<std::string, EnvironmentGenerationProcess*> EnvironmentGenerationProcess::s_loadedEnvironmentBlueprints;
const std::string EnvironmentGenerationProcess::s_RegularEnvironmentFolderLoc = "Data/Environments/";
const std::string EnvironmentGenerationProcess::s_RegularNPCFactoryLoc = "Data/NPCs/";
const std::string EnvironmentGenerationProcess::s_RegularFactionFactoryLoc = "Data/Factions/";
const std::string EnvironmentGenerationProcess::s_RegularItemFactorLoc = "Data/Items/";
const std::string EnvironmentGenerationProcess::s_RegularSaveFileLoc = "Data/Saves/";
const std::string EnvironmentGenerationProcess::s_RegularFeaturesFileLoc = "Data/Features/";
std::vector<std::string> EnvironmentGenerationProcess::s_GeneratorLocs;
std::vector<std::string> EnvironmentGenerationProcess::s_NPCFactoryLocs;
std::vector<std::string> EnvironmentGenerationProcess::s_FactionLocs;
std::vector<std::string> EnvironmentGenerationProcess::s_ItemLocs;
std::vector<std::string> EnvironmentGenerationProcess::s_SaveFileLocs;
std::vector<std::string> EnvironmentGenerationProcess::s_featuresLocs;

//-------------------------------------------------
//Constructors

GenerationProcessData::GenerationProcessData()
{

}
GenerationProcessData::GenerationProcessData(StrXMLAttr name)
	:m_name(name)
{

}
GenerationProcessData::~GenerationProcessData()
{
}

//!-----
//Attributes to handle

const std::vector<StrXMLAttr> GenerationProcessData::GetAttributes() const
{
	return m_attributes;
}
void GenerationProcessData::SetName(StrXMLAttr genName)
{
	m_name = genName;
}
void GenerationProcessData::AddAttribute(const StrXMLAttr& attr)
{
	m_attributes.push_back(attr);
}
void GenerationProcessData::RemoveAttributeByIndex(const int& index)
{
	if (index >= (int)m_attributes.size() || index < 0)
	{
		return;
	}
	m_attributes.erase(m_attributes.begin() + index);
}
void GenerationProcessData::RemoveAttributeByName(const std::string& name)
{
	if (m_attributes.size() == 0)
	{
		return;
	}

	int curIndex = -1;
	for (size_t index = 0; index < m_attributes.size(); index++)
	{
		if (m_attributes.at(index).name == name)
		{
			curIndex = index;
			break;
		}
	}

	if (curIndex != -1)
	{
		m_attributes.erase(m_attributes.begin() + curIndex);
	}
}

//Getters
const std::string GenerationProcessData::GetAttributeByIndexString(const int& index) const
{
	if (index >= (int)m_attributes.size() || index < 0)
	{
		return "";
	}
	return m_attributes.at(index).val;
}
const std::string GenerationProcessData::GetAttributeByNameString(const std::string& name) const
{
	if (m_attributes.size() == 0)
	{
		return "";
	}
	for (size_t i = 0; i < m_attributes.size(); i++)
	{
		StrXMLAttr xmlPair = m_attributes.at(i);
		if (SimpleStrCmp(xmlPair.name,name))
		{
			return xmlPair.val;
		}
	}
	return "";
}
const StrXMLAttr GenerationProcessData::GetAttributeByName(const std::string& name) const
{
	if (m_attributes.size() == 0)
	{
		return StrXMLAttr();
	}
	for (size_t i = 0; i < m_attributes.size(); i++)
	{
		StrXMLAttr xmlPair = m_attributes.at(i);
		if (xmlPair.name == name)
		{
			return xmlPair;
		}
	}
	return StrXMLAttr();
}

void GenerationProcessData::AddChild(GenerationProcessData* child)
{
	m_children.push_back(child);
}
GenerationProcessData* GenerationProcessData::GetChildByIndex(const int& index)
{
	if (index < 0 || index >= (int)m_children.size())
	{
		return nullptr;
	}
	return m_children.at(index);
}
GenerationProcessData* GenerationProcessData::GetChildByName(const std::string& name)
{
	if (name == "")
	{
		return nullptr;
	}

	for (size_t i = 0; i < m_children.size(); i++)
	{
		if (m_children.at(i) != nullptr && SimpleStrCmp(m_children.at(i)->GetName().name, name))
		{
			return m_children.at(i);
		}
	}
	return nullptr;
}




//~------------------------------------------------
//EnvironmentGenerationProcess Functions

//-------------------------------------------------
//Constructors
EnvironmentGenerationProcess::EnvironmentGenerationProcess()
	: m_running(false),
	m_name("")
{
}
EnvironmentGenerationProcess::EnvironmentGenerationProcess(const XMLNode& blueprintNode)
	: m_running(true),
	m_name("")
{
	PopulateFromXMLNode(blueprintNode);
	s_loadedEnvironmentBlueprints.insert(std::pair<std::string, EnvironmentGenerationProcess*>(m_name, this));
}
EnvironmentGenerationProcess::EnvironmentGenerationProcess(const EnvironmentGenerationProcess& other)
	: m_running(other.m_running),
	m_name(other.m_name),
	m_children(other.m_children)
{
}
EnvironmentGenerationProcess::~EnvironmentGenerationProcess()
{
	for (size_t i = 0; i < m_children.size(); i++)
	{
		if (m_children.at(i) != nullptr)
		{
			delete m_children.at(i);
			m_children[i] = nullptr;
		}
	}
	m_children.clear();
}

void EnvironmentGenerationProcess::LoadAllBlueprintFiles()
{
	LoadEnvironmentBlueprints();
	LoadNPCFactoryBlueprints();
	LoadFactionBlueprints();
	LoadItemBlueprints();
	LoadFeatureBlueprints();
	LoadSaveFiles();
}
void EnvironmentGenerationProcess::LoadEnvironmentBlueprints()
{
	s_GeneratorLocs = FileUtils::EnumerateFilesInDirectory(s_RegularEnvironmentFolderLoc, "*");
	for (size_t i = 0; i < s_GeneratorLocs.size(); i++)
	{
		std::string file = s_GeneratorLocs.at(i);
		std::string fileExtension = file.substr(file.size() - 3, file.size());
		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
		if (SimpleStrCmp(fileExtension, "xml") == false)
		{
			s_GeneratorLocs.erase(s_GeneratorLocs.begin() + i);
			i--;
		}
	}
}
void EnvironmentGenerationProcess::LoadNPCFactoryBlueprints()
{
	s_NPCFactoryLocs = FileUtils::EnumerateFilesInDirectory(s_RegularNPCFactoryLoc, "*");
	for (size_t i = 0; i < s_NPCFactoryLocs.size(); i++)
	{
		std::string file = s_NPCFactoryLocs.at(i);
		std::string fileExtension = file.substr(file.size() - 3, file.size());
		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
		if (SimpleStrCmp(fileExtension, "xml") == false)
		{
			s_NPCFactoryLocs.erase(s_NPCFactoryLocs.begin() + i);
			i--;
		}
	}
}
void EnvironmentGenerationProcess::LoadItemBlueprints()
{
	s_ItemLocs = FileUtils::EnumerateFilesInDirectory(s_RegularItemFactorLoc, "*");
	for (size_t i = 0; i < s_ItemLocs.size(); i++)
	{
		std::string file = s_ItemLocs.at(i);
		std::string fileExtension = file.substr(file.size() - 3, file.size());
		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
		if (SimpleStrCmp(fileExtension, "xml") == false)
		{
			s_ItemLocs.erase(s_ItemLocs.begin() + i);
			i--;
		}
	}
}
void EnvironmentGenerationProcess::LoadFactionBlueprints()
{
	s_FactionLocs = FileUtils::EnumerateFilesInDirectory(s_RegularFactionFactoryLoc, "*");
	for (size_t i = 0; i < s_FactionLocs.size(); i++)
	{
		std::string file = s_FactionLocs.at(i);
		std::string fileExtension = file.substr(file.size() - 3, file.size());
		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
		if (SimpleStrCmp(fileExtension, "xml") == false)
		{
			s_FactionLocs.erase(s_FactionLocs.begin() + i);
			i--;
		}
	}
}
void EnvironmentGenerationProcess::LoadFeatureBlueprints()
{
	s_featuresLocs = FileUtils::EnumerateFilesInDirectory(s_RegularFeaturesFileLoc, "*");
	for (size_t i = 0; i < s_featuresLocs.size(); i++)
	{
		std::string file = s_featuresLocs.at(i);
		std::string fileExtension = file.substr(file.size() - 3, file.size());
		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
		if (SimpleStrCmp(fileExtension, "xml") == false)
		{
			s_featuresLocs.erase(s_featuresLocs.begin() + i);
			i--;
		}
	}
}
void EnvironmentGenerationProcess::LoadSaveFiles()
{
	s_SaveFileLocs = FileUtils::EnumerateFilesInDirectory(s_RegularSaveFileLoc, "*");
	for (size_t i = 0; i < s_SaveFileLocs.size(); i++)
	{
		std::string file = s_SaveFileLocs.at(i);
		std::string fileExtension = file.substr(file.size() - 3, file.size());
		std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
		if (SimpleStrCmp(fileExtension, "xml") == false)
		{
			s_SaveFileLocs.erase(s_SaveFileLocs.begin() + i);
			i--;
		}
	}
}
const std::vector<std::string> EnvironmentGenerationProcess::GetGeneratorLocations()
{
	return s_GeneratorLocs;
}
const std::vector<std::string> EnvironmentGenerationProcess::GetNPCFactoryLocations()
{
	return s_NPCFactoryLocs;
}
const std::vector<std::string> EnvironmentGenerationProcess::GetFactionLocations()
{
	return s_FactionLocs;
}
const std::vector<std::string> EnvironmentGenerationProcess::GetItemFactoryLocations()
{
	return s_ItemLocs;
}
const std::vector<std::string> EnvironmentGenerationProcess::GetFeaturesLocations()
{
	return s_featuresLocs;
}
const std::vector<std::string> EnvironmentGenerationProcess::GetSaveFileLocations()
{
	return s_SaveFileLocs;
}

void EnvironmentGenerationProcess::SetName(const std::string& name)
{
	m_name = name;
}
const std::string EnvironmentGenerationProcess::GetAttrValueByName(const std::string& entered_name) const
{
	for (size_t i = 0; i < attrs.size(); i++)
	{
		StrXMLAttr attr = attrs.at(i);

		if (attr.name == entered_name)
		{
			return attr.val;
		}
	}
	return "";
}


const std::vector<StrXMLAttr> EnvironmentGenerationProcess::GetAttributes() const
{
	return attrs;
}
void EnvironmentGenerationProcess::AddGenerationProcessData(GenerationProcessData* data)
{
	m_children.push_back(data);
}
void EnvironmentGenerationProcess::AddAttributeData(StrXMLAttr attr)
{
	attrs.push_back(attr);
}
void EnvironmentGenerationProcess::PopulateFromXMLNode(const XMLNode& blueprintNode)
{
	size_t numAttr = blueprintNode.nAttribute();
	m_running = true;
	for (int i = 0; i < (int)numAttr; i++)
	{
		StrXMLAttr attr = StrXMLAttr(blueprintNode.getAttribute(i));
		std::string atName = attr.name;
		std::string atVal = attr.val;
		if (atName == "name")
		{
			if (atVal == "")
			{
				m_name = "default";
			}
			else
			{
				m_name = atVal;
			}
			int val = 0;
			for (std::map<std::string, EnvironmentGenerationProcess*>::iterator it = s_loadedEnvironmentBlueprints.begin(); it != s_loadedEnvironmentBlueprints.end(); it++)
			{
				if (it->first == m_name.substr(0, m_name.length()))
				{
					val++;
				}
			}
			std::string intval;
			if (val == 0)
			{
				intval = "0";
			}
			else
			{
				intval = ToString(val);
			}
			m_name = m_name + intval;
		}
		attrs.push_back(attr);
	}
	GenerationProcessData* root = new GenerationProcessData();
	root->SetOriginalNode(blueprintNode);
	for (int i = 0; i < blueprintNode.nAttribute(); i++)
	{
		StrXMLAttr atr = StrXMLAttr(blueprintNode.getAttribute(i));
		root->AddAttribute(atr);
		if (atr.name == "name")
		{
			root->SetName(atr);
		}
	}


	PopulateFromXMLNode(blueprintNode, root);
	m_running = false;
}
void EnvironmentGenerationProcess::PopulateFromXMLNode(const XMLNode& node, GenerationProcessData* data)
{
	data->SetOriginalNode(node);
	for (int i = 0; i < node.nAttribute(); i++)
	{
		StrXMLAttr atr = StrXMLAttr(node.getAttribute(i));
		data->AddAttribute(atr);
	}

	for (int i = 0; i < node.nChildNode(); i++)
	{
		XMLNode child = node.getChildNode(i);
		std::string noName = child.getName();
		GenerationProcessData* newChild = new GenerationProcessData();
		newChild->SetName(noName);
		for (int j = 0; j < child.nAttribute(); j++)
		{
			XMLAttribute atr = child.getAttribute(j);
			StrXMLAttr attr = StrXMLAttr(atr.lpszName, atr.lpszValue);
			newChild->AddAttribute(attr);
		}
		data->AddChild(newChild);
		PopulateFromXMLNode(child, newChild);
	}
	m_children.push_back(data);
}


const std::vector<GenerationProcessData*> EnvironmentGenerationProcess::GetChildren() const
{
	return m_children;
}
EnvironmentGenerationProcess* EnvironmentGenerationProcess::Copy()
{
	return new EnvironmentGenerationProcess(*this);
}