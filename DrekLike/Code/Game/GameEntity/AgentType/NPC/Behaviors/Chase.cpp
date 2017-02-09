#include "Game/GameEntity/AgentType/NPC/Behaviors/Chase.hpp"
#include "Engine/Core/StringUtils.hpp"

Behavior* DefaultCreatChaseBehavior()
{
	return new Chase();
}

Behavior* CreateChaseBehavior(const XMLNode& node)
{
	return new Chase(node);
}

const BehaviorRegistrationHelper Chase::s_ChaseBehaviorRegistration("Chase", CreateChaseBehavior, DefaultCreatChaseBehavior);

//Constructors
Chase::Chase()
	: Behavior(),
	turnsBeenChasingFor(0),
	numTurnsToChaseBeforeGiveUp(30),
	distanceTest(15.f),
	m_targetToChase(nullptr),
	targetOutOfView(false),
	chaseTargetName("")
{
	m_name = "Chase";
	ClearData();
}
Chase::Chase(const XMLNode& node)
	: Behavior("Chase", node),
	turnsBeenChasingFor(0),
	numTurnsToChaseBeforeGiveUp(30),
	distanceTest(15.f),
	m_targetToChase(nullptr),
	targetOutOfView(false),
	chaseTargetName("")
{
	std::string turnsToChase = m_data->GetAttrValueByName("maximumDuration");
	std::string distanceOutBeforeStart = m_data->GetAttrValueByName("maximumDistanceAwayToStart");
	chaseTargetName = m_data->GetAttrValueByName("chaseTargetName");
	if (SimpleStrCmp(turnsToChase, "") != true)
	{
		numTurnsToChaseBeforeGiveUp = atoi(turnsToChase.c_str());
	}
	if (SimpleStrCmp(distanceOutBeforeStart, "")  != true)
	{
		distanceTest = (float)atof(distanceOutBeforeStart.c_str());
		if (distanceTest < 0.f)
		{
			distanceTest = 0.f;
		}
	}
}
Chase::Chase(const Chase& other)
	: Behavior(other),
	turnsBeenChasingFor(other.turnsBeenChasingFor),
	numTurnsToChaseBeforeGiveUp(other.numTurnsToChaseBeforeGiveUp),
	distanceTest(other.distanceTest),
	chaseTargetName(other.chaseTargetName),
	m_targetToChase(nullptr),
	targetOutOfView(false)
{
}
Chase::~Chase()
{
	Behavior::~Behavior();
}
Behavior* Chase::Clone()
{
	return new Chase(*this);
}
const float Chase::CalcUtility()
{
	if (m_owner == nullptr)
	{
		turnsBeenChasingFor = 0;
		targetOutOfView = false;
		return 0.0f;
	}
	if (SimpleStrCmp(chaseTargetName, "") == false)
	{
		std::vector<Agent*> visibleAgents = m_owner->GetVisibleAgents();
		for (size_t i = 0; i < visibleAgents.size(); i++)
		{
			Agent* age = visibleAgents.at(i);
			if (age != nullptr && SimpleStrCmp(age->GetEntityName(), chaseTargetName))
			{
				m_targetToChase = age;
				
				break;
			}
		}
		if (turnsBeenChasingFor <= numTurnsToChaseBeforeGiveUp && m_targetToChase != nullptr)
		{
			return 40.f;
		}
	}
	else
	{
		Agent* targetOfHate = m_owner->GetTargetOfHate();
		if (targetOfHate != nullptr)
		{
			if (targetOfHate->GetIsAlive() == false)
			{
				targetOfHate = nullptr;
			}
		}
		if (m_targetToChase != nullptr)
		{
			if (m_targetToChase->GetIsAlive() == false)
			{
				m_targetToChase = nullptr;
			}
		}

		if (targetOfHate == nullptr && m_targetToChase == nullptr)
		{
			turnsBeenChasingFor = 0;
			targetOfHate = false;
			return 0.0f;
		}
		if (m_targetToChase == nullptr && targetOfHate != nullptr)
		{
			m_targetToChase = nullptr;
			m_targetToChase = targetOfHate;
			targetOutOfView = false;

		}
		else if (m_targetToChase != nullptr && targetOfHate == nullptr)
		{
			targetOutOfView = true;
		}
		else if (m_targetToChase != nullptr && targetOfHate != nullptr)
		{
			try
			{
				bool didChange = false;
				targetOutOfView = false;

				int targetChase = 0;
				int newTarget = 0;
				Faction* fac = m_owner->GetHomeFaction();
				std::vector<AgentRelationship>* rels = m_owner->GetIndividualRelationshipData();
				for (size_t i = 0; i < rels->size(); i++)
				{
					if (rels->at(i).GetAgentID() == m_targetToChase->GetAgentID())
					{
						targetChase = rels->at(i).GetRelationship();
					}
					else if (rels->at(i).GetAgentID() == m_owner->GetTargetOfHate()->GetAgentID())
					{
						newTarget = rels->at(i).GetRelationship();
					}
				}
				targetChase += fac->GetFactionRelationShipForID(m_targetToChase->GetFactionID());
				newTarget += fac->GetFactionRelationShipForID(m_owner->GetTargetOfHate()->GetAgentID());
				if (targetChase >= newTarget)
				{
					targetOutOfView = true;
				}
				else
				{
					didChange = true;
					turnsBeenChasingFor = 0;
					m_targetToChase = nullptr;
					m_targetToChase = m_owner->GetTargetOfHate();
				}
			}
			catch (...)
			{
				m_targetToChase = nullptr;
				m_targetToChase = targetOfHate;
			}
		}

		if (turnsBeenChasingFor <= numTurnsToChaseBeforeGiveUp && m_targetToChase != nullptr)
		{
			return 40.f;
		}


	}
	turnsBeenChasingFor = 0;
	m_targetToChase = nullptr;


	return 0.0f;
}
void Chase::Run()
{
	if (m_targetToChase == nullptr)
	{
		return;
	}
	if (targetOutOfView == false)
	{
		turnsBeenChasingFor += 1;
	}
	else if (targetOutOfView == true)
	{
		turnsBeenChasingFor += 2;
	}
	try
	{
		IntVector2 cur = m_owner->GetBlTilePos();
		IntVector2 target = m_targetToChase->GetBlTilePos();
		m_algo.CalculateAStarAlgo(g_map, cur, target, m_owner->GetMask());
		bool exists;
		IntVector2 wanted = m_algo.GetNextPositionFromClosedListForPosition(cur, exists);
		if (exists)
		{
			IntVector2 dir = (m_owner->GetBlTilePos() - wanted);
			dir.normalize();
			bool isTargetPos = false;
			VerifyPositionIsNotCurrentlyOrGoingToBeOccupied(wanted, cur, dir, isTargetPos);
			m_owner->SetWantedBLPosition(wanted);
		}
	}
	catch (...)
	{
		m_owner->SetWantedBLPosition(m_owner->GetBlTilePos());
	}
}
const bool Chase::VerifyPositionIsNotCurrentlyOrGoingToBeOccupied(IntVector2& pos, const IntVector2& orPos, const IntVector2& dirMoving, bool& isTargetPos)
{
	bool taken = VerifyPositionNotTaken(pos, isTargetPos);
	if (taken == false || isTargetPos == true)
	{
		return true;
	}

	//Y test
	Tile* tile;
	if (abs(dirMoving.y) > 0)
	{
		pos.x = orPos.x;
		pos.y = orPos.y + dirMoving.y;
		bool solid = g_map->GetIsTileCoordSolid(pos, tile, m_owner->GetMask());
		if (solid == false)
		{
			taken = VerifyPositionNotTaken(pos, isTargetPos);
			if (taken == false || isTargetPos == true)
			{
				return true;
			}
		}
	}
	else if (abs(dirMoving.y) == 0)
	{
		pos.x = orPos.x + dirMoving.x;
		pos.y = pos.y + 1;
		bool solid = g_map->GetIsTileCoordSolid(pos, tile, m_owner->GetMask());
		if (solid == false)
		{
			taken = VerifyPositionNotTaken(pos, isTargetPos);
			if (taken == false || isTargetPos == true)
			{
				return true;
			}
		}
		pos.y = orPos.y - 1;
		solid = g_map->GetIsTileCoordSolid(pos, tile, m_owner->GetMask());
		if (solid == false)
		{
			taken = VerifyPositionNotTaken(pos, isTargetPos);
			if (taken == false || isTargetPos == true)
			{
				return true;
			}
		}
	}

	//X test
	if (abs(dirMoving.x) > 0)
	{
		pos.x = orPos.x + dirMoving.x;
		pos.y = orPos.y;
		bool solid = g_map->GetIsTileCoordSolid(pos, tile, m_owner->GetMask());
		if (solid == false)
		{
			taken = VerifyPositionNotTaken(pos, isTargetPos);
			if (taken == false || isTargetPos == true)
			{
				return true;
			}
		}
	}
	else if (abs(dirMoving.x) == 0)
	{
		pos.x = orPos.x + 1;
		pos.y = orPos.y;
		bool solid = g_map->GetIsTileCoordSolid(pos, tile, m_owner->GetMask());
		if (solid == false)
		{
			taken = VerifyPositionNotTaken(pos, isTargetPos);
			if (taken == false || isTargetPos == true)
			{
				return true;
			}
		}
		pos.x = orPos.x - 1;
		solid = g_map->GetIsTileCoordSolid(pos, tile, m_owner->GetMask());
		if (solid == false)
		{
			taken = VerifyPositionNotTaken(pos, isTargetPos);
			if (taken == false || isTargetPos == true)
			{
				return true;
			}
		}
	}

	pos -= dirMoving;
	return false;
}
const bool Chase::VerifyPositionNotTaken(const IntVector2& position, bool& isTargetPos)
{
	isTargetPos = false;
	bool taken = false;
	for (size_t i = 0; i < Agent::s_Agents.size(); i++)
	{
		Agent* curAge = Agent::s_Agents.at(i);
		IntVector2 bl = curAge->GetBlTilePos();
		IntVector2 wanted = curAge->GetWantedBLTilePos();
		if (position == bl)
		{
			if (curAge == m_targetToChase)
			{
				isTargetPos = true;
				return taken;
			}

			if (bl == wanted)
			{
				taken = true;
				break;
			}
		}
		else if (position == wanted)
		{
			taken = true;
			break;
		}
	}
	return taken;
}

void Chase::ClearNecessaryDataForIfTargetsDead()
{
	if (m_targetToChase != nullptr && m_targetToChase->GetIsAlive() == false)
	{
		m_targetToChase = nullptr;
	}
}
void Chase::WriteToXMLNode(XMLNode& node)
{
	XMLNode chase = node.addChild("Chase");
	chase.addAttribute("turnsBeenChasingFor", std::to_string(turnsBeenChasingFor).c_str());
	chase.addAttribute("maximumDuration", std::to_string(numTurnsToChaseBeforeGiveUp).c_str());
	chase.addAttribute("maximumDistanceAwayToStart", std::to_string(distanceTest).c_str());
	chase.addAttribute("chaseTargetName", chaseTargetName.c_str());
}
void Chase::ReadFromXMLNode(GenerationProcessData* data)
{
	turnsBeenChasingFor = atoi(data->GetAttributeByNameString("turnsBeenChasingFor").c_str());
	numTurnsToChaseBeforeGiveUp = atoi(data->GetAttributeByNameString("maximumDuration").c_str());
	distanceTest = (float)atof(data->GetAttributeByNameString("maximumDistanceAwayToStart").c_str());
	chaseTargetName = data->GetAttributeByNameString("chaseTargetName");
}