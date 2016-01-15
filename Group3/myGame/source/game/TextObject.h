#pragma once
#include "DrawableGameComponent.h"
#include <sstream>
#include <iomanip>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include "Game.h"
#include "Utility.h"
#include <string>

namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}

namespace Library
{
	class TextObject : public DrawableGameComponent
	{
		RTTI_DECLARATIONS(TextObject, DrawableGameComponent)

	public:
		TextObject(Game& game);
		~TextObject();

		XMFLOAT2& TextPosition();

		virtual void Initialize(std::string _text);
		virtual void Update(const GameTime& gameTime) override;
		virtual void Draw(const GameTime& gameTime) override;

	private:
		TextObject();
		TextObject(const TextObject& rhs);
		TextObject& operator=(const TextObject& rhs);

		SpriteBatch* mSpriteBatch;
		SpriteFont* mSpriteFont;
		XMFLOAT2 mTextPosition;
		std::wostringstream text;
	};
}