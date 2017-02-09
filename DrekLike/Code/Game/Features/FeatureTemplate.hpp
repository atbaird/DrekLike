#pragma once

#ifndef FEATURETEMPLATE_HPP
#define FEATURETEMPLATE_HPP
#include "Game/Features/Feature.hpp"

class FeatureTemplate
{
private:
	//Data for active or inactive
	std::string m_name;
	FeatureType m_type;
	std::string m_messageToDisplayWhenSetActive;
	std::string m_messageToDisplayWhenSetInactive;
	bool m_blocksLineOfSightWhenInactive;
	bool m_blocksLineOfSightWhenActive;
	bool m_solidWhenInactive;
	bool m_solidWhenActive;
	int m_numTurnsUntilActive;
	int m_minDamage;
	int m_maxDamage;
	float m_chanceToHit;
	unsigned char m_charWhenInactive;
	unsigned char m_charWhenActive;
	Rgba m_colorWhenInactive;
	Rgba m_colorWhenActive;
	bool m_active;
public:
	//Constructors
	FeatureTemplate();

	//Setters
	void SetName(const std::string& name);
	void SetFeatureType(const FeatureType& type);
	void SetBlocksLineOfSightWhenInactive(const bool& inactive);
	void SetBlocksLineOfSightWhenActive(const bool& active);
	void SetSolidWhenInactive(const bool& inactive);
	void SetSolidWhenActive(const bool& active);
	void SetCharWhenInactive(const unsigned char& car);
	void SetCharWhenActive(const unsigned char& car);
	void SetColorWhenInactive(const Rgba& color);
	void SetColorWhenActive(const Rgba& color);
	void SetMessageToDisplayWhenInactive(const std::string& message);
	void SetMessageToDisplayWhenActive(const std::string& message);
	void SetMinDamage(const int& damage);
	void SetMaxDamage(const int& damage);
	void SetChanceToHit(const float& percChance);
	void SetNumTurnsUntilActive(const int& turns);
	void SetActive(const bool& active);

	//Getters
	const std::string GetName() const;
	const FeatureType GetType() const;
	const bool GetBlocksLineOfSightWhenInactive() const;
	const bool GetBlocksLineOfSightWhenActive() const;
	const bool GetSolidWhenInactive() const;
	const bool GetSolidWhenActive() const;
	const unsigned char GetCharWhenInactive() const;
	const unsigned char GetCharWhenActive() const;
	const Rgba GetColorWhenInactive() const;
	const Rgba GetColorWhenActive() const;
	const std::string GetMessageToDisplayWhenInactive() const;
	const std::string GetMessageToDisplayWhenActive() const;
	const bool GetActive() const;

	//Copiers
	void Copy(Feature* feat);

};
#endif // !FEATURETEMPLATE_HPP