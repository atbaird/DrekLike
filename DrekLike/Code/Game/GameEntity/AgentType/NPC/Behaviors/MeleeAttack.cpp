#include "Game/GameEntity/AgentType/NPC/Behaviors/MeleeAttack.hpp"

Behavior* DefaultCreateMeleeAttack()
{
	return new MeleeAttack();
}

Behavior* CreateMeleeAttackBehavior(const XMLNode& node)
{
	return new MeleeAttack(node);
}

const BehaviorRegistrationHelper MeleeAttack::s_MeleeAttackBehaviorRegistration("MeleeAttack", CreateMeleeAttackBehavior, DefaultCreateMeleeAttack);

MeleeAttack::MeleeAttack()
	:Behavior(),
	chanceToNotWantToFightIfWeakerThanTarget(15.f),
	chanceToFightIfTargetWeakerThanSelf(15.f)
{
	m_name = "MeleeAttack";
	ClearData();
}

MeleeAttack::MeleeAttack(const XMLNode& node)
	: Behavior("MeleeAttack", node),
	chanceToNotWantToFightIfWeakerThanTarget(15.f),
	chanceToFightIfTargetWeakerThanSelf(15.f)
{

	ClearData();
}
MeleeAttack::MeleeAttack(const MeleeAttack& other)
	: Behavior(other),
	chanceToNotWantToFightIfWeakerThanTarget(other.chanceToNotWantToFightIfWeakerThanTarget),
	chanceToFightIfTargetWeakerThanSelf(other.chanceToFightIfTargetWeakerThanSelf)
{

}
MeleeAttack::~MeleeAttack()
{
	Behavior::~Behavior();
}

Behavior* MeleeAttack::Clone()
{
	return new MeleeAttack(*this);
}
const float MeleeAttack::CalcUtility()
{
	if (m_owner != nullptr && m_owner->GetTargetOfHate() != nullptr)
	{
		Agent* targetOfHate = m_owner->GetTargetOfHate();
		IntVector2 htBL = targetOfHate->GetBlTilePos();
		IntVector2 owBL = m_owner->GetBlTilePos();

		IntVector2 diff = htBL - owBL;
		if (abs(diff.x) <= 1 && abs(diff.y) <= 1)
		{
			float owHealthPerc = (float)m_owner->GetCurrentHealth() / (float)m_owner->GetMaxHealth();
			float htHealthPerc = (float)targetOfHate->GetCurrentHealth() / (float)targetOfHate->GetMaxHealth();

			int friendliness = 0;
			int targetFactionID = targetOfHate->GetFactionID();
			std::vector<FactionRelationship>* factionInfo = m_owner->GetFactionRelationData();
			std::vector<AgentRelationship>* relData = m_owner->GetIndividualRelationshipData();

			for (size_t i = 0; i < factionInfo->size(); i++)
			{
				if (factionInfo->at(i).GetFactionID() == targetFactionID)
				{
					friendliness += factionInfo->at(i).GetFactionRelationship();
					break;
				}
			}
			for (size_t i = 0; i < relData->size(); i++)
			{
				if (relData->at(i).GetAgentID() == targetOfHate->GetAgentID())
				{
					friendliness += relData->at(i).GetRelationship();
				}
			}
			float chanceToAttack = 200.f;
			if (friendliness > 5)
			{
				return 0.0f;
			}
			else
			{
				friendliness -= 5;
				friendliness *= -1;
				chanceToAttack += (friendliness * 10.f);
			}


			std::vector<CombatDataOnOtherAgents>* dataCombat = m_owner->GetCombatData();
			for (size_t i = 0; i < dataCombat->size(); i++)
			{
				if (dataCombat->at(i).AgentID == targetOfHate->GetAgentID())
				{
					float usefulNessOfMelee = dataCombat->at(i).useMeleeAgainstChance;
					chanceToAttack += (usefulNessOfMelee * 5.f);
				}
			}

			if (htHealthPerc > owHealthPerc)
			{
				chanceToAttack -= (htHealthPerc - owHealthPerc) * chanceToNotWantToFightIfWeakerThanTarget;
			}
			else if (htHealthPerc < owHealthPerc)
			{
				chanceToAttack += (owHealthPerc - htHealthPerc) * chanceToFightIfTargetWeakerThanSelf;
			}


			return chanceToAttack;
		}
	}

	return 0.0f;
}
void MeleeAttack::Run()
{
	if (m_owner != nullptr && m_owner->GetTargetOfHate() != nullptr)
	{
		AttackData data;
		data.instigator = m_owner;
		data.target = m_owner->GetTargetOfHate();
		data.m_atkType = AttackType_Melee;
		data.accuracy = m_owner->GetAccuracy();
		CombatSystem::Attack(data);
	}
}
void MeleeAttack::ClearNecessaryDataForIfTargetsDead()
{

}
void MeleeAttack::WriteToXMLNode(XMLNode& node)
{
	node.addChild("MeleeAttack");
}
void MeleeAttack::ReadFromXMLNode(GenerationProcessData* data)
{
	data;
}