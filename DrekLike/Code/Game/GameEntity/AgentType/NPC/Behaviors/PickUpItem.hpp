#pragma once

#ifndef PICKUPITEM_HPP
#define PICKUPITEM_HPP
#include "Game/GameEntity/AgentType/NPC/Behavior.hpp"
#include "Game/Pathfinding/AStarAlgo.hpp"

static Behavior* DefaultCreatePickUpItemBehavior();
static Behavior* CreatePickUpItemBehavior(const XMLNode& node);

class PickUpItem : public Behavior
{
private:
	static const BehaviorRegistrationHelper s_PickUpItemBehaviorRegistration;
	float m_interestInItems;
	float m_distanceOutToCare;
	float m_minInterest;
	int m_nearestItemIdx;
	AStarAlgo m_algo;
public:
	PickUpItem();
	PickUpItem(const XMLNode& node);
	PickUpItem(const PickUpItem& other);
	~PickUpItem();


	virtual Behavior* Clone() override;
	virtual const float CalcUtility() override;
	virtual void Run() override;
	virtual void ClearNecessaryDataForIfTargetsDead() override;
	virtual void WriteToXMLNode(XMLNode& node) override;
	virtual void ReadFromXMLNode(GenerationProcessData* data) override;
};
#endif