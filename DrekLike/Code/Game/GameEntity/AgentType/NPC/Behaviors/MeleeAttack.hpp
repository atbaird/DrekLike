#pragma once

#ifndef MELEEATTACK_HPP
#define MELEEATTACK_HPP
#include "Game/GameEntity/AgentType/NPC/Behavior.hpp"

static Behavior* DefaultCreateMeleeAttack();
static Behavior* CreateMeleeAttackBehavior(const XMLNode& node);

class MeleeAttack : public Behavior
{
protected:
	static const BehaviorRegistrationHelper s_MeleeAttackBehaviorRegistration;
	float chanceToNotWantToFightIfWeakerThanTarget;
	float chanceToFightIfTargetWeakerThanSelf;
public:
	MeleeAttack();
	MeleeAttack(const XMLNode& node);
	MeleeAttack(const MeleeAttack& other);
	~MeleeAttack();

	virtual Behavior* Clone() override;
	virtual const float CalcUtility() override;
	virtual void Run() override;
	virtual void ClearNecessaryDataForIfTargetsDead() override;
	virtual void WriteToXMLNode(XMLNode& node) override;
	virtual void ReadFromXMLNode(GenerationProcessData* data) override;
};
#endif