#include "ShadowMappingDemo.h"
#include "Game.h"
#include "GameException.h"
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

namespace Rendering
{
	RTTI_DEFINITIONS(ShadowMappingDemo)

		const float ShadowMappingDemo::LightModulationRate = UCHAR_MAX;
	const float ShadowMappingDemo::LightMovementRate = 10.0f;
	const XMFLOAT2 ShadowMappingDemo::LightRotationRate = XMFLOAT2(XM_2PI, XM_2PI);
	const UINT ShadowMappingDemo::DepthMapWidth = 4096U;
	const UINT ShadowMappingDemo::DepthMapHeight = 4096U;
	const RECT ShadowMappingDemo::DepthMapDestinationRectangle = { 0, 512, 256, 768 };
	const float ShadowMappingDemo::DepthBiasModulationRate = 10000;

	ShadowMappingDemo::ShadowMappingDemo(Game& game, Camera& camera)
		: DrawableGameComponent(game, camera), mCheckerboardTexture(nullptr),
		mPlanePositionVertexBuffer(nullptr), mPlanePositionUVNormalVertexBuffer(nullptr), mPlaneIndexBuffer(nullptr), mPlaneVertexCount(0),
		mKeyboard(nullptr), mAmbientColor(1.0f, 1.0f, 1.0, 0.2f), mPointLight(nullptr),
		mSpecularColor(1.0f, 1.0f, 1.0f, 1.0f), mSpecularPower(25.0f), mPlaneWorldMatrix(MatrixHelper::Identity), mProxyModel(nullptr),
		mProjector(nullptr), mProjectorFrustum(XMMatrixIdentity()), mRenderableProjectorFrustum(nullptr),
		mShadowMappingEffect(nullptr), mShadowMappingMaterial(nullptr),
		mProjectedTextureScalingMatrix(MatrixHelper::Zero), mRenderStateHelper(game),
		mModelPositionVertexBuffer(nullptr), mModelPositionUVNormalVertexBuffer(nullptr), mModelIndexBuffer(nullptr), mModelIndexCount(0),
		mModelWorldMatrix(MatrixHelper::Identity), mDepthMapEffect(nullptr), mDepthMapMaterial(nullptr), mDepthMap(nullptr), mDrawDepthMap(true),
		mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(0.0f, 40.0f), mActiveTechnique(ShadowMappingTechniqueSimple),
		mDepthBiasState(nullptr), mDepthBias(5.0f), mSlopeScaledDepthBias(5.0f), mFloorTexture(nullptr), follow_player_(true),
		gems_(0)
	{
	}

	ShadowMappingDemo::~ShadowMappingDemo()
	{
		ReleaseObject(mDepthBiasState);
		DeleteObject(mSpriteFont);
		DeleteObject(mSpriteBatch);
		DeleteObject(mDepthMap);
		DeleteObject(mDepthMapMaterial);
		DeleteObject(mDepthMapEffect);
		ReleaseObject(mModelIndexBuffer);
		ReleaseObject(mModelPositionUVNormalVertexBuffer);
		ReleaseObject(mModelPositionVertexBuffer);
		DeleteObject(mShadowMappingMaterial);
		DeleteObject(mShadowMappingEffect);
		DeleteObject(mRenderableProjectorFrustum);
		DeleteObject(mProjector);
		DeleteObject(mProxyModel);
		DeleteObject(mPointLight);
		ReleaseObject(mCheckerboardTexture);
		ReleaseObject(mPlanePositionUVNormalVertexBuffer);
		ReleaseObject(mPlanePositionVertexBuffer);
		ReleaseObject(mPlaneIndexBuffer);

		ReleaseObject(mFloorTexture);
	}

	void ShadowMappingDemo::Initialize()
	{
		SetCurrentDirectory(Utility::ExecutableDirectory().c_str());

		//Set up physics scene
		scene_ = new Scene();

		// Initialize materials
		mShadowMappingEffect = new Effect(*mGame);
		mShadowMappingEffect->LoadCompiledEffect(L"Content\\Effects\\ShadowMapping.cso");

		mShadowMappingMaterial = new ShadowMappingMaterial();
		mShadowMappingMaterial->Initialize(*mShadowMappingEffect);

		mDepthMapEffect = new Effect(*mGame);
		mDepthMapEffect->LoadCompiledEffect(L"Content\\Effects\\DepthMap.cso");

		mDepthMapMaterial = new DepthMapMaterial();
		mDepthMapMaterial->Initialize(*mDepthMapEffect);

		// Plane vertex buffers
		VertexPositionTextureNormal positionUVNormalVertices[] =
		{
			VertexPositionTextureNormal(XMFLOAT4(-0.5f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), Vector3Helper::Backward),
			VertexPositionTextureNormal(XMFLOAT4(-0.5f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f), Vector3Helper::Backward),
			VertexPositionTextureNormal(XMFLOAT4(0.5f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), Vector3Helper::Backward),

			VertexPositionTextureNormal(XMFLOAT4(-0.5f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), Vector3Helper::Backward),
			VertexPositionTextureNormal(XMFLOAT4(0.5f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), Vector3Helper::Backward),
			VertexPositionTextureNormal(XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), Vector3Helper::Backward),
		};

		mPlaneVertexCount = ARRAYSIZE(positionUVNormalVertices);
		std::vector<VertexPositionNormal> positionNormalVertices;
		positionNormalVertices.reserve(mPlaneVertexCount);
		std::vector<VertexPosition> positionVertices;
		positionVertices.reserve(mPlaneVertexCount);
		for (UINT i = 0; i < mPlaneVertexCount; i++)
		{
			positionNormalVertices.push_back(VertexPositionNormal(positionUVNormalVertices[i].Position, positionUVNormalVertices[i].Normal));
			positionVertices.push_back(VertexPosition(positionUVNormalVertices[i].Position));
		}

		mDepthMapMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), &positionVertices[0], mPlaneVertexCount, &mPlanePositionVertexBuffer);
		mShadowMappingMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), positionUVNormalVertices, mPlaneVertexCount, &mPlanePositionUVNormalVertexBuffer);



		std::wstring textureName = L"Content\\Textures\\house.bmp";
		HRESULT hr = DirectX::CreateWICTextureFromFile(mGame->Direct3DDevice(), mGame->Direct3DDeviceContext(), textureName.c_str(), nullptr, &mCheckerboardTexture);
		if (FAILED(hr))
		{
			throw GameException("CreateWICTextureFromFile() failed.", hr);
		}


		//create the floor texture

		textureName = L"Content\\Textures\\white.jpg";
		hr = DirectX::CreateWICTextureFromFile(mGame->Direct3DDevice(), mGame->Direct3DDeviceContext(), textureName.c_str(), nullptr, &mFloorTexture);
		if (FAILED(hr))
		{
			throw GameException("CreateWICTextureFromFile() failed.", hr);
		}


		mPointLight = new PointLight(*mGame);
		mPointLight->SetRadius(50.0f);
		mPointLight->SetPosition(0.0f, 5.0f, 30.0f);

		mKeyboard = (Keyboard*)mGame->Services().GetService(Keyboard::TypeIdClass());
		assert(mKeyboard != nullptr);

		mProxyModel = new ProxyModel(*mGame, *mCamera, "Content\\Models\\PointLightProxy.obj", XMFLOAT3{ 0.5,0.5,0.5 });
		mProxyModel->Initialize();

		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				AABB* box = new AABB();
				box->center = { (i * 10.0f) - 25, -5.0f, (j * 10.0f) - 25 };
				box->halfwidths = { 5, 5, 5 };

				ProxyModel* box_debug = new ProxyModel(*mGame, *mCamera, "Content\\Models\\OBBProxy.3ds", box->halfwidths);
				box_debug->Initialize();
				box_debug->SetPosition(box->center);

				Rigidbody* box_body = new Rigidbody(0, box, box->center);
				
				boxes_.push_back(box_body);
				debug_boxes_.push_back(box_debug);

				scene_->AddRigidBody(box_body);

			}
		}

		XMStoreFloat4x4(&mPlaneWorldMatrix, XMMatrixRotationX(-1.57f)* XMMatrixScaling(100.0f, 100.0f, 100.0f)* XMMatrixTranslation(0.0f, 0.0f, 5.5f));

		mProjector = new Projector(*mGame);
		mProjector->Initialize();

		mProjectorFrustum.SetMatrix(mProjector->ViewProjectionMatrix());

		mRenderableProjectorFrustum = new RenderableFrustum(*mGame, *mCamera);
		mRenderableProjectorFrustum->Initialize();
		mRenderableProjectorFrustum->InitializeGeometry(mProjectorFrustum);

		InitializeProjectedTextureScalingMatrix();

		// Vertex and index buffers for a second model to render
		std::unique_ptr<Model> model(new Model(*mGame, "Content\\Models\\house.3ds", true));

		Mesh* mesh = model->Meshes().at(0);
		mDepthMapMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), *mesh, &mModelPositionVertexBuffer);
		mShadowMappingMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), *mesh, &mModelPositionUVNormalVertexBuffer);
		mesh->CreateIndexBuffer(&mModelIndexBuffer);
		mModelIndexCount = mesh->Indices().size();

		XMStoreFloat4x4(&mModelWorldMatrix, XMMatrixRotationX(-1.57f)* XMMatrixScaling(0.025f, 0.025f, 0.025f) * XMMatrixTranslation(0.0f, 4.25f, -4.5f));

		mDepthMap = new DepthMap(*mGame, DepthMapWidth, DepthMapHeight);
		mSpriteBatch = new SpriteBatch(mGame->Direct3DDeviceContext());
		mSpriteFont = new SpriteFont(mGame->Direct3DDevice(), L"Content\\Fonts\\Arial_14_Regular.spritefont");


		player_ = new Player(*mGame, *mCamera, *mKeyboard);
		objs_.push_back(player_);

		crate_ = new Crate(*mGame, *mCamera, { 0,2,10 });
		objs_.push_back(crate_);

		GameObject* box = new GameObject("Box", *mGame, *mCamera);
		box->SetScale({ 0.2f, 0.2f, 0.2f });
		objs_.push_back(box);

		for (int i = 0; i < objs_.size(); ++i)
		{
			mDepthMapMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), (*objs_[i]->GetMesh()), objs_[i]->GetPtrToDepthBuffer());
		}

		scene_->AddRigidBody(player_->GetRigidBody());
		scene_->AddRigidBody(crate_->GetRigidBody());

		UpdateDepthBiasState();
	}

	void ShadowMappingDemo::Update(const GameTime& gameTime)
	{
		if (mKeyboard != nullptr && mKeyboard->WasKeyPressedThisFrame(DIK_RETURN))
		{
			mDrawDepthMap = !mDrawDepthMap;
		}

		UpdateTechnique();
		UpdateDepthBias(gameTime);
		UpdateAmbientLight(gameTime);
		UpdatePointLightAndProjector(gameTime);
		UpdateSpecularLight(gameTime);

		for (int i = 0; i < objs_.size(); i++)
		{
			objs_[i]->Update(gameTime);
		}

		for (int i = 0; i < debug_boxes_.size(); ++i)
		{
			debug_boxes_[i]->Update(gameTime);
		}

		scene_->Step(gameTime.ElapsedGameTime());

		XMFLOAT3 cam_pos = { player_->GetPosition().x, player_->GetPosition().y + 5, player_->GetPosition().z + 15 };

		if (follow_player_)
		{
			mCamera->SetPosition(cam_pos);
		}

		if (mKeyboard->WasKeyReleasedThisFrame(DIK_LCONTROL))
		{
			follow_player_ = !follow_player_;
		}

		player_->SetMoveable(follow_player_);

		mProxyModel->Update(gameTime);
		mProjector->Update(gameTime);
		mRenderableProjectorFrustum->Update(gameTime);

		if (player_->GetPosition().y < -40)
		{
			player_->GetRigidBody()->SetPosition({ 0,10,0 });
			player_->GetRigidBody()->SetVelocity({ 0,0,0 });
		}

	}

	void ShadowMappingDemo::Draw(const GameTime& gameTime)
	{
		static float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		// Depth map pass (render the teapot model only)
		mRenderStateHelper.SaveRasterizerState();
		mDepthMap->Begin();

		ID3D11DeviceContext* direct3DDeviceContext = mGame->Direct3DDeviceContext();
		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		direct3DDeviceContext->ClearDepthStencilView(mDepthMap->DepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		Pass* pass = mDepthMapMaterial->CurrentTechnique()->Passes().at(0);
		ID3D11InputLayout* inputLayout = mDepthMapMaterial->InputLayouts().at(pass);
		direct3DDeviceContext->IASetInputLayout(inputLayout);

		direct3DDeviceContext->RSSetState(mDepthBiasState);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		UINT stride = mDepthMapMaterial->VertexSize();
		UINT offset = 0;
		direct3DDeviceContext->IASetVertexBuffers(0, 1, &mModelPositionVertexBuffer, &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mModelIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX modelWorldMatrix = XMLoadFloat4x4(&mModelWorldMatrix);
		mDepthMapMaterial->WorldLightViewProjection() << modelWorldMatrix * mProjector->ViewMatrix() * mProjector->ProjectionMatrix();

		pass->Apply(0, direct3DDeviceContext);

		direct3DDeviceContext->DrawIndexed(mModelIndexCount, 0, 0); //shadow map drawing

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		for (int i = 0; i < objs_.size(); ++i)
		{
			ID3D11Buffer* ind_buffer = objs_[i]->GetIndexBuffer();
			direct3DDeviceContext->IASetVertexBuffers(0, 1, objs_[i]->GetPtrToDepthBuffer(), &stride, &offset);
			direct3DDeviceContext->IASetIndexBuffer(ind_buffer, DXGI_FORMAT_R32_UINT, 0);

			XMFLOAT4X4* model_matrix = objs_[i]->GetWorldMatrix();
			XMMATRIX model_world_matrix = XMLoadFloat4x4(model_matrix);
			mDepthMapMaterial->WorldLightViewProjection() << model_world_matrix * mProjector->ViewMatrix() * mProjector->ProjectionMatrix();

			pass->Apply(0, direct3DDeviceContext);

			direct3DDeviceContext->DrawIndexed(objs_[i]->GetModelIndexCount(), 0, 0); //shadow map drawing
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		mDepthMap->End();
		mRenderStateHelper.RestoreRasterizerState();

		// Projective texture mapping pass
		pass = mShadowMappingMaterial->CurrentTechnique()->Passes().at(0);
		inputLayout = mShadowMappingMaterial->InputLayouts().at(pass);
		direct3DDeviceContext->IASetInputLayout(inputLayout);

		// Draw model
		stride = mShadowMappingMaterial->VertexSize();
		direct3DDeviceContext->IASetVertexBuffers(0, 1, &mPlanePositionUVNormalVertexBuffer, &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mPlaneIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX planeWorldMatrix = XMLoadFloat4x4(&mPlaneWorldMatrix);
		XMMATRIX planeWVP = planeWorldMatrix * mCamera->ViewMatrix() * mCamera->ProjectionMatrix();
		XMMATRIX projectiveTextureMatrix = planeWorldMatrix * mProjector->ViewMatrix() * mProjector->ProjectionMatrix() * XMLoadFloat4x4(&mProjectedTextureScalingMatrix);
		XMVECTOR ambientColor = XMLoadColor(&mAmbientColor);
		XMVECTOR specularColor = XMLoadColor(&mSpecularColor);
		XMVECTOR shadowMapSize = XMVectorSet(static_cast<float>(DepthMapWidth), static_cast<float>(DepthMapHeight), 0.0f, 0.0f);

		mShadowMappingMaterial->WorldViewProjection() << planeWVP;
		mShadowMappingMaterial->World() << planeWorldMatrix;
		mShadowMappingMaterial->SpecularColor() << specularColor;
		mShadowMappingMaterial->SpecularPower() << mSpecularPower;
		mShadowMappingMaterial->AmbientColor() << ambientColor;
		mShadowMappingMaterial->LightColor() << mPointLight->ColorVector();
		mShadowMappingMaterial->LightPosition() << mPointLight->PositionVector();
		mShadowMappingMaterial->LightRadius() << mPointLight->Radius();


		//floor
		mShadowMappingMaterial->ColorTexture() << mFloorTexture;
		mShadowMappingMaterial->CameraPosition() << mCamera->PositionVector();
		mShadowMappingMaterial->ProjectiveTextureMatrix() << projectiveTextureMatrix;
		mShadowMappingMaterial->ShadowMap() << mDepthMap->OutputTexture();
		mShadowMappingMaterial->ShadowMapSize() << shadowMapSize;

		pass->Apply(0, direct3DDeviceContext);

		direct3DDeviceContext->Draw(mPlaneVertexCount, 0); //draw the floor
		mGame->UnbindPixelShaderResources(0, 3);

		// Draw model
		direct3DDeviceContext->IASetVertexBuffers(0, 1, &mModelPositionUVNormalVertexBuffer, &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mModelIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX modelWVP = modelWorldMatrix * mCamera->ViewMatrix() * mCamera->ProjectionMatrix();
		projectiveTextureMatrix = modelWorldMatrix * mProjector->ViewMatrix() * mProjector->ProjectionMatrix() * XMLoadFloat4x4(&mProjectedTextureScalingMatrix);

		mShadowMappingMaterial->WorldViewProjection() << modelWVP;
		mShadowMappingMaterial->World() << modelWorldMatrix;
		mShadowMappingMaterial->SpecularColor() << specularColor;
		mShadowMappingMaterial->SpecularPower() << mSpecularPower;
		mShadowMappingMaterial->AmbientColor() << ambientColor;
		mShadowMappingMaterial->LightColor() << mPointLight->ColorVector();
		mShadowMappingMaterial->LightPosition() << mPointLight->PositionVector();
		mShadowMappingMaterial->LightRadius() << mPointLight->Radius();
		//house
		mShadowMappingMaterial->ColorTexture() << mCheckerboardTexture;
		mShadowMappingMaterial->CameraPosition() << mCamera->PositionVector();
		mShadowMappingMaterial->ProjectiveTextureMatrix() << projectiveTextureMatrix;
		mShadowMappingMaterial->ShadowMap() << mDepthMap->OutputTexture();
		mShadowMappingMaterial->ShadowMapSize() << shadowMapSize;

		pass->Apply(0, direct3DDeviceContext);

		direct3DDeviceContext->DrawIndexed(mModelIndexCount, 0, 0); //draw the main object
		mGame->UnbindPixelShaderResources(0, 3);

		XMMATRIX projective_texture = mProjector->ViewMatrix() * mProjector->ProjectionMatrix() * XMLoadFloat4x4(&mProjectedTextureScalingMatrix);
		//player_->Draw(gameTime, &specularColor, &mSpecularPower, &ambientColor, mPointLight, mDepthMap->OutputTexture(), &shadowMapSize, &projective_texture);

		for (int i = 0; i < objs_.size(); i++)
		{
			objs_[i]->Draw(gameTime, &specularColor, &mSpecularPower, &ambientColor, mPointLight, mDepthMap->OutputTexture(), &shadowMapSize, &projective_texture);
		}

		for (int i = 0; i < debug_boxes_.size(); ++i)
		{
			debug_boxes_[i]->Draw(gameTime);
		}

		mProxyModel->Draw(gameTime);
		mRenderableProjectorFrustum->Draw(gameTime);

		player_->DrawDebug(gameTime);

		mRenderStateHelper.SaveAll();
		mSpriteBatch->Begin();


		std::wostringstream helpLabel;
		helpLabel << L"Gems: " << gems_ << "\n\n";
		helpLabel << L"Control: Free Floating Cam" << "\n";
		helpLabel << L"player_y_rot:" << player_->GetOrientation().y << "\n";


		//helpLabel << std::setprecision(5) << L"Active Technique (Space): " << ShadowMappingDisplayNames[mActiveTechnique].c_str() << "\n";


		mSpriteFont->DrawString(mSpriteBatch, helpLabel.str().c_str(), mTextPosition);

		mSpriteBatch->End();
		mRenderStateHelper.RestoreAll();
	}

	void ShadowMappingDemo::UpdateTechnique()
	{
		if (mKeyboard != nullptr && mKeyboard->WasKeyPressedThisFrame(DIK_SPACE))
		{
			mShadowMappingMaterial->SetCurrentTechnique(*mShadowMappingMaterial->GetEffect()->TechniquesByName().at(ShadowMappingTechniqueNames[ShadowMappingTechniquePCF]));
			mDepthMapMaterial->SetCurrentTechnique(*mDepthMapMaterial->GetEffect()->TechniquesByName().at(DepthMappingTechniqueNames[ShadowMappingTechniquePCF]));
		}
	}

	void ShadowMappingDemo::UpdateDepthBias(const GameTime& gameTime)
	{
		if (mKeyboard != nullptr)
		{
			if (mKeyboard->IsKeyDown(DIK_O))
			{
				mSlopeScaledDepthBias += (float)gameTime.ElapsedGameTime();
				UpdateDepthBiasState();
			}

			if (mKeyboard->IsKeyDown(DIK_P) && mSlopeScaledDepthBias > 0)
			{
				mSlopeScaledDepthBias -= (float)gameTime.ElapsedGameTime();
				mSlopeScaledDepthBias = XMMax(mSlopeScaledDepthBias, 0.0f);
				UpdateDepthBiasState();
			}

			if (mKeyboard->IsKeyDown(DIK_J))
			{
				mDepthBias += DepthBiasModulationRate * (float)gameTime.ElapsedGameTime();
				UpdateDepthBiasState();
			}

			if (mKeyboard->IsKeyDown(DIK_K) && mDepthBias > 0)
			{
				mDepthBias -= DepthBiasModulationRate * (float)gameTime.ElapsedGameTime();
				mDepthBias = XMMax(mDepthBias, 0.0f);
				UpdateDepthBiasState();
			}
		}
	}

	void ShadowMappingDemo::UpdateDepthBiasState()
	{
		ReleaseObject(mDepthBiasState);

		D3D11_RASTERIZER_DESC rasterizerStateDesc;
		ZeroMemory(&rasterizerStateDesc, sizeof(rasterizerStateDesc));
		rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
		rasterizerStateDesc.DepthClipEnable = true;
		rasterizerStateDesc.DepthBias = (int)mDepthBias;
		rasterizerStateDesc.SlopeScaledDepthBias = mSlopeScaledDepthBias;

		HRESULT hr = mGame->Direct3DDevice()->CreateRasterizerState(&rasterizerStateDesc, &mDepthBiasState);
		if (FAILED(hr))
		{
			throw GameException("ID3D11Device::CreateRasterizerState() failed.", hr);
		}
	}

	void ShadowMappingDemo::UpdateAmbientLight(const GameTime& gameTime)
	{
		static float ambientIntensity = mAmbientColor.a;

		if (mKeyboard != nullptr)
		{
			if (mKeyboard->IsKeyDown(DIK_PGUP) && ambientIntensity < UCHAR_MAX)
			{
				ambientIntensity += LightModulationRate * (float)gameTime.ElapsedGameTime();

				mAmbientColor.a = (UCHAR)XMMin<float>(ambientIntensity, UCHAR_MAX);
			}

			if (mKeyboard->IsKeyDown(DIK_PGDN) && ambientIntensity > 0)
			{
				ambientIntensity -= LightModulationRate * (float)gameTime.ElapsedGameTime();
				mAmbientColor.a = (UCHAR)XMMax<float>(ambientIntensity, 0);
			}
		}
	}

	void ShadowMappingDemo::UpdateSpecularLight(const GameTime& gameTime)
	{
		static float specularIntensity = mSpecularPower;

		if (mKeyboard != nullptr)
		{
			if (mKeyboard->IsKeyDown(DIK_INSERT) && specularIntensity < UCHAR_MAX)
			{
				specularIntensity += LightModulationRate * (float)gameTime.ElapsedGameTime();
				specularIntensity = XMMin<float>(specularIntensity, UCHAR_MAX);

				mSpecularPower = specularIntensity;;
			}

			if (mKeyboard->IsKeyDown(DIK_DELETE) && specularIntensity > 0)
			{
				specularIntensity -= LightModulationRate * (float)gameTime.ElapsedGameTime();
				specularIntensity = XMMax<float>(specularIntensity, 0);

				mSpecularPower = specularIntensity;
			}
		}
	}

	void ShadowMappingDemo::UpdatePointLightAndProjector(const GameTime& gameTime)
	{
		static float pointLightIntensity = mPointLight->Color().a;
		float elapsedTime = (float)gameTime.ElapsedGameTime();

		// Update point light intensity		
		if (mKeyboard->IsKeyDown(DIK_HOME) && pointLightIntensity < UCHAR_MAX)
		{
			pointLightIntensity += LightModulationRate * elapsedTime;

			XMCOLOR pointLightLightColor = mPointLight->Color();
			pointLightLightColor.a = (UCHAR)XMMin<float>(pointLightIntensity, UCHAR_MAX);
			mPointLight->SetColor(pointLightLightColor);
		}
		if (mKeyboard->IsKeyDown(DIK_END) && pointLightIntensity > 0)
		{
			pointLightIntensity -= LightModulationRate * elapsedTime;

			XMCOLOR pointLightLightColor = mPointLight->Color();
			pointLightLightColor.a = (UCHAR)XMMax<float>(pointLightIntensity, 0.0f);
			mPointLight->SetColor(pointLightLightColor);
		}

		// Move point light and projector
		XMFLOAT3 movementAmount = Vector3Helper::Zero;
		if (mKeyboard != nullptr)
		{
			if (mKeyboard->IsKeyDown(DIK_NUMPAD4))
			{
				movementAmount.x = -1.0f;
			}

			if (mKeyboard->IsKeyDown(DIK_NUMPAD6))
			{
				movementAmount.x = 1.0f;
			}

			if (mKeyboard->IsKeyDown(DIK_NUMPAD9))
			{
				movementAmount.y = 1.0f;
			}

			if (mKeyboard->IsKeyDown(DIK_NUMPAD3))
			{
				movementAmount.y = -1.0f;
			}

			if (mKeyboard->IsKeyDown(DIK_NUMPAD8))
			{
				movementAmount.z = -1.0f;
			}

			if (mKeyboard->IsKeyDown(DIK_NUMPAD2))
			{
				movementAmount.z = 1.0f;
			}
		}

		XMVECTOR movement = XMLoadFloat3(&movementAmount) * LightMovementRate * elapsedTime;
		mPointLight->SetPosition(mPointLight->PositionVector() + movement);
		mProxyModel->SetPosition(mPointLight->Position());
		mProjector->SetPosition(mPointLight->Position());
		mRenderableProjectorFrustum->SetPosition(mPointLight->Position());

		// Rotate projector
		XMFLOAT2 rotationAmount = Vector2Helper::Zero;
		if (mKeyboard->IsKeyDown(DIK_LEFTARROW))
		{
			rotationAmount.x += LightRotationRate.x * elapsedTime;
		}
		if (mKeyboard->IsKeyDown(DIK_RIGHTARROW))
		{
			rotationAmount.x -= LightRotationRate.x * elapsedTime;
		}
		if (mKeyboard->IsKeyDown(DIK_UPARROW))
		{
			rotationAmount.y += LightRotationRate.y * elapsedTime;
		}
		if (mKeyboard->IsKeyDown(DIK_DOWNARROW))
		{
			rotationAmount.y -= LightRotationRate.y * elapsedTime;
		}

		XMMATRIX projectorRotationMatrix = XMMatrixIdentity();
		if (rotationAmount.x != 0)
		{
			projectorRotationMatrix = XMMatrixRotationY(rotationAmount.x);
		}

		if (rotationAmount.y != 0)
		{
			XMMATRIX projectorRotationAxisMatrix = XMMatrixRotationAxis(mProjector->RightVector(), rotationAmount.y);
			projectorRotationMatrix *= projectorRotationAxisMatrix;
		}

		if (rotationAmount.x != Vector2Helper::Zero.x || rotationAmount.y != Vector2Helper::Zero.y)
		{
			mProjector->ApplyRotation(projectorRotationMatrix);
			mRenderableProjectorFrustum->ApplyRotation(projectorRotationMatrix);
		}
	}

	void ShadowMappingDemo::InitializeProjectedTextureScalingMatrix()
	{
		mProjectedTextureScalingMatrix._11 = 0.5f;
		mProjectedTextureScalingMatrix._22 = -0.5f;
		mProjectedTextureScalingMatrix._33 = 1.0f;
		mProjectedTextureScalingMatrix._41 = 0.5f;
		mProjectedTextureScalingMatrix._42 = 0.5f;
		mProjectedTextureScalingMatrix._44 = 1.0f;
	}
}