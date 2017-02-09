#pragma once

#ifndef ITEMFACTORY_HPP
#define ITEMFACTORY_HPP
#include <map>
#include <vector>
#include "Game/Generators/EnvironmentGenerationProcess.hpp"
#include "Game/Items/ItemTemplate.hpp"
#include "Game/Generators/Generator.hpp"

/*
enum ItemType
{
ItemType_Invalid = -1,
ItemType_Weapon,
ItemType_Armor,
ItemType_Potion,
ItemType_Book,
ItemType_Scroll,
ItemType_Food,
ItemType_Container,
NUM_OF_ITEM_TYPES
};
*/
struct ItemTypeNamePair
{

};

class ItemFactory : public Generator
{
private:
	std::string m_name;
	ItemType m_type;
	ItemTemplate m_template;
	static std::map<std::string, ItemFactory*> s_ItemFactoryMaps[NUM_OF_ITEM_TYPES];
	static std::vector<std::string> s_ItemNames[NUM_OF_ITEM_TYPES];
public:
	ItemFactory();
	ItemFactory(EnvironmentGenerationProcess* data);
	~ItemFactory();

	void SetTemplate(const ItemTemplate& templ);
	void SetName(const std::string& name);
	void SetType(const ItemType& type);

	const ItemTemplate GetTemplate() const;
	const std::string GetName() const;
	const ItemType GetType() const;

	static const int GenerateNItems(const int& n, SpriteSheet* m_spriteSheet);
	static const int GenerateNItemsOfType(const int& n, SpriteSheet* m_spriteSheet, const ItemType& type);
	static Item* GenerateItemByName(const std::string& name, const bool& putOnGround = false);
	static EnvironmentGenerationProcess* ItemGenerationProcCreateFunc(const XMLNode& node);
	static Generator* ItemGeneratorCreationFunc(const std::string& name, const ItemType& type);
	static Generator* ItemGeneratorCreationFunc(const std::string& name);
	static const bool IsThereAnItemFactoryOfName(const std::string& name);

	static void ClearAllItemFactories();
	static void LoadAllItems();

};
#endif // !ITEMFACTORY_HPP