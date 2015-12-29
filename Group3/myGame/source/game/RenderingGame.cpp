#include "RenderingGame.h"
#include "GameException.h"
#include "FirstPersonCamera.h"
#include "SamplerStates.h"
#include "RasterizerStates.h"
#include "ShadowMappingDemo.h"

//Mouse and Keyboard
#include "Keyboard.h"
#include "Mouse.h"

namespace Rendering
{;

	const XMFLOAT4 RenderingGame::BackgroundColor = { 0.5f, 0.5f, 0.5f, 1.0f };

    RenderingGame::RenderingGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand)
        :  Game(instance, windowClass, windowTitle, showCommand),
		mDirectInput(nullptr), mKeyboard(nullptr), 
		mMouse(nullptr),mFpsComponent(nullptr), 
		mRenderStateHelper(nullptr), mShadowMappingDemo(nullptr)
    {
        mDepthStencilBufferEnabled = true;
        mMultiSamplingEnabled = true;
    }

    RenderingGame::~RenderingGame()
    {

    }

    void RenderingGame::Initialize()
    {

        mCamera = new FirstPersonCamera(*this);
        mComponents.push_back(mCamera);
        mServices.AddService(Camera::TypeIdClass(), mCamera);

		mShadowMappingDemo = new ShadowMappingDemo(*this, *mCamera);
		mComponents.push_back(mShadowMappingDemo);
    
        //mDemo = new TriangleDemo(*this, *mCamera);
        //mComponents.push_back(mDemo);

		//mObjectDiffuseLight = new ObjectDiffuseLight(*this, *mCamera);
		//mObjectDiffuseLight->SetPosition(-1.57f, -0.0f, -0.0f, 0.01, -1.0f, 0.75f, -2.5f);
		//mComponents.push_back(mObjectDiffuseLight);

		RasterizerStates::Initialize(mDirect3DDevice);
		SamplerStates::Initialize(mDirect3DDevice);

		//mModel = new ModelFromFile(*this, *mCamera);
		//mModel->SetPosition(-1.57f, -0.0f, -0.0f, 0.005, 0.0f, -0.6f, 0.0f);
		//mComponents.push_back(mModel);

		//mModel2 = new ModelFromFile(*this, *mCamera);
		//mModel2->SetPosition(-1.57f, -1.0f, -0.0f, 0.005, 1.5f, -0.6f, 0.0f);
		//mComponents.push_back(mModel2);

		mFpsComponent = new FpsComponent(*this);
		mFpsComponent->Initialize();
		mRenderStateHelper = new RenderStateHelper(*this);

		if (FAILED(DirectInput8Create(mInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&mDirectInput, nullptr)))
		{
			throw GameException("DirectInput8Create() failed");
		}

		mKeyboard = new Keyboard(*this, mDirectInput);
		mComponents.push_back(mKeyboard);
		mServices.AddService(Keyboard::TypeIdClass(), mKeyboard);

		mMouse = new Mouse(*this, mDirectInput);
		mComponents.push_back(mMouse);
		mServices.AddService(Mouse::TypeIdClass(), mMouse);

        Game::Initialize();

		mCamera->SetPosition(0.0f, 0.0f, 5.0f);


    }

    void RenderingGame::Shutdown()
    {
		//DeleteObject(mDemo);
		DeleteObject(mShadowMappingDemo);
		DeleteObject(mKeyboard);
		DeleteObject(mMouse);
		//DeleteObject(mModel);
		//DeleteObject(mModel2);
		DeleteObject(mFpsComponent);
		DeleteObject(mRenderStateHelper);
		//DeleteObject(mObjectDiffuseLight);
        DeleteObject(mCamera);
        Game::Shutdown();
    }

    void RenderingGame::Update(const GameTime &gameTime)
    {

        Game::Update(gameTime);
		mFpsComponent->Update(gameTime);

		if (mKeyboard->WasKeyPressedThisFrame(DIK_ESCAPE))
		{
			Exit();
		}

    }

    void RenderingGame::Draw(const GameTime &gameTime)
    {
        mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&BackgroundColor));
        mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        Game::Draw(gameTime);

		mRenderStateHelper->SaveAll();
		mFpsComponent->Draw(gameTime);
		mRenderStateHelper->RestoreAll();
       
        HRESULT hr = mSwapChain->Present(0, 0);
        if (FAILED(hr))
        {
            throw GameException("IDXGISwapChain::Present() failed.", hr);
        }
    }
}