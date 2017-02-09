#include "Game/GameEntity/AgentType/NPC/Behaviors/Wander.hpp"
#include "Game/Map/Map.hpp"

Behavior* DefaultCreateWanderBehavior()
{
	return new Wander();
}

Behavior* CreateWanderBehavior(const XMLNode& node)
{
	return new Wander(node);
}

const BehaviorRegistrationHelper Wander::s_WanderBehaviorRegistration("Wander", CreateWanderBehavior, DefaultCreateWanderBehavior);

Wander::Wander()
	:Behavior(),
	percentChanceToRest(0.5f),
	percentChanceToWalk(0.5f),
	amountToHealFromResting(1)
{
	m_name = "Wander";
	ClearData();
}

Wander::Wander(const XMLNode& node)
	: Behavior("Wander", node),
	percentChanceToRest(0.5f),
	percentChanceToWalk(0.5f),
	amountToHealFromResting(1)
{
	std::string chanceToRest = m_data->GetAttrValueByName("chanceToRest");
	std::string chanceToWalk = m_data->GetAttrValueByName("chanceToWalk");
	if (chanceToRest != "")
	{
		percentChanceToRest = (float)stof(chanceToRest);
		percentChanceToWalk = 1.f - percentChanceToRest;
	}
	if (chanceToWalk != "")
	{
		percentChanceToWalk = (float)stof(chanceToWalk);
		percentChanceToRest = 1.f - percentChanceToWalk;
	}
	ClearData();
}
Wander::Wander(const Wander& other)
	: Behavior(other),
	percentChanceToWalk(other.percentChanceToWalk),
	percentChanceToRest(other.percentChanceToRest),
	amountToHealFromResting(other.amountToHealFromResting)
{

}
Wander::~Wander()
{
	Behavior::~Behavior();
}

Behavior* Wander::Clone()
{
	return new Wander(*this);
}
const float Wander::CalcUtility()
{

	return 20.f;
}
void Wander::Run()
{
	if (m_owner != nullptr)
	{
		float val = 1.f;
		int curHel = m_owner->GetCurrentHealth();
		int maxHel = m_owner->GetMaxHealth();
		if (curHel < maxHel)
		{
			int ran = rand() % 101;
			val = (float)ran / 100.f;
		}
		if (val > (percentChanceToRest))
		{

			Tile_Coord bl = m_owner->GetBlTilePos();
			Tile_Coord tr = bl + m_owner->GetSize() - IntVector2(1, 1);

			std::vector<Tile_Coord> openTiles = g_map->FindAdjacentValidPositionsThatDontBlockOrHurt(bl, m_owner->GetMask());

			for (size_t i = 0; i < openTiles.size(); i++)
			{
				IntVector2 tileCoord = openTiles.at(i);
				bool canKeep = true;
				for (size_t j = 0; j < Agent::s_Agents.size(); j++)
				{
					if (Agent::s_Agents.at(j) != m_owner)
					{
						IntVector2 test = Agent::s_Agents.at(j)->GetBlTilePos();
						IntVector2 wanted = Agent::s_Agents.at(j)->GetWantedBLTilePos();
						if (test == tileCoord)
						{
							if (wanted == test)
							{
								canKeep = false;
								break;
							}
						}
						else if (wanted == tileCoord)
						{
							canKeep = false;
							break;
						}
					}
				}
				if (canKeep == false)
				{
					openTiles.erase(openTiles.begin() + i);
					i--;
				}
			}

			if (openTiles.size() > 0)
			{
				size_t index = rand() % openTiles.size();
				m_owner->SetWantedBLPosition(openTiles.at(index));
			}
			else
			{
				m_owner->SetWantedBLPosition(bl);
			}
		}
		else
		{
			if (abs(amountToHealFromResting) > 0)
			{
				if (amountToHealFromResting > 0 && m_owner->GetVisibleToPlayer())
				{
					s_messageBoxStrings.push_back(m_owner->GetEntityName() + " was healed by " + std::to_string(abs(amountToHealFromResting)));
				}
				else if(amountToHealFromResting < 0 && m_owner->GetVisibleToPlayer())
				{
					s_messageBoxStrings.push_back(m_owner->GetEntityName() + " was hurt by " + std::to_string( abs(amountToHealFromResting)));
				}
			}
		}
	}
}
void Wander::ClearNecessaryDataForIfTargetsDead()
{

}
void Wander::WriteToXMLNode(XMLNode& node)
{
	XMLNode wander = node.addChild("Wander");
	wander.addAttribute("chanceToRest", std::to_string(percentChanceToRest).c_str());
	wander.addAttribute("chanceToWalk", std::to_string(percentChanceToWalk).c_str());
	wander.addAttribute("amountToHealFromResting", std::to_string(amountToHealFromResting).c_str());
}
void Wander::ReadFromXMLNode(GenerationProcessData* data)
{
	percentChanceToRest = (float)atof(data->GetAttributeByNameString("chanceToRest").c_str());
	percentChanceToWalk = (float)atof(data->GetAttributeByNameString("chanceToWalk").c_str());
	amountToHealFromResting = atoi(data->GetAttributeByNameString("amountToHealFromResting").c_str());
}