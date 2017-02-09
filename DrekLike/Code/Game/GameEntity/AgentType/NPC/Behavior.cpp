#include "Game/GameEntity/AgentType/NPC/Behavior.hpp"


//Behavior Registration Helper
std::map<std::string, BehaviorRegistrationHelper>* BehaviorRegistrationHelper::s_behaviorRegistrations = nullptr;

BehaviorRegistrationHelper::BehaviorRegistrationHelper(const std::string& nam3, BehaviorCreationFunc* bevCreationFunc, BehaviorDefaultCreationFunc* func)
	: name(nam3),
	beviorCreationFunc(bevCreationFunc),
	defBehaviorCreationFunc(func)
{
	if (s_behaviorRegistrations == nullptr)
	{
		s_behaviorRegistrations = new std::map<std::string, BehaviorRegistrationHelper>();
	}
	std::map<std::string, BehaviorRegistrationHelper>::iterator it = s_behaviorRegistrations->find(nam3);
	if (it == s_behaviorRegistrations->end())
	{
		s_behaviorRegistrations->insert(std::pair<std::string, BehaviorRegistrationHelper>(nam3, *this));
	}
}
Behavior* BehaviorRegistrationHelper::CreateDefaultBehaviorByName(const std::string& name)
{
	std::map<std::string, BehaviorRegistrationHelper>::iterator it = s_behaviorRegistrations->find(name);
	if (it == s_behaviorRegistrations->end())
	{
		return nullptr;
	}
	return it->second.defBehaviorCreationFunc();
}
Behavior* BehaviorRegistrationHelper::CreateBehaviorByName(const std::string& name, const XMLNode& node)
{
	std::map<std::string, BehaviorRegistrationHelper>::iterator it = s_behaviorRegistrations->find(name);
	if (it == s_behaviorRegistrations->end())
	{
		return nullptr;
	}
	return it->second.beviorCreationFunc(node);
}
const bool BehaviorRegistrationHelper::DetermineBehaviorExists(const std::string& name)
{
	std::map<std::string, BehaviorRegistrationHelper>::iterator it = s_behaviorRegistrations->find(name);
	if (it == s_behaviorRegistrations->end())
	{
		return false;
	}
	return true;
}

void BehaviorRegistrationHelper::ClearAllBehaviorRegistrations()
{
	delete s_behaviorRegistrations;
	s_behaviorRegistrations = nullptr;
}

//Behavior
Behavior::Behavior()
	: m_name(""),
	m_canRun(false),
	m_owner(nullptr),
	m_data(nullptr)
{

}
Behavior::Behavior(std::string name, const XMLNode& node)
	: m_canRun(false),
	m_name(name),
	m_owner(nullptr),
	m_data(nullptr)
{
	m_data = new EnvironmentGenerationProcess();
	m_data->PopulateFromXMLNode(node);
}
Behavior::Behavior(const Behavior& bev)
	: m_canRun(bev.m_canRun),
	m_name(bev.m_name),
	m_owner(bev.m_owner),
	m_data(nullptr)
{
}
Behavior::~Behavior()
{
	ClearData();
}

void Behavior::ClearData()
{
	if (m_data != nullptr)
	{
		delete m_data;
		m_data = nullptr;
	}
}

const bool Behavior::DoesPassChanceToRun() const
{
	return m_canRun;
}


void Behavior::SetAgent(Agent* owner)
{
	m_owner = nullptr;
	m_owner = owner;
}

Agent* Behavior::GetAgent() const
{
	return m_owner;
}