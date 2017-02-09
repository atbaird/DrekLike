#include "Game/GameEntity/AgentType/Player.hpp"
#include "Engine/Core/Input.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameEntity/CombatSystem.hpp"
#include "Game/Items/ItemFactory.hpp"

Player::Player()
	: Agent(ENTITY_PLAYER),
	m_godModeEnabled(false),
	m_itemMenuActive(false),
	m_commaIsDown(false),
	m_qIsDown(false),
	m_aIsDown(false)
{
	m_accuracy = 0.75f;
	std::vector<MovementProperty> moveProps;
	moveProps.push_back(BLOCKED_BY_WALLS);
	moveProps.push_back(SLOWED_BY_WATER);
	moveProps.push_back(HURT_BY_FIRE);
	m_MovementMask = GenMaskFromMovementProperties(moveProps);
	CopyMaskToOriginal();
	SetFaction(Faction::CreateOrGetFactionOfName("player"));
	m_maxAtkPow = 8;
	m_minAtkPow = 5;
	m_name = "Player";
	m_stepCost = 1.f;
	m_visibleToPlayer = true;
	m_maxHealth = 30;
	m_currentHealth = 30;
	SetSightRadius(5.f);
	m_char = 64;
	m_color = Rgba(1.f, 1.f, 1.f, 1.f);
	SetEntityCollision(EntityCollision(IntVector2(0,0), IntVector2(1,1)));
	for (size_t i = 0; i < s_totalValidMoveDirs; i++)
	{
		m_dirKeysHeldDown[i] = false;
	}
	m_keyValsPerDir[0] = VK_OEM_PERIOD; //wait
	m_keyValsPerDir[1] = 'K'; //north
	m_keyValsPerDir[2] = 'U'; //northeast
	m_keyValsPerDir[3] = 'L'; //east
	m_keyValsPerDir[4] = 'N'; //southeast
	m_keyValsPerDir[5] = 'J'; //south
	m_keyValsPerDir[6] = 'B'; //southwest
	m_keyValsPerDir[7] = 'H'; //west
	m_keyValsPerDir[8] = 'Y'; //northwest
}
Player::~Player()
{
	Agent::~Agent();
}


const float Player::Update(const float& deltaSeconds) //Agent::Update(const float& deltaSeconds
{
	m_visibleToPlayer = true;
	float MovementCost = 0.0f;
	deltaSeconds;
	Direction curMove = GetMovementDirection();
	if (curMove == Direction_Not_picked)
	{
		bool changedAlready = false;
		for (size_t i = 0; i < s_totalValidMoveDirs; i++)
		{
			if (m_dirKeysHeldDown[i] == false && g_input->isKeyDown(m_keyValsPerDir[i]) == true)
			{
				m_dirKeysHeldDown[i] = true;
				if (changedAlready == false)
				{
					changedAlready = true;
					curMove = GameEntity::ForIntEnteredDetermineDirection(i);
					SetDirection(curMove);
				}
			}
		}
		if (curMove == Direction_None)
		{
			if (m_currentHealth < m_maxHealth)
			{
				m_currentHealth += 1;
				s_messageBoxStrings.push_back(m_name + " rested a little while and recovered 1 hp.");
			}
			else
			{
				s_messageBoxStrings.push_back(m_name + " rested a little while.");
			}
		}
		if (g_map == nullptr)
		{
			SetDirection(Direction_Not_picked);
		}
		else if (curMove == Direction_Not_picked)
		{
			if (g_input->isKeyDown(VK_OEM_COMMA) == true && m_commaIsDown == false)
			{
				m_commaIsDown = true;
				bool pickedUpItem = TryPickUpItemBelowMe();
				if (pickedUpItem)
				{
					curMove = Direction_None;
					SetDirection(curMove);
					MovementCost = m_stepCost;
				}
				
			}
			if (m_qIsDown == false && g_input->isKeyDown('Q') == true)
			{
				m_qIsDown = true;
				bool drank = DrinkRandomPotionFromInv();
				if (drank == true)
				{
					curMove = Direction_None;
					SetDirection(curMove);
					MovementCost = m_stepCost;
				}
			}
			if (m_aIsDown == false && g_input->isKeyDown('A') == true)
			{
				m_aIsDown = true;
				bool toggledOne = ToggleFeaturesOnOrOffThatAreAroundThis();
				if (toggledOne == true)
				{
					curMove = Direction_None;
					SetDirection(curMove);
					MovementCost = m_stepCost;
				}
			}

		}
		else
		{
			std::map<Tile_Coord, Tile*>* tiles = g_map->GetMapTiles();
			IntVector2 direction = ConvertDirectionEnumToTileCoord(curMove);
			IntVector2 wantedBLPos = m_collision.m_blTilePosition + direction;
			IntVector2 wantedTRPos = wantedBLPos + IntVector2(m_collision.m_entitySize.x - 1, m_collision.m_entitySize.y - 1);
			for (int y = wantedBLPos.y; y <= wantedTRPos.y; y++)
			{
				if (y == wantedBLPos.y || y == wantedTRPos.y)
				{
					for (int x = wantedBLPos.x; x <= wantedTRPos.x; x++)
					{
						if (x == wantedBLPos.x || x == wantedTRPos.x)
						{
							IntVector2 truePositionTest = IntVector2(x, y);
							std::map<Tile_Coord, Tile*>::iterator it = tiles->find((truePositionTest));
							if (it == tiles->end() || it->second == nullptr)
							{
								curMove = Direction_Not_picked;
								SetDirection(curMove);
							}
							else
							{
								TileType type = it->second->GetTileType();
								bool exists = false;
								bool solid = TileDefinition::GetTypeSolid(type, exists);
								if (exists == false || solid == true)
								{
									curMove = Direction_Not_picked;
									SetDirection(curMove);
								}
							}
						}
					}
				}
			}
			if (curMove != Direction_Not_picked)
			{
				m_wantedNewBLPosition = wantedBLPos;
			}
		}
	}
	else
	{
		MovementCost = m_stepCost;
	}

	for (int i = 0; i < s_totalValidMoveDirs; i++)
	{
		if (m_dirKeysHeldDown[i] == true && g_input->isKeyDown(m_keyValsPerDir[i]) == false)
		{
			m_dirKeysHeldDown[i] = false;
		}
	}
	if (m_commaIsDown == true && g_input->isKeyDown(VK_OEM_COMMA) == false)
	{
		m_commaIsDown = false;
	}
	if (m_qIsDown == true && g_input->isKeyDown('Q') == false)
	{
		m_qIsDown = false;
	}
	if (m_aIsDown == true && g_input->isKeyDown('A') == false)
	{
		m_aIsDown = false;
	}
	if (m_dirToMove != Direction_None && m_dirToMove != Direction_Not_picked)
	{
		AttackData slowed = CheckTileSlowAgent();
		if (slowed.reason != Reason_None)
		{
			MovementCost *= 2.f;
		}
	}

	return MovementCost;
}
void Player::UpdateStep() //Agent::UpdateStep()
{
	if (m_dirToMove != Direction_Not_picked)
	{
		bool attacking = false;
		Agent* attackingThis = nullptr;
		for (size_t i = 0; i < Agent::s_Agents.size(); i++)
		{
			Agent* age = s_Agents.at(i);
			if (age != this)
			{
				IntVector2 bl = age->GetBlTilePos();
				IntVector2 tr = bl + age->GetSize() - IntVector2(1, 1);
				if (m_wantedNewBLPosition >= bl && m_wantedNewBLPosition <= tr)
				{
					attacking = true;
					attackingThis = age;
					break;
				}
			}
		}
		if (attacking == false)
		{
			std::map<Tile_Coord, Tile*>::const_iterator it = g_map->GetMapTiles()->find(m_wantedNewBLPosition);
			if (it != g_map->GetMapTiles()->end() && it->second != nullptr)
			{
				TileType type = it->second->GetTileType();
				std::map<TileType, TileDefinition>::const_iterator itd = g_TileDefinitions.find(type);
				if (itd != g_TileDefinitions.end())
				{
					bool flying = GetFlying();
					TileDefinition def = itd->second;
					if (flying == false && def.GetIsFire() && GetHurtByFire() == true && GetFlying() == false)
					{
						m_currentHealth -= 5;
						if (m_visibleToPlayer == true)
						{
							s_messageBoxStrings.push_back(m_name + " was hurt by the fire.");
						}
					}
				}
			}
			if (m_collision.m_blTilePosition != m_wantedNewBLPosition)
			{
				for (size_t i = 0; i < Item::s_ItemsOnTheGround.size(); i++)
				{
					IntVector2 bl = Item::s_ItemsOnTheGround.at(i)->GetBlTilePos();
					if (bl == m_wantedNewBLPosition)
					{
						s_messageBoxStrings.push_back("There is a " + Item::s_ItemsOnTheGround.at(i)->GetEntityName() + " here.");
					}
				}
			}
			m_collision.m_blTilePosition = m_wantedNewBLPosition;
		}
		else
		{
			AttackData data;
			data.instigator = this;
			data.target = attackingThis;
			data.m_atkType = AttackType_Melee;
			data.accuracy = m_accuracy;
			CombatSystem::Attack(data);
			m_wantedNewBLPosition = m_collision.m_blTilePosition;
		}
		SetDirection(Direction_Not_picked);
		SetMoved(true);
		CycleThroughStatusEffects();
	}
	else
	{
		SetDirection(Direction_Not_picked);
		SetMoved(false);
	}
	if (m_godModeEnabled == true)
	{
		m_alive = true;
		if (m_currentHealth < 1)
		{
			m_currentHealth = 1;
		}
	}
}
void Player::Render(const Rgba& playingBackgroundColor) const //Agent::Render() const
{
	Vector2 bl, blBack;
	Vector2 tr, trBack;
	std::vector<Vertex_PCT> vertices;
	if (m_spriteSheet != nullptr)
	{
		AABB2 sprValBack = m_spriteSheet->GetTexCoordsForSpriteIndex(1);
		AABB2 sprVal = m_spriteSheet->GetTexCoordsForSpriteIndex(m_char);
		bl = Vector2(sprVal.mins.x, sprVal.maxs.y);
		tr = Vector2(sprVal.maxs.x, sprVal.mins.y);
		blBack = Vector2(sprValBack.mins.x, sprValBack.mins.y);
		trBack = Vector2(sprValBack.maxs.x, sprValBack.maxs.y);
	}
	if (g_map != nullptr)
	{
		vertices = g_map->GetVerticesForPosition(m_collision.m_blTilePosition, playingBackgroundColor, blBack, trBack);
		std::vector<Vertex_PCT> foregroundVerts = g_map->GetVerticesForPosition(m_collision.m_blTilePosition, m_color, bl, tr);
		vertices.insert(vertices.end(), foregroundVerts.begin(), foregroundVerts.end());
	}
	if (vertices.size() > 0)
	{
		g_Renderer->DrawVertexArray(vertices, PRIMITIVE_QUADS, m_spriteSheet->getSpriteSheetTexture());
	}
}
void Player::RenderInventory(const Vector3& startPos, Font* font, const float& fontSize)
{
	if (font == nullptr || m_itemMenuActive == false)
	{
		return;
	}
	m_inventory.Render(startPos, font, fontSize);
}
const bool Player::IsReadyToUpdate() const
{
	if (GetMovementDirection() == Direction_Not_picked)
	{
		return false;
	}
	return true;
}
void Player::WriteToXMLNode(XMLNode& node) const
{
	//XMLNode child = node.addChild("FeatureBlueprint");
	XMLNode child = node.addChild("PlayerBlueprint");
	Agent::WriteToXMLNode(child);
}
void Player::ReadFromXMLNode(GenerationProcessData* data)
{
	std::vector<StrXMLAttr> attrs = data->GetAttributes();
	bool makeThis = true;
	for (size_t i = 0; i < attrs.size() && makeThis; i++)
	{
		Agent::ReadFromXMLNodeCommonAttributes(this, attrs.at(i), ENTITY_PLAYER, makeThis);
	}

	std::vector<GenerationProcessData*> children = data->GetChildren();
	for (size_t i = 0; i < children.size() && makeThis; i++)
	{
		Agent::ReadFromXMLNodeCommonChildren(this, children.at(i), makeThis);
	}
}



//Setters
void Player::ToggleGodMode()
{
	m_godModeEnabled = !m_godModeEnabled;
}
void Player::ToggleItemMenuActive()
{
	m_itemMenuActive = !m_itemMenuActive;
}


//Getters

const bool Player::GetGodModeEnabled() const
{
	return m_godModeEnabled;
}
const bool Player::GetItemMenuActive() const
{
	return m_itemMenuActive;
}