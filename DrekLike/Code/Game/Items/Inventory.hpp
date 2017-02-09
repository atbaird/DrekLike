#pragma once

#ifndef INVENTORY_HPP
#define INVENTORY_HPP
#include "Game/Items/Item.hpp"
#include <vector>
#include "Engine/Font/Font.hpp"

class Inventory
{
private:
	int inventoryLimit;
	std::vector<Item*> m_items;
	Item* m_equipped[NUM_EQUIPMENT_SLOTS];
public:
	//Constructors
	Inventory(const int& inventoryCap = 20);
	~Inventory();

	//Item Related
	Item* GetItemAtIndex(const int& index) const;
	Item* GetItemEquippedAtIndex(const int& index) const;
	Item* DropItemAtIndex(const int& index, const bool& displayMessage, const std::string& ownerName = "");
	void DestroyItemInInventory(Item* item);
	const int GetOffesiveMinDamageRating() const;
	const int GetOffensiveMaxDamageRating() const;
	const int GetRandOffensiveDamageRating() const;
	const int GetDefensiveMinProtectionRating() const;
	const int GetDefensiveMaxProtectionRating() const;
	const int GetRandDefensiveProtectionRating() const;
	const int GetLastItemAddedIndex() const;
	const bool UnequipItemFromInventory(const int& index, const bool& displayMessage, const std::string& ownerName = "");
	const bool EquipItemFromInventory(const int& index, const bool& displayMessage, const std::string& ownerName = "");
	const bool AddItem(Item* item, const bool& displayMessage, const std::string& ownerName = "");
	const bool IsInventoryFull() const;
	const bool GetCanFlyNow() const;
	void WriteToXMLNode(XMLNode& node) const;
private:
	std::vector<Item*> GatherContainedItemsOfType(const ItemType& type);
	const bool EquipItemToAppropriateSlot(Item* item, const bool& displayMessage, const std::string& ownerName);
public:

	//Getters
	const bool HasValidEquipSlot(const EquipmentSlotType& slotType) const;
	const std::vector<Item*> GetItems() const;
	const std::vector<Item*> GetItemsByType(const ItemType& type) const;
	const unsigned int GetNumberOfItemsHeld() const;
	const unsigned int GetItemLimit() const;

	//Render
	void Render(const Vector3& startPos, Font* font = nullptr, const float& fontSize = 1.f) const;
};
#endif