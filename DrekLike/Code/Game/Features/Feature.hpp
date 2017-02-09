#pragma once

#ifndef FEATURE_HPP
#define FEATURE_HPP
#include "Game/GameEntity/GameEntity.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

enum FeatureType
{
	FEATURETYPE_INVALID = -1,
	FEATURETYPE_DOOR,
	FEATURETYPE_SIEGE,
	NUM_OF_FEATURE_TYPES
};

class Feature : public GameEntity
{
private:
	FeatureType m_type;

	//Data for active or inactive
	bool m_blocksLineOfSightWhenInactive;
	bool m_blocksLineOfSightWhenActive;
	bool m_solidWhenInactive;
	bool m_solidWhenActive;
	unsigned char m_charWhenInactive;
	unsigned char m_charWhenActive;
	Rgba m_colorWhenInactive;
	Rgba m_colorWhenActive;
	std::string m_messageToDisplayWhenSetActive;
	std::string m_messageToDisplayWhenSetInactive;
	int m_numTurnsUntilActive;
	int m_turnsUntilActive;
	int m_minDamage;
	int m_maxDamage;
	float m_chanceToHit;

	//data for currently active
	bool m_currentlySolid;
	bool m_currentlyBlockingLineOfSight;

	//data for setting active details.
	bool m_active;
	bool m_justSwitched;
public:
	static std::vector<Feature*> s_FeaturesOnMap;
public:
	//Constructors
	Feature();
	Feature(const Feature& other);
	virtual ~Feature();


	//Update and Render
	virtual const float Update(const float& deltaSeconds);
	void UpdateStep() override;
	void Render() const override;
	void Render(const Rgba& playingBackgroundColor) const override;
	virtual void RunIfActive(const float& deltaSeconds);
	virtual void RunIfInactive(const float& deltaSeconds);
	virtual void WriteToXMLNode(XMLNode& node) const override;
	virtual void ReadFromXMLNode(GenerationProcessData* data);

	//Setters
	virtual void EntityToggleActive(const std::string& nameOfEntity, const bool& visibleToPlayer);
	virtual void ToggleActive();
	virtual void SetFeatureType(const FeatureType& type);
	virtual void SetIsActive(const bool& active);
	virtual void SetBlocksLineOfSightWhenInactive(const bool& inactive);
	virtual void SetBlocksLineOfSightWhenActive(const bool& active);
	virtual void SetSolidWhenInactive(const bool& inactive);
	virtual void SetSolidWhenActive(const bool& active);
	virtual void SetCharWhenInactive(const unsigned char& car);
	virtual void SetCharWhenActive(const unsigned char& car);
	virtual void SetColorWhenInactive(const Rgba& color);
	virtual void SetColorWhenActive(const Rgba& color);
	virtual void SetMessageToDisplayWhenSetInactive(const std::string& message);
	virtual void SetMessageToDisplayWhenSetActive(const std::string& message);
	virtual void SetCurrentlySolid(const bool& solid);
	virtual void SetCurrentlyBlockingLineOfSight(const bool& blocking);
	virtual void SetMinDamage(const int& damage);
	virtual void SetMaxDamage(const int& damage);
	virtual void SetNumTurnsUntilActive(const int& turns);
	virtual void SetChanceToHit(const float& percChance);
private:
	void MakeActive();
	void MakeInactive();
public:

	//Getters
	virtual const std::string GetName() const;
	virtual const FeatureType GetFeatureType() const;
	virtual const bool GetIsActive() const;
	virtual const bool GetBlocksLineOfSightWhenInactive() const;
	virtual const bool GetBlocksLineOfSightWhenActive() const;
	virtual const bool GetSolidWhenInactive() const;
	virtual const bool GetSolidWhenActive() const;
	virtual const unsigned char GetCharWhenActive() const;
	virtual const unsigned char GetCharWhenInactive() const;
	virtual const Rgba GetColorWhenActive() const;
	virtual const Rgba GetColorWhenInactive() const;
	virtual const std::string GetMessageToDisplayWhenSetActive() const;
	virtual const std::string GetMessageToDisplayWhenSetInactive() const;
	virtual const bool GetCurrentlySolid() const;
	virtual const bool GetCurrentlyBlockingLineOfSight() const;
	virtual const bool GetJustSwitched() const;

	//static variables
	static const bool isPositionTakenByFeatureAndBlockingView(const IntVector2& bl);
	static const bool IsPositionTakenByAFeature(const IntVector2& bl);
	static Feature* GetFeatureAtPosition(const IntVector2& bl);
	static void DestroyAllFeatures();
	static void SetSpriteSheetAllFeatures(SpriteSheet* spriteSheet);
	static void RenderAllFeatures(const Rgba& playingBackgroundColor);

};

#endif