#include "Game/TempGameState.hpp"


TempGameState::TempGameState(const TempGameStates& state)
	:m_state(state)
{

}
TempGameState::~TempGameState()
{

}


void TempGameState::Update()
{

}
const TempGameStates TempGameState::GetState() const
{
	return m_state;
}

const bool TempGameState::operator==(const TempGameStates& state) const
{
	if (state == m_state)
	{
		return true;
	}
	return false;
}
const bool TempGameState::operator==(const TempGameState& state) const
{
	if (state.GetState() == m_state)
	{
		return true;
	}
	return false;
}