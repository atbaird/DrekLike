#pragma once

#ifndef TEMPGAMESTATES_HPP
#define TEMPGAMESTATES_HPP

enum TempGameStates
{
	State_Invalid = -1,
	State_StartUp,
	State_MainMenu,
	State_PickMap,
	State_Generation,
	State_Playing,
	State_Paused,
	State_ConfirmExit,
	State_Shutdown,
	State_Continue,
	NUMBER_OF_TEMPGAMESTATES
};


#endif