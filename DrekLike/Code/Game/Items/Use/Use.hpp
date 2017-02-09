#pragma once

#ifndef USE_HPP
#define USE_HPP
#include <string>
#include "Game/Generators/EnvironmentGenerationProcess.hpp"


//-------------------------------------------------------------------------------
//UseRegistrationHelper
class Use;

typedef Use* (UseCreationFunc)(const XMLNode& node);

class UseRegistrationHelper
{
private:
	std::string name;
	UseCreationFunc* creationFunc;
public:
	UseRegistrationHelper(const std::string& nam, UseCreationFunc* useCreationFunc);

	static Use* CreateUseByName(const std::string& name, const XMLNode& node);
	static const bool DetermineUseExists(const std::string& name);
	static void ClearAllUseRegistrations();
};


//---------------------------------------------------------------------------------
//Use
class Use
{
private:
	std::string m_name;
public:
	Use(const std::string& name);
	~Use();

	virtual Use* Copy() const = 0;
};
#endif