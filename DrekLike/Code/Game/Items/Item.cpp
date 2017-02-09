#include "Game/Items/Item.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Map/Map.hpp"
std::vector<Item*> Item::s_itemsVisibleToPlayer;
std::vector<Item*> Item::s_ItemsOnTheGround;
std::vector<Item*> Item::s_AllItems;
int Item::s_ItemIndex = 0;

//--------------------------------------------------------------------------------------------------------------------------------------
//Constructors
Item::Item(const unsigned char& car, const Rgba& color, const IntVector2& tilePos,
	const ItemType& type, const BlessingType& bType,
	const EquipmentSlotType& eType, const int& damage, const int& damageOverTime, const int& turnsForStatusEffects)
	: GameEntity(ENTITY_ITEM, tilePos, IntVector2(1,1)),
	m_itemType(type),
	m_blessingType(bType),
	m_acceptableEquipmentSlotType(eType),
	m_minDamage(damage),
	m_maxDamage(damage),
	m_minDamageOverTime(damageOverTime),
	m_maxDamageOverTime(damageOverTime),
	m_grantFlight(false),
	m_turnsForStatusEffects(turnsForStatusEffects),
	m_customEquipMessage(""),
	m_layerOfItems(0),
	m_destroyMe(false)
{
	m_itemID = s_ItemIndex;
	s_ItemIndex++;
	m_char = car;
	m_color = color;
	/*
	GameEntity(const EntityType& type, const IntVector2& blTilePos = IntVector2(0,0), const IntVector2& entitySize = IntVector2(1,1));
	*/
}
Item::Item(const Item& other, const bool& copyItemsWithin)
	: GameEntity(other),
	m_itemType(other.m_itemType),
	m_blessingType(other.m_blessingType),
	m_acceptableEquipmentSlotType(other.m_acceptableEquipmentSlotType),
	m_minDamage(other.m_minDamage),
	m_minDamageOverTime(other.m_minDamageOverTime),
	m_customEquipMessage(other.m_customEquipMessage),
	m_turnsForStatusEffects(other.m_turnsForStatusEffects),
	m_layerOfItems(other.m_layerOfItems),
	m_destroyMe(other.m_destroyMe),
	m_grantFlight(other.m_grantFlight),
	m_itemID(other.m_itemID)
{
	if (copyItemsWithin == true)
	{
	}
}
Item::~Item()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Static functions
void Item::RemoveItemFromAllAndGround(Item* item)
{
	RemoveItemFromAll(item);
	RemoveItemFromGround(item);
}
void Item::RemoveItemFromAll(Item* item)
{
	for (size_t i = 0; i < s_AllItems.size(); i++)
	{
		if (s_AllItems.at(i) == item)
		{
			s_AllItems.erase(s_AllItems.begin() + i);
			break;
		}
	}
}
void Item::RemoveItemFromGround(Item* item)
{
	for (size_t i = 0; i < s_ItemsOnTheGround.size(); i++)
	{
		if (s_ItemsOnTheGround.at(i) == item)
		{
			s_ItemsOnTheGround.erase(s_ItemsOnTheGround.begin() + i);
			break;
		}
	}
}
Item* Item::GetItemOnGroundByIndex(const int& index)
{
	if (index < 0 || index >= (int)s_ItemsOnTheGround.size())
	{
		return nullptr;
	}
	return s_ItemsOnTheGround.at(index);
}
void Item::ClearAllItems()
{
	for (size_t i = 0; i < s_AllItems.size(); i++)
	{
		if (s_AllItems.at(i) != nullptr)
		{
			delete s_AllItems[i];
			s_AllItems[i] = nullptr;
		}
	}
	s_AllItems.clear();
	s_ItemsOnTheGround.clear();
}
void Item::ClearGroundItems()
{
	for (size_t i = 0; i < s_ItemsOnTheGround.size(); i++)
	{
		if (s_ItemsOnTheGround.at(i) != nullptr)
		{
			for (size_t j = 0; j < s_AllItems.size(); j++)
			{
				if (s_AllItems.at(j) == s_ItemsOnTheGround.at(i))
				{
					s_AllItems.erase(s_AllItems.begin() + j);
					break;
				}
			}
			delete s_ItemsOnTheGround[i];
			s_ItemsOnTheGround[i] = nullptr;
		}
	}
	s_ItemsOnTheGround.clear();
}
void Item::RenderGroundItems(SpriteSheet* spriteSheet, const Vector2& renderSize)
{
	std::vector<Vertex_PCT> quadVertices;
	for (size_t i = 0; i < s_ItemsOnTheGround.size(); i++)
	{
		if (s_ItemsOnTheGround.at(i) != nullptr)
		{
			Item* item = s_ItemsOnTheGround.at(i);
			IntVector2 bl = item->GetBlTilePos();
			IntVector2 tr = bl + item->GetSize();
			Rgba colorToRender = item->GetColor();
			Vector3 renderBL = Vector3((float)bl.x * renderSize.x, (float)bl.y * renderSize.y, 0.f);
			Vector3 renderTR = Vector3((float)tr.x * renderSize.x, (float)tr.y * renderSize.y, 0.f);
			Vector2 uvBL = Vector2(0.f, 0.f);
			Vector2 uvTR = Vector2(0.f, 0.f);
			unsigned char car = item->GetChar();
			if (spriteSheet != nullptr)
			{
				if (car != ' ')
				{
					AABB2 sprVal = spriteSheet->GetTexCoordsForSpriteIndex(car);
					uvBL = Vector2(sprVal.mins.x, sprVal.maxs.y);
					uvTR = Vector2(sprVal.maxs.x, sprVal.mins.y);

				}
				else
				{
					AABB2 sprVal = spriteSheet->GetTexCoordsForSpriteIndex(1);
					uvBL = Vector2(sprVal.mins.x, sprVal.maxs.y);
					uvTR = Vector2(sprVal.maxs.x, sprVal.mins.y);
				}
			}
			Vector2 uvBR = Vector2(uvTR.x, uvBL.y);
			Vector2 uvTL = Vector2(uvBL.x, uvTR.y);
			quadVertices.push_back(Vertex_PCT(Vector3(renderBL.x, renderBL.y, 0.f), colorToRender, uvBL));
			quadVertices.push_back(Vertex_PCT(Vector3(renderTR.x, renderBL.y, 0.f), colorToRender, uvBR));
			quadVertices.push_back(Vertex_PCT(Vector3(renderTR.x, renderTR.y, 0.f), colorToRender, uvTR));
			quadVertices.push_back(Vertex_PCT(Vector3(renderBL.x, renderTR.y, 0.f), colorToRender, uvTL));
		}
	}
	g_Renderer->DrawVertexArray(quadVertices, PRIMITIVE_QUADS, spriteSheet->getSpriteSheetTexture());
}
void Item::CheckDestroyItems()
{
	for (size_t i = 0; i < s_AllItems.size(); i++)
	{
		Item* item = s_AllItems.at(i);
		if (item->GetDestroyMe() == true)
		{
			for (size_t j = 0; j < s_ItemsOnTheGround.size(); j++)
			{
				if (item == s_ItemsOnTheGround.at(j))
				{
					s_ItemsOnTheGround.erase(s_ItemsOnTheGround.begin() + j);
					break;
				}
			}
			s_AllItems.erase(s_AllItems.begin() + i);
			i--;
			delete item;
			item = nullptr;
		}
		else
		{
			item->FinalStep();
		}
	}
}
void Item::RenderAllGroundItems(const Rgba& playingBackgroundColor, const bool& showFullMap)
{
	std::vector<IntVector2> blPositions;
	std::vector<Item*> itemsToRender;
	std::vector<int> locsWithMultiItems;
	for (size_t i = 0; i < s_ItemsOnTheGround.size(); i++)
	{
		Item* item = s_ItemsOnTheGround.at(i);
		if (item != nullptr)
		{
			item->SetRenderReguardless(showFullMap);
			IntVector2 blPos = item->GetBlTilePos();
			bool spotAlreadyTaken = false;
			int indexAt = -1;
			for (size_t j = 0; j < blPositions.size(); j++)
			{
				if (blPos == blPositions.at(j))
				{
					indexAt = j;
					spotAlreadyTaken = true;
					break;
				}
			}
			if (spotAlreadyTaken == false)
			{
				blPositions.push_back(blPos);
				itemsToRender.push_back(item);
			}
			else
			{
				bool found = false;
				for (size_t j = 0; j < locsWithMultiItems.size(); j++)
				{
					if (locsWithMultiItems.at(j) != indexAt)
					{
						found = true;
					}
				}
				if (found == false)
				{
					locsWithMultiItems.push_back(indexAt);
				}
			}
		}
	}

	for (size_t i = 0; i < itemsToRender.size(); i++)
	{
		bool found = false;
		for (size_t j = 0; j < locsWithMultiItems.size(); j++)
		{
			if ((int)i == locsWithMultiItems.at(j))
			{
				found = true;
				break;
			}
		}
		if (found == false)
		{
			itemsToRender.at(i)->Render(playingBackgroundColor);
		}
		else
		{
			bool render = itemsToRender.at(i)->GetRenderNow();
			Item::Render('*', playingBackgroundColor, Rgba(1.f, 1.f, 1.f, 1.f), itemsToRender.at(i)->GetBlTilePos(), render, 
				itemsToRender.at(i)->GetSpriteSheet());
		}
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------
//Container
void Item::FinalStep()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------
//Render And Update

const float Item::Update(const float& deltaSeconds)
{
	deltaSeconds;
	return 1.f;
}
void Item::UpdateStep()
{

}
void Item::Render(const Rgba& playingBackgroundColor) const
{
	Item::Render(m_char, playingBackgroundColor, m_color, m_collision.m_blTilePosition, (GetVisibleToPlayer() == true || m_renderRegardlessOfVisibility == true),
		m_spriteSheet);
}
void Item::Render(const char& car, const Rgba& playingBackgroundColor, const Rgba& color, const IntVector2& blPos, const bool& renderThis,
	SpriteSheet* spriteSheet)
{
	if (spriteSheet == nullptr || renderThis == false)
	{
		return;
	}
	Vector2 bl, blBack;
	Vector2 tr, trBack;
	std::vector<Vertex_PCT> vertices;
	AABB2 sprValBack = spriteSheet->GetTexCoordsForSpriteIndex(2);
	AABB2 sprVal = spriteSheet->GetTexCoordsForSpriteIndex(car);
	bl = Vector2(sprVal.mins.x, sprVal.maxs.y);
	tr = Vector2(sprVal.maxs.x, sprVal.mins.y);
	blBack = Vector2(sprValBack.mins.x, sprValBack.mins.y);
	trBack = Vector2(sprValBack.maxs.x, sprValBack.maxs.y);
	if (g_map != nullptr)
	{
		vertices = g_map->GetVerticesForPosition(blPos, playingBackgroundColor, blBack, trBack);
		std::vector<Vertex_PCT> foregroundVerts = g_map->GetVerticesForPosition(blPos, color, bl, tr);
		vertices.insert(vertices.end(), foregroundVerts.begin(), foregroundVerts.end());
	}
	if (vertices.size() > 0)
	{
		g_Renderer->DrawVertexArray(vertices, PRIMITIVE_QUADS, spriteSheet->getSpriteSheetTexture());
	}
}
void Item::WriteToXMLNode(XMLNode& node) const
{
	XMLNode item = node.addChild("ItemBlueprint");
	GameEntity::WriteToXMLNode(item);
	item.addAttribute("itemType", std::to_string(m_itemType).c_str());
	item.addAttribute("blessingType", std::to_string(m_blessingType).c_str());
	item.addAttribute("equipSlot", std::to_string(m_acceptableEquipmentSlotType).c_str());
	item.addAttribute("itemID", std::to_string(m_itemID).c_str());
	item.addAttribute("damage", (std::to_string(m_minDamage) + "," + std::to_string(m_minDamage)).c_str());
	item.addAttribute("damageOverTime", (std::to_string(m_minDamageOverTime) + "," + std::to_string(m_maxDamageOverTime)).c_str());
	item.addAttribute("statusTurns", std::to_string(m_turnsForStatusEffects).c_str());
	item.addAttribute("customEquipMessage", m_customEquipMessage.c_str());
	item.addAttribute("grantFlight", std::to_string(m_grantFlight).c_str());
}
void Item::WriteToXMLNode(XMLNode& node, const bool& equiped) const
{
	XMLNode item = node.addChild("ItemInInventory");
	item.addAttribute("equipped", (std::to_string(equiped)).c_str());
	WriteToXMLNode(item);
}
void Item::ReadFromXMLNode(GenerationProcessData* data)
{
	std::vector<StrXMLAttr> attrs = data->GetAttributes();
	bool makeThis = true;
	for (size_t i = 0; i < attrs.size(); i++)
	{
		StrXMLAttr attr = attrs.at(i);
		if (GameEntity::ReadFromXMLNodeCommonAttributes(this, attr, ENTITY_ITEM, makeThis))
		{
			continue;
		}
		else if (SimpleStrCmp(attr.name, "itemType"))
		{
			m_itemType = (ItemType)atoi(attr.val.c_str());
		}
		else if (SimpleStrCmp(attr.name, "blessingType"))
		{
			m_blessingType = (BlessingType)atoi(attr.val.c_str());
		}
		else if (SimpleStrCmp(attr.name, "equipSlot"))
		{
			m_acceptableEquipmentSlotType = (EquipmentSlotType)atoi(attr.val.c_str());
		}
		else if (SimpleStrCmp(attr.name, "itemID"))
		{
			m_itemID = atoi(attr.val.c_str());
			if (m_itemID >= s_ItemIndex)
			{
				s_ItemIndex = m_itemID + 1;
			}
		}
		else if (SimpleStrCmp(attr.name, "damage"))
		{
			std::string damage = ReplaceCharInString(attr.val, ',', ' ');
			damage = ReplaceCharInString(damage, '~', ' ');
			std::vector<std::string> damageVals = ParseString(damage);
			m_minDamage = atoi(damageVals.at(0).c_str());
			m_maxDamage = atoi(damageVals.at(1).c_str());
		}
		else if (SimpleStrCmp(attr.name, "damageOverTime"))
		{
			std::string damage = ReplaceCharInString(attr.val, ',', ' ');
			damage = ReplaceCharInString(damage, '~', ' ');
			std::vector<std::string> damageVals = ParseString(damage);
			m_minDamageOverTime = atoi(damageVals.at(0).c_str());
			m_maxDamageOverTime = atoi(damageVals.at(1).c_str());
		}
		else if (SimpleStrCmp(attr.name, "statusTurns"))
		{
			m_turnsForStatusEffects = atoi(attr.val.c_str());
		}
		else if (SimpleStrCmp(attr.name, "customEquipMessage"))
		{
			m_customEquipMessage = attr.val;
		}
		else if (SimpleStrCmp(attr.name, "grantFlight"))
		{
			m_grantFlight = atoi(attr.val.c_str()) == 1;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------
//Setters
void Item::SetGrantFlight(const bool& grantFlight)
{
	m_grantFlight = grantFlight;
}
void Item::SetCustomEquipMessage(const std::string& message)
{
	m_customEquipMessage = message;
}
void Item::SetLayerOfItemsWithin(const int& set)
{
	m_layerOfItems = set;
}
void Item::SetMinDamageFactor(const int& damage)
{
	m_minDamage = damage;
}
void Item::SetMaxDamageFactor(const int& damage)
{
	m_maxDamage = damage;
}
void Item::SetMinDamageOverTimeFactor(const int& damageOverTime)
{
	m_minDamageOverTime = damageOverTime;
}
void Item::SetMaxDamageOverTimeFactor(const int& damageOverTime)
{
	m_maxDamageOverTime = damageOverTime;
}
void Item::SetTurnsForStatusEffects(const int& turnsForStatusEffects)
{
	m_turnsForStatusEffects = turnsForStatusEffects;
}
void Item::SetItemType(const ItemType& itemType)
{
	m_itemType = itemType;
}
void Item::SetBlessingType(const BlessingType& blessingType)
{
	m_blessingType = blessingType;
}
void Item::SetAcceptableEquipmentSlotType(const EquipmentSlotType& acceptableEquipmentSlot)
{
	m_acceptableEquipmentSlotType = acceptableEquipmentSlot;
}
void Item::SetDestroyMe(const bool& destroyMe)
{
	m_destroyMe = destroyMe;
}
void Item::SetItemID(const int& id)
{
	m_itemID = id;
	if (m_itemID >= s_ItemIndex)
	{
		s_ItemIndex = m_itemID + 1;
	}
}


//--------------------------------------------------------------------------------------------------------------------------------------
//Getters
const bool Item::GetGrantFlight() const
{
	return m_grantFlight;
}
const std::string Item::GetCustomEquipMessage() const
{
	return m_customEquipMessage;
}
SpriteSheet* Item::GetSpriteSheet() const
{
	return m_spriteSheet;
}
const int Item::GetItemID() const
{
	return m_itemID;
}
const unsigned int Item::GetLayerOfItemsWithin() const
{
	return m_layerOfItems;
}
const int Item::GetMinDamageFactor() const
{
	return m_minDamage;
}
const int Item::GetMaxDamageFactor() const
{
	return m_maxDamage;
}
const int Item::GetRandomDamageFactor() const
{
	int diff = m_maxDamage - m_minDamage;
	if (diff == 0)
	{
		return m_minDamage;
	}
	int neg = diff / abs(diff);
	int ran = (rand() % abs(diff)) * neg;

	return m_minDamage + ran;
}
const int Item::GetMinDamageOverTimeFactor() const
{
	return m_minDamageOverTime;
}
const int Item::GetMaxDamageOverTimeFactor() const
{
	return m_maxDamageOverTime;
}
const int Item::GetRandomdamageOverTimeFactor() const
{
	int diff = m_maxDamageOverTime - m_minDamageOverTime;
	if (diff == 0)
	{
		return m_minDamageOverTime;
	}
	int neg = diff / abs(diff);
	int ran = (rand() % abs(diff)) * neg;

	return m_minDamageOverTime + ran;

}
const int Item::GetTurnsForStatusEffects() const
{
	return m_turnsForStatusEffects;
}

const ItemType Item::GetItemType() const
{
	return m_itemType;
}
const BlessingType Item::GetBlessingType() const
{
	return m_blessingType;
}
const EquipmentSlotType Item::GetAcceptableEquipmentSlotType() const
{
	return m_acceptableEquipmentSlotType;
}
const bool Item::GetDestroyMe() const 
{
	return m_destroyMe;
}
const StatusEffect Item::GetStatusEffect() const
{
	StatusEffect effect(m_name, m_turnsForStatusEffects, m_minDamageOverTime);
	return effect;
}