#include "Game/GameEntity/GameEntity.hpp"
#include "Engine/Core/StringUtils.hpp"

//--------------------------------------------------------------------
//global functions
const IntVector2 ConvertDirectionEnumToTileCoord(const Direction& dir)
{
	IntVector2 val;
	switch (dir)
	{
	case Direction_Not_picked:
		val = IntVector2(0, 0);
		break;
	case Direction_None:
		val = IntVector2(0, 0);
		break;
	case Direction_North:
		val = IntVector2(0, 1);
		break;
	case Direction_NorthEast:
		val = IntVector2(1, 1);
		break;
	case Direction_East:
		val = IntVector2(1, 0);
		break;
	case Direction_SouthEast:
		val = IntVector2(1, -1);
		break;
	case Direction_South:
		val = IntVector2(0, -1);
		break;
	case Direction_SouthWest:
		val = IntVector2(-1, -1);
		break;
	case Direction_West:
		val = IntVector2(-1, 0);
		break;
	case Direction_NorthWest:
		val = IntVector2(-1, 1);
		break;
	default:
		val = IntVector2(0, 0);
		break;
	}
	return val;
}
const Vector2 ConvertDirectionToVec2Dir(const Direction& dir)
{
	Vector2 val;
	switch (dir)
	{
	case Direction_Not_picked:
		val = Vector2(0, 0);
		break;
	case Direction_None:
		val = Vector2(0, 0);
		break;
	case Direction_North:
		val = Vector2(0, 1);
		val.normalize();
		break;
	case Direction_NorthEast:
		val = Vector2(1, 1);
		val.normalize();
		break;
	case Direction_East:
		val = Vector2(1, 0);
		val.normalize();
		break;
	case Direction_SouthEast:
		val = Vector2(1, -1);
		val.normalize();
		break;
	case Direction_South:
		val = Vector2(0, -1);
		val.normalize();
		break;
	case Direction_SouthWest:
		val = Vector2(-1, -1);
		val.normalize();
		break;
	case Direction_West:
		val = Vector2(-1, 0);
		val.normalize();
		break;
	case Direction_NorthWest:
		val = Vector2(-1, 1);
		val.normalize();
		break;
	default:
		val = Vector2(0, 0);
		break;
	}
	return val;
}

//--------------------------------------------------------------------
//EntityCollision

EntityCollision::EntityCollision(const IntVector2& blPos, const IntVector2& size)
	: m_blTilePosition(blPos),
	m_entitySize(size)
{

}


//--------------------------------------------------------------------
//static vars
unsigned int GameEntity::s_nextID = 1;
int GameEntity::currentlyAvailableGameEntityID = 0;


GameEntity::GameEntity(const EntityType& type, const IntVector2& blTilePos, const IntVector2& entitySize)
	: Entity(),
	m_spriteSheet(nullptr),
	m_collision(blTilePos, entitySize),
	m_wantedNewBLPosition(blTilePos),
	m_entityType(type),
	m_wantedNewEntitySize(entitySize),
	m_dirToMove(Direction_Not_picked),
	m_maxHealth(0),
	m_currentHealth(0),
	m_visibleToPlayer(false),
	m_GameEntityID(currentlyAvailableGameEntityID),
	m_renderRegardlessOfVisibility(false)
{
	currentlyAvailableGameEntityID++;
}
GameEntity::GameEntity(const GameEntity& other)
	:Entity(),
	m_renderRegardlessOfVisibility(other.m_renderRegardlessOfVisibility),
	m_char(other.m_char),
	m_color(other.m_color),
	m_collision(other.m_collision),
	m_wantedNewBLPosition(other.m_wantedNewBLPosition),
	m_entityType(other.m_entityType),
	m_wantedNewEntitySize(other.m_wantedNewEntitySize),
	m_dirToMove(other.m_dirToMove),
	m_maxHealth(other.m_maxHealth),
	m_currentHealth(other.m_currentHealth),
	m_visibleToPlayer(other.m_visibleToPlayer),
	m_GameEntityID(currentlyAvailableGameEntityID),
	m_spriteSheet(other.m_spriteSheet)
{
	currentlyAvailableGameEntityID++;
}
GameEntity::~GameEntity()
{
}

const float GameEntity::Update(const float& deltaSeconds)
{
	deltaSeconds;
	return 0.0f;
}
void GameEntity::UpdateStep()
{


	m_collision.m_entitySize = m_wantedNewEntitySize;
}
void GameEntity::Render() const
{

}
void GameEntity::Render(const Rgba& playingBackgroundColor) const
{
	playingBackgroundColor;
}
void GameEntity::WriteToXMLNode(XMLNode& node) const
{
	node.addAttribute("currentChar", std::string(1, static_cast<char>(m_char)).c_str());
	node.addAttribute("gameEntityID", std::to_string(m_GameEntityID).c_str());
	node.addAttribute("maxHealth", std::to_string(m_maxHealth).c_str());
	node.addAttribute("name", m_name.c_str());
	node.addAttribute("color", (std::to_string(m_color.Red) + "," + std::to_string(m_color.Green) + "," + std::to_string(m_color.Blue)).c_str());
	node.addAttribute("entityType", std::to_string(m_entityType).c_str());
	node.addAttribute("blTilePos", (std::to_string(m_collision.m_blTilePosition.x) + "," + std::to_string(m_collision.m_blTilePosition.y)).c_str());
	node.addAttribute("size", (std::to_string(m_collision.m_entitySize.x) + "," + std::to_string(m_collision.m_entitySize.y)).c_str());
	node.addAttribute("direction", std::to_string(m_dirToMove).c_str());
	node.addAttribute("wantedBlPos", (std::to_string(m_wantedNewBLPosition.x) + "," + std::to_string(m_wantedNewBLPosition.y)).c_str());
	node.addAttribute("wantedSize", (std::to_string(m_wantedNewEntitySize.x) + "," + std::to_string(m_wantedNewEntitySize.y)).c_str());
}
const bool GameEntity::ReadFromXMLNodeCommonAttributes(GameEntity* common, const StrXMLAttr& attr, const EntityType& expectedID, bool& makeThis)
{
	if (SimpleStrCmp(attr.name, "currentChar"))
	{
		common->SetChar(attr.val.at(0));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "gameEntityID"))
	{
		int gameEntityID = atoi(attr.val.c_str());
		common->SetEntityID(gameEntityID);
		return true;
	}
	else if (SimpleStrCmp(attr.name, "maxHealth"))
	{
		int maxHealth = atoi(attr.val.c_str());
		common->SetMaxHealth(maxHealth);
		return true;
	}
	else if (SimpleStrCmp(attr.name, "name"))
	{
		common->SetEntityName(attr.val);
		return true;
	}
	else if (SimpleStrCmp(attr.name, "color"))
	{
		std::string val = ReplaceCharInString(attr.val, ',', ' ');
		val = ReplaceCharInString(val, '~', ' ');
		std::vector<std::string> vals = ParseString(val);
		float r = (float)atof(vals.at(0).c_str());
		float g = (float)atof(vals.at(1).c_str());
		float b = (float)atof(vals.at(2).c_str());

		common->SetColor(Rgba(r, g, b, 1.f));
		return true;
	}
	else if (SimpleStrCmp(attr.name, "entityType"))
	{
		if (SimpleStrCmp(attr.val, std::to_string(expectedID)) == false)
		{
			makeThis = false;
		}
		return true;
	}
	else if (SimpleStrCmp(attr.name, "blTilePos"))
	{
		std::string val = ReplaceCharInString(attr.val, ',', ' ');
		val = ReplaceCharInString(val, '~', ' ');
		std::vector<std::string> vals = ParseString(val);
		IntVector2 pos(atoi(vals.at(0).c_str()), atoi(vals.at(1).c_str()));
		common->SetBLPosition(pos);
		return true;
	}
	else if (SimpleStrCmp(attr.name, "size"))
	{
		std::string val = ReplaceCharInString(attr.val, ',', ' ');
		val = ReplaceCharInString(val, '~', ' ');
		std::vector<std::string> vals = ParseString(val);
		IntVector2 size(atoi(vals.at(0).c_str()), atoi(vals.at(1).c_str()));
		common->SetSize(size);
		return true;
	}
	else if (SimpleStrCmp(attr.name, "direction"))
	{
		int val = atoi(attr.val.c_str());
		Direction dir = (Direction)val;
		common->SetDirection(dir);
		return true;
	}
	else if (SimpleStrCmp(attr.name, "wantedBlPos"))
	{
		std::string val = ReplaceCharInString(attr.val, ',', ' ');
		val = ReplaceCharInString(val, '~', ' ');
		std::vector<std::string> vals = ParseString(val);
		IntVector2 pos(atoi(vals.at(0).c_str()), atoi(vals.at(1).c_str()));
		common->SetWantedBLPosition(pos);
		return true;
	}
	else if (SimpleStrCmp(attr.name, "wantedSize"))
	{
		std::string val = ReplaceCharInString(attr.val, ',', ' ');
		val = ReplaceCharInString(val, '~', ' ');
		std::vector<std::string> vals = ParseString(val);
		IntVector2 size(atoi(vals.at(0).c_str()), atoi(vals.at(1).c_str()));
		common->SetWantedSize(size);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------
//Setters
void GameEntity::SetDirection(const Direction& dirToMove) 
{
	m_dirToMove = dirToMove; 
}
void GameEntity::SetChar(const unsigned char& car) 
{ 
	m_char = car; 
}
void GameEntity::SetEntityCollision(const EntityCollision& collide) 
{ 
	m_collision = collide; 
}
void GameEntity::SetWantedSize(const IntVector2& entitySize) 
{ 
	m_wantedNewEntitySize = entitySize; 
}
void GameEntity::SetSize(const IntVector2& size)
{
	m_collision.m_entitySize = size;
}
void GameEntity::SetWantedBLPosition(const IntVector2& pos) 
{
	m_wantedNewBLPosition = pos;
}
void GameEntity::SetBLPosition(const IntVector2& position) 
{ 
	m_collision.m_blTilePosition = position; 
}
void GameEntity::SetSpriteSheet(SpriteSheet* spr)
{
	m_spriteSheet = nullptr;
	m_spriteSheet = spr;
}
void GameEntity::ClearSpriteSheet()
{
	m_spriteSheet = nullptr;
}
void GameEntity::SetBLPositionTrue(const IntVector2& position)
{
	m_collision.m_blTilePosition = position;
	m_wantedNewBLPosition = position;
}
void GameEntity::SetColor(const Rgba& color)
{
	m_color = color; 
}
void GameEntity::SetCurrentHealth(const int& health)
{ 
	m_currentHealth = health;
}
void GameEntity::SetMaxHealth(const int& health)
{ 
	m_maxHealth = health; 
}
void GameEntity::SetVisibleToPlayer(const bool& visible)
{ 
	m_visibleToPlayer = visible; 
}
void GameEntity::SetEntityName(const std::string& name)
{ 
	m_name = name; 
}
void GameEntity::SetEntityID(const int& id)
{
	m_GameEntityID = id;
	if (m_GameEntityID >= currentlyAvailableGameEntityID)
	{
		currentlyAvailableGameEntityID = m_GameEntityID + 1;
	}
}
void GameEntity::SetEntityType(const EntityType& type)
{
	m_entityType = type;
}
void GameEntity::SetRenderReguardless(const bool& renderRegardless)
{
	m_renderRegardlessOfVisibility = renderRegardless;
}


//-----------------------------------------------------------------------
//Getters


//------------------------------------------------------------------------
//Static methods
void GameEntity::ResetNextID()
{
	s_nextID = 1;
}

const Direction GameEntity::ForIntEnteredDetermineDirection(int index)
{

	//wait
	switch (index)
	{
	case 0:
		//wait
		return Direction_None;
	case 1:
		//north
		return Direction_North;
	case 2:
		//northeast
		return Direction_NorthEast;
	case 3:
		//east
		return Direction_East;
	case 4:
		//southeast
		return Direction_SouthEast;
	case 5:
		//south
		return Direction_South;
	case 6:
		//southwest
		return Direction_SouthWest;
	case 7:
		//west
		return Direction_West;
	case 8:
		//northwest
		return Direction_NorthWest;
	default:
		//unknown index
		return Direction_Not_picked;
	}
}




//!---------------------------------------------------------
//Getters

const bool GameEntity::GetRenderNow() const
{
	return m_renderRegardlessOfVisibility || m_visibleToPlayer;
}
const Rgba GameEntity::GetColor() const
{
	return m_color;
}
const unsigned char GameEntity::GetChar() const 
{
	return m_char;
}
const EntityCollision GameEntity::GetEntityCollision() const
{ 
	return m_collision; 
}
const IntVector2 GameEntity::GetWantedBLTilePos() const 
{
	return m_wantedNewBLPosition; 
}
const IntVector2 GameEntity::GetBlTilePos() const 
{
	return m_collision.m_blTilePosition; 
}
const IntVector2 GameEntity::GetSize() const 
{
	return m_collision.m_entitySize; 
}
const Direction GameEntity::GetMovementDirection() const
{ 
	return m_dirToMove; 
}
const EntityType GameEntity::GetEntityType() const 
{
	return m_entityType;
}
const int GameEntity::GetCurrentHealth() const 
{ 
	return m_currentHealth; 
}
const int GameEntity::GetMaxHealth() const 
{ 
	return m_maxHealth; 
}
const bool GameEntity::GetVisibleToPlayer() const 
{ 
	return m_visibleToPlayer;
}
const int GameEntity::GetGameEntityID() const 
{ 
	return m_GameEntityID; 
}
const std::string GameEntity::GetEntityName() const 
{
	return m_name;
}