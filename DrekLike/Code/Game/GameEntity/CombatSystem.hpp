#pragma once

#ifndef COMBATSYSTEM_HPP
#define COMBATSYSTEM_HPP
#include "Game/GameEntity/Agent.hpp"

class CombatSystem
{
public:
	static void Attack(AttackData& data);
protected:
	static void MeleeAttack(AttackData& data);
};
#endif