#pragma once

#ifndef BEHAVIOR_HPP
#define BEHAVIOR_HPP
#include <map>
#include <string>
#include "Game/GameEntity/Agent.hpp"
#include "Game/Generators/EnvironmentGenerationProcess.hpp"
#include "Game/GameEntity/CombatSystem.hpp"

class Behavior;
class BehaviorRegistrationHelper;

typedef Behavior* (BehaviorCreationFunc)(const XMLNode& node);
typedef Behavior* (BehaviorDefaultCreationFunc)();


class BehaviorRegistrationHelper
{
private:
	std::string name;
	BehaviorCreationFunc* beviorCreationFunc;
	BehaviorDefaultCreationFunc* defBehaviorCreationFunc;

public:
	BehaviorRegistrationHelper(const std::string& nam, BehaviorCreationFunc* bevCreationFunc, BehaviorDefaultCreationFunc*);

	static Behavior* CreateDefaultBehaviorByName(const std::string& name);
	static Behavior* CreateBehaviorByName(const std::string& name, const XMLNode& node);
	static const bool DetermineBehaviorExists(const std::string& name);
	static void ClearAllBehaviorRegistrations();
protected:
	static std::map<std::string, BehaviorRegistrationHelper>* s_behaviorRegistrations;
};

class Behavior
{
protected:
	bool m_canRun;
	std::string m_name;
	EnvironmentGenerationProcess* m_data;
	Agent* m_owner;
public:
	Behavior();
	Behavior(std::string name, const XMLNode& node);
	Behavior(const Behavior& bev);
	virtual ~Behavior();

	void SetAgent(Agent* owner);
	void SetName(std::string name) { m_name = name; };
	Agent* GetAgent() const;
	const std::string GetName() const { return m_name; };

	virtual void ClearData();

	const bool DoesPassChanceToRun() const;
	virtual Behavior* Clone() = 0;
	virtual const float CalcUtility() = 0;
	virtual void Run() = 0;
	virtual void ClearNecessaryDataForIfTargetsDead() = 0;
	virtual void WriteToXMLNode(XMLNode& node) = 0;
	virtual void ReadFromXMLNode(GenerationProcessData* data) = 0;

};
#endif