#include "Game/Features/Feature.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Renderer/Renderer.hpp"


//---------------------------------------------------------------------------------------
//Constructors
std::vector<Feature*> Feature::s_FeaturesOnMap;
Feature::Feature()
	: GameEntity(ENTITY_FEATURE),
	m_type(FEATURETYPE_INVALID),
	m_active(false),
	m_blocksLineOfSightWhenInactive(false),
	m_blocksLineOfSightWhenActive(true),
	m_solidWhenInactive(false),
	m_solidWhenActive(true),
	m_currentlyBlockingLineOfSight(false),
	m_justSwitched(false),
	m_colorWhenActive(1.f, 1.f, 1.f, 1.f),
	m_colorWhenInactive(1.f, 1.f, 1.f, 1.f),
	m_turnsUntilActive(0),
	m_numTurnsUntilActive(0),
	m_minDamage(0),
	m_maxDamage(0),
	m_chanceToHit(0.f),
	m_charWhenActive('+'), //'0' - 5
	m_charWhenInactive('-') //'0' - 3
{

}
Feature::Feature(const Feature& other)
	: GameEntity(other),
	m_type(other.m_type),
	m_blocksLineOfSightWhenInactive(other.m_blocksLineOfSightWhenInactive), //Data for active or inactive
	m_blocksLineOfSightWhenActive(other.m_blocksLineOfSightWhenActive),
	m_solidWhenInactive(other.m_solidWhenInactive),
	m_solidWhenActive(other.m_solidWhenActive),
	m_charWhenInactive(other.m_charWhenInactive),
	m_charWhenActive(other.m_charWhenActive),
	m_colorWhenInactive(other.m_colorWhenInactive),
	m_colorWhenActive(other.m_colorWhenActive),
	m_turnsUntilActive(other.m_turnsUntilActive),
	m_numTurnsUntilActive(other.m_numTurnsUntilActive),
	m_currentlySolid(other.m_currentlySolid), //data for currently active
	m_currentlyBlockingLineOfSight(other.m_currentlyBlockingLineOfSight),
	m_minDamage(other.m_minDamage),
	m_maxDamage(other.m_maxDamage),
	m_chanceToHit(other.m_chanceToHit),
	m_active(other.m_active), //data for setting active details.
	m_justSwitched(other.m_justSwitched)
{
}
Feature::~Feature()
{
}


//---------------------------------------------------------------------------------------
//Update and Render
const float Feature::Update(const float& deltaSeconds)
{
	switch (m_active)
	{
	case true:
		RunIfActive(deltaSeconds);
		break;
	case false:
		RunIfInactive(deltaSeconds);
		break;
	}
	m_justSwitched = false;
	return 0.f;
}
void Feature::UpdateStep()
{
}
void Feature::Render() const
{
}
void Feature::Render(const Rgba& playingBackgroundColor) const
{
	std::map<Tile_Coord, Tile*>::const_iterator tile = g_map->GetMapTiles()->find(m_collision.m_blTilePosition);
	if (tile == g_map->GetMapTiles()->end() || tile->second == nullptr || m_spriteSheet == nullptr)
	{
		return;
	}
	bool visible = tile->second->GetVisible();
	bool seen = tile->second->GetSeen();
	if (visible == false && seen == false && g_map->GetViewFullMapRegardlessOfVisibility() == false)
	{
		return;
	}
	Vector2 renderSize = g_map->GetTileRenderSize();
	
	AABB2 backGround = m_spriteSheet->GetTexCoordsForSpriteIndex(1);
	AABB2 curChar = m_spriteSheet->GetTexCoordsForSpriteIndex(m_char);

	Vector2 bl, backBL;
	Vector2 tr, backTr;
	bl = Vector2(curChar.mins.x, curChar.maxs.y);
	tr = Vector2(curChar.maxs.x, curChar.mins.y);
	backBL = Vector2(backGround.mins.x, backGround.mins.y);
	backTr = Vector2(backGround.maxs.x, backGround.maxs.y);


	std::vector<Vertex_PCT> vertices = g_map->GetVerticesForPosition(m_collision.m_blTilePosition, playingBackgroundColor, backBL, backTr);
	std::vector<Vertex_PCT> front;
	if (visible == true || g_map->GetViewFullMapRegardlessOfVisibility())
	{
		front = g_map->GetVerticesForPosition(m_collision.m_blTilePosition, m_color, bl, tr);
	}
	else if (seen == true)
	{
		front = g_map->GetVerticesForPosition(m_collision.m_blTilePosition, m_color, bl, tr);
	}
	vertices.insert(vertices.end(), front.begin(), front.end());
	g_Renderer->DrawVertexArray(vertices, PRIMITIVE_QUADS, m_spriteSheet->getSpriteSheetTexture());
}
void Feature::RunIfActive(const float&)
{
}
void Feature::RunIfInactive(const float&)
{
}
void Feature::WriteToXMLNode(XMLNode& node) const
{
	XMLNode child = node.addChild("FeatureBlueprint");
	GameEntity::WriteToXMLNode(child);
	std::string test = std::to_string(m_active);
	child.addAttribute("featureType", std::to_string(m_type).c_str());
	child.addAttribute("charWhenActive", std::string(1, static_cast<char>(m_charWhenActive)).c_str());
	child.addAttribute("charWhenInactive", std::string(1, static_cast<char>(m_charWhenInactive)).c_str());
	child.addAttribute("blocksLineOfSightWhenInactive", std::to_string(m_blocksLineOfSightWhenInactive).c_str());
	child.addAttribute("blocksLineOfSightWhenActive", std::to_string(m_blocksLineOfSightWhenActive).c_str());
	child.addAttribute("solidWhenActive", std::to_string(m_solidWhenActive).c_str());
	child.addAttribute("solidWhenInactive", std::to_string(m_solidWhenInactive).c_str());
	child.addAttribute("messageToDisplayWhenSetActive", m_messageToDisplayWhenSetActive.c_str());
	child.addAttribute("messageToDisplayWhenSetInactive", m_messageToDisplayWhenSetInactive.c_str());
	child.addAttribute("colorWhenActive", (std::to_string(m_colorWhenActive.Red) + "," + std::to_string(m_colorWhenActive.Green) + "," + std::to_string(m_colorWhenActive.Blue)).c_str());
	child.addAttribute("colorWhenInactive", (std::to_string(m_colorWhenInactive.Red) + "," + std::to_string(m_colorWhenInactive.Green) + "," + std::to_string(m_colorWhenInactive.Blue)).c_str());
	child.addAttribute("active", std::to_string(m_active).c_str());
	child.addAttribute("turnsUntilActive", std::to_string(m_numTurnsUntilActive).c_str());
	child.addAttribute("currentTurnsTillActive", std::to_string(m_turnsUntilActive).c_str());
	child.addAttribute("damage", (std::to_string(m_minDamage) + "~" + std::to_string(m_maxDamage)).c_str());
	child.addAttribute("chanceToHit", std::to_string(m_chanceToHit).c_str());
}
void Feature::ReadFromXMLNode(GenerationProcessData* data)
{
	bool makeThis = false;
	std::vector<StrXMLAttr> attrs = data->GetAttributes();
	for (size_t i = 0; i < attrs.size(); i++)
	{
		StrXMLAttr attr = attrs.at(i);
		if (GameEntity::ReadFromXMLNodeCommonAttributes(this, attr, ENTITY_FEATURE, makeThis))
		{
			continue;
		}
		else if (SimpleStrCmp(attr.name, "featureType"))
		{
			m_type = (FeatureType)atoi(attr.val.c_str());
		}
		else if (SimpleStrCmp(attr.name, "charWhenActive"))
		{
			m_charWhenActive = attr.val.at(0);
		}
		else if (SimpleStrCmp(attr.name, "charWhenInactive"))
		{
			m_charWhenInactive = attr.val.at(0);
		}
		else if (SimpleStrCmp(attr.name, "blocksLineOfSightWhenActive"))
		{
			m_blocksLineOfSightWhenActive = atoi(attr.val.c_str()) == 1;
		}
		else if (SimpleStrCmp(attr.name, "blocksLineOfSightWhenInactive"))
		{
			m_blocksLineOfSightWhenInactive = atoi(attr.val.c_str()) == 1;
		}
		else if (SimpleStrCmp(attr.name, "solidWhenInactive"))
		{
			m_solidWhenInactive = atoi(attr.val.c_str()) == 1;
		}
		else if (SimpleStrCmp(attr.name, "solidWhenActive"))
		{
			m_solidWhenActive = atoi(attr.val.c_str()) == 1;
		}
		else if (SimpleStrCmp(attr.name, "messageToDisplayWhenSetActive"))
		{
			m_messageToDisplayWhenSetActive = attr.val;
		}
		else if (SimpleStrCmp(attr.name, "messageToDisplayWhenSetInactive"))
		{
			m_messageToDisplayWhenSetInactive = attr.val;
		}
		else if (SimpleStrCmp(attr.name, "colorWhenActive"))
		{
			std::string val = ReplaceCharInString(attr.val, ',', ' ');
			std::vector<std::string> vals = ParseString(val);
			m_colorWhenActive = Rgba((float)atof(vals.at(0).c_str()), (float)atof(vals.at(1).c_str()), (float)atof(vals.at(2).c_str()), 1.f);
		}
		else if (SimpleStrCmp(attr.name, "colorWhenInactive"))
		{
			std::string val = ReplaceCharInString(attr.val, ',', ' ');
			std::vector<std::string> vals = ParseString(val);
			m_colorWhenInactive = Rgba((float)atof(vals.at(0).c_str()), (float)atof(vals.at(1).c_str()), (float)atof(vals.at(2).c_str()), 1.f);
		}
		else if (SimpleStrCmp(attr.name, "active"))
		{
			m_active = atoi(attr.val.c_str()) == 1;
		}
		else if (SimpleStrCmp(attr.name, "turnsUntilActive"))
		{
			m_numTurnsUntilActive = atoi(attr.val.c_str());
		}
		else if (SimpleStrCmp(attr.name, "currentTurnsTillActive"))
		{
			m_turnsUntilActive = atoi(attr.val.c_str());
		}
		else if (SimpleStrCmp(attr.name, "damage"))
		{
			std::string val = ReplaceCharInString(attr.val, '~', ' ');
			std::vector<std::string> vals = ParseString(val);
			m_minDamage = atoi(vals.at(0).c_str());
			m_maxDamage = atoi(vals.at(1).c_str());
		}
		else if (SimpleStrCmp(attr.name, "chanceToHit"))
		{
			m_chanceToHit = (float)atof(attr.val.c_str());
		}
	}
}

//---------------------------------------------------------------------------------------
//Setters
void Feature::EntityToggleActive(const std::string& nameOfEntity, const bool& visibleToPlayer)
{
	ToggleActive();
	if (m_justSwitched == true && m_active == true && visibleToPlayer)
	{
		s_messageBoxStrings.push_back(nameOfEntity + m_messageToDisplayWhenSetActive);
	}
	else if(m_justSwitched == true && m_active == false && visibleToPlayer)
	{
		s_messageBoxStrings.push_back(nameOfEntity + m_messageToDisplayWhenSetInactive);
	}
}
void Feature::ToggleActive()
{
	m_active = !m_active;
	if (m_active)
	{
		MakeActive();
	}
	else
	{
		MakeInactive();
	}
	m_justSwitched = true;
}
void Feature::SetFeatureType(const FeatureType& type)
{
	m_type = type;
}
void Feature::SetIsActive(const bool& active)
{
	m_active = active;
}
void Feature::SetBlocksLineOfSightWhenInactive(const bool& inactive)
{
	m_blocksLineOfSightWhenInactive = inactive;
}
void Feature::SetBlocksLineOfSightWhenActive(const bool& active)
{
	m_blocksLineOfSightWhenActive = active;
}
void Feature::SetSolidWhenInactive(const bool& inactive)
{
	m_solidWhenInactive = inactive;
}
void Feature::SetSolidWhenActive(const bool& active)
{
	m_solidWhenActive = active;
}
void Feature::SetCharWhenInactive(const unsigned char& car)
{
	m_charWhenInactive = car;
}
void Feature::SetCharWhenActive(const unsigned char& car)
{
	m_charWhenActive = car;
}
void Feature::SetColorWhenInactive(const Rgba& color)
{
	m_colorWhenInactive = color;
}
void Feature::SetColorWhenActive(const Rgba& color)
{
	m_colorWhenActive = color;
}
void Feature::SetMessageToDisplayWhenSetInactive(const std::string& message)
{
	m_messageToDisplayWhenSetInactive = message;
}
void Feature::SetMessageToDisplayWhenSetActive(const std::string& message)
{
	m_messageToDisplayWhenSetActive = message;
}
void Feature::SetCurrentlySolid(const bool& solid)
{
	m_currentlySolid = solid;
}
void Feature::SetCurrentlyBlockingLineOfSight(const bool& blocking)
{
	m_currentlyBlockingLineOfSight = blocking;
}

void Feature::SetMinDamage(const int& damage)
{
	m_minDamage = damage;
}
void Feature::SetMaxDamage(const int& damage)
{
	m_maxDamage = damage;
}
void Feature::SetNumTurnsUntilActive(const int& turns)
{
	m_numTurnsUntilActive = turns;
}
void Feature::SetChanceToHit(const float& percChance)
{
	m_chanceToHit = percChance;
}
void Feature::MakeActive()
{
	m_currentlySolid = m_solidWhenActive;
	m_currentlyBlockingLineOfSight = m_blocksLineOfSightWhenActive;
	m_char = m_charWhenActive;
	m_color = m_colorWhenActive;
}
void Feature::MakeInactive()
{
	m_currentlySolid = m_solidWhenInactive;
	m_currentlyBlockingLineOfSight = m_blocksLineOfSightWhenInactive;
	m_char = m_charWhenInactive;
	m_color = m_colorWhenInactive;
}


//---------------------------------------------------------------------------------------
//Getters
const std::string Feature::GetName() const
{
	return m_name;
}
const FeatureType Feature::GetFeatureType() const
{
	return m_type;
}
const bool Feature::GetIsActive() const
{
	return m_active;
}
const bool Feature::GetBlocksLineOfSightWhenInactive() const
{
	return m_blocksLineOfSightWhenInactive;
}
const bool Feature::GetBlocksLineOfSightWhenActive() const
{
	return m_blocksLineOfSightWhenActive;
}
const bool Feature::GetSolidWhenInactive() const
{
	return m_solidWhenInactive;
}
const bool Feature::GetSolidWhenActive() const
{
	return m_solidWhenActive;
}
const unsigned char Feature::GetCharWhenActive() const
{
	return m_charWhenActive;
}
const unsigned char Feature::GetCharWhenInactive() const
{
	return m_charWhenInactive;
}
const Rgba Feature::GetColorWhenActive() const
{
	return m_colorWhenActive;
}
const Rgba Feature::GetColorWhenInactive() const
{
	return m_colorWhenInactive;
}
const std::string Feature::GetMessageToDisplayWhenSetActive() const
{
	return m_messageToDisplayWhenSetActive;
}
const std::string Feature::GetMessageToDisplayWhenSetInactive() const
{
	return m_messageToDisplayWhenSetInactive;
}
const bool Feature::GetCurrentlySolid() const
{
	return m_currentlySolid;
}
const bool Feature::GetCurrentlyBlockingLineOfSight() const
{
	return m_currentlyBlockingLineOfSight;
}
const bool Feature::GetJustSwitched() const
{
	return m_justSwitched;
}

//--------------------------------------------------------------------------------------
//Static methods
const bool Feature::isPositionTakenByFeatureAndBlockingView(const IntVector2& bl)
{
	for (size_t i = 0; i < s_FeaturesOnMap.size(); i++)
	{
		Feature* feat = s_FeaturesOnMap.at(i);
		if (feat != nullptr)
		{
			IntVector2 featBl = feat->GetBlTilePos();
			bool blocksLineOfSight = feat->GetCurrentlyBlockingLineOfSight();
			if (featBl == bl && blocksLineOfSight == true)
			{
				return true;
			}
		}
	}
	return false;
}
const bool Feature::IsPositionTakenByAFeature(const IntVector2& bl)
{
	for (size_t i = 0; i < s_FeaturesOnMap.size(); i++)
	{
		Feature* feat = s_FeaturesOnMap.at(i);
		if (feat != nullptr)
		{
			IntVector2 featBl = feat->GetBlTilePos();
			if (featBl == bl)
			{
				return true;
			}
		}
	}
	return false;
}
Feature* Feature::GetFeatureAtPosition(const IntVector2& bl)
{
	for (size_t i = 0; i < s_FeaturesOnMap.size(); i++)
	{
		Feature* feat = s_FeaturesOnMap.at(i);
		if (feat != nullptr)
		{
			IntVector2 featBl = feat->GetBlTilePos();
			if (featBl == bl)
			{
				return feat;
			}
		}
	}
	return nullptr;
}
void Feature::DestroyAllFeatures()
{
	for (size_t i = 0; i < s_FeaturesOnMap.size(); i++)
	{
		if (s_FeaturesOnMap.at(i) != nullptr)
		{
			delete s_FeaturesOnMap[i];
			s_FeaturesOnMap[i] = nullptr;
		}
		s_FeaturesOnMap.erase(s_FeaturesOnMap.begin());
		i--;
	}
	s_FeaturesOnMap.clear();
}
void Feature::SetSpriteSheetAllFeatures(SpriteSheet* spriteSheet)
{
	for (size_t i = 0; i < s_FeaturesOnMap.size(); i++)
	{
		s_FeaturesOnMap.at(i)->SetSpriteSheet(spriteSheet);
	}
}
void Feature::RenderAllFeatures(const Rgba& playingBackgroundColor)
{
	for (size_t i = 0; i < s_FeaturesOnMap.size(); i++)
	{
		s_FeaturesOnMap.at(i)->Render(playingBackgroundColor);
	}
}