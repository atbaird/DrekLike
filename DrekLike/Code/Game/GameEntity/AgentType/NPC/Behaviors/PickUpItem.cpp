#include "Game/GameEntity/AgentType/NPC/Behaviors/PickUpItem.hpp"

Behavior* DefaultCreatePickUpItemBehavior()
{
	return new PickUpItem();
}

Behavior* CreatePickUpItemBehavior(const XMLNode& node)
{
	return new PickUpItem(node);
}

const BehaviorRegistrationHelper PickUpItem::s_PickUpItemBehaviorRegistration("PickUpItem", CreatePickUpItemBehavior, DefaultCreatePickUpItemBehavior);

PickUpItem::PickUpItem()
	:Behavior(),
	m_interestInItems(15.f),
	m_minInterest(10.f),
	m_distanceOutToCare(10.f),
	m_nearestItemIdx(-1)
{
	m_name = "PickUpItem";
	ClearData();
}

PickUpItem::PickUpItem(const XMLNode& node)
	: Behavior("PickUpItem", node),
	m_interestInItems(15.f),
	m_minInterest(10.f),
	m_distanceOutToCare(10.f),
	m_nearestItemIdx(-1)
{
	std::string interestInItems = m_data->GetAttrValueByName("interestInItems");
	std::string minInterestInItems = m_data->GetAttrValueByName("minInterestInItems");
	std::string distanceOutToCare = m_data->GetAttrValueByName("distanceOutToCare");
	if (interestInItems != "")
	{
		m_interestInItems = (float)atof(interestInItems.c_str());
	}
	if (minInterestInItems != "")
	{
		m_minInterest = (float)atof(minInterestInItems.c_str());
	}
	if (distanceOutToCare != "")
	{
		m_distanceOutToCare = abs((float)atof(distanceOutToCare.c_str()));
	}
	ClearData();
}
PickUpItem::PickUpItem(const PickUpItem& other)
	: Behavior(other),
	m_interestInItems(other.m_interestInItems),
	m_minInterest(other.m_minInterest),
	m_distanceOutToCare(other.m_distanceOutToCare),
	m_nearestItemIdx(-1)
{

}
PickUpItem::~PickUpItem()
{
	Behavior::~Behavior();
}


Behavior* PickUpItem::Clone()
{
	return new PickUpItem(*this);
}
const float PickUpItem::CalcUtility()
{
	if (m_owner == nullptr || (m_owner != nullptr && m_owner->GetInventoryIsFull() == true))
	{
		return 0.f;
	}
	std::vector<Item*> items = m_owner->GetVisibleItems();
	IntVector2 ownerBL = m_owner->GetBlTilePos();
	IntVector2 ownerWantedBL = m_owner->GetWantedBLTilePos();

	float nearestDist = 0.f;
	bool setOnce = false;
	int nearestIdx = -1;
	for (size_t i = 0; i < items.size(); i++)
	{
		if (items.at(i) == nullptr)
		{
			continue;
		}
		IntVector2 bl = items.at(i)->GetBlTilePos();
		if (bl == ownerBL)
		{
			nearestIdx = i;
			setOnce = true;
			break;
		}
		IntVector2 diff = bl - ownerBL;
		float dist = abs(diff.getLength());
		if ((setOnce == false && dist <= m_distanceOutToCare)
			|| (setOnce == true && dist < nearestDist && dist <= m_distanceOutToCare))
		{
			nearestDist = dist;
			nearestIdx = i;
			setOnce = true;
		}
	}

	m_nearestItemIdx = nearestIdx;
	float util = 0.f;
	if (m_nearestItemIdx != -1)
	{
		float distanceAway = m_distanceOutToCare - nearestDist;
		float perc = abs(distanceAway / m_distanceOutToCare);
		util = perc * (m_interestInItems - m_minInterest);
		util += m_minInterest;
	}

	return util;
}
void PickUpItem::Run()
{
	if (m_owner == nullptr)
	{
		return;
	}
	std::vector<Item*> items = m_owner->GetVisibleItems();

	if (m_nearestItemIdx != -1)
	{
		IntVector2 itemBL = items.at(m_nearestItemIdx)->GetBlTilePos();
		IntVector2 ownerBL = m_owner->GetBlTilePos();
		if (itemBL != ownerBL)
		{
			IntVector2 diff = ownerBL - itemBL;
			diff.x = abs(diff.x);
			diff.y = abs(diff.y);

			m_algo.CalculateAStarAlgo(g_map, ownerBL, itemBL, m_owner->GetMask());
			bool exists = false;
			IntVector2 next = m_algo.GetNextPositionFromClosedListForPosition(ownerBL, exists);
			if (exists == true && next != IntVector2(-1, -1))
			{
				bool canMoveThere = true;
				for (size_t i = 0; i < Agent::s_Agents.size(); i++)
				{
					Agent* age = Agent::s_Agents.at(i);
					if (age != nullptr && ((age->GetBlTilePos() == next && age->GetWantedBLTilePos() == next)
						|| age->GetBlTilePos() != next && age->GetWantedBLTilePos() == next))
						{
							canMoveThere = false;
							break;
						}
				}
				if (canMoveThere == true)
				{
					m_owner->SetWantedBLPosition(next);
				}
			}
			
		}
		else
		{
			bool pickedUp = m_owner->TryPickUpItem(items.at(m_nearestItemIdx));
			if (pickedUp == true)
			{
				Item::RemoveItemFromGround(items.at(m_nearestItemIdx));
			}
		}
	}
}

void PickUpItem::ClearNecessaryDataForIfTargetsDead()
{

}
void PickUpItem::WriteToXMLNode(XMLNode& node)
{
	XMLNode pickUp = node.addChild("PickUpItem");
	pickUp.addAttribute("interestInItems", std::to_string( m_interestInItems).c_str());
	pickUp.addAttribute("distanceOutToCare", std::to_string(m_distanceOutToCare).c_str());
	pickUp.addAttribute("minInterestInItems", std::to_string(m_minInterest).c_str());
}
void PickUpItem::ReadFromXMLNode(GenerationProcessData* data)
{
	m_interestInItems = (float)atof(data->GetAttributeByNameString("interestInItems").c_str());
	m_distanceOutToCare = (float)atof(data->GetAttributeByNameString("distanceOutToCare").c_str());
	m_minInterest = (float)atof(data->GetAttributeByNameString("minInterestInItems").c_str());
}