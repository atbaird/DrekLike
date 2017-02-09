#pragma once

#ifndef PATROL_HPP
#define PATROL_HPP
#include "Game/GameEntity/AgentType/NPC/Behavior.hpp"
#include "Game/Pathfinding/AStarAlgo.hpp"

static Behavior* DefaultCreatePatrolBehavior();
static Behavior* CreatePatrolBehavior(const XMLNode& node);

class Patrol : public Behavior
{
protected:
	static const BehaviorRegistrationHelper s_PatrolBehaviorRegistration;
	bool m_patrolSet;
	float m_patrolRadiusFromOriginalPosition;
	int m_minNumOfPatrolPoints;
	int m_maxNumOfPatrolPoints;
	int m_currentPatrolInd;
	AStarAlgo m_pathFinder;
	std::vector<IntVector2> m_patrolPoints;
public:
	Patrol();
	Patrol(const XMLNode& node);
	Patrol(const Patrol& other);
	~Patrol();

	void GeneratePatrol();
	void VerifyPositionIsNotCurrentlyOrGoingToBeOccupied(IntVector2& pos, const IntVector2& orPos, const IntVector2& dirMoving);

	virtual Behavior* Clone() override;
	virtual const float CalcUtility() override;
	virtual void Run() override;
	virtual void ClearNecessaryDataForIfTargetsDead() override;
	virtual void WriteToXMLNode(XMLNode& node) override;
	virtual void ReadFromXMLNode(GenerationProcessData* data) override;

private:
	const bool TryPath(const IntVector2& start, const IntVector2& end, IntVector2& wanted);
	const bool VerifyPositionNotTaken(const IntVector2& position);
};

#endif