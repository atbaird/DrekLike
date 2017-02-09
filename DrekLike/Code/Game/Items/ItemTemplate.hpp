#pragma once

#ifndef ITEMTEMPLATE_HPP
#define ITEMTEMPLATE_HPP
#include "Game/Items/Item.hpp"

class ItemTemplate
{
private:

	std::string m_name;
	ItemType m_itemType;
	BlessingType m_blessingType;
	EquipmentSlotType m_acceptableEquipmentSlotType;
	std::string m_customEquipMessage;

	//Item variables
	int m_minDamage; //consistent //Set Negative for heal XD
	int m_maxDamage;
	int m_minDamageOverTime; //applies this ammount over time after used.
	int m_maxDamageOverTime;
	int m_turnsForStatusEffects;
	bool m_grantFlight;
	unsigned char m_char;
	Rgba m_color;
public:
	//Constructors
	ItemTemplate();
	~ItemTemplate();

	//Setters
	void SetName(const std::string& name);
	void SetItemType(const ItemType& itemType);
	void SetBlessingType(const BlessingType& blessingType);
	void SetAcceptableEquipSlot(const EquipmentSlotType& slotType);
	void SetMinDamage(const int& damage);
	void SetMaxDamage(const int& damage);
	void SetMinDamageOverTime(const int& damageOverTime);
	void SetMaxDamageOverTime(const int& damageOverTime);
	void SetTurnsForStatusEffects(const int& statusEffectTurns);
	void SetCustomEquipMessage(const std::string& message);
	void SetChar(unsigned char& car);
	void SetColor(const Rgba& color);
	void SetGrantFlight(const bool& flight);

	//Getters
	const std::string GetCustomEquipMessage() const;
	const std::string GetName() const;
	const ItemType GetItemType() const;
	const BlessingType GetBlessingType() const;
	const EquipmentSlotType GetAcceptableEquipSlot() const;
	const int GetDamage() const;
	const int GetDamageOverTime() const;
	const int GetTurnsForStatusEffects() const;
	const unsigned char GetChar() const;
	const Rgba GetColor() const;

	//CopyToItem
	void CopyToItem(Item* item);
};
#endif