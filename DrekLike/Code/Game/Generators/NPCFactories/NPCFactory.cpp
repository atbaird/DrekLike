#include "Game/Generators/NPCFactories/NPCFactory.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Items/ItemFactory.hpp"

std::map<std::string, NPCFactory*> NPCFactory::s_factories;
std::vector<std::string> NPCFactory::s_factoryNames;
NPCFactory::NPCFactory()
	: Generator(GENERATOR_TYPE_NPC),
	m_name("")
{
	m_preferredSpawnTileType.push_back(TILE_AIR);
}
NPCFactory::NPCFactory(EnvironmentGenerationProcess* data)
	: Generator(GENERATOR_TYPE_NPC)
{
	bool preferredTilesSet = false;
	bool setMoveProps = false;
	std::vector<GenerationProcessData*> children = data->GetChildren();
	std::vector<MovementProperty> props;
	for (size_t childIterator = 0; childIterator < children.size(); childIterator++)
	{
		GenerationProcessData* child = children.at(childIterator);

		std::string name = child->GetName().name;
		//NPCBlueprint
		//Get Default Values from NPCBlueprint; these are mostly the stats
		std::vector<StrXMLAttr> attrs = child->GetAttributes();
		for (size_t k = 0; k < attrs.size(); k++)
		{
			StrXMLAttr attr = attrs.at(k);
			if (SimpleStrCmp(attr.name, "name"))
			{
				m_name = attr.val;
			}
			else if (SimpleStrCmp(attr.name ,"defaultName"))
			{
				m_template.AddName(attr.val);
			}
			else if (SimpleStrCmp(attr.name, "preferredTile"))
			{
				std::string val = ReplaceCharInString(attr.val, ',', ' ');
				std::vector<std::string> vals = ParseString(val);
				for (size_t i = 0; i < vals.size(); i++)
				{
					if (SimpleStrCmp(vals.at(i), "air"))
					{
						m_preferredSpawnTileType.push_back(TILE_AIR);
						preferredTilesSet = true;
					}
					else if (SimpleStrCmp(vals.at(i), "stone"))
					{
						m_preferredSpawnTileType.push_back(TILE_STONE);
						preferredTilesSet = true;
					}
					else if (SimpleStrCmp(vals.at(i), "water"))
					{
						m_preferredSpawnTileType.push_back(TILE_WATER);
						preferredTilesSet = true;
					}
					else if (SimpleStrCmp(vals.at(i), "lava"))
					{
						m_preferredSpawnTileType.push_back(TILE_LAVA);
						preferredTilesSet = true;
					}
				}
			}
			else if (SimpleStrCmp(attr.name, "maxHealth"))
			{
				std::string val = ReplaceCharInString(attr.val, '~', ' ');
				val = ReplaceCharInString(val, ',', ' ');
				std::vector<std::string> vals = ParseString(val);
				if (vals.size() == 1)
				{
					m_template.SetMaxHealth(stoi(vals.at(0)));
				}
				else if(vals.size() > 1)
				{
					m_template.SetMaxHealth(stoi(vals.at(1)));
					m_template.SetMinMaxHealth(stoi(vals.at(0)));
				}
			}
			else if (SimpleStrCmp(attr.name, "accuracy"))
			{
				float acc = (float)atof(attr.val.c_str());
				m_template.SetAccuracy(acc);
			}
			else if (SimpleStrCmp(attr.name, "lineOfSight"))
			{
				float lineOfSight = (float)atof(attr.val.c_str());
				m_template.SetLineOfSight(lineOfSight);
			}
			else if (SimpleStrCmp(attr.name, "faction"))
			{
				Faction* fac = Faction::CreateOrGetFactionOfName(attr.val);
				m_template.SetFaction(fac);
			}
			else if (SimpleStrCmp(attr.name, "damage"))
			{
				std::string val = ReplaceCharInString(attr.val, '~', ' ');
				val = ReplaceCharInString(val, ',', ' ');
				std::vector<std::string> vals = ParseString(val);
				if (vals.size() == 1)
				{
					int damage = stoi(vals.at(0));
					m_template.SetMaxDamage(damage);
					m_template.SetMinDamage(damage);
				}
				else if (vals.size() > 1)
				{
					int min = stoi(vals.at(0));
					int max = stoi(vals.at(1));
					m_template.SetMinDamage(min);
					m_template.SetMaxDamage(max);
				}
			}
			else if (SimpleStrCmp(attr.name, "moveSpeed"))
			{
				float val = stof(attr.val);
				m_template.SetStepCost(val);
			}
			else if (SimpleStrCmp(attr.name, "race"))
			{
				m_template.SetRace(attr.val);
			}
			else if (SimpleStrCmp(attr.name, "glyph"))
			{
				if (strlen(attr.val.c_str()) > 0)
				{
					m_template.SetChar(attr.val.c_str()[0]);
				}
			}
			else if (SimpleStrCmp(attr.name, "color"))
			{
				//Get Color values from the node on a scale of 255

				std::string strCol = ReplaceCharInString(attr.val, ',', ' ');
				std::vector<std::string> vals = ParseString(strCol);

				Vector3 rgbMins;
				Vector3 rgbMaxs;
				for (size_t colorIterator = 0; colorIterator < vals.size(); colorIterator++)
				{
					std::string valCol = ReplaceCharInString(vals.at(colorIterator), '~', ' ');
					std::vector<std::string> vals2 = ParseString(valCol);

					switch (colorIterator)
					{
					case 0:
						GetColorOutOfData(valCol, rgbMins.x, rgbMaxs.x);
						break;
					case 1:
						GetColorOutOfData(valCol, rgbMins.y, rgbMaxs.y);
						break;
					case 2:
						GetColorOutOfData(valCol, rgbMins.z, rgbMaxs.z);
						break;
					default:
						break;
					}
				}
				m_template.SetMinRGB(rgbMins);
				m_template.SetMaxRGB(rgbMaxs);
			}
		}
		//get Values from Titles
		if (SimpleStrCmp(name , "Names"))
		{
			std::vector<GenerationProcessData*> names = child->GetChildren();
			for (size_t nameIt = 0; nameIt < names.size(); nameIt++)
			{
				std::string nameIn = names.at(nameIt)->GetName().name;
				m_template.AddName(nameIn);
			}
		}

		else if (SimpleStrCmp(name, "MovementProperties"))
		{
			setMoveProps = true;
			/*
			BLOCKED_BY_WALLS,
			BLOCKED_BY_FIRE,
			BLOCKED_BY_WATER,
			SLOWED_BY_WALLS,
			SLOWED_BY_FIRE,
			SLOWED_BY_WATER,
			HURT_BY_FIRE,
			FLYING,
			*/
			std::vector<GenerationProcessData*> moveProps = child->GetChildren();
			for (size_t i = 0; i < moveProps.size(); i++)
			{
				GenerationProcessData* proc = moveProps.at(i);
				std::string procName = proc->GetName().val;
				if (SimpleStrCmp(procName, "blockedByWalls"))
				{
					props.push_back(BLOCKED_BY_WALLS);
				}
				else if (SimpleStrCmp(procName, "BlockedByAir"))
				{
					props.push_back(BLOCKED_BY_AIR);
				}
				else if (SimpleStrCmp(procName, "BlockedByFire"))
				{
					props.push_back(BLOCKED_BY_FIRE);
				}
				else if (SimpleStrCmp(procName, "BlockedByWater"))
				{
					props.push_back(BLOCKED_BY_WATER);
				}
				else if (SimpleStrCmp(procName, "SlowedByWalls"))
				{
					props.push_back(SLOWED_BY_WALLS);
				}
				else if (SimpleStrCmp(procName, "SlowedByFire"))
				{
					props.push_back(SLOWED_BY_FIRE);
				}
				else if (SimpleStrCmp(procName, "SlowedByWater"))
				{
					props.push_back(SLOWED_BY_WATER);
				}
				else if (SimpleStrCmp(procName, "HurtByFire"))
				{
					props.push_back(HURT_BY_FIRE);
				}
				else if (SimpleStrCmp(procName, "Flying"))
				{
					props.push_back(FLYING);
				}
			}

		}
		//Get Values from Titles
		else if (SimpleStrCmp(name,"Titles"))
		{
			std::vector<GenerationProcessData*> titles = child->GetChildren();
			for (size_t titleIt = 0; titleIt < titles.size(); titleIt++)
			{
				GenerationProcessData* curTitle = titles.at(titleIt);
				std::vector<StrXMLAttr> titleVals = curTitle->GetAttributes();
				AgentTitle title = AgentTitle();
				for (size_t i = 0; i < titleVals.size(); i++)
				{
					if (titleVals.at(i).name == "Title" || titleVals.at(i).name == "name")
					{
						std::string nameIn = titleVals.at(i).val;
						title.title = nameIn;
					}
				}
				m_template.AddPossibleTitle(title);
				//m_template.AddName(nameIn);
			}
		}
		else if (SimpleStrCmp(name, "Inventory") || SimpleStrCmp(name, "inventory"))
		{
			std::vector<GenerationProcessData*> items = child->GetChildren();
			for (size_t i = 0; i < items.size(); i++)
			{
				StrXMLAttr itemAttrs = items.at(i)->GetAttributeByName("name");
				if (ItemFactory::IsThereAnItemFactoryOfName(itemAttrs.val))
				{
					m_template.AddItem(itemAttrs.val);
				}
			}
		}
		else if (SimpleStrCmp(name, "Behaviors"))
		{
			std::vector<GenerationProcessData*> behaviors = child->GetChildren();
			for (size_t bevIt = 0; bevIt < behaviors.size(); bevIt++)
			{
				GenerationProcessData* curBev = behaviors.at(bevIt);
				std::string bevName = curBev->GetName().name;
				bool bevExits = BehaviorRegistrationHelper::DetermineBehaviorExists(bevName);
				if (bevExits == true)
				{
					m_template.AddBehavior(BehaviorRegistrationHelper::CreateBehaviorByName(bevName, curBev->GetOriginalNode()));
				}
			}
		}
	}
	if (setMoveProps == false)
	{
		props.push_back(BLOCKED_BY_WALLS);
		props.push_back(HURT_BY_FIRE);
	}
	int mas = Agent::GenMaskFromMovementProperties(props);
	m_template.SetMask(mas);
	if (preferredTilesSet == false)
	{
		m_preferredSpawnTileType.push_back(TILE_AIR);
	}

}
NPCFactory::~NPCFactory()
{
	m_template.ClearTemplateBehaviors();
}


EnvironmentGenerationProcess* NPCFactory::NPCGenerationProcCreateFunc(const XMLNode& node)
{
	EnvironmentGenerationProcess* data = new EnvironmentGenerationProcess();
	data->PopulateFromXMLNode(node);
	return data;
}
Generator* NPCFactory::NPCGeneratorCreationFunc(const std::string& name)
{
	std::map<std::string, NPCFactory*>::iterator it = s_factories.find(name);
	if (it == s_factories.end())
	{
		return nullptr;
	}

	NPCFactory* newFact = new NPCFactory();
	newFact->SetTemplate(it->second->GetTemplate());

	return newFact;
}

//Setters
void NPCFactory::SetTemplate(NPCTemplate temp)
{
	m_template = temp;
}
void NPCFactory::GetColorOutOfData(const std::string& val, float& val1, float& val2)
{
	int Min;
	int Max;
	std::vector<std::string> strings = ParseString(val);
	if (strings.size() == 1)
	{
		Min = stoi(strings.at(0));
		val1 = (((float)Min) / 255.f);
		val2 = val1;
	}
	else if (strings.size() >= 2)
	{
		Min = stoi(strings.at(0));
		Max = stoi(strings.at(1));
		val1 = (((float)Min) / 255.f);
		val2 = (((float)Max) / 255.f);
	}
}

//Getters
const std::vector<TileType> NPCFactory::GetPreferredSpawnTileType() const
{
	return m_preferredSpawnTileType;
}
const std::string NPCFactory::GetName() 
{ 
	return m_name;
}
const IntVector2 NPCFactory::GetMonsterSize() const
{ 
	return IntVector2(1, 1);
}

const NPCTemplate NPCFactory::GetTemplate() const
{
	return m_template; 
}

const int NPCFactory::GenerateNMonsters(const int& n, SpriteSheet* spriteSheet)
{
	if (g_map == nullptr || s_factories.size() <= 0)
	{
		return 0;
	}
	int num = n;
	int trueNumGenned = 0;

	std::map<Tile_Coord, Tile*>* mapTiles = g_map->GetMapTiles();
	for (int i = 0; i < num; i++)
	{
		int ranGen = rand() % s_factoryNames.size();
		std::string factName = s_factoryNames.at(ranGen);
		NPCFactory* fact = s_factories.at(factName);
		IntVector2 mapSize = g_map->GetMapSize();
		if (fact != nullptr)
		{
			IntVector2 size = fact->GetMonsterSize();
			std::vector<TileType> types = fact->GetPreferredSpawnTileType();
			std::vector<std::vector<Tile_Coord>> validCoordsCoords;
			for (size_t j = 0; j < types.size(); j++)
			{
				std::vector<Tile_Coord> validCoords = g_map->GetAllUnoccupiedTileCoordsOfType(types.at(j));
				if (validCoords.size() > 0)
				{
					validCoordsCoords.push_back(validCoords);
				}
			}
			if (validCoordsCoords.size() <= 0)
			{
				i--;
				continue;
			}
			int ran = rand() % validCoordsCoords.size();
			std::vector<Tile_Coord> validCoords = validCoordsCoords.at(ran);
			bool canExit = false;
			bool builtNPC = true;
			while (canExit == false)
			{
				if (validCoords.size() <= 0)
				{
					canExit = true;
					builtNPC = false;
					break;
				}
				int ranVal = rand() % validCoords.size();
				Tile_Coord coord = validCoords.at(ranVal);
				IntVector2 test = coord + size - IntVector2(1,1);
				if (coord.x >= mapSize.x || coord.y >= mapSize.y)
				{
					validCoords.erase(validCoords.begin() + ranVal);
				}
				else
				{
					for (int x = coord.x; x <= test.x; x++)
					{
						for (int y = coord.y; y <= test.y; y++)
						{
							IntVector2 testCoord(x, y);
							std::map<Tile_Coord, Tile*>::iterator it = mapTiles->find(testCoord);
							if (it != mapTiles->end() && it->second != nullptr)
							{
								TileType type = it->second->GetTileType();
								bool exists = false;
								bool solid = TileDefinition::GetTypeSolid(type, exists);
								if (exists == true && solid == true)
								{
									it->second->SetTileType(TILE_AIR);
								}
							}
						}
					}
					canExit = true;
					NPC* newNpc = NPC::CreateNPC();
					NPCTemplate temp = fact->GetTemplate();
					temp.CopyValuesToNPC(newNpc, spriteSheet);
					newNpc->SetBLPosition(coord);
					newNpc->SetWantedBLPosition(coord);
					newNpc->SetSpriteSheet(spriteSheet);
					Agent::s_Agents.push_back(newNpc);
					trueNumGenned++;
				}
			}
		}
	}
	return trueNumGenned;
}
void NPCFactory::ClearAllNPCFactories()
{
	for (std::map<std::string, NPCFactory*>::iterator it = s_factories.begin(); it != s_factories.end(); it++)
	{
		if (it->second != nullptr)
		{
			GeneratorRegistration::RemoveRegistration(it->first);
			delete it->second;
			it->second = nullptr;
		}
	}
	s_factories.clear();
}
void NPCFactory::LoadAllNPCS()
{
	//Create Data/NPCs/Goblins/npc.xml with NPCBlueprints root
	//Add 2 "Goblins to it as 2 NPCBlueprint elements
	//Create NPCFactory class
	//Write NPCFactor::LoadAllNPCs() (static func)
	//	for each NPCBlueprint-> new NPCFactor()
	//	if factory is ok (no errors) add to static map<string, NPCFactor*> s_npcFactories
	//Run game to playing, break, examine s_npcFactories
	//	it should have 2 factories!
	//Have NPCFactory call its PopulateFromXMLNode(npcBlueprintNode)
	//	m_templateNPC = new NPC()
	//Write NPC* NPCFactor::CreateNPC();
	//In Game in or near "FinalizeMap", add populateMap()
	//	Get s_npcFactories.
	//	for(num Monsters)
	//		Pick random npcFactory from map
	//		Call Create
	//		Add NPC to map
	//		Add NPC to list of Agents

	//GeneratorRegistration("CellularAutomationGenerator", CreateCellularAutomationGenerator, CellularAutomationGenerationProcessCreateFunc);

	ItemFactory::LoadAllItems();
	Faction::LoadAllFactions();

	std::vector<std::string> NPCFactLocs = EnvironmentGenerationProcess::GetNPCFactoryLocations();
	
	int monsterCount = 0;

	for (size_t xmlFileIndex = 0; xmlFileIndex < NPCFactLocs.size(); xmlFileIndex++)
	{
		std::string xmlFileLocation = NPCFactLocs.at(xmlFileIndex);
		bool couldNotLoad = false;
		XMLNode root = EngineXMLParser::ParseXMLFile(xmlFileLocation, "NPCBlueprints", couldNotLoad);
		if (couldNotLoad == true)
		{
			continue;
		}

		for (int j = 0; j < root.nAttribute(); j++)
		{
			//do nothing for now.
		}

		for (int j = 0; j < root.nChildNode(); j++)
		{
			XMLNode rootChild = root.getChildNode(j);
			std::string childName = rootChild.getName();
			if (childName == "NPCBlueprint")
			{
				EnvironmentGenerationProcess* data = NPCFactory::NPCGenerationProcCreateFunc(rootChild);

				std::string monsterName = data->GetAttrValueByName("name");
				std::string defaultName = data->GetAttrValueByName("defaultName");
				if (monsterName == "")
				{
					if (defaultName == "")
					{
						monsterName = "UNNAMED_MONSTER_" + ToString(monsterCount);
					}
					else
					{
						monsterName = defaultName;
					}
				}
				NPCFactory* newFactory = new NPCFactory(data);
				if (newFactory->GetName() != "")
				{
					monsterName = newFactory->GetName();
				}
				s_factories.insert(std::pair<std::string, NPCFactory*>(monsterName, newFactory));

				s_factoryNames.push_back(monsterName);
				//GeneratorRegistration(monsterName, NPCFactory::NPCGeneratorCreationFunc, NPCFactory::NPCGenerationProcCreateFunc);

				monsterCount++;
				delete data;
			}
		}
	}
}