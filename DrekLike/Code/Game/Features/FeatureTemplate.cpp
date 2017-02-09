#include "Game/Features/FeatureTemplate.hpp"

//------------------------------------------------------------------------------------
//Constructors
FeatureTemplate::FeatureTemplate()
	: m_name(""),
	m_type(FEATURETYPE_INVALID),
	m_blocksLineOfSightWhenInactive(false),
	m_blocksLineOfSightWhenActive(false),
	m_solidWhenInactive(false),
	m_solidWhenActive(false),
	m_charWhenInactive(1),
	m_charWhenActive(1),
	m_numTurnsUntilActive(0),
	m_minDamage(0),
	m_maxDamage(0),
	m_chanceToHit(0.f),
	m_colorWhenInactive(1.f,1.f,1.f,1.f),
	m_colorWhenActive(0.5f, 0.5f, 0.5f, 1.f),
	m_active(false)
{

}

//------------------------------------------------------------------------------------
//Setters
void FeatureTemplate::SetName(const std::string& name)
{
	m_name = name;
}
void FeatureTemplate::SetFeatureType(const FeatureType& type)
{
	m_type = type;
}
void FeatureTemplate::SetBlocksLineOfSightWhenInactive(const bool& inactive)
{
	m_blocksLineOfSightWhenInactive = inactive;
}
void FeatureTemplate::SetBlocksLineOfSightWhenActive(const bool& active)
{
	m_blocksLineOfSightWhenActive = active;
}
void FeatureTemplate::SetSolidWhenInactive(const bool& inactive)
{
	m_solidWhenInactive = inactive;
}
void FeatureTemplate::SetSolidWhenActive(const bool& active)
{
	m_solidWhenActive = active;
}
void FeatureTemplate::SetCharWhenInactive(const unsigned char& car)
{
	m_charWhenInactive = car;
}
void FeatureTemplate::SetCharWhenActive(const unsigned char& car)
{
	m_charWhenActive = car;
}
void FeatureTemplate::SetColorWhenInactive(const Rgba& color)
{
	m_colorWhenInactive = color;
}
void FeatureTemplate::SetColorWhenActive(const Rgba& color)
{
	m_colorWhenActive = color;
}
void FeatureTemplate::SetMessageToDisplayWhenInactive(const std::string& message)
{
	m_messageToDisplayWhenSetInactive = message;
}
void FeatureTemplate::SetMessageToDisplayWhenActive(const std::string& message)
{
	m_messageToDisplayWhenSetActive = message;
}

void FeatureTemplate::SetMinDamage(const int& damage)
{
	m_minDamage = damage;
}
void FeatureTemplate::SetMaxDamage(const int& damage)
{
	m_maxDamage = damage;
}
void FeatureTemplate::SetChanceToHit(const float& percChance)
{
	m_chanceToHit = percChance;
}
void FeatureTemplate::SetNumTurnsUntilActive(const int& turns)
{
	m_numTurnsUntilActive = turns;
}
void FeatureTemplate::SetActive(const bool& active)
{
	m_active = active;
}

//------------------------------------------------------------------------------------
//Getters
const std::string FeatureTemplate::GetName() const
{
	return m_name;
}
const FeatureType FeatureTemplate::GetType() const
{
	return m_type;
}
const bool FeatureTemplate::GetBlocksLineOfSightWhenInactive() const
{
	return m_blocksLineOfSightWhenInactive;
}
const bool FeatureTemplate::GetBlocksLineOfSightWhenActive() const
{
	return m_blocksLineOfSightWhenActive;
}
const bool FeatureTemplate::GetSolidWhenInactive() const
{
	return m_solidWhenInactive;
}
const bool FeatureTemplate::GetSolidWhenActive() const
{
	return m_solidWhenActive;
}
const unsigned char FeatureTemplate::GetCharWhenInactive() const
{
	return m_charWhenInactive;
}
const unsigned char FeatureTemplate::GetCharWhenActive() const
{
	return m_charWhenActive;
}
const Rgba FeatureTemplate::GetColorWhenInactive() const
{
	return m_colorWhenInactive;
}
const Rgba FeatureTemplate::GetColorWhenActive() const
{
	return m_colorWhenActive;
}
const std::string FeatureTemplate::GetMessageToDisplayWhenInactive() const
{
	return m_messageToDisplayWhenSetInactive;
}
const std::string FeatureTemplate::GetMessageToDisplayWhenActive() const
{
	return m_messageToDisplayWhenSetActive;
}
const bool FeatureTemplate::GetActive() const
{
	return m_active;
}


//--------------------------------------------------------------------------------------
//Copiers
void FeatureTemplate::Copy(Feature* feat)
{
	feat->SetEntityName(m_name);
	feat->SetFeatureType(m_type);
	feat->SetBlocksLineOfSightWhenActive(m_blocksLineOfSightWhenActive);
	feat->SetBlocksLineOfSightWhenInactive(m_blocksLineOfSightWhenInactive);
	feat->SetSolidWhenActive(m_solidWhenActive);
	feat->SetSolidWhenInactive(m_solidWhenInactive);
	feat->SetColorWhenActive(m_colorWhenActive);
	feat->SetColorWhenInactive(m_colorWhenInactive);
	feat->SetCharWhenActive(m_charWhenActive);
	feat->SetCharWhenInactive(m_charWhenInactive);
	feat->SetMessageToDisplayWhenSetActive(m_messageToDisplayWhenSetActive);
	feat->SetMessageToDisplayWhenSetInactive(m_messageToDisplayWhenSetInactive);
	feat->SetNumTurnsUntilActive(m_numTurnsUntilActive);
	feat->SetMinDamage(m_minDamage);
	feat->SetMaxDamage(m_maxDamage);
	feat->SetChanceToHit(m_chanceToHit);
	feat->SetIsActive(m_active);
	if (m_active)
	{
		feat->SetColor(m_colorWhenActive);
		feat->SetChar(m_charWhenActive);
		feat->SetCurrentlyBlockingLineOfSight(m_blocksLineOfSightWhenActive);
		feat->SetCurrentlySolid(m_solidWhenActive);
	}
	else
	{
		feat->SetColor(m_colorWhenInactive);
		feat->SetChar(m_charWhenInactive);
		feat->SetCurrentlyBlockingLineOfSight(m_blocksLineOfSightWhenInactive);
		feat->SetCurrentlySolid(m_solidWhenInactive);
	}
}