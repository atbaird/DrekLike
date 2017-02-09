#include "Game/GameEntity/Agent.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Items/ItemFactory.hpp"
#include <string>



AgentRelationship::AgentRelationship()
	: m_agentID(0),
	m_relationship(0)
{
}
AgentRelationship::AgentRelationship(const int& ageID, const int& rel)
	: m_agentID(ageID),
	m_relationship(rel)
{
}


//Constructors

const int Agent::s_BlockedByAir_Mask = 1 << BLOCKED_BY_AIR;
const int Agent::s_BlockedByWalls_Mask = 1 << BLOCKED_BY_WALLS;
const int Agent::s_BlockedByFire_Mask = 1 << BLOCKED_BY_FIRE;
const int Agent::s_BlockedByWater_Mask = 1 << BLOCKED_BY_WATER;
const int Agent::s_SlowedByWalls_Mask = 1 << SLOWED_BY_WALLS;
const int Agent::s_SlowedByFire_Mask = 1 << SLOWED_BY_FIRE;
const int Agent::s_SlowedByWater_Mask = 1 << SLOWED_BY_WATER;
const int Agent::s_HurtByFire_Mask = 1 << HURT_BY_FIRE;
const int Agent::s_Flying_Mask = 1 << FLYING;

const int Agent::s_hate = -20;
const int Agent::s_dislike = -10;
const int Agent::s_neutral = 0;
const int Agent::s_like = 10;
const int Agent::s_friendly = 20;
std::vector<Agent*> Agent::s_AgentsVisibleToPlayer;
std::vector<Agent*> Agent::s_Agents;
int Agent::currentAvailableAgentID = 0;

Agent::Agent(const EntityType& type, const IntVector2& blTilePos, const IntVector2& entitySize)
	: GameEntity(type, blTilePos, entitySize),
	m_sightRange(10.f),
	m_moved(false),
	m_alive(true),
	m_agentID(currentAvailableAgentID),
	m_minAtkPow(2),
	m_maxAtkPow(2),
	m_accuracy(0.68f),
	m_faction(nullptr),
	m_MovementMask(0),
	m_OriginalMovementMask(0),
	m_targetOfFriendship(nullptr),
	m_targetOfHatred(nullptr),
	m_lastTargetOfHatred(nullptr),
	m_personWhoAttackedLast(nullptr),
	m_attackedLastTurn(false),
	m_idOfFriendship(-1),
	m_idLastTargetOfHatred(-1),
	m_idPersonWhoAttackedLast(-1),
	m_idTargetOfHatred(-1)
{
	currentAvailableAgentID++;
}

Agent::Agent(const Agent& other)
	: GameEntity(other),
	m_sightRange(other.m_sightRange),
	m_stepCost(other.m_stepCost),
	m_moved(other.m_moved),
	m_alive(other.m_alive),
	m_MovementMask(other.m_MovementMask),
	m_OriginalMovementMask(other.m_OriginalMovementMask),
	m_agentID(currentAvailableAgentID),
	m_minAtkPow(other.m_minAtkPow),
	m_maxAtkPow(other.m_maxAtkPow),
	m_faction(other.m_faction),
	m_accuracy(other.m_accuracy),
	m_targetOfFriendship(nullptr),
	m_targetOfHatred(nullptr),
	m_lastTargetOfHatred(nullptr),
	m_personWhoAttackedLast(nullptr),
	m_attackedLastTurn(false),
	m_inventory(20)
{
	currentlyAvailableGameEntityID++;
}
Agent::~Agent()
{
	std::vector<Item*> items = m_inventory.GetItems();
	for (size_t i = 0; i < items.size(); i++)
	{
		Item* item = items.at(i);
		item->SetBLPosition(m_collision.m_blTilePosition);
		Item::s_ItemsOnTheGround.push_back(item);
	}
}

void Agent::AddAgentToAllAgents(Agent* age)
{
	if (age != nullptr)
	{
		s_Agents.push_back(age);
	}
}
void Agent::ClearAllAgents()
{
	for(size_t i = 1; i < s_Agents.size(); i++)
	{
		if (s_Agents.at(i) != nullptr)
		{
			delete s_Agents[i];
			s_Agents[i] = nullptr;
		}
	}
	s_Agents.clear();
}
void Agent::ClearAgentNonPlayerPointer(Agent* agent)
{
	for (size_t i = 0; i < s_Agents.size(); i++)
	{
		if (s_Agents.at(i) != nullptr && s_Agents.at(i) == agent)
		{
			delete s_Agents[i];
			s_Agents[i] = nullptr;
			s_Agents.erase(s_Agents.begin() + i);
		}
	}
}
void Agent::ClearPlayerPointer(Agent* agent)
{
	for (size_t i = 0; i < s_Agents.size(); i++)
	{
		if (s_Agents.at(i) != nullptr && s_Agents.at(i) == agent)
		{
			s_Agents.erase(s_Agents.begin() + i);
		}
	}
}
void Agent::SetAllAgentsInvisible()
{
	for (size_t i = 0; i < s_Agents.size(); i++)
	{
		if (s_Agents.at(i) != nullptr)
		{
			s_Agents.at(i)->SetVisibleToPlayer(false);
		}
	}
}
void Agent::FinalPassBeforeRemoveAgents()
{
	for (size_t i = 0; i < s_Agents.size(); i++)
	{
		if (s_Agents.at(i) != nullptr)
		{
			s_Agents.at(i)->FinalPass();
		}
	}
}
void Agent::ReconnectPointersToOtherAgents()
{
	for (size_t i = 0; i < s_Agents.size(); i++)
	{
		Agent* age = s_Agents.at(i);
		if (age != nullptr)
		{
			int targOfHate = age->GetTargetOfHateID();
			int lastTargOfHate = age->GetLastTargetOfHateID();
			int personAttackedLast = age->GetLastAgentWhoAttackedID();
			int targOfFriend = age->GetTargetOfFriendshipID();
			if ((targOfHate == lastTargOfHate) && (targOfHate == personAttackedLast)
				&& (targOfHate == targOfFriend) && (targOfHate == -1))
			{
				continue;
			}
			for (size_t j = 0; j < s_Agents.size(); j++)
			{
				Agent* ageCompare = s_Agents.at(i);
				if (ageCompare != nullptr && ageCompare != age)
				{
					int ageCompareID = ageCompare->GetAgentID();
					if (ageCompareID == targOfHate)
					{
						age->SetTargetOfHatred(ageCompare);
					}
					if (ageCompareID == lastTargOfHate)
					{
						age->SetLastTargetOfHatred(ageCompare);
					}
					if (ageCompareID == personAttackedLast)
					{
						age->SetAttackedLastTurn(ageCompare);
					}
					if (ageCompareID == targOfFriend)
					{
						age->SetTargetOfFriendship(ageCompare);
					}
				}
			}
		}
	}
}

const float Agent::Update(const float& deltaSeconds)
{
	deltaSeconds;
	return 1.0f;
}
void Agent::UpdateStep()
{

}
void Agent::Render(const Rgba& playingBackgroundColor) const
{
	GameEntity::Render(playingBackgroundColor);
}


//-----------------------------------------------------------------------------------------------------------------
//Getters
const bool Agent::IsTileOccupied(const IntVector2& tileCoord)
{
	for (size_t i = 0; i < s_Agents.size(); i++)
	{
		if ((s_Agents.at(i)->GetBlTilePos() == tileCoord && s_Agents.at(i)->GetWantedBLTilePos() == s_Agents.at(i)->GetBlTilePos())
			|| (s_Agents.at(i)->GetWantedBLTilePos() == tileCoord))
		{
			return true;
		}
	}
	return false;
}
const int Agent::GenMaskFromMovementProperties(std::vector<MovementProperty> props)
{
	int mas = 0;

	for (size_t i = 0; i < props.size(); i++)
	{
		MovementProperty prop = props.at(i);
		mas |= (1 << prop);
	}

	return mas;
}
const bool Agent::GetBlockedByWalls() const
{
	return GetBlockedByWalls(m_MovementMask);
}
const bool Agent::GetBlockedByFire() const
{
	return GetBlockedByFire(m_MovementMask);
}
const bool Agent::GetBlockedByWater() const
{
	return GetBlockedByWater(m_MovementMask);
}
const bool Agent::GetSlowedByWalls() const
{
	return GetSlowedByWalls(m_MovementMask);
}
const bool Agent::GetSlowedByFire() const
{
	return GetSlowedByFire(m_MovementMask);
}
const bool Agent::GetSlowedByWater() const
{
	return GetSlowedByWater(m_MovementMask);
}
const bool Agent::GetHurtByFire() const
{
	return GetHurtByFire(m_MovementMask);
}
const bool Agent::GetFlying() const
{
	return GetFlying(m_MovementMask);
}

const bool Agent::GetBlockedByAir(const int& mask)
{
	return (mask & s_BlockedByAir_Mask) != 0;
}
const bool Agent::GetBlockedByWalls(const int& mask)
{
	return (mask & s_BlockedByWalls_Mask) != 0;
}
const bool Agent::GetBlockedByFire(const int& mask)
{
	return (mask & s_BlockedByFire_Mask) != 0;
}
const bool Agent::GetBlockedByWater(const int& mask)
{
	return (mask & s_BlockedByWater_Mask) != 0;
}
const bool Agent::GetSlowedByWalls(const int& mask)
{
	return (mask & s_SlowedByWalls_Mask) != 0;
}
const bool Agent::GetSlowedByFire(const int& mask)
{
	return (mask & s_SlowedByFire_Mask) != 0;
}
const bool Agent::GetSlowedByWater(const int& mask)
{
	return (mask & s_SlowedByWater_Mask) != 0;
}
const bool Agent::GetHurtByFire(const int& mask)
{
	return (mask & s_HurtByFire_Mask) != 0;
}
const bool Agent::GetFlying(const int& mask)
{
	return (mask & s_Flying_Mask) != 0;
}

std::vector<Feature*> Agent::GetVisibleFeatures() const
{
	return m_visibleFeatures;
}
const int Agent::GetMask() const
{
	return m_MovementMask;
}
const int Agent::GetRandomEquipmentAttackRating() const
{
	return m_inventory.GetRandOffensiveDamageRating();
}
const int Agent::GetRandomEquipmentDefenseRating() const
{
	return m_inventory.GetRandDefensiveProtectionRating();
}
const bool Agent::GetInventoryIsFull() const
{
	return m_inventory.IsInventoryFull();
}
std::vector<Item*> Agent::GetVisibleItems() const
{
	return m_visibleItems;
}
const bool Agent::IsReadyToUpdate() const
{
	return true;
}
void Agent::FinalPass()
{
	for (size_t i = 0; i < m_statusEffectsCurrentlyEffectedBy.size(); i++)
	{
		if (m_statusEffectsCurrentlyEffectedBy.at(i).GetRidOfMe() == true)
		{
			if (m_visibleToPlayer)
			{
				std::string message = m_statusEffectsCurrentlyEffectedBy.at(i).GetName() + " effect on " + m_name + " has worn off.";
				s_messageBoxStrings.push_back(message);
			}

			m_statusEffectsCurrentlyEffectedBy.erase(m_statusEffectsCurrentlyEffectedBy.begin() + i);
			i--;
		}
	}
	if (m_lastTargetOfHatred != nullptr && m_lastTargetOfHatred->GetIsAlive() == false)
	{
		m_lastTargetOfHatred = nullptr;
	}
	if (m_targetOfHatred != nullptr && m_targetOfHatred->GetIsAlive() == false)
	{
		m_targetOfHatred = nullptr;
	}
	if (m_targetOfFriendship != nullptr && m_targetOfFriendship->GetIsAlive() == false)
	{
		m_targetOfFriendship = nullptr;
	}
	if (m_personWhoAttackedLast != nullptr && m_personWhoAttackedLast->GetIsAlive() == false)
	{
		m_personWhoAttackedLast = nullptr;
	}
}
void Agent::CycleThroughStatusEffects()
{
	bool visibleToPlayer = false;
	for (size_t i = 0; i < s_AgentsVisibleToPlayer.size(); i++)
	{
		if (this == s_AgentsVisibleToPlayer.at(i))
		{
			visibleToPlayer = true;
			break;
		}
	}
	if (m_visibleToPlayer == true)
	{
		visibleToPlayer = m_visibleToPlayer;
	}

	for (size_t i = 0; i < m_statusEffectsCurrentlyEffectedBy.size(); i++)
	{
		if (m_statusEffectsCurrentlyEffectedBy.at(i).GetRidOfMe() == false)
		{
			m_statusEffectsCurrentlyEffectedBy.at(i).TurnPassed();
			int damageOverTime = m_statusEffectsCurrentlyEffectedBy.at(i).GetDamageOverTime();
			int newHealth = m_currentHealth - damageOverTime;
			if (damageOverTime > 0 && visibleToPlayer)
			{
				s_messageBoxStrings.push_back(m_name + " was poisoned for " + std::to_string(abs(damageOverTime)));
			}
			else if (damageOverTime < 0 && visibleToPlayer)
			{
				s_messageBoxStrings.push_back(m_name + " was healed for " + std::to_string(abs(damageOverTime)));
			}
			if (newHealth < 0)
			{
				newHealth = 0;
			}
			else if (newHealth > m_maxHealth)
			{
				newHealth = m_maxHealth;
			}

			if (m_visibleToPlayer)
			{
				if (newHealth > m_currentHealth)
				{
					std::string message = m_name + " was healed by " + m_statusEffectsCurrentlyEffectedBy.at(i).GetName();
					message = message + " for " + std::to_string(m_statusEffectsCurrentlyEffectedBy.at(i).GetDamageOverTime());
					s_messageBoxStrings.push_back(message);
				}
				else if (newHealth < m_currentHealth)
				{
					std::string message = m_name + " was poisoned by " + m_statusEffectsCurrentlyEffectedBy.at(i).GetName();
					message = message + " for " + std::to_string(m_statusEffectsCurrentlyEffectedBy.at(i).GetDamageOverTime());
					s_messageBoxStrings.push_back(message);
				}
			}

			m_currentHealth = newHealth;

			if (m_currentHealth <= 0)
			{
				m_alive = false;
			}
		}
		else
		{
			m_statusEffectsCurrentlyEffectedBy.erase(m_statusEffectsCurrentlyEffectedBy.begin() + i);
			i--;
		}
	}
}
const AttackData Agent::CheckTileSlowAgent()
{
	AttackData data;
	data.reason = Reason_None;
	bool attacking = false;
	for (size_t i = 0; i < Agent::s_Agents.size(); i++)
	{
		Agent* age = s_Agents.at(i);
		if (age != this)
		{
			IntVector2 bl = age->GetBlTilePos();
			IntVector2 tr = bl + age->GetSize() - IntVector2(1, 1);
			if (m_wantedNewBLPosition >= bl && m_wantedNewBLPosition <= tr)
			{
				attacking = true;
				break;
			}
		}
	}

	if (attacking == false)
	{
		std::map<Tile_Coord, Tile*>::const_iterator it = g_map->GetMapTiles()->find(m_wantedNewBLPosition);
		if (it != g_map->GetMapTiles()->end() && it->second != nullptr)
		{
			TileType type = it->second->GetTileType();
			std::map<TileType, TileDefinition>::const_iterator itd = g_TileDefinitions.find(type);
			if (itd != g_TileDefinitions.end())
			{
				bool flying = GetFlying();
				TileDefinition def = itd->second;
				if (flying == false && def.GetIsFire() && GetSlowedByFire() == true)
				{
					data.reason = Reason_WeakToFire;
				}
				else if (flying == false && def.GetIsLiquid() == true && GetSlowedByWater() == true)
				{
					data.reason = Reason_WeakToWater;
				}
				else if (def.GetIsWall() == true && GetSlowedByWalls() == true)
				{
					data.reason = Reason_WeakToPhysical;
				}
			}
		}
	}
	return data;
}
const AttackData Agent::CheckTileHurtAgent()
{
	AttackData data;
	data.reason = Reason_None;
	bool attacking = false;
	Agent* attackingThis = nullptr;
	for (size_t i = 0; i < Agent::s_Agents.size(); i++)
	{
		Agent* age = s_Agents.at(i);
		if (age != this)
		{
			IntVector2 bl = age->GetBlTilePos();
			IntVector2 tr = bl + age->GetSize() - IntVector2(1, 1);
			if (m_wantedNewBLPosition >= bl && m_wantedNewBLPosition <= tr)
			{
				attacking = true;
				attackingThis = age;
				break;
			}
		}
	}
	if (attacking == false)
	{
		std::map<Tile_Coord, Tile*>::const_iterator it = g_map->GetMapTiles()->find(m_wantedNewBLPosition);
		if (it != g_map->GetMapTiles()->end() && it->second != nullptr)
		{
			TileType type = it->second->GetTileType();
			std::map<TileType, TileDefinition>::const_iterator itd = g_TileDefinitions.find(type);
			if (itd != g_TileDefinitions.end())
			{
				bool flying = GetFlying();
				TileDefinition def = itd->second;
				if (flying == false && def.GetIsFire() && GetHurtByFire() == true)
				{
					data.reason = Reason_WeakToFire;
					return data;
				}
			}
		}
	}
	data.reason = Reason_None;
	return data;
}

const bool Agent::DrinkRandomPotionFromInv()
{
	bool drank = false;
	std::vector<Item*> potions = m_inventory.GetItemsByType(ItemType_Potion);
	if (potions.size() > 0)
	{
		drank = true;
		int ran = rand() % potions.size();
		Item* potion = potions.at(ran);
		s_messageBoxStrings.push_back(m_name + " drank " + potion->GetEntityName());
		StatusEffect effect = potion->GetStatusEffect();
		m_dirToMove = Direction_None;
		if (effect.GetRidOfMe() == false)
		{
			m_statusEffectsCurrentlyEffectedBy.push_back(effect);
		}
		int damage = potion->GetRandomDamageFactor();
		int newHp = m_currentHealth - damage;
		int healthDiff = m_currentHealth - newHp;

		//Adjust health.
		if (newHp <= 0)
		{
			healthDiff = m_currentHealth;
			m_currentHealth = 0;
			m_alive = false;
		}
		else if (newHp >= m_maxHealth)
		{
			healthDiff = m_currentHealth - m_maxHealth;
			m_currentHealth = m_maxHealth;
		}
		else
		{
			m_currentHealth = newHp;
		}

		if (damage < 0 && m_visibleToPlayer)
		{
			s_messageBoxStrings.push_back(m_name + " was instantly healed for " + std::to_string(abs(healthDiff)));
		}
		else if (damage > 0 && m_visibleToPlayer)
		{
			s_messageBoxStrings.push_back(m_name + " was instantly damaged for " + std::to_string(abs(healthDiff)));
		}
		m_inventory.DestroyItemInInventory(potion);
	}
	else
	{
		if (m_visibleToPlayer)
		{
			s_messageBoxStrings.push_back(m_name + " has no potions to drink.");
		}
	}
	return drank;
}
const bool Agent::TryPickUpItemBelowMe()
{
	bool pickedSomethingUp = false;
	bool displayedMessage = false;
	IntVector2 pos = m_collision.m_blTilePosition;
	for (size_t i = 0; i < Item::s_ItemsOnTheGround.size(); i++)
	{
		IntVector2 itemPos = Item::s_ItemsOnTheGround.at(i)->GetBlTilePos();
		if (itemPos == pos)
		{
			if (m_inventory.IsInventoryFull() == false)
			{
				pickedSomethingUp = true;
				displayedMessage = true;
				Item* item = Item::s_ItemsOnTheGround.at(i);
				Item::RemoveItemFromGround(item);
				//std::string message = m_name + " picked up " + item->GetEntityName();
				m_inventory.AddItem(item, true, m_name);

				ItemType type = item->GetItemType();
				if (type == ItemType_Armor || type == ItemType_Weapon)
				{
					int itemIdx = m_inventory.GetLastItemAddedIndex();
					bool successful = m_inventory.EquipItemFromInventory(itemIdx, true, m_name);
					if (successful)
					{
						bool grantFlight = m_inventory.GetCanFlyNow() || Agent::GetFlying(m_OriginalMovementMask);
						SetCanFly(grantFlight);
					}
				}
			}
			else
			{
				if (m_visibleToPlayer)
				{
					displayedMessage = true;
					std::string message = m_name + "\'s inventory is full.";
					s_messageBoxStrings.push_back(message);
				}
			}
			break;
		}
	}

	if (displayedMessage == false && pickedSomethingUp == false && m_visibleToPlayer)
	{
		std::string message = "There are no items here.";
		s_messageBoxStrings.push_back(message);
	}
	return pickedSomethingUp;
}
const bool Agent::TryInteractWithFeature()
{
	std::vector<Tile_Coord> tilesAround = g_map->GetNeighborTileCoordsAll(m_collision.m_blTilePosition);
	return false;
}
const bool Agent::IsThereAFeatureAroundThis() const
{
	bool foundOne = false;
	for (size_t i = 0; i < Feature::s_FeaturesOnMap.size(); i++)
	{
		Feature* feat = Feature::s_FeaturesOnMap.at(i);
		if (feat != nullptr)
		{
			IntVector2 bl = feat->GetBlTilePos();
			IntVector2 diff = m_collision.m_blTilePosition - bl;
			if (abs(diff.x) <= 1 && abs(diff.y) <= 1)
			{
				foundOne = true;
			}
		}
	}
	return foundOne;
}
const bool Agent::ToggleFeaturesOnOrOffThatAreAroundThis() const
{
	bool toggledOne = false;
	for (size_t i = 0; i < Feature::s_FeaturesOnMap.size(); i++)
	{
		Feature* feat = Feature::s_FeaturesOnMap.at(i);
		if (feat != nullptr)
		{
			IntVector2 bl = feat->GetBlTilePos();
			IntVector2 diff = m_collision.m_blTilePosition - bl;
			if (abs(diff.x) <= 1 && abs(diff.y) <= 1)
			{
				if (diff.x == 0 && diff.y == 0)
				{
					bool active = feat->GetIsActive();
					bool solid = false;
					if (active)
					{
						solid = feat->GetSolidWhenInactive();
					}
					else
					{
						solid = feat->GetSolidWhenActive();
					}
					if (solid == false)
					{
						feat->EntityToggleActive(m_name, m_visibleToPlayer);
						toggledOne = true;
					}
				}
				else
				{
					bool active = feat->GetIsActive();
					bool solid = false;
					if (active)
					{
						solid = feat->GetSolidWhenInactive();
					}
					else
					{
						solid = feat->GetSolidWhenActive();
					}
					if (IsTileOccupied(bl) == false || solid == false)
					{
						feat->EntityToggleActive(m_name, m_visibleToPlayer);
						toggledOne = true;
					}
				}
			}
		}
	}
	return toggledOne;
}
void Agent::WriteToXMLNode(XMLNode& node) const
{
	GameEntity::WriteToXMLNode(node);
	node.addAttribute("movementMask", std::to_string(m_MovementMask).c_str());
	node.addAttribute("agentID", std::to_string(m_minAtkPow).c_str());
	node.addAttribute("minAtkPow", std::to_string(m_minAtkPow).c_str());
	node.addAttribute("maxAtkPow", std::to_string(m_maxAtkPow).c_str());
	node.addAttribute("sightRange", std::to_string(m_sightRange).c_str());
	node.addAttribute("stepCost", std::to_string(m_stepCost).c_str());
	node.addAttribute("m_accuracy", std::to_string(m_accuracy).c_str());
	node.addAttribute("moved", std::to_string(m_moved).c_str());
	node.addAttribute("alive", std::to_string(m_alive).c_str());
	node.addAttribute("attackedLastTurn", std::to_string(m_attackedLastTurn).c_str());
	node.addAttribute("originalMovementMask", std::to_string(m_OriginalMovementMask).c_str());
	if (m_faction != nullptr)
	{
		node.addAttribute("faction", (m_faction->GetFactionName().c_str()));
	}
	if (m_targetOfFriendship != nullptr)
	{
		node.addAttribute("targetOfFriendship", std::to_string(m_targetOfFriendship->GetAgentID()).c_str());
	}
	if (m_lastTargetOfHatred != nullptr)
	{
		node.addAttribute("lastTargetOfHatred", std::to_string(m_lastTargetOfHatred->GetAgentID()).c_str());
	}
	if (m_personWhoAttackedLast != nullptr)
	{
		node.addAttribute("personWhoAttackedLast", std::to_string(m_personWhoAttackedLast->GetAgentID()).c_str());
	}
	if (m_targetOfHatred != nullptr)
	{
		node.addAttribute("targetOfHatred", std::to_string(m_targetOfHatred->GetAgentID()).c_str());
	}

	m_inventory.WriteToXMLNode(node);
	XMLNode agentRels = node.addChild("AgentRelationships");
	for (size_t i = 0; i < m_ageRelations.size(); i++)
	{
		XMLNode ageRel = agentRels.addChild("AgentRelationship");
		ageRel.addAttribute("AgentID", std::to_string(m_ageRelations.at(i).GetAgentID()).c_str());
		ageRel.addAttribute("Relationship", std::to_string(m_ageRelations.at(i).GetRelationship()).c_str());
	}
	XMLNode factionRels = node.addChild("FactionRelationships");
	for (size_t i = 0; i < m_factRelations.size(); i++)
	{
		XMLNode facRel = factionRels.addChild("FactionRelationship");
		facRel.addAttribute("FactionID", std::to_string(m_factRelations.at(i).GetFactionID()).c_str());
		facRel.addAttribute("Relationship", std::to_string(m_factRelations.at(i).GetFactionRelationship()).c_str());
	}
	XMLNode CombatData = node.addChild("CombatData");
	for (size_t i = 0; i < m_combatDataOnOtherAgents.size(); i++)
	{
		XMLNode combatData = CombatData.addChild("DataOnAgent");
		combatData.addAttribute("AgentID", std::to_string(m_combatDataOnOtherAgents.at(i).AgentID).c_str());
		combatData.addAttribute("UseMeleeAgainst", std::to_string(m_combatDataOnOtherAgents.at(i).useMeleeAgainstChance).c_str());
	}
	XMLNode StatusEffects = node.addChild("StatusEffects");
	for (size_t i = 0; i < m_statusEffectsCurrentlyEffectedBy.size(); i++)
	{
		m_statusEffectsCurrentlyEffectedBy.at(i).WriteToXMLNode(StatusEffects);
	}
}
const bool Agent::ReadFromXMLNodeCommonAttributes(Agent* common, const StrXMLAttr& attr, const EntityType& expectedID, bool& makeThis)
{
	if (GameEntity::ReadFromXMLNodeCommonAttributes(common, attr, expectedID, makeThis))
	{
		return true;
	}
	else if (SimpleStrCmp(attr.name, "movementMask"))
	{
		common->SetMovementMask(atoi(attr.val.c_str()));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "agentID"))
	{
		common->SetAgentID(atoi(attr.val.c_str()));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "minAtkPow"))
	{
		common->SetMinAtkPow(atoi(attr.val.c_str()));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "maxAtkPow"))
	{
		common->SetMaxAtkPow(atoi(attr.val.c_str()));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "sightRange"))
	{
		common->SetSightRadius((float)atof(attr.val.c_str()));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "stepCost"))
	{
		common->SetStepCost((float)atof(attr.val.c_str()));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "m_accuracy"))
	{
		common->SetAccuracy((float)atof(attr.val.c_str()));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "moved"))
	{
		common->SetMoved(SimpleStrCmp(attr.val, "1"));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "alive"))
	{
		common->SetIsAlive(SimpleStrCmp(attr.val, "1"));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "attackedLastTurn"))
	{
		common->SetAttackedLastTurnBool(SimpleStrCmp(attr.val, "1"));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "factionID"))
	{
		common->SetFaction(Faction::CreateOrGetFactionOfName(attr.val));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "targetOfFriendship"))
	{
		common->SetTargetOfFriendshipID(atoi(attr.val.c_str()));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "lastTargetOfHatred"))
	{
		common->SetLastTargetOfHatredID(atoi(attr.val.c_str()));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "personWhoAttackedLast"))
	{
		common->SetAgentWhoAttackedLastID(atoi(attr.val.c_str()));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "targetOfHatred"))
	{
		common->SetLastTargetOfHatredID(atoi(attr.val.c_str()));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "originalMovemnetMask"))
	{
		common->SetOriginalMovementMask(atoi(attr.val.c_str()));
	}
	return false;
}
const bool Agent::ReadFromXMLNodeCommonChildren(Agent* common, const GenerationProcessData* data, bool& makeThis)
{
	makeThis;
	if (SimpleStrCmp(data->GetName().name, "AgentRelationships"))
	{
		std::vector<GenerationProcessData*> children = data->GetChildren();
		for (size_t i = 0; i < children.size(); i++)
		{
			GenerationProcessData* child = children.at(i);
			if (child != nullptr)
			{
				StrXMLAttr agentID = child->GetAttributeByName("AgentID");
				StrXMLAttr rel = child->GetAttributeByName("Relationship");
				common->AddIndRelScore(atoi(agentID.val.c_str()), atoi(rel.val.c_str()));
			}
		}
		return true;
	}
	else if (SimpleStrCmp(data->GetName().name, "FactionRelationships"))
	{
		std::vector<GenerationProcessData*> children = data->GetChildren();
		for (size_t i = 0; i < children.size(); i++)
		{
			GenerationProcessData* child = children.at(i);
			if (child != nullptr)
			{
				int factionID = atoi(child->GetAttributeByNameString("FactionID").c_str());
				int rel = atoi(child->GetAttributeByNameString("Relationship").c_str());
				common->AddFactRelScore(factionID, rel);
			}
		}
		return true;
	}
	else if (SimpleStrCmp(data->GetName().name, "CombatData"))
	{
		std::vector<GenerationProcessData*> children = data->GetChildren();
		for (size_t i = 0; i < children.size(); i++)
		{
			GenerationProcessData* child = children.at(i);
			if (child != nullptr)
			{
				int agentID = atoi(child->GetAttributeByNameString("AgentID").c_str());
				float useMeleeAgainst = (float)atof(child->GetAttributeByNameString("UseMeleeAgainst").c_str());
				CombatDataOnOtherAgents cData;
				cData.AgentID = agentID;
				cData.useMeleeAgainstChance = useMeleeAgainst;
				common->AddCombatDataScore(cData);
			}
		}
		return true;
	}
	else if (SimpleStrCmp(data->GetName().name, "StatusEffects"))
	{
		std::vector<GenerationProcessData*> children = data->GetChildren();
		for (size_t i = 0; i < children.size(); i++)
		{
			GenerationProcessData* child = children.at(i);
			if (child != nullptr)
			{
				StatusEffect effect;
				effect.ReadFromXMLNode(child);
				common->AddStatusEffect(effect);
			}
		}
		return true;
	}
	else if (SimpleStrCmp(data->GetName().name, "Inventory"))
	{
		std::vector<GenerationProcessData*> children = data->GetChildren();
		for (size_t i = 0; i < children.size(); i++)
		{
			GenerationProcessData* child = children.at(i);
			if (child != nullptr)
			{
				std::string equippedStr = child->GetAttributeByNameString("equipped");
				GenerationProcessData* itemD = child->GetChildByName("ItemInInventory");
				if (itemD != nullptr)
				{
					Item* item = new Item();
					item->ReadFromXMLNode(itemD);
					Item::s_AllItems.push_back(item);
					bool equipped = atoi(equippedStr.c_str()) == 1;
					common->AddItemToInventory(item, equipped);
				}
			}
		}
		return true;
	}
	return false;
}

//!---------------------------------------------------------------------
//Setters
void Agent::CopyMaskToOriginal()
{
	m_OriginalMovementMask = m_MovementMask;
}
void Agent::SetOriginalMovementMask(const int& mask)
{
	m_OriginalMovementMask = mask;
}
void Agent::SetAgentID(const int& agentID)
{
	m_agentID = agentID;
	if (m_agentID >= currentAvailableAgentID)
	{
		currentAvailableAgentID = m_agentID + 1;
	}
}
void Agent::SetVisibleFeatures(const std::vector<Feature*> features)
{
	m_visibleFeatures = features;
}
void Agent::SetMovementMask(const int& mask)
{
	m_MovementMask = mask;
}
const bool Agent::TryPickUpItem(Item* item, const bool& forcePickUp)
{
	bool invFull = m_inventory.IsInventoryFull();
	if (item == nullptr || invFull == true)
	{
		return false;
	}
	if (m_collision.m_blTilePosition != item->GetBlTilePos() && forcePickUp == false)
	{
		return false;
	}

	if (m_visibleToPlayer == true)
	{
		m_inventory.AddItem(item, true, m_name);
	}
	else
	{
		m_inventory.AddItem(item, false, m_name);
	}
	ItemType type = item->GetItemType();
	if (type == ItemType_Armor || type == ItemType_Weapon)
	{
		int index = m_inventory.GetLastItemAddedIndex();
		EquipmentSlotType eType = item->GetAcceptableEquipmentSlotType();
		Item* curE = m_inventory.GetItemEquippedAtIndex(eType);
		if (curE != nullptr)
		{
			int max = item->GetMaxDamageFactor();
			int max2 = curE->GetMaxDamageFactor();
			if (max > max2)
			{
				if (m_visibleToPlayer == true)
				{
					m_inventory.EquipItemFromInventory(index, true, m_name);
				}
				else
				{
					m_inventory.EquipItemFromInventory(index, false, m_name);
				}
			}
		}
		else
		{
			if (m_visibleToPlayer == true)
			{
				m_inventory.EquipItemFromInventory(index, true, m_name);
			}
			else
			{
				m_inventory.EquipItemFromInventory(index, false, m_name);
			}
		}
	}


	return true;
}
void Agent::SetItemsVisible(const std::vector<Item*> visibleItems)
{
	m_visibleItems = visibleItems;
}
void Agent::AddStatusEffect(const StatusEffect& status)
{
	if (status.GetRidOfMe() == true)
	{
		return;
	}
	int index = -1;
	for (size_t i = 0; i < m_statusEffectsCurrentlyEffectedBy.size(); i++)
	{
		if (status == m_statusEffectsCurrentlyEffectedBy.at(i))
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		m_statusEffectsCurrentlyEffectedBy.push_back(status);
	}
	else
	{
		m_statusEffectsCurrentlyEffectedBy[index] = status;
	}
}
void Agent::SetMoved(const bool& moved) 
{
	m_moved = moved; 
}
const void Agent::SetSightRadius(const float& dist) 
{ 
	m_sightRange = dist; 
}
void Agent::SetIsAlive(const bool& alive) 
{ 
	m_alive = alive; 
}
void Agent::SetStepCost(const float& stepCost) 
{ 
	m_stepCost = stepCost; 
}
void Agent::SetVisibleAgents(std::vector<Agent*> visibleAgents) 
{ 
	m_visibleAgents = visibleAgents; 
}
const void Agent::SetListActorsVisibleToPlayer(std::vector<Agent*> agents) 
{ 
	Agent::s_AgentsVisibleToPlayer = agents; 
}
void Agent::SetFaction(Faction* fact)
{
	m_faction = nullptr;
	m_faction = fact;
}
void Agent::SetAttackedLastTurn(Agent* attack)
{
	if (attack != nullptr)
	{
		m_attackedLastTurn = true;
		m_personWhoAttackedLast = nullptr;
		m_personWhoAttackedLast = attack;
	}
}
void Agent::SetAttackedLastTurnBool(const bool& atked)
{
	m_attackedLastTurn = atked;
}
void Agent::SetTargetOfHatred(Agent* target)
{
	m_targetOfHatred = nullptr;
	m_targetOfHatred = target;
}
void Agent::SetTargetOfFriendship(Agent* target)
{
	m_targetOfFriendship = nullptr;
	m_targetOfFriendship = target;
}
void Agent::SetLastTargetOfHatred(Agent* age)
{
	m_lastTargetOfHatred = nullptr;
	m_lastTargetOfHatred = age;
}

void Agent::SetMinAtkPow(const int& pow)
{
	m_minAtkPow = pow; 
}
void Agent::SetMaxAtkPow(const int& pow)
{
	m_maxAtkPow = pow;
}
void Agent::SetAccuracy(const float& accuracy)
{ 
	m_accuracy = accuracy; 
}
void Agent::AddIndRelScore(const int& agentID, const int& relPointsToAdd)
{
	for (size_t i = 0; i <  m_ageRelations.size(); i++)
	{
		if (m_ageRelations.at(i).GetAgentID() == agentID)
		{
			m_ageRelations.at(i).SetRelationship(m_ageRelations.at(i).GetRelationship() + relPointsToAdd);
			return;
		}
	}
	AgentRelationship rel = AgentRelationship();
	rel.SetAgentID(agentID);
	rel.SetRelationship(relPointsToAdd);
	m_ageRelations.push_back(rel);
}
void Agent::AddFactRelScore(const int& factionID, const int& relPointsToAdd)
{
	for (size_t i = 0; i < m_factRelations.size(); i++)
	{
		if (m_factRelations.at(i).GetFactionID() == factionID)
		{
			m_factRelations.at(i).SetFactionRelationship(m_factRelations.at(i).GetFactionRelationship() + relPointsToAdd);
			return;
		}
	}
	FactionRelationship rel = FactionRelationship(factionID, relPointsToAdd);
	m_factRelations.push_back(rel);
}
void Agent::AddCombatDataScore(CombatDataOnOtherAgents data)
{
	int ageID = data.AgentID;
	for (size_t i = 0; i < m_combatDataOnOtherAgents.size(); i++)
	{
		if (m_combatDataOnOtherAgents.at(i).AgentID == ageID)
		{
			m_combatDataOnOtherAgents.at(i).useMeleeAgainstChance += data.useMeleeAgainstChance;
			return;
		}
	}
	m_combatDataOnOtherAgents.push_back(data);
}

const unsigned int Agent::GetNumberOfActors()
{
	return s_Agents.size();
}
Agent* Agent::GetActorAtIndex(const int& index)
{
	if (index < 0 || (size_t)index >= s_Agents.size())
	{
		return nullptr;
	}
	return s_Agents.at(index);
}
std::vector<Agent*> Agent::GetListActorsVisibleToPlayer() 
{
	return Agent::s_AgentsVisibleToPlayer; 
}
std::vector<CombatDataOnOtherAgents>* Agent::GetCombatData()
{
	return &m_combatDataOnOtherAgents;
}
std::vector<FactionRelationship>* Agent::GetFactionRelationData()
{
	return &m_factRelations;
}
std::vector<AgentRelationship>* Agent::GetIndividualRelationshipData()
{
	return &m_ageRelations;
}

const bool Agent::DetermineActorInViewOfPlayer(Agent* age)
{
	for (size_t i = 0; i < s_AgentsVisibleToPlayer.size(); i++)
	{
		Agent* ageTest = s_AgentsVisibleToPlayer.at(i);
		if (ageTest == age)
		{
			return true;
		}
	}
	return false;
}



const int Agent::EvaluateBaseHateLikeScale(Agent* originator, Agent* target)
{
	Faction* orFact = originator->GetHomeFaction();
	Faction* taFact = target->GetHomeFaction();
	if (orFact != nullptr)
	{
		return orFact->EvaluateHatredScale(taFact);
	}
	return 0;
}
const std::string Agent::GetFactionName() const 
{
	if (m_faction != nullptr)
	{
		return m_faction->GetFactionName();
	}
	return "";
}
const int Agent::GetFactionID() const 
{ 
	if (m_faction != nullptr)
	{
		return m_faction->GetFactionID();
	}
	return -1;
}
const void Agent::RemoveDeadNPCs()
{
	for (size_t i = 1; i < Agent::s_Agents.size(); i++)
	{
		if (Agent::s_Agents.at(i) != nullptr && Agent::s_Agents.at(i)->GetIsAlive() == false)
		{
			delete Agent::s_Agents[i];
			Agent::s_Agents[i] = nullptr;
			Agent::s_Agents.erase(Agent::s_Agents.begin() + i);
			i--;
		}
	}
}
const bool Agent::AddItemToInventory(Item* item, const bool& tryAddToEquip)
{
	bool addedSuccessfully = m_inventory.AddItem(item, m_visibleToPlayer, m_name);
	if (addedSuccessfully && tryAddToEquip)
	{
		//THIS IS IF STATEMENT ONLY FOR USE WITH NPC TEMPLATE
		int index = m_inventory.GetLastItemAddedIndex();
		m_inventory.EquipItemFromInventory(index, false);
	}
	return addedSuccessfully;
}
Item* Agent::DropItemOfIndex(const int& index)
{
	if (index < 0 || index >= GetMaxItemsCanHold())
	{
		return nullptr;
	}
	Item* item = m_inventory.DropItemAtIndex(index, m_visibleToPlayer, m_name);
	return item;
}
void Agent::SetLastTargetOfHatredID(const int& id)
{
	m_idLastTargetOfHatred = id;
}
void Agent::SetTargetOfHatredID(const int& id)
{
	m_idTargetOfHatred = id;
}
void Agent::SetTargetOfFriendshipID(const int& id)
{
	m_idOfFriendship = id;
}
void Agent::SetAgentWhoAttackedLastID(const int& id)
{
	m_idPersonWhoAttackedLast = id;
}

void Agent::SetCanFly(const bool& fly)
{
	m_MovementMask &= ~s_Flying_Mask;

	if (fly)
	{
		m_MovementMask |= s_Flying_Mask;
	}

	//	m_lightingAndFlags &= ~LIGHTING_MASK; //11110000
	//m_lightingAndFlags |= (unsigned char)(lightValue0To15);
}



//!-------------------------------------------------------
//Getters
const float Agent::GetSightRadius() const 
{
	return m_sightRange; 
}
const bool Agent::GetMoved() const 
{
	return m_moved; 
}
const float Agent::GetStepCost() const
{
	return m_stepCost; 
}
const int Agent::GetAgentID() const
{
	return m_agentID;
}
std::vector<Agent*> Agent::GetVisibleAgents() const
{
	return m_visibleAgents;
}

const int Agent::GetMinAtkPow() const
{
	return m_minAtkPow;
}
const int Agent::GetMaxAtkPow() const
{
	return m_maxAtkPow;
}
Faction* Agent::GetHomeFaction() const
{
	return m_faction;
}
Agent* Agent::GetWhoLastAttacked() const
{
	return m_personWhoAttackedLast;
}
Agent* Agent::GetTargetOfHate() const
{
	return m_targetOfHatred;
}
Agent* Agent::GetTargetOfFriendship() const
{
	return m_targetOfFriendship;
}
const float Agent::GetAccuracy() const
{
	return m_accuracy;
}

const int Agent::GetItemsHolding() const
{
	return m_inventory.GetNumberOfItemsHeld();
}
const int Agent::GetMaxItemsCanHold() const
{
	return m_inventory.GetItemLimit();
}
const int Agent::GetTargetOfHateID() const
{
	return m_idTargetOfHatred;
}
const int Agent::GetLastTargetOfHateID() const
{
	return m_idLastTargetOfHatred;
}
const int Agent::GetTargetOfFriendshipID() const
{
	return m_idOfFriendship;
}
const int Agent::GetLastAgentWhoAttackedID() const
{
	return m_idPersonWhoAttackedLast;
}



//--------------------------------------------------------------------------------------------------

void Agent::CheckPickUpItems(Agent* age)
{
	IntVector2 bl = age->GetBlTilePos();
	IntVector2 tr = bl + age->GetSize() - IntVector2(1, 1);
	bool carryingTooMuch = false;
	int itemLimit = age->GetMaxItemsCanHold();
	if (age->GetItemsHolding() >= itemLimit)
	{
		carryingTooMuch = true;
	}

	std::vector<size_t> indexOfItemsHere;

	for (size_t i = 0; i < Item::s_ItemsOnTheGround.size(); i++)
	{
		Item* item = Item::s_ItemsOnTheGround.at(i);
		if (item != nullptr)
		{
			IntVector2 itemPos = item->GetBlTilePos();
			if (itemPos.x >= bl.x && itemPos.x <= tr.x
				&& itemPos.y >= bl.y && itemPos.y <= tr.y)
			{
				indexOfItemsHere.push_back(i);
			}
		}
	}

	if (age->GetEntityType() == ENTITY_PLAYER && carryingTooMuch == true)
	{
		if (indexOfItemsHere.size() < 5)
		{
			std::string message = "There is ";
			for (size_t i = 0; i < indexOfItemsHere.size(); i++)
			{
				size_t index = indexOfItemsHere.at(i);
				Item* item = Item::s_ItemsOnTheGround.at(index);
				if (item != nullptr)
				{
					message += item->GetEntityName();
					if (i < indexOfItemsHere.size() - 1)
					{
						message += ' ';
					}
				}
			}
			message += " here.";
			s_messageBoxStrings.push_back(message);
		}
		else
		{
			s_messageBoxStrings.push_back("There are a lot of items here.");
		}
		return;
	}

	if (carryingTooMuch == false)
	{
		for (size_t i = 0; i < indexOfItemsHere.size(); i++)
		{
			size_t index = indexOfItemsHere.at(i);
			Item* item = Item::s_ItemsOnTheGround.at(index);
			if (item != nullptr)
			{
				if (age->GetItemsHolding() >= itemLimit && age->GetEntityType() == ENTITY_PLAYER)
				{
					s_messageBoxStrings.push_back("There is still more items, but you are carrying too much to take the rest.");
					break;
				}
				else
				{
					Item::s_ItemsOnTheGround.erase(Item::s_ItemsOnTheGround.begin() + i);
					i--;
					age->AddItemToInventory(item);
					if (age->GetEntityType() == ENTITY_PLAYER)
					{
						s_messageBoxStrings.push_back("Picked up " + item->GetEntityName());
					}
				}
			}
		}
	}
}
void Agent::DropItem(Agent* age, const int& index)
{
	Item* item = age->DropItemOfIndex(index);

	if (item != nullptr)
	{
		Item::s_ItemsOnTheGround.push_back(item);
		if (age->GetEntityType() == ENTITY_PLAYER)
		{
			s_messageBoxStrings.push_back("Dropped " + item->GetEntityName());
		}
	}
}