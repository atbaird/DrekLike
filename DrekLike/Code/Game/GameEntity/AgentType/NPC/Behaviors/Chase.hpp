#pragma once

#ifndef CHASE_HPP
#define CHASE_HPP
#include "Game/GameEntity/AgentType/NPC/Behavior.hpp"
#include "Game/Pathfinding/AStarAlgo.hpp"

static Behavior* CreateChaseBehavior(const XMLNode& node);
static Behavior* DefaultCreatChaseBehavior();

class Chase : public Behavior
{
protected:
	static const BehaviorRegistrationHelper s_ChaseBehaviorRegistration;
	int turnsBeenChasingFor;
	int numTurnsToChaseBeforeGiveUp;
	bool targetOutOfView;
	float distanceTest;
	std::string chaseTargetName;
	Agent* m_targetToChase;
	AStarAlgo m_algo;
private:
	const bool VerifyPositionIsNotCurrentlyOrGoingToBeOccupied(IntVector2& pos, const IntVector2& orPos, const IntVector2& dirMoving, bool& isTargetPos);
	const bool VerifyPositionNotTaken(const IntVector2& position, bool& isTargetPos);
public:
	//Constructors
	Chase();
	Chase(const XMLNode& node);
	Chase(const Chase& other);
	~Chase();

	virtual Behavior* Clone() override;
	virtual const float CalcUtility() override;
	virtual void Run() override;
	virtual void ClearNecessaryDataForIfTargetsDead() override;
	virtual void WriteToXMLNode(XMLNode& node) override;
	virtual void ReadFromXMLNode(GenerationProcessData* data) override;

};
#endif