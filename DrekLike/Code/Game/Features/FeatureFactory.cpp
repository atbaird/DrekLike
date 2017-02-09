#include "Game/Features/FeatureFactory.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <algorithm>

//------------------------------------------------------------------------------------------------------
//Constructors
std::map<std::string, FeatureFactory*> FeatureFactory::s_FeatureFactoryMaps[NUM_OF_FEATURE_TYPES];
std::vector<std::string> FeatureFactory::s_FeatureNames[NUM_OF_FEATURE_TYPES];

FeatureFactory::FeatureFactory()
	: Generator(GENERATOR_TYPE_FEATURE),
	m_type(FEATURETYPE_INVALID)
{

}
FeatureFactory::FeatureFactory(EnvironmentGenerationProcess* data)
	: Generator(GENERATOR_TYPE_FEATURE),
	m_type(FEATURETYPE_INVALID)
{
	std::vector<StrXMLAttr> attrs = data->GetAttributes();
	for (size_t attrIdx = 0; attrIdx < attrs.size(); attrIdx++)
	{
		StrXMLAttr attr = attrs.at(attrIdx);
		if (strcmp(attr.name.c_str(), "name") == 0)
		{
			m_name = attr.val;
			m_template.SetName(attr.val);
		}
		else if (strcmp(attr.name.c_str(), "featureType") == 0)
		{
			std::string val = attr.val;
			if (strcmp(val.c_str(), "door") == 0)
			{
				m_template.SetFeatureType(FEATURETYPE_DOOR);
				m_type = FEATURETYPE_DOOR;
			}
			else if (strcmp(val.c_str(), "siege") == 0)
			{
				m_template.SetFeatureType(FEATURETYPE_SIEGE);
				m_type = FEATURETYPE_SIEGE;
			}
		}
		else if (SimpleStrCmp(attr.name, "charWhenInactive"))
		{
			char car = attr.val.at(0);
			m_template.SetCharWhenInactive(car);
		}
		else if (SimpleStrCmp(attr.name, "charWhenActive"))
		{
			char car = attr.val.at(0);
			m_template.SetCharWhenActive(car);
		}
		else if (SimpleStrCmp(attr.name, "messageToDisplayWhenSetActive"))
		{
			m_template.SetMessageToDisplayWhenActive(attr.val);
		}
		else if (SimpleStrCmp(attr.name, "messageToDisplayWhenSetInactive"))
		{
			m_template.SetMessageToDisplayWhenInactive(attr.val);
		}
		else if (SimpleStrCmp(attr.name, "colorWhenInactive"))
		{

			float red = 1.f;
			float green = 1.f;
			float blue = 1.f;
			std::string val = ReplaceCharInString(attr.val, ',', ' ');
			std::vector<std::string> floats = ParseString(val);
			if (val.size() >= 1)
			{
				red = (float)atof(floats.at(0).c_str());
			}
			if (val.size() >= 2)
			{
				green = (float)atof(floats.at(1).c_str());
			}
			if (val.size() >= 3)
			{
				blue = (float)atof(floats.at(2).c_str());
			}
			m_template.SetColorWhenInactive(Rgba(red, green, blue, 1.f));
		}
		else if (SimpleStrCmp(attr.name, "colorWhenActive"))
		{
			float red = 1.f;
			float green = 1.f;
			float blue = 1.f;
			std::string val = ReplaceCharInString(attr.val, ',', ' ');
			std::vector<std::string> floats = ParseString(val);
			if (val.size() >= 1)
			{
				red = (float)atof(floats.at(0).c_str());
			}
			if (val.size() >= 2)
			{
				green = (float)atof(floats.at(1).c_str());
			}
			if (val.size() >= 3)
			{
				blue = (float)atof(floats.at(2).c_str());
			}
			m_template.SetColorWhenActive(Rgba(red, green, blue, 1.f));
		}

		std::transform(attr.val.begin(), attr.val.end(), attr.val.begin(), ::tolower);
		if (SimpleStrCmp(attr.name, "featureType"))
		{
			if (SimpleStrCmp(attr.val, "door"))
			{
				m_template.SetFeatureType(FEATURETYPE_DOOR);
			}
			else if (SimpleStrCmp(attr.val, "siege"))
			{
				m_template.SetFeatureType(FEATURETYPE_SIEGE);
			}
		}
		else if (SimpleStrCmp(attr.name, "blocksLineOfSightWhenInactive"))
		{
			if (SimpleStrCmp(attr.val, "t")
				|| SimpleStrCmp(attr.val, "true")
				|| SimpleStrCmp(attr.val, "1"))
			{
				m_template.SetBlocksLineOfSightWhenInactive(true);
			}
			else if (SimpleStrCmp(attr.val, "f")
				|| SimpleStrCmp(attr.val, "false")
				|| SimpleStrCmp(attr.val, "0"))
			{
				m_template.SetBlocksLineOfSightWhenInactive(false);
			}
		}
		else if (SimpleStrCmp(attr.name, "blocksLineOfSightWhenActive"))
		{
			if (SimpleStrCmp(attr.val, "t")
				|| SimpleStrCmp(attr.val, "true")
				|| SimpleStrCmp(attr.val, "1"))
			{
				m_template.SetBlocksLineOfSightWhenActive(true);
			}
			else if (SimpleStrCmp(attr.val, "f")
				|| SimpleStrCmp(attr.val, "false")
				|| SimpleStrCmp(attr.val, "0"))
			{
				m_template.SetBlocksLineOfSightWhenActive(false);
			}
		}
		else if (SimpleStrCmp(attr.name, "solidWhenInactive"))
		{
			if (SimpleStrCmp(attr.val, "t")
				|| SimpleStrCmp(attr.val, "true")
				|| SimpleStrCmp(attr.val, "1"))
			{
				m_template.SetBlocksLineOfSightWhenInactive(true);
			}
			else if (SimpleStrCmp(attr.val, "f")
				|| SimpleStrCmp(attr.val, "false")
				|| SimpleStrCmp(attr.val, "0"))
			{
				m_template.SetBlocksLineOfSightWhenInactive(false);
			}
		}
		else if (SimpleStrCmp(attr.name, "solidWhenActive"))
		{
			if (SimpleStrCmp(attr.val, "t")
				|| SimpleStrCmp(attr.val, "true")
				|| SimpleStrCmp(attr.val, "1"))
			{
				m_template.SetBlocksLineOfSightWhenActive(true);
			}
			else if (SimpleStrCmp(attr.val, "f")
				|| SimpleStrCmp(attr.val, "false")
				|| SimpleStrCmp(attr.val, "0"))
			{
				m_template.SetBlocksLineOfSightWhenActive(false);
			}
		}
		else if (SimpleStrCmp(attr.name, "active"))
		{
			if (SimpleStrCmp(attr.val, "t")
				|| SimpleStrCmp(attr.val, "true")
				|| SimpleStrCmp(attr.val, "1"))
			{
				m_template.SetActive(true);
			}
			else if (SimpleStrCmp(attr.val, "f")
				|| SimpleStrCmp(attr.val, "false")
				|| SimpleStrCmp(attr.val, "0"))
			{
				m_template.SetActive(false);
			}
		}
		else if (SimpleStrCmp(attr.name, "turnsUntilActive"))
		{
			int turnsTilActive = atoi(attr.val.c_str());
			m_template.SetNumTurnsUntilActive(turnsTilActive);
		}
		else if (SimpleStrCmp(attr.name, "damage"))
		{
			std::string val = ReplaceCharInString(attr.val, '~', ' ');
			val = ReplaceCharInString(val, ',', ' ');
			std::vector<std::string> vals = ParseString(val);
			if (vals.size() > 1)
			{
				int min = atoi(vals.at(0).c_str());
				int max = atoi(vals.at(1).c_str());
				m_template.SetMinDamage(min);
				m_template.SetMaxDamage(max);
			}
			else if(vals.size() == 1)
			{
				int dam = atoi(vals.at(0).c_str());
				m_template.SetMinDamage(dam);
				m_template.SetMaxDamage(dam);
			}
		}
		else if (SimpleStrCmp(attr.name, "turnsUntilActive"))
		{
			m_template.SetNumTurnsUntilActive(atoi(attr.val.c_str()));
		}
	}
}
FeatureFactory::~FeatureFactory()
{

}

//------------------------------------------------------------------------------------------------------
//Setters
void FeatureFactory::SetTemplate(const FeatureTemplate& templ)
{
	m_template = templ;
}
void FeatureFactory::SetName(const std::string& name)
{
	m_name = name;
}

//------------------------------------------------------------------------------------------------------
//Getters
const FeatureTemplate FeatureFactory::GetTemplate() const
{
	return m_template;
}
const std::string FeatureFactory::GetName() const
{
	return m_name;
}
const FeatureType FeatureFactory::GetType() const
{
	return m_type;
}

//------------------------------------------------------------------------------------------------------
//Static Methods
Feature* FeatureFactory::GenerateFeatureByType(const FeatureType& type)
{
	if (s_FeatureNames[type].size() <= 0)
	{
		return nullptr;
	}
	Feature* newFeature = new Feature();
	int ran = rand() % s_FeatureNames[type].size();
	std::string name = s_FeatureNames[type].at(ran);
	std::map<std::string, FeatureFactory*>::const_iterator it = s_FeatureFactoryMaps[type].find(name);
	if (it != s_FeatureFactoryMaps[type].end() && it->second != nullptr)
	{
		it->second->m_template.Copy(newFeature);
		Feature::s_FeaturesOnMap.push_back(newFeature);
		return newFeature;
	}
	else
	{
		delete newFeature;
		return nullptr;
	}
}
const int FeatureFactory::GenerateNFeatures(const int& n, SpriteSheet* m_spriteSheet)
{
	n;
	m_spriteSheet;
	return 0;
}
EnvironmentGenerationProcess* FeatureFactory::FeatureGenerationProcCreateFunc(const XMLNode& node)
{
	EnvironmentGenerationProcess* data = new EnvironmentGenerationProcess();
	data->PopulateFromXMLNode(node);
	return data;
}
Generator* FeatureFactory::FeatureGeneratorCreationFunc(const std::string& name)
{
	FeatureFactory* featureFact = new FeatureFactory();
	featureFact->SetName(name);
	return featureFact;
}
void FeatureFactory::LoadAllFeatures()
{
	std::vector<std::string> xmlFiles = EnvironmentGenerationProcess::GetFeaturesLocations();
	for (size_t fileLocIdx = 0; fileLocIdx < xmlFiles.size(); fileLocIdx++)
	{
		std::string fileLoc = xmlFiles.at(fileLocIdx);

		bool couldNotLoad = false;
		XMLNode root = EngineXMLParser::ParseXMLFile(fileLoc, "FeatureBlueprints", couldNotLoad);
		if (couldNotLoad == true)
		{
			continue;
		}
		for (int i = 0; i < root.nChildNode(); i++)
		{
			XMLNode rootChild = root.getChildNode(i);
			std::string childName = rootChild.getName();
			if (childName == "FeatureBlueprint")
			{
				EnvironmentGenerationProcess* data = FeatureFactory::FeatureGenerationProcCreateFunc(rootChild);

				FeatureFactory* newFactory = new FeatureFactory(data);
				std::string itemName = newFactory->GetName();
				FeatureType type = newFactory->GetType();

				s_FeatureFactoryMaps[type].insert(std::pair<std::string, FeatureFactory*>(itemName, newFactory));

				s_FeatureNames[type].push_back(itemName);
				//GeneratorRegistration(itemName, ItemFactory::ItemGeneratorCreationFunc, ItemFactory::ItemGenerationProcCreateFunc);

				delete data;
			}
		}
	}
}
void FeatureFactory::ClearAllFeatureFactories()
{
	for (size_t i = 0; i < NUM_OF_FEATURE_TYPES; i++)
	{
		for (std::map<std::string, FeatureFactory*>::iterator it = s_FeatureFactoryMaps[i].begin(); it != s_FeatureFactoryMaps[i].end(); it++)
		{
			if (it->second != nullptr)
			{
				delete it->second;
				it->second = nullptr;
			}
		}
		s_FeatureFactoryMaps[i].clear();
	}
}