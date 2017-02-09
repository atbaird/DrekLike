#include "Game/GameEntity/CombatSystem.hpp"

void CombatSystem::Attack(AttackData& data)
{
	switch (data.m_atkType)
	{
	case AttackType_Melee:
		MeleeAttack(data);
		break;
	default:
		break;
	}
}

void CombatSystem::MeleeAttack(AttackData& data)
{
	if ((data.instigator== nullptr || (data.instigator != nullptr && data.instigator->GetIsAlive() == false))
		|| (data.target == nullptr || (data.target != nullptr && data.target->GetIsAlive() == false)))
	{
		return;
	}

	IntVector2 blA = data.instigator->GetBlTilePos();
	IntVector2 trA = blA + data.instigator->GetSize() - IntVector2(1,1);
	IntVector2 blT = data.target->GetBlTilePos();
	IntVector2 trT = blT + data.target->GetSize() - IntVector2(1, 1);

	//does the attacker miss?
	int chanceToHit = (rand() % 100);
	float hit = (float)chanceToHit / 100.f;
	if (hit > data.accuracy)
	{
		data.damage = 0;
		data.didAttackHit = false;
		data.reason = Reason_Missed;
		data.target->AddIndRelScore(data.instigator->GetAgentID(), -1);

		if (Agent::DetermineActorInViewOfPlayer(data.target) == true || Agent::DetermineActorInViewOfPlayer(data.instigator) == true)
		{
			std::string attackerName = data.instigator->GetEntityName();
			std::string targetName = data.target->GetEntityName();
			std::string message;
			message = attackerName + " missed melee attack on ";
			message = message + targetName;
			s_messageBoxStrings.push_back(message);
		}
		return;
	}

	//Get base attack power.
	int minAtkPow = data.instigator->GetMinAtkPow();
	int maxAtkPow = data.instigator->GetMaxAtkPow();
	int difAtkPow = maxAtkPow - minAtkPow;
	int atkPow = minAtkPow;
	if (difAtkPow > 0)
	{
		int ran = rand() % (difAtkPow);
		atkPow += ran;
	}

	//Take Equipment into consideration
	int additionalAtk = data.instigator->GetRandomEquipmentAttackRating();
	int additionalDef = data.target->GetRandomEquipmentDefenseRating();
	atkPow += additionalAtk;
	atkPow -= additionalDef;
	if (atkPow < 0)
	{
		atkPow = 0;
	}

	//Begin attack
	int curHP = data.target->GetCurrentHealth();
	int prevHP = curHP;

	curHP -= atkPow;
	int diff = prevHP - curHP;
	std::string facName = data.target->GetFactionName();
	int atkFacID = data.target->GetFactionID();
	Faction* fact = Faction::CreateOrGetFactionOfName(facName);
	fact->AddRelationShipScore(atkFacID, -1);
	data.target->AddIndRelScore(data.instigator->GetAgentID(), -3);
	data.target->SetAttackedLastTurn(data.instigator);
	data.damage = diff;
	data.didAttackHit = true;
	data.reason = Reason_None;
	
	
	if (Agent::DetermineActorInViewOfPlayer(data.target) == true || Agent::DetermineActorInViewOfPlayer(data.instigator) == true)
	{
		std::string attackerName = data.instigator->GetEntityName();
		std::string targetName = data.target->GetEntityName();
		std::string message;
		message = attackerName + " did ";
		message = message + std::to_string(diff) + " damage to ";
		message = message + targetName;
		s_messageBoxStrings.push_back(message);
	}

	data.target->SetCurrentHealth(curHP);
	if (curHP <= 0)
	{
		data.target->SetIsAlive(false);
		fact->AddRelationShipScore(atkFacID, -5);
	}
	
}