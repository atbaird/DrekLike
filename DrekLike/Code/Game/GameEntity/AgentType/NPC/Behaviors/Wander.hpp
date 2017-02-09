#pragma once

#ifndef WANDER_HPP
#define WANDER_HPP
#include "Game/GameEntity/AgentType/NPC/Behavior.hpp"

static Behavior* DefaultCreateWanderBehavior();
static Behavior* CreateWanderBehavior(const XMLNode& node);


class Wander : public Behavior
{
private:
	float percentChanceToRest;
	float percentChanceToWalk;
	int amountToHealFromResting;
	static const BehaviorRegistrationHelper s_WanderBehaviorRegistration;
public:
	Wander();
	Wander(const XMLNode& node);
	Wander(const Wander& other);
	~Wander();

	virtual Behavior* Clone() override;
	virtual const float CalcUtility() override;
	virtual void Run() override;
	virtual void ClearNecessaryDataForIfTargetsDead() override;
	virtual void WriteToXMLNode(XMLNode& node) override;
	virtual void ReadFromXMLNode(GenerationProcessData* data) override;
};
#endif