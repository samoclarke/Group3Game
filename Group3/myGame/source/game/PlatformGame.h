#pragma once
#include <vector>
#include <map>
#include "rapidxml.hpp"
#include <fstream>
#include <string>

#include "Game.h"
#include "FpsComponent.h"
#include "RenderStateHelper.h"
#include "MenuManager.h"
#include "Sprite.h"
#include "Player.h"
#include "Scene.h"
#include "Crate.h"

#include "GameException.h"
#include "FirstPersonCamera.h"
#include "SamplerStates.h"
#include "RasterizerStates.h"


#include "Frustum.h"
#include "Game.h"
#include "VectorHelper.h"
#include "MatrixHelper.h"
#include "ColorHelper.h"
#include "Camera.h"
#include "Model.h"
#include "Mesh.h"
#include "Utility.h"
#include "PointLight.h"
#include "Keyboard.h"
#include <WICTextureLoader.h>
#include "ProxyModel.h"
#include "Projector.h"
#include "RenderableFrustum.h"
#include "ShadowMappingMaterial.h"
#include "DepthMapMaterial.h"
#include "DepthMap.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <sstream>
#include <iomanip>
#include "GameObject.h"

//Mouse and Keyboard
#include "Keyboard.h"
#include "Mouse.h"
using namespace Library;

namespace Library
{
	class Frustum;
	class Effect;
	class PointLight;
	class Keyboard;
	class ProxyModel;
	class Projector;
	class RenderableFrustum;
	class ShadowMappingMaterial;
	class DepthMapMaterial;
	class DepthMap;
}

namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}

namespace Rendering
{
	////////////////////////////////////////////////////////////////////////////////////
	enum ShadowMappingTechnique
	{
		ShadowMappingTechniqueSimple = 0,
		ShadowMappingTechniqueManualPCF,
		ShadowMappingTechniquePCF,
		ShadowMappingTechniqueEnd
	};

	const std::string ShadowMappingTechniqueNames[] = { "shadow_mapping", "shadow_mapping_manual_pcf", "shadow_mapping_pcf" };
	const std::string ShadowMappingDisplayNames[] = { "Shadow Mapping Simple", "Shadow Mapping w/ Manual PCF", "Shadow Mapping w/ PCF" };
	const std::string DepthMappingTechniqueNames[] = { "create_depthmap", "create_depthmap", "create_depthmap_w_bias", };

	//////////////////////////////////////////////////////////////////////////////////

	class Platformer;
	class PlatformGame : public Game
	{
	public:
		PlatformGame(HINSTANCE instance, const std::wstring& windowClass, const std::wstring& windowTitle, int showCommand);
		~PlatformGame();

		virtual void Initialize() override;
		virtual void Update(const GameTime& gameTime) override;
		virtual void Draw(const GameTime& gameTime) override;
		//Load the user settings file.
		void LoadSettings();
		//This function will handle updating the key presses based on the current game state.
		void UpdateKeys();
	protected:
		virtual void Shutdown() override;
	private:
		static XMFLOAT4 BackgroundColor;
		FirstPersonCamera* mCamera;
		//Mouse and keyboard setup
		LPDIRECTINPUT8 mDirectInput;
		Keyboard* mKeyboard;
		Mouse* mMouse;
		FpsComponent* mFpsComponent;
		RenderStateHelper* mRenderStateHelper;
		Platformer* mPlatformer;
		MenuManager menuManager;

		//Contains all of the keys mapped to action names.
		std::map<std::string, int> keyBindings;
		std::map<std::string, bool> input;

		/////////////////////////////////////////////////////////////////////////////

		void UpdateTechnique();
		void UpdateDepthBias(const GameTime& gameTime);
		void UpdateDepthBiasState();
		void UpdateAmbientLight(const GameTime& gameTime);
		void UpdatePointLightAndProjector(const GameTime& gameTime);
		void UpdateSpecularLight(const GameTime& gameTime);
		void InitializeProjectedTextureScalingMatrix();

		static const float LightModulationRate;
		static const float LightMovementRate;
		static const XMFLOAT2 LightRotationRate;
		static const UINT DepthMapWidth;
		static const UINT DepthMapHeight;
		static const RECT DepthMapDestinationRectangle;
		static const float DepthBiasModulationRate;

		XMCOLOR mAmbientColor;
		PointLight* mPointLight;
		XMCOLOR mSpecularColor;
		float mSpecularPower;
		//ProxyModel* mProxyModel;

		Projector* mProjector;
		Frustum mProjectorFrustum;
		RenderableFrustum* mRenderableProjectorFrustum;

		ID3D11Buffer* mPlanePositionVertexBuffer;
		ID3D11Buffer* mPlanePositionUVNormalVertexBuffer;
		ID3D11Buffer* mPlaneIndexBuffer;
		UINT mPlaneVertexCount;
		XMFLOAT4X4 mPlaneWorldMatrix;
		ID3D11ShaderResourceView* mCheckerboardTexture;

		ID3D11ShaderResourceView* mFloorTexture;

		Effect* mShadowMappingEffect;
		ShadowMappingMaterial * mShadowMappingMaterial;
		ID3D11Buffer* mModelPositionVertexBuffer;
		ID3D11Buffer* mModelPositionUVNormalVertexBuffer;
		ID3D11Buffer* mModelIndexBuffer;
		UINT mModelIndexCount;
		XMFLOAT4X4 mModelWorldMatrix;
		XMFLOAT4X4 mProjectedTextureScalingMatrix;

		Effect* mDepthMapEffect;
		DepthMapMaterial* mDepthMapMaterial;
		DepthMap* mDepthMap;
		bool mDrawDepthMap;

		ShadowMappingTechnique mActiveTechnique;
		XMFLOAT2 mTextPosition;
		ID3D11RasterizerState* mDepthBiasState;
		float mDepthBias;
		float mSlopeScaledDepthBias;

		std::vector<GameObject*> objs_;

		Player* player_;
		Crate* crate_;

		std::vector<Rigidbody*> boxes_;
		//std::vector<ProxyModel*> debug_boxes_;

		Scene* scene_;

		bool follow_player_;

		int gems_;

		void GamePlayUpdate(const GameTime& gameTime);
		void GamePlayDraw(const GameTime& gameTime);

	};
}

