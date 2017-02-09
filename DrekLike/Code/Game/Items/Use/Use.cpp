#include "Game/Items/Use/Use.hpp"


//------------------------------------------------------------------------------------
UseRegistrationHelper::UseRegistrationHelper(const std::string& nam, UseCreationFunc* useCreationFunc)
{
	nam;
	useCreationFunc;
}

Use* UseRegistrationHelper::CreateUseByName(const std::string& name, const XMLNode& node)
{
	name;
	node;
	return nullptr;
}
const bool UseRegistrationHelper::DetermineUseExists(const std::string& name)
{
	name;
	return false;
}
void UseRegistrationHelper::ClearAllUseRegistrations()
{

}


//------------------------------------------------------------------------------------
Use::Use(const std::string& name)
{
	name;
}
Use::~Use()
{

}