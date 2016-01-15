#pragma once
#include <vector>
#include <map>
#include "rapidxml.hpp"
#include "Model.h"
#include <fstream>
#include <string>
#include "Mesh.h"
#include "game.h"
#include "Rectangle2D.h"
#include "Sprite.h"
#include "StateManager.h"
#include "TextObject.h"

struct MenuCursor
{
	MenuCursor();
	MenuCursor(Game& game, Camera& camera);
	void SetPosition(XMFLOAT2 pos);
	void Update(const Library::GameTime& gameTime);
	void Draw(const Library::GameTime& gameTime);
	int currentButton;
	XMFLOAT2 position;
	Sprite* sprite;
};

class MenuButton
{
public:
	MenuButton(Game& game, Camera& camera);
	MenuButton(std::string function, XMFLOAT2 position, float width, float height, std::string buttonName);
	void Initialize();
	void SetPosition(float x, float y);
	XMFLOAT3 GetPosition();
	std::string GetFunction();
	void SetFunction(std::string);
	void SetText(std::string);
	void Draw(const Library::GameTime& gameTime);
	void Update(const Library::GameTime& gameTime);
	Rectangle2D rect;

protected:
	XMFLOAT2 pos;
	float w, h;
	std::string func;
	bool isActive;
	ID3D11Texture2D* buttonTex;
	std::string name;
	TextObject* text;
	Sprite* sprite;
	XMFLOAT4X4 GetWorldMatrix();

};
class Menu
{
public:
	std::vector<MenuButton>* GetButtonLayout();
	void SetTitle(std::string);
	void SetParent(std::string);
	std::string GetParent();
	void AddButton(MenuButton);
	void Update(const Library::GameTime& gameTime);
	void Draw(const Library::GameTime gameTime);
	void Initialize();
	int GetButtonCount(){ return buttonList.size(); }
private:
	std::string parentMenu;
	std::string title;
	std::vector<MenuButton> buttonList;
};

class MenuManager
{
public:
	MenuManager(Game& game, Camera& camera);
	MenuManager();
	~MenuManager();
	enum MenuState { ENUM_MENUSTATE_ROOT, ENUM_MENUSTATE_PLAYGAME, ENUM_MENUSTATE_OPTIONS, ENUM_MENUSTATE_AUDIO, ENUM_MENUSTATE_VIDEO, ENUM_MENUSTATE_CONTROLS };
	void SetMenuState(MenuState newState);
	MenuState GetMenuState() { return menuState;  }
	void Initialise(Library::Game &game, Camera& camera);
	void LoadMenuLayouts(Game& game, Camera& camera);
	void UseMenuLayout(std::string menuName);
	void ActivateButtonAtPosition(float x, float y);
	void ActivateButton(int buttonID);
	void Update(const Library::GameTime& gameTime, std::map<std::string, bool>* playerInput);
	void Draw(const Library::GameTime& gameTime);
private:
	MenuCursor* cursor;
	MenuState menuState;
	/*	We are storing the Menu in a seperate object as we don't want to overwrite the templates with our changes*/
	Menu currentMenu;
	/*A vector of all of the possible menu layouts. Use UseMenuLayout() to load one.*/
	std::map<std::string, Menu> menuLayouts;
	std::string buttonModelPath;
	const Library::Game* game;
};