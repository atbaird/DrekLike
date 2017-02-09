#pragma once

#ifndef TILEDEFINITION_HPP
#define TILEDEFINITION_HPP
#include "Game/Map/TileType.hpp"
#include "Game/Generators/MapGenerators/DungeonMap/DungeonMapGen_Room.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include <map>

class TileDefinition;

#define STATIC
extern STATIC std::map<TileType, TileDefinition> g_TileDefinitions;

class TileDefinition
{
private:
	TileType m_type;
	Rgba m_color;
	unsigned char m_char;
	bool m_liquid;
	bool m_fire;
	bool m_hasFloor;
	bool m_solid;
	bool m_wall;
public:
	//Constructors
	TileDefinition(const TileType& type = TILE_INVALID, const Rgba& color = Rgba(1.f,1.f,1.f,1.f), const unsigned char& car = ' ', const bool& hasFloor = false, const bool& isWall = false,
		const bool& isSolid = false, const bool& isLiquid = false, const bool& isFire = false);
	~TileDefinition();

	//static methods
	static void AddOrUpdateTileDefinition(const TileDefinition& def);
	static const std::map<TileType, TileDefinition> GetAllTileDefinitionsThatMeetBoolVals(const bool& hasFloor, const bool& isWall);
	static const TileType GetTypeAtIndex(const int& index);
	static const bool TypeExists(const TileType& type);
	static const char GetTypeChar(const TileType& type);
	static const Rgba GetTypeColor(const TileType& type);
	static const bool GetTypeSolid(const TileType& type, bool& exists);
	static const bool GetTypeHasFloor(const TileType& type, bool& exists);
	static const bool GetTypeIsWall(const TileType& type, bool& exists);
	static const bool GetTypeIsLiquid(const TileType& type, bool& exists);
	static const bool GetTypeIsFire(const TileType& type, bool& exists);

	//setters
	void SetType(const TileType& type);
	void SetColor(const Rgba& color);
	void SetChar(const char& car);
	void SetSolid(const bool& solid) { m_solid = solid; };
	void SetHasFloor(const bool& hasFloor);
	void SetIsWall(const bool& isWall);
	void SetIsLiquid(const bool& isLiquid);
	void SetIsFire(const bool& isFire);

	//getters
	const TileType GetType() const;
	const Rgba GetColor() const;
	const unsigned char GetChar() const;
	const bool GetSolid() const { return m_solid; };
	const bool GetHasFloor() const;
	const bool GetIsWall() const;
	const bool GetIsLiquid() const;
	const bool GetIsFire() const;

	//operators
	const bool operator==(const TileDefinition& other) const;
};
#endif