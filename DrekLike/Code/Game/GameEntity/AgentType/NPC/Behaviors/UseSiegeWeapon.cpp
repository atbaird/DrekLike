#include "Game/GameEntity/AgentType/NPC/Behaviors/UseSiegeWeapon.hpp"



UseSiegeWeapon::UseSiegeWeapon()
{

}
UseSiegeWeapon::UseSiegeWeapon(const XMLNode& node)
{

}
UseSiegeWeapon::UseSiegeWeapon(const UseSiegeWeapon& other)
{

}
UseSiegeWeapon::~UseSiegeWeapon()
{
	Behavior::~Behavior();
}


Behavior* UseSiegeWeapon::Clone()
{
	return new UseSiegeWeapon(*this);
}
const float UseSiegeWeapon::CalcUtility()
{
	if (m_owner == nullptr)
	{
		return 0.f;
	}
	std::vector<Feature*> visibleFeatures = m_owner->GetVisibleFeatures();
	if (visibleFeatures.size() <= 0)
	{
		return 0.f;
	}
	bool siegeWepWithin2Tiles = false;
	for (size_t i = 0; i < visibleFeatures.size(); i++)
	{

	}

	if (siegeWepWithin2Tiles == false)
	{
		return 0.f;
	}
	return 5.f;
}
void UseSiegeWeapon::Run()
{

}
void UseSiegeWeapon::ClearNecessaryDataForIfTargetsDead()
{

}
void UseSiegeWeapon::WriteToXMLNode(XMLNode& node)
{

}
void UseSiegeWeapon::ReadFromXMLNode(GenerationProcessData* data)
{

}