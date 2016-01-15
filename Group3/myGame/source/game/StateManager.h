#pragma once
class StateManager
{
public:
	enum GameState { ENUM_GAMESTATE_SPLASHSCREEN, ENUM_GAMESTATE_MAINMENU, ENUM_GAMESTATE_NEWGAME, ENUM_GAMESTATE_GAMEPLAY, ENUM_GAMESTATE_LOADGAME };

	static void SetState(GameState newState);
	static GameState gameState;
};

