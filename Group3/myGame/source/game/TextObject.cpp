#include "TextObject.h"


namespace Library
{
	RTTI_DEFINITIONS(TextObject)

		TextObject::TextObject(Game& game)
		: DrawableGameComponent(game), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(0.0f, 20.0f)
		{
			text << "undefined";
		}

	TextObject::~TextObject()
	{
		DeleteObject(mSpriteFont);
		DeleteObject(mSpriteBatch);
	}

	XMFLOAT2& TextObject::TextPosition()
	{
		return mTextPosition;
	}

	void TextObject::Initialize(std::string _text)
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());
		
		text.str(L"");
		text << L"" <<_text.c_str();
		mSpriteBatch = new SpriteBatch(mGame->Direct3DDeviceContext());
		mSpriteFont = new SpriteFont(mGame->Direct3DDevice(), L"Content\\Fonts\\Arial_14_Regular.spritefont");
	}

	void TextObject::Update(const GameTime& gameTime)
	{

	}

	void TextObject::Draw(const GameTime& gameTime)
	{
		mSpriteBatch->Begin();
		mSpriteFont->DrawString(mSpriteBatch, text.str().c_str(), mTextPosition, DirectX::Colors::Red);

		mSpriteBatch->End();
	}
}