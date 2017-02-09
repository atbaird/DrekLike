#pragma once

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Game/GameEntity/Agent.hpp"

class Player : public Agent
{
private:
	static const int s_totalValidMoveDirs = 9;
	unsigned char m_keyValsPerDir[s_totalValidMoveDirs];
	bool m_dirKeysHeldDown[s_totalValidMoveDirs];
	bool m_godModeEnabled;
	bool m_itemMenuActive;
	bool m_commaIsDown;
	bool m_qIsDown;
	bool m_aIsDown;
protected:
public:
	//Constructors
	Player();
	virtual ~Player() override; //Agent::~Agent()

	//Updates and Render
	virtual const float Update(const float& deltaSeconds) override; //Agent::Update(const float& deltaSeconds)
	virtual void UpdateStep() override; //Agent::UpdateStep();
	virtual void Render(const Rgba& playingBackgroundColor) const override; //Agent::Render() const;
	virtual void RenderInventory(const Vector3& startPos, Font* font = nullptr, const float& fontSize = 1.f);
	virtual const bool IsReadyToUpdate() const override;
	virtual void WriteToXMLNode(XMLNode& node) const override;
	virtual void ReadFromXMLNode(GenerationProcessData* data);

	//Setters
	void ToggleGodMode();
	void ToggleItemMenuActive();

	//Getters
	const bool GetGodModeEnabled() const;
	const bool GetItemMenuActive() const;

};
#endif