#include "Game/Items/ItemTemplate.hpp"

//-------------------------------------------------------------------------------
//Constructors
ItemTemplate::ItemTemplate()
	: m_name(""),
	m_itemType(ItemType_Potion),
	m_blessingType(BlessingType_Normal),
	m_acceptableEquipmentSlotType(EquipmentSlotType_None),
	m_minDamage(0),
	m_maxDamage(0),
	m_minDamageOverTime(0),
	m_maxDamageOverTime(0),
	m_turnsForStatusEffects(0),
	m_customEquipMessage(""),
	m_char('i'),
	m_color(1.f,1.f,1.f,1.f)
{

}
ItemTemplate::~ItemTemplate()
{

}

//-------------------------------------------------------------------------------
//Setters
void ItemTemplate::SetName(const std::string& name)
{
	m_name = name;
}
void ItemTemplate::SetItemType(const ItemType& itemType)
{
	m_itemType = itemType;
}
void ItemTemplate::SetBlessingType(const BlessingType& blessingType)
{
	m_blessingType = blessingType;
}
void ItemTemplate::SetAcceptableEquipSlot(const EquipmentSlotType& slotType)
{
	m_acceptableEquipmentSlotType = slotType;
}
void ItemTemplate::SetMinDamage(const int& damage)
{
	m_minDamage = damage;
}
void ItemTemplate::SetMaxDamage(const int& damage)
{
	m_maxDamage = damage;
}
void ItemTemplate::SetMinDamageOverTime(const int& damageOverTime)
{
	m_minDamageOverTime = damageOverTime;
}
void ItemTemplate::SetMaxDamageOverTime(const int& damageOverTime)
{
	m_maxDamageOverTime = damageOverTime;
}
void ItemTemplate::SetTurnsForStatusEffects(const int& statusEffectTurns)
{
	m_turnsForStatusEffects = statusEffectTurns;
}

void ItemTemplate::SetCustomEquipMessage(const std::string& message)
{
	m_customEquipMessage = message;
}
void ItemTemplate::SetChar(unsigned char& car)
{
	m_char = car;
}
void ItemTemplate::SetColor(const Rgba& color)
{
	m_color = color;
}
void ItemTemplate::SetGrantFlight(const bool& flight)
{
	m_grantFlight = flight;
}

//-------------------------------------------------------------------------------
//Getters
const std::string ItemTemplate::GetCustomEquipMessage() const
{
	return m_customEquipMessage;
}
const std::string ItemTemplate::GetName() const
{
	return m_name;
}
const ItemType ItemTemplate::GetItemType() const
{
	return m_itemType;
}
const BlessingType ItemTemplate::GetBlessingType() const
{
	return m_blessingType;
}
const EquipmentSlotType ItemTemplate::GetAcceptableEquipSlot() const
{
	return m_acceptableEquipmentSlotType;
}
const int ItemTemplate::GetDamage() const
{
	return m_minDamage;
}
const int ItemTemplate::GetDamageOverTime() const
{
	return m_minDamageOverTime;
}
const int ItemTemplate::GetTurnsForStatusEffects() const
{
	return m_turnsForStatusEffects;
}
const unsigned char ItemTemplate::GetChar() const
{
	return m_char;
}
const Rgba ItemTemplate::GetColor() const
{
	return m_color;
}

//-------------------------------------------------------------------------------
//CopyToItem
void ItemTemplate::CopyToItem(Item* item)
{
	if (item == nullptr)
	{
		return;
	}
	item->SetColor(m_color);
	item->SetChar(m_char);
	item->SetEntityName(m_name);
	item->SetItemType(m_itemType);
	item->SetBlessingType(m_blessingType);
	item->SetAcceptableEquipmentSlotType(m_acceptableEquipmentSlotType);
	item->SetMinDamageFactor(m_minDamage);
	item->SetMaxDamageFactor(m_maxDamage);
	item->SetMinDamageOverTimeFactor(m_minDamageOverTime);
	item->SetMaxDamageOverTimeFactor(m_maxDamageOverTime);
	item->SetTurnsForStatusEffects(m_turnsForStatusEffects);
	item->SetCustomEquipMessage(m_customEquipMessage);
	item->SetGrantFlight(m_grantFlight);
}