#include "StateManager.h"
StateManager::GameState StateManager::gameState = StateManager::ENUM_GAMESTATE_SPLASHSCREEN;

void StateManager::SetState(GameState newState)
{
	gameState = newState;
}