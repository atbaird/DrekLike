#include "Game/Items/Inventory.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/GameCommonFunctions.hpp"

//------------------------------------------------------------------------------------
//Constructors
Inventory::Inventory(const int& inventoryCap)
	: inventoryLimit(inventoryCap)
{
	for (size_t i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
	{
		m_equipped[i] = nullptr;
	}
}
Inventory::~Inventory()
{
	m_items.clear();
}

//------------------------------------------------------------------------------------
//Item Related
Item* Inventory::GetItemAtIndex(const int& index) const
{
	if (index < 0 || index >= (int)m_items.size())
	{
		return nullptr;
	}
	return m_items.at(index);
}
Item* Inventory::GetItemEquippedAtIndex(const int& index) const
{
	if (index < 0 || index >= NUM_EQUIPMENT_SLOTS)
	{
		return nullptr;
	}
	return m_equipped[index];
}
Item* Inventory::DropItemAtIndex(const int& index, const bool& displayMessage, const std::string& ownerName)
{
	if (index < 0 || index >= (int)m_items.size())
	{
		return nullptr;
	}
	Item* item = m_items.at(index);

	for (size_t i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
	{
		if (item == m_equipped[i])
		{
			m_equipped[i] = nullptr;
		}
	}
	if (displayMessage == true)
	{
		std::string message = ownerName + " dropped " + item->GetEntityName();
		s_messageBoxStrings.push_back(message);
	}

	m_items.erase(m_items.begin() + index);

	return item;
}
void Inventory::DestroyItemInInventory(Item* item)
{
	if (item == nullptr)
	{
		return;
	}
	for (size_t i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
	{
		if (m_equipped[i] != nullptr && m_equipped[i] == item)
		{
			m_equipped[i] = nullptr;
			break;
		}
	}
	for (size_t i = 0; i < m_items.size(); i++)
	{
		if (item == m_items.at(i))
		{
			m_items.erase(m_items.begin() + i);
			break;
		}
	}
	Item::RemoveItemFromAll(item);
	delete item;
}
const int Inventory::GetOffesiveMinDamageRating() const
{
	int damageRating = 0;
	for (size_t i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
	{
		Item* item = m_equipped[i];
		if (item != nullptr)
		{
			ItemType type = item->GetItemType();
			EquipmentSlotType eType = item->GetAcceptableEquipmentSlotType();
			if (type == ItemType_Weapon)
			{
				if (eType == EquipmentSlotType_PrimHand)
				{
					damageRating += item->GetMinDamageFactor();
				}
				else if (eType == EquipmentSlotType_SecHand)
				{
					damageRating += (int)(round((float)item->GetMinDamageFactor() / 2.f));
				}
			}
			else
			{
				if (eType == EquipmentSlotType_PrimHand)
				{
					damageRating += (int)(round((float)item->GetMinDamageFactor() / 2.f));
				}
				else if (eType == EquipmentSlotType_SecHand)
				{
					damageRating += (int)(round((float)item->GetMinDamageFactor() / 4.f));
				}
			}
		}
	}

	return damageRating;
}
const int Inventory::GetOffensiveMaxDamageRating() const
{
	int damageRating = 0;
	for (size_t i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
	{
		Item* item = m_equipped[i];
		if (item != nullptr)
		{
			ItemType type = item->GetItemType();
			EquipmentSlotType eType = item->GetAcceptableEquipmentSlotType();
			if (type == ItemType_Weapon)
			{
				//Full Force for primary hand weapon.
				if (eType == EquipmentSlotType_PrimHand)
				{
					damageRating += item->GetMaxDamageFactor();
				}
				else if (eType == EquipmentSlotType_SecHand)
				{
					damageRating += (int)(round((float)item->GetMaxDamageFactor() / 2.f));
				}
			}
			else
			{
				//lesser for non-weapon in hands.
				if (eType == EquipmentSlotType_PrimHand)
				{
					damageRating += (int)(round((float)item->GetMaxDamageFactor() / 2.f));
				}
				else if (eType == EquipmentSlotType_SecHand)
				{
					damageRating += (int)(round((float)item->GetMaxDamageFactor() / 4.f));
				}
			}
		}
	}

	return damageRating;
}
const int Inventory::GetRandOffensiveDamageRating() const
{
	int ran = 0;
	int min = GetOffesiveMinDamageRating();
	int max = GetOffensiveMaxDamageRating();
	int diff = max - min;
	int neg = 1;
	if (abs(diff) > 0)
	{
		neg = diff / abs(diff);
		ran = rand() % abs(diff);
		ran = ran * neg;
	}
	ran += min;
	return ran;
}
const int Inventory::GetDefensiveMinProtectionRating() const
{
	int defenseRating = 0;
	for (size_t i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
	{
		Item* item = m_equipped[i];
		if (item != nullptr)
		{
			ItemType type = item->GetItemType();
			EquipmentSlotType eType = item->GetAcceptableEquipmentSlotType();
			if (type == ItemType_Armor)
			{
				//Actually is armor
				if (eType == EquipmentSlotType_Head
					|| eType == EquipmentSlotType_Chest
					|| eType == EquipmentSlotType_Legs)
				{
					defenseRating += item->GetMinDamageFactor();
				}
				else if (eType == EquipmentSlotType_PrimHand
					|| eType == EquipmentSlotType_SecHand
					|| eType == EquipmentSlotType_PrimHandRing
					|| eType == EquipmentSlotType_SecHandRing)
				{
					defenseRating += (int)(round((float)item->GetMinDamageFactor() / 2.f));
				}
			}
			else
			{
				//Actually is armor
				if (eType == EquipmentSlotType_Head
					|| eType == EquipmentSlotType_Chest
					|| eType == EquipmentSlotType_Legs)
				{
					defenseRating += item->GetMinDamageFactor();
				}
				else if (eType == EquipmentSlotType_PrimHandRing
					|| eType == EquipmentSlotType_SecHandRing)
				{
					defenseRating += (int)(round((float)item->GetMinDamageFactor() / 2.f));
				}
			}
		}
	}
	return defenseRating;
}
const int Inventory::GetDefensiveMaxProtectionRating() const
{
	int defenseRating = 0;
	for (size_t i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
	{
		Item* item = m_equipped[i];
		if (item != nullptr)
		{
			ItemType type = item->GetItemType();
			EquipmentSlotType eType = item->GetAcceptableEquipmentSlotType();
			if (type == ItemType_Armor)
			{
				//Actually is armor
				if (eType == EquipmentSlotType_Head
					|| eType == EquipmentSlotType_Chest
					|| eType == EquipmentSlotType_Legs)
				{
					defenseRating += item->GetMaxDamageFactor();
				}
				else if (eType == EquipmentSlotType_PrimHand
					|| eType == EquipmentSlotType_SecHand
					|| eType == EquipmentSlotType_PrimHandRing
					|| eType == EquipmentSlotType_SecHandRing)
				{
					defenseRating += (int)(round((float)item->GetMaxDamageFactor() / 2.f));
				}
			}
			else
			{
				//Actually is armor
				if (eType == EquipmentSlotType_Head
					|| eType == EquipmentSlotType_Chest
					|| eType == EquipmentSlotType_Legs)
				{
					defenseRating += item->GetMaxDamageFactor();
				}
				else if (eType == EquipmentSlotType_PrimHandRing
					|| eType == EquipmentSlotType_SecHandRing)
				{
					defenseRating += (int)(round((float)item->GetMaxDamageFactor() / 2.f));
				}
			}
		}
	}
	return defenseRating;
}
const int Inventory::GetRandDefensiveProtectionRating() const
{
	int ran = 0;
	int min = GetDefensiveMinProtectionRating();
	int max = GetDefensiveMaxProtectionRating();
	int diff = max - min;
	int neg = 1;
	if (abs(diff) > 0)
	{
		neg = diff / abs(diff);
		ran = rand() % abs(diff);
		ran = ran * neg;
	}
	ran += min;
	return ran;
}
const int Inventory::GetLastItemAddedIndex() const
{
	int toRet = m_items.size() - 1;
	return toRet;
}
const bool Inventory::AddItem(Item* item, const bool& displayMessage, const std::string& ownerName)
{
	if ((int)m_items.size() >= inventoryLimit)
	{
		return false;
	}
	if (displayMessage == true)
	{
		std::string message = ownerName + " picked up " + item->GetEntityName();
		s_messageBoxStrings.push_back(message);
	}
	m_items.push_back(item);

	return true;
}
const bool Inventory::IsInventoryFull() const
{
	if ((int)m_items.size() >= inventoryLimit)
	{
		return true;
	}
	return false;
}
const bool Inventory::GetCanFlyNow() const
{
	bool fly = false;
	for (int i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
	{
		if (m_equipped[i] != nullptr && m_equipped[i]->GetGrantFlight() == true)
		{
			fly = true;
			break;
		}
	}
	return fly;
}
void Inventory::WriteToXMLNode(XMLNode& node) const
{
	XMLNode inventory = node.addChild("Inventory");
	for (size_t i = 0; i < m_items.size(); i++)
	{
		Item* item = m_items.at(i);
		bool equipped = false;
		for(int j = 0; j < NUM_EQUIPMENT_SLOTS; j++)
		{
			if (item == m_equipped[j])
			{
				equipped = true;
				break;
			}
		}
		item->WriteToXMLNode(inventory, equipped);
	}
}
const bool Inventory::UnequipItemFromInventory(const int& index, const bool& displayMessage, const std::string& ownerName)
{
	if (index < 0 || index >= NUM_EQUIPMENT_SLOTS)
	{
		return false;
	}
	Item* nullItem = (Item*)index;
	nullItem;
	EquipItemToAppropriateSlot(nullItem, displayMessage, ownerName);
	return true;
}
const bool Inventory::EquipItemFromInventory(const int& index, const bool& displayMessage, const std::string& ownerName)
{
	if (index < 0 || index >= (int)m_items.size())
	{
		return false;
	}

	Item* item = m_items.at(index);
	if (item == nullptr || (item != nullptr && HasValidEquipSlot(item->GetAcceptableEquipmentSlotType()) == false))
	{
		return false;
	}
	EquipItemToAppropriateSlot(item, displayMessage, ownerName);

	return true;
}
std::vector<Item*> Inventory::GatherContainedItemsOfType(const ItemType& type)
{
	std::vector<Item*> items;

	for (size_t i = 0; i < m_items.size(); i++)
	{
		if (m_items.at(i) != nullptr && m_items.at(i)->GetItemType() == type)
		{
			items.push_back(m_items.at(i));
		}
	}

	return items;

}
const bool Inventory::EquipItemToAppropriateSlot(Item* item, const bool& displayMessage, const std::string& ownerName)
{
	/*
	//Index of all equipment slot enum values as of 5/3/2016
	EquipmentSlotType_Invalid = -2,
	EquipmentSlotType_None, // -1
	EquipmentSlotType_PrimHand, // 0
	EquipmentSlotType_SecHand, // 1
	EquipmentSlotType_PrimHandRing, // 2
	EquipmentSlotType_SecHandRing, // 3
	EquipmentSlotType_Chest, // 4
	EquipmentSlotType_Legs, //5
	EquipmentSlotType_Head, // 6
	NUM_EQUIPMENT_SLOTS, //7
	TRUE_NUM_EQUIPMENT_SLOTS // 8
	*/

	//Hey, this is a sneaky trick i'm pulling here,
	//I am setting nullptr to the equipment slot index if i'm unequipping.
	//Thus this first part is specifically to detect that.
	EquipmentSlotType slot = EquipmentSlotType_None;
	if (item >= (Item*)EquipmentSlotType_PrimHand
		&& item < (Item*)NUM_EQUIPMENT_SLOTS)
	{
		//In this case, item is actually null, but contains the slot index we wish to access.
		//so we set slot equal to the item, as an EquipmentSlotType, then set item to null.
		slot = (EquipmentSlotType)(int)item;
		item = nullptr;
	}
	else if (item != nullptr)
	{
		//in this case, item exists, and we'll get the value out of it normally.
		slot = item->GetAcceptableEquipmentSlotType();
	}

	//Last chance to guarantee that I am trying to access a valid equipment slot.
	if (slot < EquipmentSlotType_PrimHand
		&& slot >= NUM_EQUIPMENT_SLOTS)
	{
		//Then some how we managed to get no appropriate equipment slot, thus we want to do nothing.
		s_messageBoxStrings.push_back("ERROR: Attempted to unequip/equip invalid equipment slot.");
		return false;
	}

	//First, we got to verify that the equipment slot is empty... if it isn't, then we should unequip it.
	Item* equipSlot = m_equipped[slot];
	if (equipSlot != nullptr)
	{
		if (displayMessage == true)
		{
			std::string message = ownerName + " unequipped " + item->GetEntityName();
			s_messageBoxStrings.push_back(message);
		}
		m_equipped[slot] = nullptr;
	}
	
	//now that the equipment slot is empty, we can put the item into the equipment slot.
	m_equipped[slot] = item;
	if (item != nullptr && displayMessage == true)
	{
		std::string message = ownerName + " equipped " + item->GetEntityName();
		switch (slot)
		{
		case EquipmentSlotType_Head:
			message += " onto it\'s head.";
			break;
		case EquipmentSlotType_Chest:
			message += " onto it\'s chest.";
			break;
		case EquipmentSlotType_Legs:
			message += " onto it\'s legs.";
			break;
		case EquipmentSlotType_PrimHand:
			message += " into it\'s primary hand.";
			break;
		case EquipmentSlotType_SecHand:
			message += " into it\'s secondary hand.";
			break;
		case EquipmentSlotType_PrimHandRing:
			message += " onto it\'s primary hand ring finger.";
			break;
		case EquipmentSlotType_SecHandRing:
			message += " onto it\'s secondary hand ring finger.";
			break;
		default:
			break;
		}
		s_messageBoxStrings.push_back(message);
		std::string customEquipMessage = item->GetCustomEquipMessage();
		if (SimpleStrCmp(customEquipMessage, "") == false)
		{
			s_messageBoxStrings.push_back(ownerName + customEquipMessage);
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------
//Getters
const bool Inventory::HasValidEquipSlot(const EquipmentSlotType& slotType) const
{
	if (slotType < 0 || slotType >= NUM_EQUIPMENT_SLOTS)
	{
		return false;
	}
	return true;
}
const std::vector<Item*> Inventory::GetItems() const
{
	return m_items;
}
const std::vector<Item*> Inventory::GetItemsByType(const ItemType& type) const
{
	std::vector<Item*> items;
	for (size_t i = 0; i < m_items.size(); i++)
	{
		if (m_items.at(i)->GetItemType() == type)
		{
			items.push_back(m_items.at(i));
		}
	}
	return items;
}
const unsigned int Inventory::GetNumberOfItemsHeld() const
{
	return m_items.size();
}
const unsigned int Inventory::GetItemLimit() const
{
	return inventoryLimit;
}

//-------------------------------------------------------------------------------------------
//Render
void Inventory::Render(const Vector3& startPos, Font* font, const float& fontSize) const
{
	if (font == nullptr)
	{
		//font is null, don't both trying to render.
		return;
	}
	std::string message = "";
	message = message + "EQUIPPED";
	for (size_t i = 0; i < NUM_EQUIPMENT_SLOTS; i++)
	{
		if (m_equipped[i] != nullptr)
		{
			message = message + "\n   " + m_equipped[i]->GetEntityName();
		}
	}

	message = message + "\n" + "INVENTORY";
	for (size_t i = 0; i < m_items.size(); i++)
	{
		if (m_items.at(i) != nullptr)
		{
			Item* item = m_items.at(i);
			bool equipped = false;
			for (size_t j = 0; j < NUM_EQUIPMENT_SLOTS; j++)
			{
				if (m_equipped[j] == item)
				{
					equipped = true;
					break;
				}
			}
			if (equipped == false)
			{
				message = message + "\n   " + item->GetEntityName();
			}
		}
	}

	Vector3 trueStart = startPos;
	float height = font->CalcTextHeight(message, fontSize);
	trueStart.y -= height;

	font->RenderText(trueStart, message, fontSize, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Rgba(0.f, 0.f, 0.f, 1.f),
		Font_LeftHand_Aligned, true);
}