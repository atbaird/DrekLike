#pragma once

#ifndef ITEM_HPP
#define ITEM_HPP
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/GameEntity/GameEntity.hpp"
#include "Game/Items/StatusEffect.hpp"
#include <vector>

enum ItemType
{
	ItemType_Invalid = -1,
	ItemType_Weapon, //need 5
	ItemType_Armor, //need 5
	ItemType_Potion, //need 2
	ItemType_Container,
	NUM_OF_ITEM_TYPES
};

enum BlessingType
{
	BlessingType_Invalid = -1,
	BlessingType_Normal,
	BlessingType_Cursed,
	BlessingType_Blessed,
	NUM_OF_BLESSING_TYPES
};

enum EquipmentSlotType
{
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
};

//map related
struct ItemLastSeenAt 
{
	unsigned char m_char;
	IntVector2 tileLastSeenAt;
};

class Item;


class Item : public GameEntity
{
public:
	static std::vector<Item*> s_itemsVisibleToPlayer;
	static std::vector<Item*> s_ItemsOnTheGround;
	static std::vector<Item*> s_AllItems;
private:
	static int s_ItemIndex;
	//Item definers
	ItemType m_itemType;
	BlessingType m_blessingType;
	EquipmentSlotType m_acceptableEquipmentSlotType;
	std::string m_customEquipMessage;

	//Item variables
	int m_itemID;
	int m_minDamage; //consistent //Set Negative for heal XD
	int m_maxDamage;
	int m_minDamageOverTime; //applies this ammount over time after used.
	int m_maxDamageOverTime;
	int m_turnsForStatusEffects;
	bool m_grantFlight;
	unsigned int m_layerOfItems;
	bool m_destroyMe;
public:
	Item(const unsigned char& car = 'i', const Rgba& color = Rgba(1.f,1.f,1.f,1.f), const IntVector2& tilePos = IntVector2(0,0),
		const ItemType& type = ItemType_Invalid, const BlessingType& bType = BlessingType_Normal, 
		const EquipmentSlotType& eType = EquipmentSlotType_None, const int& damage = 0, const int& damageOverTime = 0,
		const int& turnsForStatusEffects = 0);
	Item(const Item& other, const bool& copyItemsWithin = false);
	virtual ~Item();

	//Static functions
	static void RemoveItemFromAllAndGround(Item* item);
	static void RemoveItemFromAll(Item* item);
	static void RemoveItemFromGround(Item* item);
	static Item* GetItemOnGroundByIndex(const int& index);
	static void ClearAllItems();
	static void ClearGroundItems();
	static void RenderGroundItems(SpriteSheet* spriteSheet, const Vector2& renderSize);
	static void CheckDestroyItems();
	static void RenderAllGroundItems(const Rgba& playingBackgroundColor, const bool& showFullMap);

	//Container
	void FinalStep();

	//Render And Update
	virtual const float Update(const float& deltaSeconds) override; //Entity::Update(const float& deltaSeconds)
	virtual void UpdateStep() override;
	virtual void Render(const Rgba& playingBackgroundColor) const override;
	static void Render(const char& car, const Rgba& playingBackgroundColor, const Rgba& color, const IntVector2& blPos, const bool& renderThis,
		SpriteSheet* spriteSheet);
	virtual void WriteToXMLNode(XMLNode& node) const override;
	virtual void WriteToXMLNode(XMLNode& node, const bool& equiped) const;
	virtual void ReadFromXMLNode(GenerationProcessData* data);

	//Setters
	void SetGrantFlight(const bool& grantFlight);
	void SetCustomEquipMessage(const std::string& message);
	void SetLayerOfItemsWithin(const int& set);
	void SetMinDamageFactor(const int& damage);
	void SetMaxDamageFactor(const int& damage);
	void SetMinDamageOverTimeFactor(const int& damageOverTime);
	void SetMaxDamageOverTimeFactor(const int& damageOverTime);
	void SetTurnsForStatusEffects(const int& turnsForStatusEffects);
	void SetItemType(const ItemType& itemType);
	void SetBlessingType(const BlessingType& blessingType);
	void SetAcceptableEquipmentSlotType(const EquipmentSlotType& acceptableEquipmentSlot);
	void SetDestroyMe(const bool& destroyMe);
	void SetItemID(const int& id);

	//Getters
	const bool GetGrantFlight() const;
	const std::string GetCustomEquipMessage() const;
	SpriteSheet* GetSpriteSheet() const;
	const int GetItemID() const;
	const unsigned int GetLayerOfItemsWithin() const;
	const int GetMinDamageFactor() const;
	const int GetMaxDamageFactor() const;
	const int GetRandomDamageFactor() const;
	const int GetMinDamageOverTimeFactor() const;
	const int GetMaxDamageOverTimeFactor() const;
	const int GetRandomdamageOverTimeFactor() const;
	const int GetTurnsForStatusEffects() const;
	const ItemType GetItemType() const;
	const BlessingType GetBlessingType() const;
	const EquipmentSlotType GetAcceptableEquipmentSlotType() const;
	const bool GetDestroyMe() const;
	const StatusEffect GetStatusEffect() const;
};
#endif