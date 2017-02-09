#include "Game/GameEntity/AgentType/Factions/Faction.hpp"
#include "Game/Generators/EnvironmentGenerationProcess.hpp"
#include "Engine/Core/StringUtils.hpp"

//FactionRelationship
FactionRelationship::FactionRelationship(const int& factionID, const int& factionRel)
	: m_factionID(factionID),
	m_factionRelationship(factionRel)
{

}






//Faction

std::vector<Faction*> Faction::m_factions;
int Faction::s_currentlyAvailableFactionID = 0;

//Constructors
Faction::Faction(const std::string& name)
	: m_factionName(name),
	m_factionID(s_currentlyAvailableFactionID)
{
	s_currentlyAvailableFactionID++;
}
Faction::Faction(const Faction& other)
	: m_factionName(other.m_factionName),
	m_factionID(other.m_factionID),
	m_factionRelationships(other.m_factionRelationships)
{

}
Faction::~Faction()
{
}


//static functions
Faction* Faction::CreateFactionOfNameIfDoesNotAlreadyExist(const std::string& name)
{
	if (DoesFactionAlreadyExist(name) == true)
	{
		return nullptr;
	}
	Faction* fac = new Faction(name);
	m_factions.push_back(fac);
	return m_factions.at(m_factions.size() - 1);
}
Faction* Faction::GetFactionOfIDIfExists(const int& id)
{
	for (size_t i = 0; i < m_factions.size(); i++)
	{
		if (m_factions.at(i)->GetFactionID() == id)
		{
			return m_factions.at(i);
		}
	}
	return nullptr;
}
Faction* Faction::CreateOrGetFactionOfName(const std::string& name)
{
	for (size_t i = 0; i < m_factions.size(); i++)
	{
		if (strcmp(m_factions.at(i)->GetFactionName().c_str(), name.c_str()) == 0)
		{
			return m_factions.at(i);
		}
	}

	Faction* fac = new Faction(name);
	m_factions.push_back(fac);
	return m_factions.at(m_factions.size() - 1);
}
Faction* Faction::CreateOrGetFactionOfID(const int& id)
{
	for (size_t i = 0; i < m_factions.size(); i++)
	{
		if (m_factions.at(i)->GetFactionID() == id)
		{
			return m_factions.at(i);
		}
	}

	Faction* fac = new Faction("");
	m_factions.push_back(fac);
	return m_factions.at(m_factions.size() - 1);
}
const bool Faction::DoesFactionAlreadyExist(const std::string& name)
{
	for (size_t i = 0; i < m_factions.size(); i++)
	{
		if (m_factions.at(i)->GetFactionName() == name)
		{
			return true;
		}
	}
	return false;
}
void Faction::ClearAllFactions()
{
	for (size_t i = 0; i < m_factions.size(); i++)
	{
		if (m_factions.at(i) != nullptr)
		{
			delete m_factions[i];
			m_factions[i] = nullptr;
		}
	}
	m_factions.clear();
}
void Faction::LoadAllFactions()
{

	std::vector<std::string> FactionFactLocs = EnvironmentGenerationProcess::GetFactionLocations();


	for (size_t xmlFileIndex = 0; xmlFileIndex < FactionFactLocs.size(); xmlFileIndex++)
	{
		std::string xmlFileLocation = FactionFactLocs.at(xmlFileIndex);
		bool couldNotLoad = false;
		XMLNode root = EngineXMLParser::ParseXMLFile(xmlFileLocation, "Factions", couldNotLoad);

		for (int childIter = 0; childIter < root.nChildNode(); childIter++)
		{
			XMLNode rootChild = root.getChildNode(childIter);
			std::string childName = rootChild.getName();
			if (childName == "Faction")
			{
				EnvironmentGenerationProcess* data = new EnvironmentGenerationProcess();
				data->PopulateFromXMLNode(rootChild);

				std::string name = data->GetAttrValueByName("name");
				Faction* baseFaction = nullptr;



				baseFaction = Faction::CreateOrGetFactionOfName(name);

				std::vector<StrXMLAttr> attrs = data->GetAttributes();
				std::vector<Faction*> hates;
				std::vector<Faction*> dislikes;
				std::vector<Faction*> neutral;
				std::vector<Faction*> likes;
				std::vector<Faction*> loves;

				for (size_t AttrIter = 0; AttrIter < attrs.size(); AttrIter++)
				{
					StrXMLAttr attr = attrs.at(AttrIter);
					if (attr.name == "loves")
					{
						std::string val = attr.val;
						val = ReplaceCharInString(val, ',', ' ');
						val = ReplaceCharInString(val, '~', ' ');
						std::vector<std::string> vals = ParseString(val);
						for (size_t j = 0; j < vals.size(); j++)
						{
							Faction* fac = Faction::CreateOrGetFactionOfName(vals.at(j));
							loves.push_back(fac);
						}
					}
					else if (attr.name == "likes")
					{
						std::string val = attr.val;
						val = ReplaceCharInString(val, ',', ' ');
						val = ReplaceCharInString(val, '~', ' ');
						std::vector<std::string> vals = ParseString(val);
						for (size_t j = 0; j < vals.size(); j++)
						{
							Faction* fac = Faction::CreateOrGetFactionOfName(vals.at(j));
							likes.push_back(fac);
						}
					}
					else if (attr.name == "neutral")
					{
						std::string val = attr.val;
						val = ReplaceCharInString(val, ',', ' ');
						val = ReplaceCharInString(val, '~', ' ');
						std::vector<std::string> vals = ParseString(val);
						for (size_t j = 0; j < vals.size(); j++)
						{
							Faction* fac = Faction::CreateOrGetFactionOfName(vals.at(j));
							neutral.push_back(fac);
						}
					}
					else if (attr.name == "dislikes")
					{
						std::string val = attr.val;
						val = ReplaceCharInString(val, ',', ' ');
						val = ReplaceCharInString(val, '~', ' ');
						std::vector<std::string> vals = ParseString(val);
						for (size_t j = 0; j < vals.size(); j++)
						{
							Faction* fac = Faction::CreateOrGetFactionOfName(vals.at(j));
							dislikes.push_back(fac);
						}
					}
					else if (attr.name == "hates")
					{
						std::string val = attr.val;
						val = ReplaceCharInString(val, ',', ' ');
						val = ReplaceCharInString(val, '~', ' ');
						std::vector<std::string> vals = ParseString(val);
						for (size_t j = 0; j < vals.size(); j++)
						{
							Faction* fac = Faction::CreateOrGetFactionOfName(vals.at(j));
							hates.push_back(fac);
						}
					}
				}
				if (baseFaction != nullptr)
				{
					for (size_t i = 0; i < loves.size(); i++)
					{
						if (loves.at(i) != nullptr)
						{
							FactionRelationship rel(loves.at(i)->GetFactionID(), 20);
							baseFaction->AddFactionRelationship(rel);
						}
					}
					for (size_t i = 0; i < likes.size(); i++)
					{
						if (likes.at(i) != nullptr)
						{
							FactionRelationship rel(likes.at(i)->GetFactionID(), 10);
							baseFaction->AddFactionRelationship(rel);
						}
					}
					for (size_t i = 0; i < neutral.size(); i++)
					{
						if (neutral.at(i) != nullptr)
						{
							FactionRelationship rel(neutral.at(i)->GetFactionID(), 0);
							baseFaction->AddFactionRelationship(rel);
						}
					}
					for (size_t i = 0; i < dislikes.size(); i++)
					{
						if (dislikes.at(i) != nullptr)
						{
							FactionRelationship rel(dislikes.at(i)->GetFactionID(), -10);
							baseFaction->AddFactionRelationship(rel);
						}

					}
					for (size_t i = 0; i < hates.size(); i++)
					{
						if (hates.at(i) != nullptr)
						{
							FactionRelationship rel(hates.at(i)->GetFactionID(), -20);
							baseFaction->AddFactionRelationship(rel);
						}
					}
				}

				delete data;
			}
		}
	}
}
void Faction::WriteAllToXMLNode(XMLNode& node)
{
	for (size_t i = 0; i < m_factions.size(); i++)
	{
		Faction* fac = m_factions.at(i);
		if (fac != nullptr)
		{
			XMLNode facNode = node.addChild("Faction");
			fac->WriteToXMLNode(facNode);
		}
	}
}
void Faction::ReadAllFromXMLNode(GenerationProcessData* data)
{
	//data is "Factions" node
	//thus it's children should be "Faction" nodes
	std::vector<GenerationProcessData*> children = data->GetChildren();
	for (size_t i = 0; i < children.size(); i++)
	{
		GenerationProcessData* child = children.at(i);
		if (child != nullptr)
		{
			std::string name = child->GetAttributeByNameString("name");
			std::string id = child->GetAttributeByNameString("factionID");
			Faction* fac = Faction::CreateOrGetFactionOfName(name);
			fac->SetFactionID(atoi(id.c_str()));
			fac->ReadFromXMLNode(child);

		}
	}
}



//Setters
void Faction::WriteToXMLNode(XMLNode& node)
{
	node.addAttribute("name", m_factionName.c_str());
	node.addAttribute("factionID", std::to_string(m_factionID).c_str());
	std::string loves = "";
	std::string likes = "";
	std::string neutral = "";
	std::string dislikes = "";
	std::string hates = "";
	for (size_t i = 0; i < m_factionRelationships.size(); i++)
	{
		int facID = m_factionRelationships.at(i).GetFactionID();
		Faction* fac = Faction::GetFactionOfIDIfExists(facID);
		if (fac != nullptr)
		{
			int relStatus = m_factionRelationships.at(i).GetFactionRelationship();
			if (relStatus <= -20)
			{
				//hates
				hates += "," + fac->GetFactionName();
			}
			else if (relStatus < -10)
			{
				//dislikes
				dislikes += "," + fac->GetFactionName();
			}
			else if (relStatus < 10)
			{
				//neutral
				neutral += "," + fac->GetFactionName();
			}
			else if (relStatus < 20)
			{
				//likes
				likes += "," + fac->GetFactionName();
			}
			else
			{
				//loves
				loves += "," + fac->GetFactionName();
			}
		}
	}
	node.addAttribute("hates", hates.c_str());
	node.addAttribute("dislikes", dislikes.c_str());
	node.addAttribute("neutral", neutral.c_str());
	node.addAttribute("likes", likes.c_str());
	node.addAttribute("loves", loves.c_str());

}
void Faction::ReadFromXMLNode(GenerationProcessData* data)
{
	std::vector<StrXMLAttr> attrs = data->GetAttributes();
	for (size_t attrIdx = 0; attrIdx < attrs.size(); attrIdx++)
	{
		StrXMLAttr attr = attrs.at(attrIdx);
		if (SimpleStrCmp(attr.name, "loves"))
		{
			std::string val = ReplaceCharInString(attr.val, ',', ' ');
			std::vector<std::string> vals = ParseString(val);
			for (size_t valIdx = 0; valIdx < vals.size(); valIdx++)
			{
				Faction* fac = Faction::CreateOrGetFactionOfName(vals.at(valIdx));
				FactionRelationship rel(fac->GetFactionID(), 20);
				m_factionRelationships.push_back(rel);
			}
		}
		else if (SimpleStrCmp(attr.name, "likes"))
		{
			std::string val = ReplaceCharInString(attr.val, ',', ' ');
			std::vector<std::string> vals = ParseString(val);
			for (size_t valIdx = 0; valIdx < vals.size(); valIdx++)
			{
				Faction* fac = Faction::CreateOrGetFactionOfName(vals.at(valIdx));
				FactionRelationship rel(fac->GetFactionID(), 10);
				m_factionRelationships.push_back(rel);
			}
		}
		else if (SimpleStrCmp(attr.name, "neutral"))
		{
			std::string val = ReplaceCharInString(attr.val, ',', ' ');
			std::vector<std::string> vals = ParseString(val);
			for (size_t valIdx = 0; valIdx < vals.size(); valIdx++)
			{
				Faction* fac = Faction::CreateOrGetFactionOfName(vals.at(valIdx));
				FactionRelationship rel(fac->GetFactionID(), 0);
				m_factionRelationships.push_back(rel);
			}
		}
		else if (SimpleStrCmp(attr.name, "dislikes"))
		{
			std::string val = ReplaceCharInString(attr.val, ',', ' ');
			std::vector<std::string> vals = ParseString(val);
			for (size_t valIdx = 0; valIdx < vals.size(); valIdx++)
			{
				Faction* fac = Faction::CreateOrGetFactionOfName(vals.at(valIdx));
				FactionRelationship rel(fac->GetFactionID(), -10);
				m_factionRelationships.push_back(rel);
			}
		}
		else if (SimpleStrCmp(attr.name, "hates"))
		{
			std::string val = ReplaceCharInString(attr.val, ',', ' ');
			std::vector<std::string> vals = ParseString(val);
			for (size_t valIdx = 0; valIdx < vals.size(); valIdx++)
			{
				Faction* fac = Faction::CreateOrGetFactionOfName(vals.at(valIdx));
				FactionRelationship rel(fac->GetFactionID(), -20);
				m_factionRelationships.push_back(rel);
			}
		}
	}
}
void Faction::SetFactionID(const int& id)
{
	m_factionID = id;
}
void Faction::SetFactionName(const std::string& name)
{
	m_factionName = name;
}
void Faction::AddFactionRelationship(const FactionRelationship& relationship)
{
	m_factionRelationships.push_back(relationship);
}
void Faction::AddRelationShipScore(const int& factionID, const int& toAddToScore)
{
	if (factionID <= -1)
	{
		return;
	}
	int index = -1;
	for (size_t i = 0; i < m_factionRelationships.size(); i++)
	{
		if (m_factionRelationships.at(i).GetFactionID() == factionID)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		FactionRelationship rel = FactionRelationship(factionID, toAddToScore);
		m_factionRelationships.push_back(rel);
	}
	else
	{
		m_factionRelationships.at(index).SetFactionRelationship(m_factionRelationships.at(index).GetFactionRelationship() + toAddToScore);
	}
}

//Getters
const std::string Faction::GetFactionName() const
{
	return m_factionName;
}
const int Faction::GetFactionRelationShipForID(const int& id)
{
	for (size_t i = 0; i < m_factionRelationships.size(); i++)
	{
		if (m_factionRelationships.at(i).GetFactionID() == id)
		{
			return m_factionRelationships.at(i).GetFactionRelationship();
		}
	}
	return 0;
}
const std::vector<FactionRelationship> Faction::GetFactionRelationships()
{
	return m_factionRelationships;
}
const int Faction::GetFactionID() const
{
	return m_factionID;
}
const int Faction::EvaluateHatredScale(Faction* other)
{
	if (other == nullptr)
	{
		return 0;
	}
	int factionID = other->GetFactionID();
	for (size_t i = 0; i < m_factionRelationships.size(); i++)
	{
		if (factionID == m_factionRelationships.at(i).GetFactionID())
		{
			return m_factionRelationships.at(i).GetFactionRelationship();
		}
	}
	FactionRelationship newRel = FactionRelationship(other->GetFactionID(), 0);
	m_factionRelationships.push_back(newRel);
	return 0;
}