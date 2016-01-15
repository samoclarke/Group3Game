#include "MenuManager.h"

#pragma region MENUCURSOR
//=============================CURSOR==========================
MenuCursor::MenuCursor() : sprite(nullptr)
{
	position = XMFLOAT2(0.f, 0.f);
	currentButton = 0;
}

MenuCursor::MenuCursor(Game& game, Camera& camera) :currentButton(0)
{
	sprite = new Sprite(game, camera);
	sprite->Initialize();
	sprite->SetOrientation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	sprite->SetScale(XMFLOAT3(0.05f, 0.05f, 1.0f));
	sprite->SetPosition(XMFLOAT3(-3.5f, 0.0f, -10.0f));
	position = XMFLOAT2(-3.5f, 0.f);
}

void MenuCursor::SetPosition(XMFLOAT2 pos)
{
	position = pos;
	sprite->SetPosition(XMFLOAT3(pos.x, pos.y, -10.0f));
}

void MenuCursor::Update(const Library::GameTime& gameTime)
{
	sprite->Update(gameTime);
}

void MenuCursor::Draw(const Library::GameTime& gameTime)
{
	sprite->Draw(gameTime);
}
#pragma endregion
#pragma region MENUBUTTON
//==========================BUTTON==========================
MenuButton::MenuButton(Game& game, Camera& camera) : w(0), h(0), func("BUTTONFUNC_UNINITIALIZED"), buttonTex(nullptr),
text(nullptr)
{
	sprite = new Sprite(game, camera);
	sprite->SetOrientation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	sprite->SetScale(XMFLOAT3(0.25f, 0.05f, 1.0f));
	text = new TextObject(game);
}

void MenuButton::Initialize()
{
	//TODO MAKE THE TEXT APPEAR ON THE BUTTON
	sprite->SetPosition(XMFLOAT3(pos.x, pos.y, -10.0f));
	text->Initialize(name);
	float y = pos.y;
	if (pos.y < 0.0f)
	{
		y = pos.y * -2;
	}
	text->TextPosition() = XMFLOAT2(pos.x* 30, y * 30);
}

MenuButton::MenuButton(std::string function, XMFLOAT2 position, float width, float height, std::string buttonName) : buttonTex(nullptr)
{
	func = function;
	pos = position;
	w = width;
	h = height;
}
std::string MenuButton::GetFunction()
{
	return func;
}

void MenuButton::SetFunction(std::string newFunction)
{
	func = newFunction;
}
void MenuButton::SetPosition(float x, float y)
{
	pos = XMFLOAT2(x, y);
}

XMFLOAT3 MenuButton::GetPosition()
{
	return sprite->GetPosition();
}

void MenuButton::SetText(std::string newText)
{
	name = newText;
}

void MenuButton::Draw(const Library::GameTime& gameTime)
{
	sprite->Draw(gameTime);
	text->Draw(gameTime);
}

void MenuButton::Update(const Library::GameTime& gameTime)
{
	sprite->Update(gameTime);
	text->Update(gameTime);
}
#pragma endregion
#pragma region MENU
//===============================================MENU==================
void Menu::SetTitle(std::string newTitle)
{
	title = newTitle;
}

void Menu::SetParent(std::string newParent)
{
	parentMenu = newParent;
}

std::string Menu::GetParent()
{
	return parentMenu;
}

void Menu::AddButton(MenuButton btn)
{
	buttonList.push_back(btn);
}

std::vector<MenuButton>* Menu::GetButtonLayout()
{
	return &buttonList;
}

void Menu::Update(const Library::GameTime& gameTime)
{
	//TODO
	for (int i = 0; i < buttonList.size(); i++)
	{
		buttonList.at(i).Update(gameTime);
	}
}

void Menu::Draw(Library::GameTime gameTime)
{
	//TODO
	for (int i = 0; i < buttonList.size(); i++)
	{
		buttonList.at(i).Draw(gameTime);
	}
}
void Menu::Initialize()
{
	for (int i = 0; i < buttonList.size(); i++)
	{
		buttonList.at(i).Initialize();
	}
}
#pragma endregion
#pragma region MENUMANAGER
//==========================================MENUMANAGER==================
void MenuManager::Initialise(Library::Game &_game, Camera& camera)
{
	game = &_game;
	cursor = new MenuCursor(_game, camera);
}

MenuManager::MenuManager()
{

}

MenuManager::MenuManager(Game& _game, Camera& camera) : buttonModelPath("")
{
	//TODO?
	game = &_game;
	//cursor = MenuCursor(_game, camera);
	currentMenu = Menu();
	buttonModelPath = "Content/Models/plane.obj";
	cursor = new MenuCursor(_game, camera);
}


MenuManager::~MenuManager()
{
	//TODO
}

void MenuManager::ActivateButtonAtPosition(float x, float y)
{
	for (int i = 0; i < currentMenu.GetButtonLayout()->size(); i++)
	{
		if (currentMenu.GetButtonLayout()->at(i).rect.ContainsPoint(x, y))
		{
			ActivateButton(i);
		}
	}
}

void MenuManager::ActivateButton(int buttonID)
{
	std::string function = currentMenu.GetButtonLayout()->at(buttonID).GetFunction();
	{
		if (function == "BUTTONFUNC_UNINITIALIZED")
		{
			return;
		}
		if (function == "BUTTONFUNC_QUITTODESKTOP")
		{	/*TODO quit the game.*/
		}
		if (function == "BUTTONFUNC_OPTIONS")
		{
			SetMenuState(ENUM_MENUSTATE_OPTIONS);
			UseMenuLayout("Options");
		}
		if (function == "BUTTONFUNC_AUDIOOPTIONS")
		{
			SetMenuState(ENUM_MENUSTATE_AUDIO);
			UseMenuLayout("Audio Options");
		}
		if (function == "BUTTONFUNC_VIDEOOPTIONS")
		{
			SetMenuState(ENUM_MENUSTATE_VIDEO);
			UseMenuLayout("Video Options");
		}
		if (function == "BUTTONFUNC_CONTROLOPTIONS")
		{
			SetMenuState(ENUM_MENUSTATE_CONTROLS);
			UseMenuLayout("Control Options");
		}
		if (function == "BUTTONFUNC_PLAYGAME")
		{
			SetMenuState(ENUM_MENUSTATE_PLAYGAME);
			UseMenuLayout("Play Game");
			//TODO DEBUG
			StateManager::SetState(StateManager::ENUM_GAMESTATE_GAMEPLAY);
		}
		if (function == "BUTTONFUNC_NEWGAME")
		{
			StateManager::SetState(StateManager::ENUM_GAMESTATE_NEWGAME);
		}
		if (function == "BUTTONFUNC_LOADGAME")
		{
			StateManager::SetState(StateManager::ENUM_GAMESTATE_LOADGAME);
		}
		if (function == "BUTTONFUNC_BACK")
		{
			UseMenuLayout(currentMenu.GetParent());
		}
	}
}


void MenuManager::LoadMenuLayouts(Game& game, Camera& camera)
{
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<> * root_node;
	//Read the .xml file into a vector of chars
	std::ifstream xmlFile("Content\\Data\\menu.xml");
	std::vector<char> buffer((std::istreambuf_iterator<char>(xmlFile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	//Parse the buffer using the xml file parsing library
	doc.parse<0>(&buffer[0]);
	//find the root node (Menus)
	root_node = doc.first_node("Menus");
	//Find the first child node to root (Menu)
	rapidxml::xml_node<>* attrib_node = root_node->first_node("Menu");
	while (attrib_node != NULL)
	{
		Menu menu; //
		std::vector<MenuButton> btnList;

		rapidxml::xml_node<>* title_node = attrib_node->first_node("Title");
		menu.SetTitle(title_node->value());

		rapidxml::xml_node<>* parent_node = attrib_node->first_node("ParentMenu");
		menu.SetParent(parent_node->value());

		rapidxml::xml_node<>* button_node = attrib_node->first_node("Button");
		while (button_node != NULL)
		{
			MenuButton btn(game, camera);

			rapidxml::xml_node<>* text_node = button_node->first_node("Text");
			btn.SetText(text_node->value());

			rapidxml::xml_node<>* posX_node = button_node->first_node("PosX");
			rapidxml::xml_node<>* posY_node = button_node->first_node("PosY");
			btn.SetPosition((atof(posX_node->value())), atof(posY_node->value()));

			rapidxml::xml_node<>* func_node = button_node->first_node("Function");
			btn.SetFunction(func_node->value());

			button_node = button_node->next_sibling();
			menu.AddButton(btn);
		}
		menu.Initialize();
		menuLayouts.insert(std::pair<std::string, Menu>(title_node->value(), menu));
		attrib_node = attrib_node->next_sibling();
	}
}

void MenuManager::UseMenuLayout(std::string menuName)
{
	currentMenu = menuLayouts.find(menuName)->second;
}

void MenuManager::SetMenuState(MenuState newState)
{
	menuState = newState;
}

void MenuManager::Update(const Library::GameTime& gameTime, std::map<std::string, bool>* playerInput)
{
	currentMenu.Update(gameTime);
	
	if (playerInput->find("MenuUp")->second)
	{
		if (cursor->currentButton <= 0)
			cursor->currentButton = currentMenu.GetButtonCount() - 1;
		else
			cursor->currentButton = cursor->currentButton - 1;

		cursor->SetPosition(XMFLOAT2(cursor->position.x, currentMenu.GetButtonLayout()->at(cursor->currentButton).GetPosition().y));
	}
	if (playerInput->find("MenuDown")->second)
	{
		if (cursor->currentButton >= currentMenu.GetButtonCount() - 1)
			cursor->currentButton = 0;
		else
			cursor->currentButton = cursor->currentButton + 1;

		cursor->SetPosition(XMFLOAT2(cursor->position.x, currentMenu.GetButtonLayout()->at(cursor->currentButton).GetPosition().y));
	}

	if (playerInput->find("MenuBack")->second)
	{
		ActivateButton(cursor->currentButton);
	}


	cursor->Update(gameTime);
}

void MenuManager::Draw(const Library::GameTime &gameTime)
{
	currentMenu.Draw(gameTime);
	cursor->Draw(gameTime);
}
#pragma endregion