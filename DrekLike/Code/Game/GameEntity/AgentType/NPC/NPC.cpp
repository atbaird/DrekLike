#include "Game/GameEntity/AgentType/NPC/NPC.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/FieldOfView/FieldOfViewFloodFill.hpp"

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//Constructors

NPC::NPC(const IntVector2& blTilePos, const unsigned char& car, const Rgba& color, const IntVector2& entitySize, const float& stepCost)
	: Agent(ENTITY_NPC),
	m_baseHatredScale(0)
{
	m_stepCost = stepCost;
	m_char = car;
	m_color = color;
	SetEntityCollision(EntityCollision(blTilePos, entitySize));
}
NPC::NPC(const NPC& other)
	: Agent(other),
	m_baseHatredScale(other.m_baseHatredScale)
{
	for (size_t i = 0; i < other.m_behaviors.size(); i++)
	{
		if (other.m_behaviors.at(i) != nullptr)
		{
			std::string name = other.m_behaviors.at(i)->GetName();
			Behavior* bev = other.m_behaviors.at(i);
			if (bev != nullptr)
			{
				m_behaviors.push_back(bev->Clone());
			}
		}
	}
}
NPC::~NPC()
{
	for (size_t i = 0; i < m_behaviors.size(); i++)
	{
		if (m_behaviors.at(i) != nullptr)
		{
			delete m_behaviors[i];
			m_behaviors[i] = nullptr;
		}
	}
	m_behaviors.clear();
	Agent::~Agent();
}


const bool NPC::DoesHaveThisBehavior(const std::string& name)
{
	for (size_t i = 0; m_behaviors.size(); i++)
	{
		if (m_behaviors.at(i)->GetName() == name)
		{
			return true;
		}
	}
	return false;
}
const bool NPC::AddBehavior(Behavior* bev)
{
	m_behaviors.push_back(bev->Clone());
	return true;
}
const bool NPC::RemoveBehavior(const std::string& name)
{
	if (name == "")
	{
		return false;
	}

	for (size_t i = 0; i < m_behaviors.size(); i++)
	{
		if (m_behaviors.at(i)->GetName() == name)
		{
			m_behaviors.erase(m_behaviors.begin() + i);
			return true;
		}
	}
	return false;
}


const float NPC::Update(const float& deltaSeconds) //GameEntity::Update(const float& deltaSeconds)
{
	//UtilityRun
	std::vector<Agent*> visibleAgents;
	FieldOfViewFloodFill::CalculateFieldOfViewForAgent(this, m_sightRange, g_map, visibleAgents, false);
	SetVisibleAgents(visibleAgents);

	int highestFriendShipRating = -2000000;
	int highestHatredRating = 2000000;
	int indexOfFriendship = -1;
	int indexOfHatred = -1;
	bool isLastTargetOfHateStillThere = false;

	for (size_t i = 0; i < visibleAgents.size(); i++)
	{
		if (visibleAgents.at(i) != nullptr)
		{
			if (visibleAgents.at(i) == m_lastTargetOfHatred)
			{
				isLastTargetOfHateStillThere = true;
			}
			bool idExists = false;
			int visAgentID = visibleAgents.at(i)->GetAgentID();
			int hatred = 0;
			int ageRel = -1;

			for (size_t j = 0; j < m_ageRelations.size(); j++)
			{
				if (m_ageRelations.at(j).GetAgentID() == visAgentID)
				{
					idExists = true;
					ageRel = j;
					break;
				}
			}
			if (idExists == true)
			{
				Faction* fact = visibleAgents.at(i)->GetHomeFaction();
				hatred = m_faction->EvaluateHatredScale(fact);
				hatred += m_ageRelations.at(ageRel).GetRelationship();
			}
			else
			{
				Faction* fact = visibleAgents.at(i)->GetHomeFaction();
				if (m_faction == nullptr)
				{
					m_faction = Faction::CreateOrGetFactionOfName("default");
				}

				hatred = m_faction->EvaluateHatredScale(fact);
				AgentRelationship data;
				data.SetAgentID(visAgentID);
				data.SetRelationship(hatred + m_baseHatredScale);
				m_ageRelations.push_back(data);
			}

			if (m_personWhoAttackedLast != nullptr && visAgentID == m_personWhoAttackedLast->GetAgentID())
			{
				hatred -= 20;
			}

			if (hatred >= s_friendly && hatred > highestFriendShipRating)
			{
				highestFriendShipRating = hatred;
				indexOfFriendship = i;
			}
			else if (hatred <= s_hate && hatred < highestHatredRating)
			{
				highestHatredRating = hatred;
				indexOfHatred = i;
			}
		}
	}
	Agent* age = nullptr;
	if (indexOfHatred != -1)
	{
		age = visibleAgents.at(indexOfHatred);

		m_targetOfHatred = nullptr;
		m_targetOfHatred = age;
		m_lastTargetOfHatred = nullptr;
		m_lastTargetOfHatred = age;
	}
	else
	{
		if (isLastTargetOfHateStillThere == true)
		{
			age = m_lastTargetOfHatred;
			SetTargetOfHatred(m_lastTargetOfHatred);
		}
		else if (m_attackedLastTurn == true)
		{
			age = m_personWhoAttackedLast;
			SetTargetOfHatred(m_personWhoAttackedLast);
			m_lastTargetOfHatred = nullptr;
			m_lastTargetOfHatred = m_personWhoAttackedLast;
		}
	}
	if (indexOfFriendship != -1)
	{
		SetTargetOfFriendship(visibleAgents.at(indexOfFriendship));
	}
	else
	{
	}

	int index = -1;
	float greatestUtil = 0.0f;
	m_targetOfHatred = age;
	for (size_t i = 0; i < m_behaviors.size(); i++)
	{
		Behavior* bev = m_behaviors.at(i);
		if (bev != nullptr)
		{
			float curUtil = bev->CalcUtility();
			if (curUtil > greatestUtil)
			{
				index = i;
				greatestUtil = curUtil;
			}
		}
	}
	if (index != -1)
	{
		m_behaviors.at(index)->Run();
	}

	float cost = m_stepCost;

	if (m_wantedNewBLPosition != m_collision.m_blTilePosition)
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
				if (flying == false && def.GetIsFire() && GetSlowedByFire() == true)
				{
					cost *= 2.f;
				}
				else if (flying == false && def.GetIsLiquid() == true && GetSlowedByWater() == true)
				{
					cost *= 2.f;
				}
				else if (def.GetIsWall() == true && GetSlowedByWalls() == true)
				{
					cost *= 2.f;
				}
			}
		}
	}

	deltaSeconds;
	m_attackedLastTurn = false;
	return m_stepCost;
}
void NPC::UpdateStep() //GameEntity::UpdateStep()
{
	if (GetBlTilePos() == m_wantedNewBLPosition)
	{
		m_dirToMove = Direction_None;
	}
	else
	{
		IntVector2 diff = m_wantedNewBLPosition - m_collision.m_blTilePosition;
		if (diff.x < 0)
		{
			if (diff.y < 0)
			{
				m_dirToMove = Direction_SouthWest;
			}
			else if (diff.y > 0)
			{
				m_dirToMove = Direction_NorthWest;
			}
			else
			{
				m_dirToMove = Direction_West;
			}
		}
		else if (diff.x > 0)
		{
			if (diff.y < 0)
			{
				m_dirToMove = Direction_SouthEast;
			}
			else if (diff.y > 0)
			{
				m_dirToMove = Direction_NorthEast;
			}
			else
			{
				m_dirToMove = Direction_East;
			}
		}
		else
		{
			if (diff.y < 0)
			{
				m_dirToMove = Direction_South;
			}
			else if (diff.y > 0)
			{
				m_dirToMove = Direction_North;
			}
			else
			{
				m_dirToMove = Direction_None;
			}
		}
	}
	SetBLPosition(m_wantedNewBLPosition);
	CycleThroughStatusEffects();
	if (m_dirToMove != Direction_None)
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
				if (flying == false && def.GetIsFire() && GetHurtByFire() == true)
				{
					m_currentHealth -= 5;
					if (m_visibleToPlayer == true)
					{
						s_messageBoxStrings.push_back(m_name + " was hurt by the fire.");
					}
				}
			}
		}
	}
}
void NPC::Render(const Rgba& playingBackgroundColor) const //GameEntity::Render() const
{
	if (GetVisibleToPlayer() == true || m_renderRegardlessOfVisibility == true)
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
}
void NPC::FinalPass()
{
	Agent::FinalPass();
	for (size_t i = 0; i < m_behaviors.size(); i++)
	{
		m_behaviors.at(i)->ClearNecessaryDataForIfTargetsDead();
	}
}
void NPC::WriteToXMLNode(XMLNode& node) const
{
	XMLNode child = node.addChild("NPCBlueprint");
	Agent::WriteToXMLNode(child);

	child.addAttribute("baseHatredScale", std::to_string(m_baseHatredScale).c_str());
	XMLNode behaviors = child.addChild("Behaviors");

	for (size_t i = 0; i < m_behaviors.size(); i++)
	{
		m_behaviors.at(i)->WriteToXMLNode(behaviors);
	}
}
const bool NPC::ReadFromXMLNode(GenerationProcessData* data)
{
	std::vector<StrXMLAttr> attrs = data->GetAttributes();
	bool makeThis = true;
	for (size_t i = 0; i < attrs.size() && makeThis; i++)
	{
		StrXMLAttr attr = attrs.at(i);
		if (Agent::ReadFromXMLNodeCommonAttributes(this, attr, ENTITY_NPC, makeThis))
		{
			continue;
		}
		else if (SimpleStrCmp(attr.val, "baseHatredScale"))
		{
			m_baseHatredScale = atoi(attr.val.c_str());
		}
	}
	std::vector<GenerationProcessData*> children = data->GetChildren();
	for (size_t i = 0; i < children.size() && makeThis; i++)
	{
		GenerationProcessData* child = children.at(i);
		if (Agent::ReadFromXMLNodeCommonChildren(this, child, makeThis))
		{
			continue;
		}
		else if (SimpleStrCmp(child->GetName().name,"Behaviors"))
		{
			std::vector<GenerationProcessData*> behaviors = child->GetChildren();
			for (size_t bevIdx = 0; bevIdx < behaviors.size(); bevIdx++)
			{
				GenerationProcessData* bevhaviorDat = behaviors.at(bevIdx);
				if (bevhaviorDat != nullptr)
				{
					std::string name = bevhaviorDat->GetName().name;
					Behavior* bev = BehaviorRegistrationHelper::CreateDefaultBehaviorByName(name);
					bev->SetAgent(this);
					bev->ReadFromXMLNode(bevhaviorDat);
					m_behaviors.push_back(bev);

				}
			}
		}
	}

	return makeThis;
}

NPC* NPC::CreateNPC()
{
	return new NPC();
}


//Setters
void NPC::SetBaseHatredScale(const int& scale)
{
	m_baseHatredScale = scale;
}

//Getters
const int NPC::GetBaseHatredScale() const
{
	return m_baseHatredScale;
}