#include "Game/Map/TileDefinition.hpp"

STATIC std::map<TileType, TileDefinition> g_TileDefinitions;

//Constructors
TileDefinition::TileDefinition(const TileType& type, const Rgba& color, const unsigned char& car, const bool& hasFloor, 
	const bool& isWall, const bool& isSolid, const bool& isLiquid, const bool& isFire)
	: m_type(type),
	m_color(color),
	m_char(car),
	m_hasFloor(hasFloor),
	m_wall(isWall),
	m_solid(isSolid),
	m_liquid(isLiquid),
	m_fire(isFire)
{

}
TileDefinition::~TileDefinition()
{

}

//setters
void TileDefinition::SetType(const TileType& type)
{
	m_type = type;
}
void TileDefinition::SetColor(const Rgba& color)
{
	m_color = color;
}
void TileDefinition::SetChar(const char& car)
{
	m_char = car;
}
void TileDefinition::SetHasFloor(const bool& hasFloor)
{
	m_hasFloor = hasFloor;
}
void TileDefinition::SetIsWall(const bool& isWall)
{
	m_wall = isWall;
}
void TileDefinition::SetIsLiquid(const bool& isLiquid)
{
	m_liquid = isLiquid;
}
void TileDefinition::SetIsFire(const bool& isFire)
{
	m_fire = isFire;
}

//getters
const TileType TileDefinition::GetType() const
{
	return m_type;
}
const unsigned char TileDefinition::GetChar() const
{
	return m_char;
}
const Rgba TileDefinition::GetColor() const 
{
	return m_color;
}
const bool TileDefinition::GetHasFloor() const
{
	return m_hasFloor;
}
const bool TileDefinition::GetIsWall() const
{
	return m_wall;
}
const bool TileDefinition::GetIsLiquid() const
{
	return m_liquid;
}
const bool TileDefinition::GetIsFire() const
{
	return m_fire;
}

//-----------------------------------------------------------------------------------------------------------------------------------
//static variables
void TileDefinition::AddOrUpdateTileDefinition(const TileDefinition& def)
{
	if (g_TileDefinitions.size() == 0)
	{
		if (def.GetType() != TILE_INVALID)
		{
			g_TileDefinitions.insert(std::pair<TileType, TileDefinition>(TILE_INVALID, TileDefinition()));
		}
		g_TileDefinitions.insert(std::pair<TileType, TileDefinition>(def.GetType(), def));
		return;
	}

	std::map<TileType, TileDefinition>::iterator test = g_TileDefinitions.find(def.GetType());
	if (test != g_TileDefinitions.end())
	{
		test->second = def;
	}
	else
	{
		g_TileDefinitions.insert(std::pair<TileType, TileDefinition>(def.GetType(), def));
	}
}

const std::map<TileType, TileDefinition> TileDefinition::GetAllTileDefinitionsThatMeetBoolVals(const bool& hasFloor, const bool& isWall)
{
	std::map<TileType, TileDefinition> defs;

	for (std::map<TileType, TileDefinition>::iterator it = g_TileDefinitions.begin(); it != g_TileDefinitions.end(); it++)
	{
		TileDefinition def = it->second;
		if (def.GetHasFloor() == hasFloor && def.GetIsWall() == isWall)
		{
			defs.insert(std::pair<TileType, TileDefinition>(it->first, it->second));
		}
	}

	return defs;
}
const TileType TileDefinition::GetTypeAtIndex(const int& index)
{
	if (index >= (int)g_TileDefinitions.size() || index < 0)
	{
		return TILE_INVALID;
	}
	int i = 0;
	for (std::map<TileType, TileDefinition>::const_iterator it = g_TileDefinitions.begin(); it != g_TileDefinitions.end(); it++)
	{
		if (i == index)
		{
			return it->first;
		}
		i++;
	}
	return TILE_INVALID;
}
const bool TileDefinition::TypeExists(const TileType& type)
{
	std::map<TileType, TileDefinition>::const_iterator it = g_TileDefinitions.find(type);
	if (it != g_TileDefinitions.end())
	{
		return true;
	}
	return false;
}
const char TileDefinition::GetTypeChar(const TileType& type)
{
	std::map<TileType, TileDefinition>::const_iterator it = g_TileDefinitions.find(type);
	if (it == g_TileDefinitions.end())
	{
		return 1;
	}
	return it->second.GetChar();
}
const Rgba TileDefinition::GetTypeColor(const TileType& type)
{
	std::map<TileType, TileDefinition>::const_iterator it = g_TileDefinitions.find(type);
	if (it == g_TileDefinitions.end())
	{
		return Rgba(0.f, 0.f, 0.f, 1.f);
	}
	return it->second.GetColor();
}

const bool TileDefinition::GetTypeSolid(const TileType& type, bool& exists)
{
	std::map<TileType, TileDefinition>::const_iterator it = g_TileDefinitions.find(type);
	if (it == g_TileDefinitions.end())
	{
		exists = false;
		return false;
	}
	exists = true;

	return it->second.GetSolid();
}
const bool TileDefinition::GetTypeHasFloor(const TileType& type, bool& exists)
{
	std::map<TileType, TileDefinition>::const_iterator it = g_TileDefinitions.find(type);
	if (it == g_TileDefinitions.end())
	{
		exists = false;
		return false;
	}
	exists = true;
	return it->second.GetHasFloor();
}
const bool TileDefinition::GetTypeIsWall(const TileType& type, bool& exists)
{
	std::map<TileType, TileDefinition>::const_iterator it = g_TileDefinitions.find(type);
	if (it == g_TileDefinitions.end())
	{
		exists = false;
		return false;
	}
	exists = true;
	return it->second.GetIsWall();
}
const bool TileDefinition::GetTypeIsLiquid(const TileType& type, bool& exists)
{
	std::map<TileType, TileDefinition>::const_iterator it = g_TileDefinitions.find(type);
	if (it == g_TileDefinitions.end())
	{
		exists = false;
		return false;
	}
	exists = true;
	return it->second.GetIsLiquid();
}
const bool TileDefinition::GetTypeIsFire(const TileType& type, bool& exists)
{
	std::map<TileType, TileDefinition>::const_iterator it = g_TileDefinitions.find(type);
	if (it == g_TileDefinitions.end())
	{
		exists = false;
		return false;
	}
	exists = true;
	return it->second.GetIsFire();
}

//-----------------------------------------------------------------------------------------------------------------------------------
//operators
const bool TileDefinition::operator==(const TileDefinition& other) const
{
	if (m_type == other.m_type)
	{
		return true;
	}
	return false;
}
