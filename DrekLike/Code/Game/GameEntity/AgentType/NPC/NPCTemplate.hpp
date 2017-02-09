#pragma once
#include "Game/GameEntity/AgentType/NPC/NPC.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/Rgba.hpp"

#ifndef NPCTEMPLATE_HPP
#define NPCTEMPLATE_HPP

class NPCTemplate
{
protected:
	int m_minMaxHealth;
	int m_maxMaxHealth;
	int m_minDamage;
	int m_maxDamage;
	int m_movementMask;
	float m_accuracy;
	float m_lineOfSight;
	float m_stepCost;
	Vector3 m_minRGB;
	Vector3 m_maxRGB;
	unsigned char m_char;
	Faction* m_faction = nullptr;

	std::string m_race;

	std::vector<std::string> m_names;
	std::vector<std::string> m_nameOfItemsToAddToNPC;
	std::vector<AgentTitle> m_titles;
	std::vector<Behavior*> m_behaviors;
	
public:
	//constructors
	NPCTemplate();
	~NPCTemplate();

	//Setters
	void SetMask(const int& mas);
	void SetMinMaxHealth(const int& health);
	void SetMaxMaxHealth(const int& health);
	void SetMaxHealth(const int& health);
	void SetMinRGB(const Vector3& rgb);
	void SetMaxRGB(const Vector3& rgb);
	void SetMinDamage(const int& min);
	void SetMaxDamage(const int& max);
	void SetStepCost(const float& stepCost);
	void SetRGB(const Vector3& rgb);
	void SetChar(const unsigned char& car);
	void SetRace(const std::string& race);
	void SetLineOfSight(const float& lineOfSight);
	void SetAccuracy(const float& accuracy);
	void AddBehavior(Behavior* bev);
	void AddPossibleTitle(const AgentTitle& title);
	void AddName(const std::string& name);
	void AddItem(const std::string& name);
	void SetFaction(Faction* fact);

	//Getters
	const int GetMovementMask() const;
	const int GetMinMaxHealth() const;
	const int GetMaxMaxHealth() const;
	const int GetRandMaxHealth() const;
	const int GetMaxDamage() const;
	const int GetMinDamage() const;
	const float GetStepCost() const;
	const float GetLineOfSight() const;
	const Vector3 GetMinRgb() const;
	const Vector3 GetMaxRgb() const;
	const Rgba GetRandRgb() const;
	const unsigned char GetChar() const;
	const std::string GetRace() const;
	const std::vector<std::string> GetItemNames() const;
	const bool DoesNameAlreadyExist(const std::string& name) const;
	const bool DoesItemAlreadyExist(const std::string& name) const;

	//
	void CopyValuesToNPC(NPC* npc, SpriteSheet* spriteSheet);

	void ClearTemplateBehaviors();

};
#endif // !NPCTEMPLATE_HPP
