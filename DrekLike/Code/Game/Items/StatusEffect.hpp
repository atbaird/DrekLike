#pragma once

#ifndef STATUSEFFECT_HPP
#define STATUSEFFECT_HPP
#include "Game/Generators/EnvironmentGenerationProcess.hpp"
#include <string>

class StatusEffect
{
private:
	std::string m_name;
	int m_totalTurns;
	int m_turnsPassed;
	int m_damageOverTime;
	bool m_getRidOfMe;
public:
	//Constructors
	StatusEffect(const std::string& name = "", const int& turns = 0, const int& damageOverTime = 0);

	//Updates
	void TurnPassed();

	//operators
	const bool operator==(const StatusEffect& other) const;
	void WriteToXMLNode(XMLNode& node) const;
	void ReadFromXMLNode(GenerationProcessData* data);

	//Setters
	void SetName(const std::string& name);
	void SetTotalTurns(const int& setTotalTurns);
	void SetTurnsPassed(const int& turnsPassed);
	void SetDamageOverTime(const int& damageOverTime);
	void SetGetRidOfMe(const bool& getRidOfMe);

	//Getters
	const std::string GetName() const;
	const int GetTotalTurns() const;
	const int GetTurnsPassed() const;
	const int GetDamageOverTime() const;
	const bool GetRidOfMe() const;
};
#endif