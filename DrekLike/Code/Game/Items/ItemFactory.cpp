#include "Game/Items/ItemFactory.hpp"
#include "Game/Map/Map.hpp"

std::map<std::string, ItemFactory*> ItemFactory::s_ItemFactoryMaps[NUM_OF_ITEM_TYPES];
std::vector<std::string> ItemFactory::s_ItemNames[NUM_OF_ITEM_TYPES];


ItemFactory::ItemFactory()
	: Generator(GENERATOR_TYPE_ITEM)
{

}
ItemFactory::ItemFactory(EnvironmentGenerationProcess* data)
	: Generator(GENERATOR_TYPE_ITEM)
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
		else if (strcmp(attr.name.c_str(), "itemType") == 0)
		{
			std::string val = attr.val;
			if (strcmp(val.c_str(), "potion") == 0)
			{
				m_template.SetItemType(ItemType_Potion);
				m_type = ItemType_Potion;
			}
			else if (strcmp(val.c_str(), "armor") == 0)
			{
				m_template.SetItemType(ItemType_Armor);
				m_type = ItemType_Armor;
			}
			else if (strcmp(val.c_str(), "weapon") == 0)
			{
				m_template.SetItemType(ItemType_Weapon);
				m_type = ItemType_Weapon;
			}
			else
			{
				m_template.SetItemType(ItemType_Invalid);
				m_type = ItemType_Invalid;
			}
		}
		else if (strcmp(attr.name.c_str(), "blessingType") == 0)
		{
			std::string val = attr.val;
			if (strcmp(val.c_str(), "normal") == 0)
			{
				m_template.SetBlessingType(BlessingType_Normal);
			}
			else if (strcmp(val.c_str(), "blessed") == 0)
			{
				m_template.SetBlessingType(BlessingType_Blessed);
			}
			else if (strcmp(val.c_str(), "cursed") == 0)
			{
				m_template.SetBlessingType(BlessingType_Cursed);
			}
			else
			{
				m_template.SetBlessingType(BlessingType_Invalid);
			}
		}
		else if (strcmp(attr.name.c_str(), "equipSlot") == 0)
		{
			std::string val = attr.val;
			if (strcmp(val.c_str(), "none") == 0)
			{
				m_template.SetAcceptableEquipSlot(EquipmentSlotType_None);
			}
			else if (strcmp(val.c_str(), "primaryHandRing") == 0 || strcmp(val.c_str(), "primaryRing") == 0)
			{
				m_template.SetAcceptableEquipSlot(EquipmentSlotType_PrimHandRing);
			}
			else if (strcmp(val.c_str(), "secondaryHandRing") == 0 || strcmp(val.c_str(), "secondaryRing") == 0)
			{
				m_template.SetAcceptableEquipSlot(EquipmentSlotType_SecHandRing);
			}
			else if (strcmp(val.c_str(), "primaryHand") == 0)
			{
				m_template.SetAcceptableEquipSlot(EquipmentSlotType_PrimHand);
			}
			else if (strcmp(val.c_str(), "secondaryHand") == 0)
			{
				m_template.SetAcceptableEquipSlot(EquipmentSlotType_SecHand);
			}
			else if (strcmp(val.c_str(), "chest") == 0)
			{
				m_template.SetAcceptableEquipSlot(EquipmentSlotType_Chest);
			}
			else if (strcmp(val.c_str(), "legs") == 0)
			{
				m_template.SetAcceptableEquipSlot(EquipmentSlotType_Legs);
			}
			else if (strcmp(val.c_str(), "head") == 0)
			{
				m_template.SetAcceptableEquipSlot(EquipmentSlotType_Head);
			}
			else
			{
				m_template.SetAcceptableEquipSlot(EquipmentSlotType_Invalid);
			}
		}
		else if (strcmp(attr.name.c_str(), "color") == 0)
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
			m_template.SetColor(Rgba(red, green, blue, 1.f));
		}
		else if (strcmp(attr.name.c_str(), "glyph") == 0)
		{
			unsigned char val = attr.val.at(0);
			m_template.SetChar(val);
		}
		else if (strcmp(attr.name.c_str(), "damage") == 0)
		{
			std::string val = ReplaceCharInString(attr.val, '~', ' ');
			val = ReplaceCharInString(val, ',', ' ');
			std::vector<std::string> strs = ParseString(val);
			if (strs.size() == 1)
			{
				int damage = atoi(attr.val.c_str());
				m_template.SetMinDamage(damage);
				m_template.SetMaxDamage(damage);
			}
			else if (strs.size() >= 2)
			{
				int min = atoi(strs.at(0).c_str());
				int max = atoi(strs.at(1).c_str());
				m_template.SetMinDamage(min);
				m_template.SetMaxDamage(max);
			}
		}
		else if (strcmp(attr.name.c_str(), "damageOverTime") == 0)
		{
			std::string val = ReplaceCharInString(attr.val, '~', ' ');
			val = ReplaceCharInString(val, ',', ' ');
			std::vector<std::string> strs = ParseString(val);
			if (strs.size() == 1)
			{
				int damage = atoi(attr.val.c_str());
				m_template.SetMinDamageOverTime(damage);
				m_template.SetMaxDamageOverTime(damage);
			}
			else if (strs.size() >= 2)
			{
				int min = atoi(strs.at(0).c_str());
				int max = atoi(strs.at(1).c_str());
				m_template.SetMinDamageOverTime(min);
				m_template.SetMaxDamageOverTime(max);
			}
		}
		else if (strcmp(attr.name.c_str(), "statusTurns") == 0)
		{
			int statusTurns = atoi(attr.val.c_str());
			m_template.SetTurnsForStatusEffects(statusTurns);
		}
		else if (SimpleStrCmp(attr.name, "customEquipMessage"))
		{
			m_template.SetCustomEquipMessage(attr.val);
		}
		else if (SimpleStrCmp(attr.name, "grantFlight"))
		{
			attr.val = MakeLower(attr.val);
			if (SimpleStrCmp(attr.val, "true")
				|| SimpleStrCmp(attr.val, "t")
				|| SimpleStrCmp(attr.val, "1"))
			{
				m_template.SetGrantFlight(true);
			}
		}
	}

	//std::vector<GenerationProcessData*> children = data->GetChildren();
	//for (size_t childIdx = 0; childIdx < children.size(); childIdx++)
	//{
	//	GenerationProcessData* child = children.at(childIdx);
	//
	//	std::string name = child->GetName().name;
	//
	//	std::vector<StrXMLAttr> attrs = child->GetAttributes();
	//	for (size_t attrIdx = 0; attrIdx < attrs.size(); attrIdx++)
	//	{
	//		StrXMLAttr attr = attrs.at(attrIdx);
	//	}
	//}
}
ItemFactory::~ItemFactory()
{

}
void ItemFactory::SetTemplate(const ItemTemplate& templ)
{
	m_template = templ;
}
void ItemFactory::SetName(const std::string& name)
{
	m_name = name;
}
void ItemFactory::SetType(const ItemType& type)
{
	m_type = type;
}

const ItemTemplate ItemFactory::GetTemplate() const
{
	return m_template;
}
const std::string ItemFactory::GetName() const
{
	return m_name;
}
const ItemType ItemFactory::GetType() const
{
	return m_type;
}
const int ItemFactory::GenerateNItems(const int& n, SpriteSheet* m_spriteSheet)
{
	bool atleastOneItemExists = false;
	for (size_t j = 0; j < NUM_OF_ITEM_TYPES; j++)
	{
		if (s_ItemNames[j].size() > 0)
		{
			atleastOneItemExists = true;
			break;
		}
	}
	if (atleastOneItemExists == false)
	{
		return 0;
	}
	std::vector<Tile_Coord> openCoords = g_map->GetAllNonSolidValidTileCoords();
	for (int i = 0; i < n; i++)
	{
		int ran = rand() % NUM_OF_ITEM_TYPES;
		if (s_ItemNames[ran].size() > 0)
		{
			int ran2 = rand() % (s_ItemNames[ran]).size();
			std::string itemName = (s_ItemNames[ran]).at(ran2);
			std::map<std::string, ItemFactory*>::iterator it = (s_ItemFactoryMaps[ran]).find(itemName);
			ItemTemplate templ = it->second->GetTemplate();
			Item* item = new Item();
			item->SetSpriteSheet(m_spriteSheet);
			templ.CopyToItem(item);

			int ran3 = rand() % openCoords.size();
			Tile_Coord coord = openCoords.at(ran3);
			item->SetBLPosition(coord);
			Item::s_ItemsOnTheGround.push_back(item);
			Item::s_AllItems.push_back(item);
		}
		else
		{
			i--;
		}
	}
	m_spriteSheet;
	return n;
}
const int ItemFactory::GenerateNItemsOfType(const int& n, SpriteSheet* m_spriteSheet, const ItemType& type)
{
	std::vector<Tile_Coord> openCoords = g_map->GetAllNonSolidValidTileCoords();
	for (int i = 0; i < n; i++)
	{
		int ran2 = rand() % s_ItemNames[type].size();
		ItemTemplate templ = s_ItemFactoryMaps[type].at(s_ItemNames->at(ran2))->GetTemplate();
		Item* item = new Item();
		templ.CopyToItem(item);

		int ran3 = rand() % openCoords.size();
		Tile_Coord coord = openCoords.at(ran3);
		item->SetBLPosition(coord);
		item->SetSpriteSheet(m_spriteSheet);
		Item::s_ItemsOnTheGround.push_back(item);
		Item::s_AllItems.push_back(item);
	}
	m_spriteSheet;
	return n;
}
Item* ItemFactory::GenerateItemByName(const std::string& name, const bool& putOnGround)
{
	for (size_t i = 0; i < NUM_OF_ITEM_TYPES; i++)
	{
		for (size_t j = 0; j < s_ItemNames[i].size(); j++)
		{
			if (SimpleStrCmp(s_ItemNames[i].at(j), name))
			{
				std::map<std::string, ItemFactory*>::const_iterator it = s_ItemFactoryMaps[i].find(name);
				if (it != s_ItemFactoryMaps[i].end())
				{
					ItemTemplate templ = it->second->GetTemplate();
					Item* item = new Item();
					templ.CopyToItem(item);
					if (putOnGround == true)
					{
						Item::s_ItemsOnTheGround.push_back(item);
					}
					Item::s_AllItems.push_back(item);
					return item;
				}
			}
		}
	}
	return nullptr;
}
EnvironmentGenerationProcess* ItemFactory::ItemGenerationProcCreateFunc(const XMLNode& node)
{
	EnvironmentGenerationProcess* data = new EnvironmentGenerationProcess();
	data->PopulateFromXMLNode(node);
	return data;
}
Generator* ItemFactory::ItemGeneratorCreationFunc(const std::string& name, const ItemType& type)
{
	std::map<std::string, ItemFactory*>::iterator it = s_ItemFactoryMaps[type].find(name);
	if (it == s_ItemFactoryMaps[type].end())
	{
		return nullptr;
	}

	ItemFactory* newFact = new ItemFactory();
	newFact->SetTemplate(it->second->GetTemplate());

	return newFact;
}
Generator* ItemFactory::ItemGeneratorCreationFunc(const std::string& name)
{

	for (size_t i = 0; i < NUM_OF_ITEM_TYPES; i++)
	{
		std::map<std::string, ItemFactory*>::iterator it = s_ItemFactoryMaps[i].find(name);
		if (it != s_ItemFactoryMaps[i].end())
		{
			ItemFactory* newFact = new ItemFactory();
			newFact->SetTemplate(it->second->GetTemplate());
			return newFact;
		}
	}

	return nullptr;
}
const bool ItemFactory::IsThereAnItemFactoryOfName(const std::string& name)
{
	for (size_t i = 0; i < NUM_OF_ITEM_TYPES; i++)
	{
		std::map<std::string, ItemFactory*>::const_iterator it = s_ItemFactoryMaps[i].find(name);
		if (it != s_ItemFactoryMaps[i].end())
		{
			return true;
		}
	}
	return false;
}



void ItemFactory::ClearAllItemFactories()
{
	for (size_t i = 0; i < NUM_OF_ITEM_TYPES; i++)
	{
		std::map<std::string, ItemFactory*> itemFactoriesOfType = s_ItemFactoryMaps[i];
		for (std::map<std::string, ItemFactory*>::iterator it = itemFactoriesOfType.begin(); it != itemFactoriesOfType.end(); it++)
		{
			if (it->second != nullptr)
			{
				GeneratorRegistration::RemoveRegistration(it->first);
				delete it->second;
				it->second = nullptr;
			}
		}
		itemFactoriesOfType.clear();
	}
}
void ItemFactory::LoadAllItems()
{

	std::vector<std::string> itemFactLocs = EnvironmentGenerationProcess::GetItemFactoryLocations();

	for (size_t itemIdx = 0; itemIdx < itemFactLocs.size(); itemIdx++)
	{
		std::string itemLoc = itemFactLocs.at(itemIdx);
		bool couldNotLoad = false;
		XMLNode root = EngineXMLParser::ParseXMLFile(itemLoc, "ItemBlueprints", couldNotLoad);
		if (couldNotLoad == true)
		{
			continue;
		}

		for (int i = 0; i < root.nChildNode(); i++)
		{
			XMLNode rootChild = root.getChildNode(i);
			std::string childName = rootChild.getName();
			if (childName == "ItemBlueprint")
			{
				EnvironmentGenerationProcess* data = ItemFactory::ItemGenerationProcCreateFunc(rootChild);

				ItemFactory* newFactory = new ItemFactory(data);
				std::string itemName = newFactory->GetName();
				ItemType type = newFactory->GetType();

				s_ItemFactoryMaps[type].insert(std::pair<std::string, ItemFactory*>(itemName, newFactory));

				s_ItemNames[type].push_back(itemName);
				//GeneratorRegistration(itemName, ItemFactory::ItemGeneratorCreationFunc, ItemFactory::ItemGenerationProcCreateFunc);

				delete data;
			}
		}
	}
}