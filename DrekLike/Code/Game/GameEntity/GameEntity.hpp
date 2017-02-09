#pragma once
#include "Engine/Entity/Entity.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Rgba.hpp"
#include "Game/GameCommonFunctions.hpp"
#include "Game/Generators/EnvironmentGenerationProcess.hpp"

#ifndef GAMEENTITY_HPP
#define GAMEENTITY_HPP

enum EntityType
{
	ENTITY_INVALID = -1,
	ENTITY_ITEM,
	ENTITY_NPC,
	ENTITY_PLAYER,
	ENTITY_FEATURE,
	NUMBER_OF_ENTITY_TYPES
};
enum Direction
{
	Direction_Not_picked = -1,
	Direction_None, //0
	Direction_North, //1
	Direction_NorthEast, //2
	Direction_East, //3
	Direction_SouthEast, //4
	Direction_South, //5
	Direction_SouthWest, //6
	Direction_West,
	Direction_NorthWest,
	NUMBER_OF_DIRECTIONS
};

struct EntityCollision
{
	EntityCollision(const IntVector2& blPos = IntVector2(0,0), const IntVector2& size = IntVector2(1,1));
	IntVector2 m_blTilePosition;
	IntVector2 m_entitySize;
};

const IntVector2 ConvertDirectionEnumToTileCoord(const Direction& dir);
const Vector2 ConvertDirectionToVec2Dir(const Direction& dir);

class GameEntity : public Entity
{
protected:
	static int currentlyAvailableGameEntityID;

	unsigned char m_char;
	bool m_renderRegardlessOfVisibility;
	bool m_visibleToPlayer;
	int m_GameEntityID;
	int m_currentHealth;
	int m_maxHealth;
	std::string m_name;
	Rgba m_color;
	EntityType m_entityType;
	EntityCollision m_collision;
	Direction m_dirToMove;
	IntVector2 m_wantedNewBLPosition;
	IntVector2 m_wantedNewEntitySize;
	SpriteSheet* m_spriteSheet;
private:

public:
	//Constructors
	GameEntity(const EntityType& type, const IntVector2& blTilePos = IntVector2(0,0), const IntVector2& entitySize = IntVector2(1,1));
	GameEntity(const GameEntity& other);
	virtual ~GameEntity() override; //Entity::~Entity();

	//Updates and Render
	virtual const float Update(const float& deltaSeconds) override; //Entity::Update(const float& deltaSeconds)
	virtual void UpdateStep();
	virtual void Render() const override;
	virtual void Render(const Rgba& playingBackgroundColor) const; //Entity::Render() const;
	virtual void WriteToXMLNode(XMLNode& node) const;
	static const bool ReadFromXMLNodeCommonAttributes(GameEntity* common, const StrXMLAttr& attr, const EntityType& expectedID, bool& makeThis);

	//setters
	virtual void SetSpriteSheet(SpriteSheet* spr);
	virtual void ClearSpriteSheet();
	virtual void SetDirection(const Direction& dirToMove);
	virtual void SetChar(const unsigned char& car);
	virtual void SetEntityCollision(const EntityCollision& collide);
	virtual void SetWantedSize(const IntVector2& entitySize);
	virtual void SetSize(const IntVector2& size);
	virtual void SetWantedBLPosition(const IntVector2& pos);
	virtual void SetBLPosition(const IntVector2& position);
	virtual void SetBLPositionTrue(const IntVector2& position);
	virtual void SetColor(const Rgba& color);
	virtual void SetCurrentHealth(const int& health);
	virtual void SetMaxHealth(const int& health);
	virtual void SetVisibleToPlayer(const bool& visible);
	virtual void SetEntityName(const std::string& name);
	virtual void SetEntityID(const int& id);
	virtual void SetEntityType(const EntityType& type);
	virtual void SetRenderReguardless(const bool& renderRegardless);
	//Getters
	virtual const bool GetRenderNow() const;
	virtual const Rgba GetColor() const;
	virtual const unsigned char GetChar() const;
	virtual const EntityCollision GetEntityCollision() const;
	virtual const IntVector2 GetWantedBLTilePos() const;
	virtual const IntVector2 GetBlTilePos() const;
	virtual const IntVector2 GetSize() const;
	virtual const Direction GetMovementDirection() const;
	virtual const EntityType GetEntityType() const;
	virtual const int GetCurrentHealth() const;
	virtual const int GetMaxHealth() const;
	virtual const bool GetVisibleToPlayer() const;
	virtual const int GetGameEntityID() const;
	virtual const std::string GetEntityName() const;

	//Static methods
	static void ResetNextID();
	static const Direction ForIntEnteredDetermineDirection(int index);

	static unsigned int s_nextID;
};
#endif