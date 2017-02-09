#pragma once

#ifndef NPC_HPP
#define NPC_HPP
#include "Game/GameEntity/Agent.hpp"
#include "Game/GameEntity/AgentType/NPC/Behavior.hpp"
#include <vector>


class NPC : public Agent
{
protected:
	std::vector<Behavior*> m_behaviors;

	int m_baseHatredScale;

public:
	//Constructors
	NPC(const IntVector2& blTilePos = IntVector2(0, 0), const unsigned char& car = '-', const Rgba& color = Rgba(1.f,1.f,1.f,1.f), const IntVector2& entitySize = IntVector2(1, 1),
		const float& stepCost = 1.f);
	NPC(const NPC& other);
	~NPC();

	const bool DoesHaveThisBehavior(const std::string& name);
	const bool AddBehavior(Behavior* bev);
	const bool RemoveBehavior(const std::string& name);

	static NPC* CreateNPC();

	virtual const float Update(const float& deltaSeconds) override; //GameEntity::Update(const float& deltaSeconds)
	virtual void UpdateStep() override; //GameEntity::UpdateStep();
	virtual void Render(const Rgba& playingBackgroundColor) const override; //GameEntity::Render() const;
	virtual void FinalPass() override;
	virtual void WriteToXMLNode(XMLNode& node) const override;
	virtual const bool ReadFromXMLNode(GenerationProcessData* data);

	//Setters
	virtual void SetBaseHatredScale(const int& scale);

	//Getters
	virtual const int GetBaseHatredScale() const;
};
#endif