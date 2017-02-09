#pragma once

#ifndef USESIEGEWEAPON
#define USESIEGEWEAPON
#include "Game/GameEntity/AgentType/NPC/Behavior.hpp"

class UseSiegeWeapon : public Behavior
{
private:
public:
	UseSiegeWeapon();
	UseSiegeWeapon(const XMLNode& node);
	UseSiegeWeapon(const UseSiegeWeapon& other);
	~UseSiegeWeapon();


	virtual Behavior* Clone() override;
	virtual const float CalcUtility() override;
	virtual void Run() override;
	virtual void ClearNecessaryDataForIfTargetsDead() override;
	virtual void WriteToXMLNode(XMLNode& node) override;
	virtual void ReadFromXMLNode(GenerationProcessData* data) override;
};
#endif