#include "Game/GameEntity/AgentType/NPC/Behaviors/Patrol.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Core/StringUtils.hpp"

Behavior* DefaultCreatePatrolBehavior()
{
	return new Patrol();
}

Behavior* CreatePatrolBehavior(const XMLNode& node)
{
	return new Patrol(node);
}

const BehaviorRegistrationHelper Patrol::s_PatrolBehaviorRegistration("Patrol", CreatePatrolBehavior, DefaultCreatePatrolBehavior);

Patrol::Patrol()
	:Behavior(),
	m_patrolSet(false),
	m_patrolRadiusFromOriginalPosition(20.f),
	m_minNumOfPatrolPoints(3),
	m_maxNumOfPatrolPoints(3),
	m_currentPatrolInd(0)
{
	m_name = "Patrol";
	ClearData();
}

Patrol::Patrol(const XMLNode& node)
	: Behavior("Patrol", node),
	m_patrolSet(false),
	m_patrolRadiusFromOriginalPosition(20.f),
	m_minNumOfPatrolPoints(3),
	m_maxNumOfPatrolPoints(3),
	m_currentPatrolInd(0)
{
	std::string radius = m_data->GetAttrValueByName("radius");
	std::string numPatrolPoints = m_data->GetAttrValueByName("numberOfPatrolPoints");
	std::string currentPatrol = m_data->GetAttrValueByName("currentPatrolIdx");
	std::vector<GenerationProcessData*> patrolPoints;
	std::vector<GenerationProcessData*> children = m_data->GetChildren();
	for (size_t i = 0; i < children.size(); i++)
	{
		if (SimpleStrCmp(children.at(i)->GetName().val, "PatrolPoints"))
		{
			patrolPoints = children.at(i)->GetChildren();
		}
	}
	if (radius != "")
	{
		m_patrolRadiusFromOriginalPosition = (float)stof(radius);
	}
	if (numPatrolPoints != "")
	{
		numPatrolPoints = ReplaceCharInString(numPatrolPoints, '~', ' ');
		numPatrolPoints = ReplaceCharInString(numPatrolPoints, ',', ' ');
		std::vector<std::string> patrolRange = ParseString(numPatrolPoints);
		if (patrolRange.size() == 1)
		{
			m_minNumOfPatrolPoints = stoi(patrolRange.at(0));
			m_maxNumOfPatrolPoints = m_minNumOfPatrolPoints;
		}
		else if (patrolRange.size() >= 2)
		{
			m_minNumOfPatrolPoints = stoi(patrolRange.at(0));
			m_maxNumOfPatrolPoints = stoi(patrolRange.at(1));
		}
	}
	if ((int)patrolPoints.size() > 0 && (int)patrolPoints.size() >= m_minNumOfPatrolPoints && (int)patrolPoints.size() <= m_maxNumOfPatrolPoints)
	{
		if (currentPatrol != "")
		{
			m_currentPatrolInd = atoi(currentPatrol.c_str());
		}
		for (size_t i = 0; i < patrolPoints.size(); i++)
		{
			//m_patrolPoints
			std::string name = patrolPoints.at(i)->GetName().val;
			ReplaceCharInString(name, ',', ' ');
			ReplaceCharInString(name, '~', ' ');
			std::vector<std::string> vals = ParseString(name);
			if (vals.size() >= 2)
			{
				IntVector2 point;
				point.x = atoi(vals.at(0).c_str());
				point.y = atoi(vals.at(1).c_str());
				m_patrolPoints.push_back(point);
			}
		}
		if ((int)m_patrolPoints.size() >= m_minNumOfPatrolPoints && (int)m_patrolPoints.size() <= m_maxNumOfPatrolPoints)
		{
			m_patrolSet = true;
		}
		else
		{
			m_currentPatrolInd = 0;
			m_patrolPoints.clear();
		}
	}
	ClearData();
}
Patrol::Patrol(const Patrol& other)
	: Behavior(other),
	m_patrolSet(other.m_patrolSet),
	m_patrolRadiusFromOriginalPosition(other.m_patrolRadiusFromOriginalPosition),
	m_maxNumOfPatrolPoints(other.m_maxNumOfPatrolPoints),
	m_minNumOfPatrolPoints(other.m_minNumOfPatrolPoints),
	m_patrolPoints(other.m_patrolPoints),
	m_currentPatrolInd(other.m_currentPatrolInd)
{
}
Patrol::~Patrol()
{
	Behavior::~Behavior();
}

Behavior* Patrol::Clone()
{
	return new Patrol(*this);
}
const float Patrol::CalcUtility()
{
	if (m_patrolPoints.size() > 0 && m_owner != nullptr)
	{
		return 30.f;
	}
	return 0.f;
}
void Patrol::Run()
{
	if (m_owner != nullptr)
	{
		if (m_patrolSet == false)
		{
			GeneratePatrol();
		}
		int lastPatrolPoint = m_currentPatrolInd - 1;
		if (lastPatrolPoint < 0)
		{
			lastPatrolPoint = m_patrolPoints.size() - 1;
		}
		IntVector2 bl = m_owner->GetBlTilePos();
		if (m_patrolPoints.size() > 1)
		{
			IntVector2 wanted;
			bool succeeded = TryPath(m_patrolPoints.at(lastPatrolPoint), m_patrolPoints.at(m_currentPatrolInd), wanted);
			if (succeeded == true)
			{
				IntVector2 dir = (m_owner->GetBlTilePos() - wanted);
				dir.normalize();
				VerifyPositionIsNotCurrentlyOrGoingToBeOccupied(wanted, bl, dir);
				m_owner->SetWantedBLPosition(wanted);
			}
			else
			{
				succeeded = TryPath(bl, m_patrolPoints.at(m_currentPatrolInd), wanted);
				if (succeeded)
				{
					IntVector2 dir = (m_owner->GetBlTilePos() - wanted);
					dir.normalize();
					VerifyPositionIsNotCurrentlyOrGoingToBeOccupied(wanted, bl, dir);
					m_owner->SetWantedBLPosition(wanted);
				}
			}
		}
		else if (m_patrolPoints.size() == 1)
		{
			IntVector2 wanted;
			bool succeeded = TryPath(bl, m_patrolPoints.at(m_currentPatrolInd), wanted);
			if (succeeded)
			{
				IntVector2 dir = (m_owner->GetBlTilePos() - wanted);
				dir.normalize();
				VerifyPositionIsNotCurrentlyOrGoingToBeOccupied(wanted, bl, dir);
				m_owner->SetWantedBLPosition(wanted);
			}
		}

		if (m_patrolPoints.size() >= 1)
		{
			if (bl == m_patrolPoints.at(m_currentPatrolInd))
			{
				m_currentPatrolInd++;
				if ((size_t)m_currentPatrolInd >= m_patrolPoints.size())
				{
					m_currentPatrolInd = 0;
				}
			}
		}
	}
}


void Patrol::GeneratePatrol()
{
	m_patrolPoints.clear();

	std::vector<Tile_Coord> coords = g_map->GetAllNonSolidValidTileCoords();

	IntVector2 originalPos = m_owner->GetBlTilePos();
	Vector2 orgPosVec2 = Vector2((float)originalPos.x, (float)originalPos.y);

	for (size_t i = 0; i < coords.size(); i++)
	{
		IntVector2 coord = coords.at(i);
		Vector2 diff = Vector2((float)coord.x, (float)coord.y) - orgPosVec2;
		if (abs(diff.GetLength()) > m_patrolRadiusFromOriginalPosition)
		{
			coords.erase(coords.begin() + i);
			i--;
		}
	}

	int diffPatrol = abs(m_maxNumOfPatrolPoints - m_minNumOfPatrolPoints);

	int ran = rand() % diffPatrol;
	int numPoints = m_minNumOfPatrolPoints + ran;
	while ((int)m_patrolPoints.size() < numPoints)
	{
		int ranInd = rand() % coords.size();
		m_patrolPoints.push_back(coords.at(ranInd));
	}

	m_patrolSet = true;
}
void Patrol::VerifyPositionIsNotCurrentlyOrGoingToBeOccupied(IntVector2& pos, const IntVector2& orPos, const IntVector2& dirMoving)
{
	bool taken = VerifyPositionNotTaken(pos);
	if (taken == false)
	{
		return;
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
			taken = VerifyPositionNotTaken(pos);
			if (taken == false)
			{
				return;
			}
		}
	}
	else if(abs(dirMoving.y) == 0)
	{
		pos.x = orPos.x + dirMoving.x;
		pos.y = orPos.y + 1;
		bool solid = g_map->GetIsTileCoordSolid(pos, tile, m_owner->GetMask());
		if (solid == false)
		{
			taken = VerifyPositionNotTaken(pos);
			if (taken == false)
			{
				return;
			}
		}
		pos.y = orPos.y - 1;
		solid = g_map->GetIsTileCoordSolid(pos, tile, m_owner->GetMask());
		if (solid == false)
		{
			taken = VerifyPositionNotTaken(pos);
			if (taken == false)
			{
				return;
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
			taken = VerifyPositionNotTaken(pos);
			if (taken == false)
			{
				return;
			}
		}
	}
	else if (abs(dirMoving.x) == 0)
	{
		pos.x = orPos.x + 1;
		pos.y = orPos.y + dirMoving.y;
		bool solid = g_map->GetIsTileCoordSolid(pos, tile, m_owner->GetMask());
		if (solid == false)
		{
			taken = VerifyPositionNotTaken(pos);
			if (taken == false)
			{
				return;
			}
		}
		pos.x = orPos.x - 1;
		solid = g_map->GetIsTileCoordSolid(pos, tile, m_owner->GetMask());
		if (solid == false)
		{
			taken = VerifyPositionNotTaken(pos);
			if (taken == false)
			{
				return;
			}
		}
	}

	pos = orPos;
}

const bool Patrol::TryPath(const IntVector2& start, const IntVector2& end, IntVector2& wanted)
{
	if (m_owner == nullptr)
	{
		return false;
	}
	m_pathFinder.CalculateAStarAlgo(g_map, start, end, m_owner->GetMask());
	bool positionExistsOnRoute = false;
	IntVector2 nextPosition = m_pathFinder.GetNextPositionFromClosedListForPosition(m_owner->GetBlTilePos(), positionExistsOnRoute);

	if (positionExistsOnRoute == true)
	{
		bool canUse = true;
		for (size_t i = 0; i < Agent::s_Agents.size(); i++)
		{
			if (Agent::s_Agents.at(i) != m_owner && nextPosition == Agent::s_Agents.at(i)->GetBlTilePos())
			{
				canUse = false;
				break;
			}
		}
		if (canUse == true)
		{
			wanted = nextPosition;
			return true;
		}
	}
	return false;
}
const bool Patrol::VerifyPositionNotTaken(const IntVector2& position)
{
	bool taken = false;
	for (size_t i = 0; i < Agent::s_Agents.size(); i++)
	{
		Agent* curAge = Agent::s_Agents.at(i);
		IntVector2 bl = curAge->GetBlTilePos();
		IntVector2 wanted = curAge->GetWantedBLTilePos();
		if (position == bl)
		{
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
void Patrol::ClearNecessaryDataForIfTargetsDead()
{

}
void Patrol::WriteToXMLNode(XMLNode& node)
{
	XMLNode patrol = node.addChild("Patrol");
	//
	//std::string radius = m_data->GetAttrValueByName("radius");
	//std::string numPatrolPoints = m_data->GetAttrValueByName("numberOfPatrolPoints");
	patrol.addAttribute("radius", std::to_string(m_patrolRadiusFromOriginalPosition).c_str());
	patrol.addAttribute("numberOfPatrolPoints", (std::to_string(m_minNumOfPatrolPoints) + "," + std::to_string(m_maxNumOfPatrolPoints)).c_str());
	patrol.addAttribute("currentPatrolIdx", (std::to_string(m_currentPatrolInd)).c_str());
	XMLNode patrolPoints = patrol.addChild("PatrolPoints");
	for (size_t i = 0; i < m_patrolPoints.size(); i++)
	{
		IntVector2 point = m_patrolPoints.at(i);
		patrolPoints.addChild((std::to_string(point.x) + "," + std::to_string(point.y)).c_str());
	}
}
void Patrol::ReadFromXMLNode(GenerationProcessData* data)
{
	m_patrolRadiusFromOriginalPosition = (float)atof(data->GetAttributeByNameString("radius").c_str());
	std::string patrolPointCount = data->GetAttributeByNameString("numberOfPatrolPoints");
	patrolPointCount = ReplaceCharInString(patrolPointCount, ',', ' ');
	patrolPointCount = ReplaceCharInString(patrolPointCount, '~', ' ');
	std::vector<std::string> patrolPointCounts = ParseString(patrolPointCount);
	m_minNumOfPatrolPoints = atoi(patrolPointCounts.at(0).c_str());
	m_maxNumOfPatrolPoints = atoi(patrolPointCounts.at(1).c_str());

	m_currentPatrolInd = atoi(data->GetAttributeByNameString("currentPatrolIdx").c_str());

	GenerationProcessData* patrolPointData = data->GetChildByName("PatrolPoints");
	if (patrolPointData != nullptr)
	{
		std::vector<GenerationProcessData*> children = patrolPointData->GetChildren();
		for (size_t i = 0; i < children.size(); i++)
		{
			GenerationProcessData* child = children.at(i);
			if (child != nullptr)
			{
				std::string name = child->GetName().val;
				name = ReplaceCharInString(name, ',', ' ');
				name = ReplaceCharInString(name, '~', ' ');
				std::vector<std::string> pointVals = ParseString(name);
				IntVector2 point(atoi(pointVals.at(0).c_str()), atoi(pointVals.at(1).c_str()));
				m_patrolPoints.push_back(point);
			}
		}
	}
}