#pragma once
#include "Game/TempGameStates.hpp"
#include <map>

#ifndef TEMPGAMESTATE_HPP
#define TEMPGAMESTATE_HPP

class TempGameState;
extern TempGameState* prevState;

class TempGameState
{
private:
	TempGameStates m_state;
	std::map<std::string, TempGameState> stateToVisit;
public:
	TempGameState(const TempGameStates& state);
	~TempGameState();

	void Update();
	const TempGameStates GetState() const;

	const bool operator==(const TempGameStates& state) const;
	const bool operator==(const TempGameState& state) const;

};

/*
start up -> when finished -> MainMenu

MainMenu -> "esc" -> Confirm Exit
MainMenu -> "1" -> PickMap

PickMap-> "esc" -> MainMenu
PickMap -> "A" -> PickMap
PickMap -> "1, 2, ... n" -> Generating

Generating -> "esc" -> PickMap
Generating -> Spacebar -> Generating
Generating -> "Done Generating" + "Enter" -> Playing

Playing -> "Esc" -> Paused

Paused -> "c"  -> Playing
Paused -> "Q" -> ConfirmExit
Paused -> "M" -> MainMenu

ConfirmExit -> "y" -> Shutdown
ConfirmExit -> "n" -> [return to last visited state]

*/
#endif