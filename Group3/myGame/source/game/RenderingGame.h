#pragma once


#include "Game.h"
//#include "ModelFromFile.h"
#include "FpsComponent.h"
#include "RenderStateHelper.h"

using namespace Library;

namespace Library
{
    class FirstPersonCamera;
	class RenderStateHelper;
	class Keyboard;
	class Mouse;
	class FpsComponent;
}

namespace Rendering
{
    //class TriangleDemo;
	//class ObjectDiffuseLight;

	class ShadowMappingDemo;

    class RenderingGame : public Game
    {
    public:
        RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand);
        ~RenderingGame();

        virtual void Initialize() override;		
        virtual void Update(const GameTime& gameTime) override;
        virtual void Draw(const GameTime& gameTime) override;

    protected:
        virtual void Shutdown() override;

    private:
		static const XMFLOAT4 BackgroundColor;
        FirstPersonCamera* mCamera;
        //TriangleDemo* mDemo;

		//Mouse and keyboard setup
		LPDIRECTINPUT8 mDirectInput;
		Keyboard* mKeyboard;
		Mouse* mMouse;
		//ModelFromFile* mModel;
		//ModelFromFile* mModel2;
		FpsComponent* mFpsComponent;
		RenderStateHelper* mRenderStateHelper;
		//ObjectDiffuseLight* mObjectDiffuseLight;

		ShadowMappingDemo* mShadowMappingDemo;

    };
}