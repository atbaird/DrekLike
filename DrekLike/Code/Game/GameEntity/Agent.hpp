#pragma once
#include "Game/GameEntity/GameEntity.hpp"
#include "Game/GameEntity/AgentType/Titles/AgentTitle.hpp"
#include "Game/GameEntity/AgentType/Factions/Faction.hpp"
#include "Game/Items/Inventory.hpp"
#include "Game/Features/Feature.hpp"
#include <vector>

#ifndef AGENT_HPP
#define AGENT_HPP

enum Reason
{
	Reason_INVALID = -1,
	Reason_None,
	Reason_Missed,
	Reason_WeakToFire,
	Reason_WeakToIce,
	Reason_WeakToWater,
	Reason_WeakToLightning,
	Reason_WeakToMagic,
	Reason_WeakToPhysical,
	Reason_StrongToFire,
	Reason_StrongToIce,
	Reason_StrongToWater,
	Reason_StrongToLightning,
	Reason_StrongToMagic,
	Reason_StrongToPhysical,
	NUM_OF_REASONS
};
enum AttackType
{
	AttackType_INVALID = -1,
	AttackType_Melee,
	NUM_OF_ATTACK_TYPES
};

class Agent;
struct AttackData
{
	int damage; // support a range; such as 2 to 5
	Agent* target = nullptr;
	Agent* instigator = nullptr;
	bool instigatorIsZaWorldo = false;
	//Item* instigatorWeapon;
	float accuracy;
	bool didAttackHit;
	int damageDealt;
	AttackType m_atkType;
	Reason reason;
};

struct CombatDataOnOtherAgents
{
	int AgentID;
	float useMeleeAgainstChance;
};

class AgentRelationship
{
protected:
	int m_agentID;
	int m_relationship;
public:
	AgentRelationship();
	AgentRelationship(const int& ageID, const int& rel);

	void SetAgentID(const int& ageID) { m_agentID = ageID; };
	void SetRelationship(const int& rel) { m_relationship = rel; };

	const int GetAgentID() const { return m_agentID; };
	const int GetRelationship() const { return m_relationship; };
};
enum MovementProperty
{
	BLOCKED_BY_AIR,
	BLOCKED_BY_WALLS,
	BLOCKED_BY_FIRE,
	BLOCKED_BY_WATER,
	SLOWED_BY_AIR,
	SLOWED_BY_WALLS,
	SLOWED_BY_FIRE,
	SLOWED_BY_WATER,
	HURT_BY_FIRE,
	FLYING,
	NUM_OF_MOVEMENTPROPERTIES
};

class Agent : public GameEntity
{
public:
	static const int s_hate;
	static const int s_dislike;
	static const int s_neutral;
	static const int s_like;
	static const int s_friendly;
	static const int s_BlockedByAir_Mask;
	static const int s_BlockedByWalls_Mask;
	static const int s_BlockedByFire_Mask;
	static const int s_BlockedByWater_Mask;
	static const int s_SlowedByWalls_Mask;
	static const int s_SlowedByFire_Mask;
	static const int s_SlowedByWater_Mask;
	static const int s_HurtByFire_Mask;
	static const int s_Flying_Mask;
protected:

	//-----------
	//Only to be used on load; don't bother saving.
	int m_idOfFriendship;
	int m_idLastTargetOfHatred;
	int m_idPersonWhoAttackedLast;
	int m_idTargetOfHatred;
	//-----------------
	//Inventor* m_inventory;
	//Item* m_equipmentSlots[NUM_EQUIPMENT_SLOTS]
	Inventory m_inventory;

	Agent* m_targetOfFriendship;
	Agent* m_lastTargetOfHatred;
	Agent* m_personWhoAttackedLast;
	Agent* m_targetOfHatred;
	static int currentAvailableAgentID;
	static std::vector<Agent*> s_AgentsVisibleToPlayer;

	int m_MovementMask;
	int m_OriginalMovementMask;
	int m_agentID;
	int m_minAtkPow;
	int m_maxAtkPow;
	float m_sightRange;
	float m_stepCost; //aka, speed
	float m_accuracy;
	bool m_moved;
	bool m_alive;
	bool m_attackedLastTurn;
	std::vector<Agent*> m_visibleAgents;
	std::vector<Item*> m_visibleItems;
	std::vector<Feature*> m_visibleFeatures;
	std::vector<AgentRelationship> m_ageRelations;
	std::vector<FactionRelationship> m_factRelations;
	std::vector<CombatDataOnOtherAgents> m_combatDataOnOtherAgents;
	std::vector<StatusEffect> m_statusEffectsCurrentlyEffectedBy;
	Faction* m_faction;
private:
public:
	static std::vector<Agent*> s_Agents;
	//Constructors
	Agent(const EntityType& type, const IntVector2& blTilePos = IntVector2(0, 0), const IntVector2& entitySize = IntVector2(1, 1));
	Agent(const Agent& other);
	virtual ~Agent() override; //~GameEntity();

	static void AddAgentToAllAgents(Agent* age);
	static void ClearAllAgents();
	static void ClearAgentNonPlayerPointer(Agent* agent);
	static void ClearPlayerPointer(Agent* agent);
	static void SetAllAgentsInvisible();
	static void FinalPassBeforeRemoveAgents();
	static void ReconnectPointersToOtherAgents();

	//Updates and Render
	virtual const float Update(const float& deltaSeconds) override; //GameEntity::Update(const float& deltaSeconds)
	virtual void UpdateStep() override; //GameEntity::UpdateStep();
	virtual void Render(const Rgba& playingBackgroundColor) const override; //GameEntity::Render() const;
	virtual void FinalPass();
	virtual void CycleThroughStatusEffects();
	virtual const AttackData CheckTileSlowAgent();
	virtual const AttackData CheckTileHurtAgent();
	virtual const bool DrinkRandomPotionFromInv();
	virtual const bool TryPickUpItemBelowMe();
	virtual const bool TryInteractWithFeature();
	virtual const bool IsThereAFeatureAroundThis() const;
	virtual const bool ToggleFeaturesOnOrOffThatAreAroundThis() const;
	virtual void WriteToXMLNode(XMLNode& node) const override;
	static const bool ReadFromXMLNodeCommonAttributes(Agent* common, const StrXMLAttr& attr, const EntityType& expectedID, bool& makeThis);
	static const bool ReadFromXMLNodeCommonChildren(Agent* common, const GenerationProcessData* data, bool& makeThis);

	//Setters
	virtual void CopyMaskToOriginal();
	virtual void SetOriginalMovementMask(const int& mask);
	virtual void SetAgentID(const int& agentID);
	virtual void SetVisibleFeatures(const std::vector<Feature*> features);
	virtual void SetMovementMask(const int& mask);
	virtual const bool TryPickUpItem(Item* item, const bool& forcePickUp = false);
	virtual void SetItemsVisible(const std::vector<Item*> visibleItems);
	virtual void AddStatusEffect(const StatusEffect& status);
	virtual void SetMoved(const bool& moved);
	virtual const void SetSightRadius(const float& dist);
	virtual void SetIsAlive(const bool& alive);
	virtual void SetStepCost(const float& stepCost);
	virtual void SetVisibleAgents(std::vector<Agent*> visibleAgents);
	static const void SetListActorsVisibleToPlayer(std::vector<Agent*> agents);
	virtual void SetMinAtkPow(const int& pow);
	virtual void SetMaxAtkPow(const int& pow);
	virtual void SetFaction(Faction* fact);
	virtual void SetAttackedLastTurn(Agent* attack);
	virtual void SetAttackedLastTurnBool(const bool& atked);
	virtual void SetTargetOfHatred(Agent* target);
	virtual void SetTargetOfFriendship(Agent* target);
	virtual void SetLastTargetOfHatred(Agent* age);
	virtual void AddIndRelScore(const int& agentID, const int& relPointsToAdd);
	virtual void AddFactRelScore(const int& factionID, const int& relPointsToAdd);
	virtual void AddCombatDataScore(CombatDataOnOtherAgents data);
	virtual void SetAccuracy(const float& accuracy);
	virtual const bool AddItemToInventory(Item* item, const bool& tryAddToEquip = false);
	virtual Item* DropItemOfIndex(const int& index);
	virtual void SetLastTargetOfHatredID(const int& id);
	virtual void SetTargetOfHatredID(const int& id);
	virtual void SetTargetOfFriendshipID(const int& id);
	virtual void SetAgentWhoAttackedLastID(const int& id);
	virtual void SetCanFly(const bool& fly);

	//Getters
	static const bool IsTileOccupied(const IntVector2& tileCoord);
	static const int GenMaskFromMovementProperties(std::vector<MovementProperty> props);
	const bool GetBlockedByWalls() const;
	const bool GetBlockedByFire() const;
	const bool GetBlockedByWater() const;
	const bool GetSlowedByWalls() const;
	const bool GetSlowedByFire() const;
	const bool GetSlowedByWater() const;
	const bool GetHurtByFire() const;
	const bool GetFlying() const;
	static const bool GetBlockedByAir(const int& mask);
	static const bool GetBlockedByWalls(const int& mask);
	static const bool GetBlockedByFire(const int& mask);
	static const bool GetBlockedByWater(const int& mask);
	static const bool GetSlowedByWalls(const int& mask);
	static const bool GetSlowedByFire(const int& mask);
	static const bool GetSlowedByWater(const int& mask);
	static const bool GetHurtByFire(const int& mask);
	static const bool GetFlying(const int& mask);
	virtual std::vector<Feature*> GetVisibleFeatures() const;
	virtual const int GetMask() const;
	virtual const int GetRandomEquipmentAttackRating() const;
	virtual const int GetRandomEquipmentDefenseRating() const;
	virtual const bool GetInventoryIsFull() const;
	virtual std::vector<Item*> GetVisibleItems() const;
	virtual const bool IsReadyToUpdate() const;
	virtual const bool GetIsAlive() const { return m_alive; };
	static const unsigned int GetNumberOfActors();
	static Agent* GetActorAtIndex(const int& index);
	virtual const float GetSightRadius() const;
	virtual const bool GetMoved() const;
	virtual const float GetStepCost() const;
	virtual std::vector<Agent*> GetVisibleAgents() const;
	virtual const int GetAgentID() const;
	static std::vector<Agent*> GetListActorsVisibleToPlayer();
	virtual std::vector<CombatDataOnOtherAgents>* GetCombatData();
	virtual std::vector<FactionRelationship>* GetFactionRelationData();
	virtual std::vector<AgentRelationship>* GetIndividualRelationshipData();
	virtual const int GetMinAtkPow() const;
	virtual const int GetMaxAtkPow() const;
	virtual Faction* GetHomeFaction() const;
	virtual const std::string GetFactionName() const;
	virtual const int GetFactionID() const;
	virtual Agent* GetWhoLastAttacked() const;
	virtual Agent* GetTargetOfHate() const;
	virtual Agent* GetTargetOfFriendship() const;
	virtual const float GetAccuracy() const;
	virtual const int GetItemsHolding() const;
	virtual const int GetMaxItemsCanHold() const;
	virtual const int GetTargetOfHateID() const;
	virtual const int GetLastTargetOfHateID() const;
	virtual const int GetTargetOfFriendshipID() const;
	virtual const int GetLastAgentWhoAttackedID() const;


	static const int EvaluateBaseHateLikeScale(Agent* originator, Agent* target);
	static const bool DetermineActorInViewOfPlayer(Agent* age);
	static const void RemoveDeadNPCs();
	static void CheckPickUpItems(Agent* age);
	static void DropItem(Agent* age, const int& index);
};



#endif