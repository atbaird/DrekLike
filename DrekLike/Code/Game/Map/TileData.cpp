#include "Game/Map/TileData.hpp"


TileData::TileData(const IntVector2& min, const IntVector2& max, const TileType& typ)
	: m_mins(min),
	m_maxs(max),
	m_type(typ)
{

}