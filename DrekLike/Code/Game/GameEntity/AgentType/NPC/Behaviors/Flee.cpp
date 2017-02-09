#include "Game/GameEntity/AgentType/NPC/Behaviors/Flee.hpp"
#include "Game/GameCommonFunctions.hpp"
#include "Game/Map/Map.hpp"

Behavior* DefaultCreateFleeBehavior()
{
	return new Flee();
}

Behavior* CreateFleeBehavior(const XMLNode& node)
{
	return new Flee(node);
}

const BehaviorRegistrationHelper s_FleeBehaviorRegistration("Flee", CreateFleeBehavior, DefaultCreateFleeBehavior);

Flee::Flee()
	: Behavior(),
	m_healthBelowThisPercentage(1.f),
	m_runMultiplier(15.f),
	m_percentageBeforeScream(0.5f),
	m_chanceToRunRegardless(0.0f),
	m_baseHatredToCountAsHostile(0)
{
	m_name = "Flee";
	ClearData();
}

Flee::Flee(const XMLNode& node)
	: Behavior("Flee", node),
	m_healthBelowThisPercentage(1.f),
	m_runMultiplier(15.f),
	m_percentageBeforeScream(0.5f),
	m_chanceToRunRegardless(0.0f),
	m_baseHatredToCountAsHostile(0)
{
	std::string healthPerc = m_data->GetAttrValueByName("healthPercentLessThan");
	std::string multiplier = m_data->GetAttrValueByName("multiplier");
	std::string chanceToScream = m_data->GetAttrValueByName("percentChanceToScream");
	std::string chanceToRunRegardless = m_data->GetAttrValueByName("baseChanceToRun");
	std::string baseHatredScale = m_data->GetAttrValueByName("baseHatredToCountAsHostile");

	if (baseHatredScale != "")
	{
		m_baseHatredToCountAsHostile = stoi(baseHatredScale);
	}

	if (chanceToRunRegardless != "")
	{
		m_chanceToRunRegardless = (float)atof(chanceToRunRegardless.c_str());
		if (m_chanceToRunRegardless < 0.f)
		{
			m_chanceToRunRegardless = 0.f;
		}
		if (m_chanceToRunRegardless > 1.f)
		{
			m_chanceToRunRegardless = 1.f;
		}
	}
	if (healthPerc != "")
	{
		m_healthBelowThisPercentage = (float)atof(healthPerc.c_str());
		if (m_healthBelowThisPercentage < 0.f)
		{
			m_healthBelowThisPercentage = 0.f;
		}
		else if (m_healthBelowThisPercentage > 1.f)
		{
			m_healthBelowThisPercentage = 1.f;
		}
	}
	if (multiplier != "")
	{
		m_runMultiplier = (float)atof(multiplier.c_str());
	}
	if (chanceToScream != "")
	{
		m_percentageBeforeScream = (float)atof(chanceToScream.c_str());
		if (m_percentageBeforeScream < 0.f)
		{
			m_percentageBeforeScream = 0.f;
		}
		else if(m_percentageBeforeScream > 1.f)
		{
			m_percentageBeforeScream = 1.f;
		}
	}
	ClearData();
}
Flee::Flee(const Flee& other)
	: Behavior(other),
	m_healthBelowThisPercentage(other.m_healthBelowThisPercentage),
	m_runMultiplier(other.m_runMultiplier),
	m_percentageBeforeScream(other.m_percentageBeforeScream),
	m_chanceToRunRegardless(other.m_chanceToRunRegardless),
	m_baseHatredToCountAsHostile(other.m_baseHatredToCountAsHostile)
{
}
Flee::~Flee()
{
	Behavior::~Behavior();
}


/*
	step 1: Get the agent's current health and max health.
	step 2: Calculate percent wounded
	step 3: Roll RandNumber, if less than percent wounded, then run.
	step 4: mult percent wounded by some base number (15?)
	step 5:

*/
Behavior* Flee::Clone()
{
	return new Flee(*this);
}
const float Flee::CalcUtility()
{
	if (m_owner == nullptr)
	{
		return 0.0f;
	}
	std::string ownerName = m_owner->GetEntityName();

	float healthPerc = (float)m_owner->GetCurrentHealth() / (float)m_owner->GetMaxHealth();
	if (healthPerc > m_healthBelowThisPercentage)
	{
		return 0.0f;
	}
	IntVector2 bl = m_owner->GetBlTilePos();
	IntVector2 tr = bl + m_owner->GetSize() - IntVector2(1,1);

	std::vector<Agent*> visibleAgents = m_owner->GetVisibleAgents();
	std::vector<FactionRelationship>* FacRels =  m_owner->GetFactionRelationData();
	std::vector<AgentRelationship>* ageRels = m_owner->GetIndividualRelationshipData();
	m_owner->GetIndividualRelationshipData();
	int cntHostile = 0;
	for (size_t i = 0; i < visibleAgents.size(); i++)
	{
		int hatredScore = 0;
		Agent* age = visibleAgents.at(i);
		int FactionID = age->GetFactionID();
		for (size_t j = 0; j < FacRels->size(); j++)
		{
			if (FacRels->at(j).GetFactionID() == FactionID)
			{
				hatredScore = FacRels->at(j).GetFactionRelationship();
				break;
			}
		}
		for (size_t j = 0; j < ageRels->size(); j++)
		{
			if (ageRels->at(j).GetAgentID() == age->GetAgentID())
			{
				hatredScore += ageRels->at(j).GetRelationship();
			}
		}
		if (hatredScore <= m_baseHatredToCountAsHostile || (m_owner->GetWhoLastAttacked() != nullptr && age == m_owner->GetWhoLastAttacked()))
		{
			cntHostile++;
		}
	}
	if (cntHostile == 0)
	{
		return 0.0f;
	}

	float percWounded = 1.f - healthPerc;
	float baseRun = percWounded * m_runMultiplier;

	return baseRun + (m_chanceToRunRegardless * m_runMultiplier);
}
void Flee::Run()
{
	bool ownerVisibleToPlayer = false;
	std::vector<Agent*> agentsVisToPlayer = Agent::GetListActorsVisibleToPlayer();
	for (size_t i = 0; i < agentsVisToPlayer.size(); i++)
	{
		if (m_owner == agentsVisToPlayer.at(i))
		{
			ownerVisibleToPlayer = true;
			break;
		}
	}

	if (ownerVisibleToPlayer == true)
	{
		int ranChanceToScream = rand() % 100;
		float percent = (float)ranChanceToScream / 100.f;
		if (percent <= m_percentageBeforeScream)
		{
			std::string message = m_owner->GetEntityName() + " screams in fear.";
			s_messageBoxStrings.push_back(message);
		}
	}

	std::vector<Agent*> agentsSeen = m_owner->GetListActorsVisibleToPlayer();
	std::vector<Agent*> enemyAgents;


	IntVector2 bl = m_owner->GetBlTilePos();
	IntVector2 tr = bl + m_owner->GetSize() - IntVector2(1, 1);

	//std::vector<IntVector2> tiles = g_map->GetOpenCoordsAboutEntityBLAndTRWithRespectToActors(bl, tr, m_owner);

	std::vector<Tile_Coord> tiles = g_map->FindAdjacentValidPositionsThatMeetMask(bl, m_owner->GetMask());

	std::vector<IntVector2> coordsFleeingFrom;

	for (size_t i = 0; i < agentsSeen.size(); i++)
	{
		if (agentsSeen.at(i) == m_owner)
		{
			continue;
		}
		IntVector2 pos = agentsSeen.at(i)->GetBlTilePos();
		IntVector2 trPos = pos + agentsSeen.at(i)->GetSize() - IntVector2(1, 1);
		IntVector2 cur = pos;
		enemyAgents.push_back(agentsSeen.at(i));
		while (cur != trPos)
		{
			for (size_t ind = 0; ind < tiles.size(); ind++)
			{
				if (tiles.at(ind) == cur)
				{
					tiles.erase(tiles.begin() + ind);
					ind--;
				}
			}
			coordsFleeingFrom.push_back(cur);

			cur.x++;
			if (cur.x > trPos.x && cur.y < trPos.y)
			{
				cur.x = pos.x;
				cur.y++;
			}
		}
	}
	Vector2 center = Vector2((float)(bl.x + tr.x), (float)(bl.y + tr.y)) / 2.f;
	Vector2 direction = Vector2(0.f,0.f);

	for (size_t i = 0; i < coordsFleeingFrom.size(); i++)
	{
		IntVector2 coordTest = coordsFleeingFrom.at(i);
		Vector2 diff = Vector2((float)coordTest.x, (float)coordTest.y) - center;
		direction += diff;
	}
	direction.normalize();
	float yaw = atan2(direction.y, direction.x);

	float smallestAngleDiff = 360.f;
	int closestTile = -1;
	for (size_t i = 0; i < tiles.size(); i++)
	{
		Vector2 diff = Vector2((float)tiles.at(i).x, (float)tiles.at(i).y) - center;
		diff.normalize();
		float yawTest = atan2(diff.y, diff.x) - yaw;
		if (yawTest < smallestAngleDiff)
		{
			closestTile = i;
			smallestAngleDiff = yawTest;
		}
	}
	if (closestTile != -1 && m_owner->GetBlTilePos() != tiles.at(closestTile))
	{
		//RUN!
		m_owner->SetWantedBLPosition(tiles.at(closestTile));
	}
	else
	{
		//Retaliate!
		Agent* closestEnemy = nullptr;
		for (size_t i = 0; i < enemyAgents.size(); i++)
		{
			Agent* curAgent = enemyAgents.at(i);
			if (closestEnemy == nullptr)
			{
				closestEnemy = curAgent;
			}
			else
			{
				IntVector2 cloBLPos = closestEnemy->GetBlTilePos();
				IntVector2 curBLPos = curAgent->GetBlTilePos();
				IntVector2 cloTRPos = (cloBLPos + closestEnemy->GetSize() - IntVector2(1, 1));
				IntVector2 curTRPos = (curBLPos + curAgent->GetSize() - IntVector2(1, 1));
				Vector2 cloCenter = (Vector2((float)cloBLPos.x, (float)cloBLPos.y) + Vector2((float)cloTRPos.x, (float)cloTRPos.y)) / 2.f;
				Vector2 curCenter = (Vector2((float)curBLPos.x, (float)curBLPos.y) + Vector2((float)curTRPos.x, (float)curTRPos.y)) / 2.f;
				Vector2 cloDiff = cloCenter - center;
				Vector2 curDiff = curCenter - center;
				if (curDiff.GetLength() > cloDiff.GetLength())
				{
					closestEnemy = nullptr;
					closestEnemy = curAgent;
				}
			}
		}
		if (closestEnemy != nullptr)
		{
			IntVector2 pos = closestEnemy->GetBlTilePos() - m_owner->GetBlTilePos();
			
			if (abs(pos.x) <= 1 && abs(pos.y) <= 1)
			{
				AttackData data;
				data.instigator = m_owner;
				data.target = m_owner->GetTargetOfHate();
				data.m_atkType = AttackType_Melee;
				data.accuracy = m_owner->GetAccuracy();
				CombatSystem::Attack(data);
			}

		}
	}
}
void Flee::ClearNecessaryDataForIfTargetsDead()
{

}
void Flee::WriteToXMLNode(XMLNode& node)
{
	/*

	std::string healthPerc = m_data->GetAttrValueByName("healthPercentLessThan"); //m_healthBelowThisPercentage
	std::string multiplier = m_data->GetAttrValueByName("multiplier"); //m_runMultiplier
	std::string chanceToScream = m_data->GetAttrValueByName("percentChanceToScream"); //m_percentageBeforeScream
	std::string chanceToRunRegardless = m_data->GetAttrValueByName("baseChanceToRun"); // m_chanceToRunRegardless
	std::string baseHatredScale = m_data->GetAttrValueByName("baseHatredToCountAsHostile"); //m_baseHatredToCountAsHostile
	*/
	XMLNode flee = node.addChild("Flee");
	flee.addAttribute("healthPercentLessThan", std::to_string(m_healthBelowThisPercentage).c_str());
	flee.addAttribute("multiplier", std::to_string(m_runMultiplier).c_str());
	flee.addAttribute("percentChanceToScream", std::to_string(m_percentageBeforeScream).c_str());
	flee.addAttribute("baseChanceToRun", std::to_string(m_chanceToRunRegardless).c_str());
	flee.addAttribute("baseHatredToCountAsHostile", std::to_string(m_baseHatredToCountAsHostile).c_str());
}
void Flee::ReadFromXMLNode(GenerationProcessData* data)
{
	m_healthBelowThisPercentage = (float)atof(data->GetAttributeByNameString("healthPercentLessThan").c_str());
	m_runMultiplier = (float)atof(data->GetAttributeByNameString("multiplier").c_str());
	m_percentageBeforeScream = (float)atof(data->GetAttributeByNameString("percentChanceToScream").c_str());
	m_chanceToRunRegardless = (float)atof(data->GetAttributeByNameString("baseChanceToRun").c_str());
	m_baseHatredToCountAsHostile = atoi(data->GetAttributeByNameString("baseHatredToCountAsHostile").c_str());
}