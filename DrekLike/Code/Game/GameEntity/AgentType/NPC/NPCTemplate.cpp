#include "Game/GameEntity/AgentType/NPC/NPCTemplate.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Items/ItemFactory.hpp"

//--------------------------------------------------------
//Constructors
NPCTemplate::NPCTemplate()
	: m_maxDamage(2),
	m_minDamage(2),
	m_lineOfSight(10.f),
	m_minMaxHealth(10),
	m_maxMaxHealth(10),
	m_movementMask(0),
	m_accuracy(0.68f)
{

}
NPCTemplate::~NPCTemplate()
{
}

//-------------------------------------------------------
//setters
void NPCTemplate::SetMask(const int& mas)
{
	m_movementMask = mas;
}
void NPCTemplate::SetMinMaxHealth(const int& health)
{ 
	m_minMaxHealth = health; 
}
void NPCTemplate::SetMaxMaxHealth(const int& health)
{ 
	m_maxMaxHealth = health; 
}
void NPCTemplate::SetMaxHealth(const int& health)
{
	m_minMaxHealth = health;
	m_maxMaxHealth = health;
}
void NPCTemplate::SetMinRGB(const Vector3& rgb)
{ 
	m_minRGB = rgb; 
}
void NPCTemplate::SetMaxRGB(const Vector3& rgb)
{ 
	m_maxRGB = rgb; 
}
void NPCTemplate::SetMinDamage(const int& min)
{ 
	m_minDamage = min;
}
void NPCTemplate::SetMaxDamage(const int& max)
{ 
	m_maxDamage = max;
}
void NPCTemplate::SetStepCost(const float& stepCost)
{
	m_stepCost = stepCost; 
}
void NPCTemplate::SetRGB(const Vector3& rgb)
{
	m_minRGB = rgb;
	m_maxRGB = rgb;
}
void NPCTemplate::SetChar(const unsigned char& car)
{ 
	m_char = car; 
}
void NPCTemplate::SetRace(const std::string& race)
{ 
	m_race = race;
}
void NPCTemplate::SetLineOfSight(const float& lineOfSight)
{ 
	m_lineOfSight = lineOfSight; 
}
void NPCTemplate::SetAccuracy(const float& accuracy)
{
	m_accuracy = accuracy;
}
void NPCTemplate::AddBehavior(Behavior* bev)
{
	m_behaviors.push_back(bev);
}
void NPCTemplate::AddPossibleTitle(const AgentTitle& title)
{
	m_titles.push_back(title);
}
void NPCTemplate::AddName(const std::string& name)
{
	if (name == "")
	{
		return;
	}
	if(DoesNameAlreadyExist(name) == true)
	{
		return;
	}
	m_names.push_back(name);
}
void NPCTemplate::AddItem(const std::string& name)
{
	if (name == "")
	{
		return;
	}
	if (DoesItemAlreadyExist(name) == true)
	{
		return;
	}
	m_nameOfItemsToAddToNPC.push_back(name);
}
void NPCTemplate::SetFaction(Faction* fact)
{
	m_faction = nullptr;
	m_faction = fact;
}


//----------------------------------------------------------
//getters

const int NPCTemplate::GetMovementMask() const
{
	return m_movementMask;
}
const int NPCTemplate::GetMinMaxHealth() const
{
	return m_minMaxHealth;
}
const int NPCTemplate::GetMaxMaxHealth() const
{ 
	return m_maxMaxHealth; 
}

const int NPCTemplate::GetRandMaxHealth() const
{
	int maxHealth = m_minMaxHealth;
	int diff = m_maxMaxHealth - m_minMaxHealth;
	if (diff > 0)
	{
		int ran = rand() % diff;
		maxHealth = maxHealth + ran;
	}
	return maxHealth;
}
const int NPCTemplate::GetMaxDamage() const
{
	return m_maxDamage; 
}
const int NPCTemplate::GetMinDamage() const
{ 
	return m_minDamage; 
}
const float NPCTemplate::GetStepCost() const
{ 
	return m_stepCost;
}
const float NPCTemplate::GetLineOfSight() const
{ 
	return m_lineOfSight; 
}
const Vector3 NPCTemplate::GetMinRgb() const
{
	return m_minRGB; 
}
const Vector3 NPCTemplate::GetMaxRgb() const
{ 
	return m_maxRGB; 
}
const Rgba NPCTemplate::GetRandRgb() const
{
	Rgba color = Rgba(m_minRGB.x, m_minRGB.y, m_minRGB.z, 1.f);

	Vector3 diff = m_maxRGB - m_minRGB;

	if (diff.x > 0.f && diff.x <= 1.f)
	{
		int val = (int)round(diff.x * 100.f);
		int ran = 0;
		if (val > 0)
		{
			ran = rand() % val;
		}
		float amt = ((float)ran) / 100.f;
		color.Red += amt;
	}
	if (diff.y > 0.f && diff.y <= 1.f)
	{
		int val = (int)round(diff.y * 100.f);
		int ran = 0;
		if (val > 0)
		{
			ran = rand() % val;
		}
		float amt = ((float)ran) / 100.f;
		color.Green += amt;
	}
	if (diff.z > 0.f && diff.z <= 1.f)
	{
		int val = (int)round(diff.z * 100.f);
		int ran = 0;
		if (val > 0)
		{
			ran = rand() % val;
		}
		float amt = ((float)ran) / 100.f;
		color.Blue += amt;
	}

	if (color.Red > 1.f)
	{
		color.Red = 1.f;
	}
	if (color.Green > 1.f)
	{
		color.Green = 1.f;
	}
	if (color.Blue > 1.f)
	{
		color.Blue = 1.f;
	}

	return color;
}
const unsigned char NPCTemplate::GetChar() const
{ 
	return m_char; 
}
const std::string NPCTemplate::GetRace() const
{ 
	return m_race; 
}
const std::vector<std::string> NPCTemplate::GetItemNames() const
{
	return m_nameOfItemsToAddToNPC;
}
const bool NPCTemplate::DoesNameAlreadyExist(const std::string& name) const
{
	std::string lowerC = MakeLower(name);
	for (size_t i = 0; i < m_names.size(); i++)
	{
		std::string nam = MakeLower(m_names.at(i));
		if (SimpleStrCmp(nam, lowerC))
		{
			return true;
		}
	}
	return false;
}
const bool NPCTemplate::DoesItemAlreadyExist(const std::string& name) const
{
	std::string lowerC = MakeLower(name);
	for (size_t i = 0; i < m_nameOfItemsToAddToNPC.size(); i++)
	{
		std::string nam = MakeLower(m_nameOfItemsToAddToNPC.at(i));
		{
			if (SimpleStrCmp(nam, lowerC))
			{
				return true;
			}
		}
	}

	return false;
}


//---------------------------------------------------------
//Copy template to NPC
void NPCTemplate::CopyValuesToNPC(NPC* npc, SpriteSheet* spriteSheet)
{
	if (npc == nullptr)
	{
		return;
	}

	int maxHealth = GetRandMaxHealth();
	Rgba color = GetRandRgb();

	int ranName = rand() % m_names.size();
	std::string name = m_names.at(ranName);
	if (m_titles.size() > 0)
	{
		int ranTitle = rand() % m_titles.size();
		name += " " + m_titles.at(ranTitle).title;
	}

	npc->SetEntityName(name);
	npc->SetChar(m_char);
	npc->SetCurrentHealth(maxHealth);
	npc->SetMaxHealth(maxHealth);
	npc->SetColor(color);
	npc->SetMinAtkPow(m_minDamage);
	npc->SetMinAtkPow(m_maxDamage);
	npc->SetFaction(m_faction);
	npc->SetSightRadius(m_lineOfSight);
	npc->SetMovementMask(m_movementMask);
	npc->CopyMaskToOriginal();
	npc->SetAccuracy(m_accuracy);

	for (size_t i = 0; i < m_nameOfItemsToAddToNPC.size(); i++)
	{
		int ran = rand() % 10;
		if (ran >= 5)
		{
			std::string itemName = m_nameOfItemsToAddToNPC.at(i);
			Item* item = ItemFactory::GenerateItemByName(itemName);
			if (item != nullptr)
			{
				item->SetSpriteSheet(spriteSheet);
				bool addedSuccessfully = npc->AddItemToInventory(item, true);
				if (addedSuccessfully == false)
				{
					Item::RemoveItemFromAll(item);
					delete item;
				}
			}
		}
	}

	for (size_t i = 0; i < m_behaviors.size(); i++)
	{
		Behavior* bev = m_behaviors.at(i);
		if (bev != nullptr)
		{
			bev->SetAgent(npc);
			npc->AddBehavior(bev);
		}
	}
}

void NPCTemplate::ClearTemplateBehaviors()
{
	m_faction = nullptr;
	for (size_t i = 0; i < m_behaviors.size(); i++)
	{
		if (m_behaviors.at(i) != nullptr)
		{
			delete m_behaviors[i];
			m_behaviors[i] = nullptr;
		}
	}
	m_behaviors.clear();
}