#pragma once

#ifndef FACTION_HPP
#define FACTION_HPP
#include <string>
#include <vector>
#include "Game/Generators/EnvironmentGenerationProcess.hpp"


class FactionRelationship
{
protected:
	int m_factionID;
	int m_factionRelationship;
public:
	FactionRelationship(const int& factionID, const int& factionRel);

	void SetFactionID(const int& id) { m_factionID = id; };
	void SetFactionRelationship(const int& rel) { m_factionRelationship = rel; };

	const int GetFactionID() const { return m_factionID; };
	const int GetFactionRelationship() const { return m_factionRelationship; };
};

class Faction
{
protected:
	static std::vector<Faction*> m_factions;
	static int s_currentlyAvailableFactionID;
	std::string m_factionName;
	int m_factionID;
	std::vector<FactionRelationship> m_factionRelationships;
public:
	//Constructors
	Faction(const std::string& name = "");
	Faction(const Faction& other);
	~Faction();

	//static functions
	static Faction* CreateFactionOfNameIfDoesNotAlreadyExist(const std::string& name);
	static Faction* GetFactionOfIDIfExists(const int& id);
	static Faction* CreateOrGetFactionOfName(const std::string& name);
	static Faction* CreateOrGetFactionOfID(const int& id);
	static const bool DoesFactionAlreadyExist(const std::string& name);
	static void ClearAllFactions();
	static void LoadAllFactions();
	static void WriteAllToXMLNode(XMLNode& node);
	static void ReadAllFromXMLNode(GenerationProcessData* data);

	//Setters
	void WriteToXMLNode(XMLNode& node);
	void ReadFromXMLNode(GenerationProcessData* data);
	void SetFactionID(const int& id);
	void SetFactionName(const std::string& name);
	void AddFactionRelationship(const FactionRelationship& relationship);
	void AddRelationShipScore(const int& factionID, const int& toAddToScore);

	//Getters
	const std::string GetFactionName() const;
	const int GetFactionRelationShipForID(const int& id);
	const std::vector<FactionRelationship> GetFactionRelationships();
	const int GetFactionID() const;
	const int EvaluateHatredScale(Faction* other);

};

#endif