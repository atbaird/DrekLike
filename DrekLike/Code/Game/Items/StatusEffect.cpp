#include "Game/Items/StatusEffect.hpp"


//--------------------------------------------------------------------
//Constructors
StatusEffect::StatusEffect(const std::string& name, const int& turns, const int& damageOverTime)
	: m_name(name),
	m_totalTurns(turns),
	m_turnsPassed(0),
	m_damageOverTime(damageOverTime),
	m_getRidOfMe(false)
{
	if (turns <= 0)
	{
		m_getRidOfMe = true;
	}
}

//--------------------------------------------------------------------
//Updaters
void StatusEffect::TurnPassed()
{
	if (m_totalTurns <= 0 || m_turnsPassed >= m_totalTurns)
	{
		m_getRidOfMe = true;
		return;
	}
	m_turnsPassed++;
}


//--------------------------------------------------------------------
//operators
const bool StatusEffect::operator==(const StatusEffect& other) const
{
	if (strcmp(m_name.c_str(), other.GetName().c_str()) == 0)
	{
		return true;
	}
	return false;
}
void StatusEffect::WriteToXMLNode(XMLNode& node) const
{
	XMLNode statusEffect = node.addChild("StatusEffect");
	statusEffect.addAttribute("name", m_name.c_str());
	statusEffect.addAttribute("TotalTurns", std::to_string(m_totalTurns).c_str());
	statusEffect.addAttribute("TurnsPassed", std::to_string(m_turnsPassed).c_str());
	statusEffect.addAttribute("DamageOverTime", std::to_string(m_damageOverTime).c_str());
	statusEffect.addAttribute("GetRidOfMe", std::to_string(m_getRidOfMe).c_str());
}
void StatusEffect::ReadFromXMLNode(GenerationProcessData* data)
{
	m_name = data->GetAttributeByNameString("name");
	m_totalTurns = atoi(data->GetAttributeByNameString("TotalTurns").c_str());
	m_turnsPassed = atoi(data->GetAttributeByNameString("TurnsPassed").c_str());
	m_damageOverTime = atoi(data->GetAttributeByNameString("DamageOverTime").c_str());
	m_getRidOfMe = atoi(data->GetAttributeByNameString("GetRidOfMe").c_str()) == 1;
}


//--------------------------------------------------------------------
//Setters
void StatusEffect::SetName(const std::string& name)
{
	m_name = name;
}
void StatusEffect::SetTotalTurns(const int& setTotalTurns)
{
	m_totalTurns = setTotalTurns;
}
void StatusEffect::SetTurnsPassed(const int& turnsPassed)
{
	m_turnsPassed = turnsPassed;
}
void StatusEffect::SetDamageOverTime(const int& damageOverTime)
{
	m_damageOverTime = damageOverTime;
}
void StatusEffect::SetGetRidOfMe(const bool& getRidOfMe)
{
	m_getRidOfMe = getRidOfMe;
}

//--------------------------------------------------------------------
//Getters
const std::string StatusEffect::GetName() const
{
	return m_name;
}
const int StatusEffect::GetTotalTurns() const
{
	return m_totalTurns;
}
const int StatusEffect::GetTurnsPassed() const
{
	return m_turnsPassed;
}
const int StatusEffect::GetDamageOverTime() const
{
	return m_damageOverTime;
}
const bool StatusEffect::GetRidOfMe() const
{
	return m_getRidOfMe;
}