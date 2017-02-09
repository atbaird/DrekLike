#pragma once

#ifndef FLEE_HPP
#define FLEE_HPP
#include "Game/GameEntity/AgentType/NPC/Behavior.hpp"

static Behavior* DefaultCreateFleeBehavior();
static Behavior* CreateFleeBehavior(const XMLNode& node);

class Flee : public Behavior
{
protected:
	static const BehaviorRegistrationHelper s_FleeBehaviorRegistration;
	float m_healthBelowThisPercentage;
	float m_runMultiplier;
	float m_percentageBeforeScream;
	float m_chanceToRunRegardless;
	int m_baseHatredToCountAsHostile;
public:
	Flee();
	Flee(const XMLNode& node);
	Flee(const Flee& other);
	~Flee();

	virtual Behavior* Clone() override;
	virtual const float CalcUtility() override;
	virtual void Run() override;
	virtual void ClearNecessaryDataForIfTargetsDead() override;
	virtual void WriteToXMLNode(XMLNode& node) override;
	virtual void ReadFromXMLNode(GenerationProcessData* data) override;

};
#endif