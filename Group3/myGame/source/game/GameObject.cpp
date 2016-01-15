#include "GameObject.h"
#include "Game.h"
#include "MatrixHelper.h"
#include "VectorHelper.h"
#include "Model.h"
#include "Effect.h"
#include "Camera.h"
#include "Utility.h"
#include "GameException.h"
#include <WICTextureLoader.h>
#include "ShadowMappingMaterial.h"
#include "PointLight.h"

//namespace Rendering
//{

	//RTTI_DEFINITIONS(GameObject)

	GameObject::GameObject(const std::string& name, Game& game, Camera& camera)
		: DrawableGameComponent(game, camera), mEffect(nullptr), mMaterial(nullptr),
		mVertexBuffer(nullptr), mIndexBuffer(nullptr), m_model_index_count(0),
		m_world_matrix(MatrixHelper::Identity), mScaleMatrix(MatrixHelper::Identity), 
		model_texture_(nullptr), mesh_(nullptr), mDepthBuffer(nullptr)
	{

		std::string s1 = "Content\\Models\\";
		s1 = s1 + name + ".3Ds";

		std::string s1_texture = "Content\\Textures\\";
		s1_texture = s1_texture + name + ".PNG";

		model_ = new Model(*mGame, s1, true);
		mesh_ = model_->Meshes().at(0);

		m_model_index_count = mesh_->Indices().size();

		position_ = { 0.0f, 0.0f, 0.0f };
		scale_ = { 0.02f, 0.02f, 0.02f };
		orientation_ = { -1.5f, 0.0f, 0.0f };

		XMStoreFloat4x4(&mScaleMatrix, XMMatrixScaling(scale_.x, scale_.y, scale_.z));

		mEffect = new Effect(*mGame);
		mEffect->LoadCompiledEffect(L"Content\\Effects\\ShadowMapping.cso");

		mMaterial = new ShadowMappingMaterial();
		mMaterial->Initialize(*mEffect);


		mMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), *mesh_, &mVertexBuffer);
		mesh_->CreateIndexBuffer(&mIndexBuffer);

		std::wstring s1_texture_wstr;
		StringToWString(s1_texture_wstr, s1_texture);
		std::wstring textureName = s1_texture_wstr;
		HRESULT hr = DirectX::CreateWICTextureFromFile(mGame->Direct3DDevice(), mGame->Direct3DDeviceContext(), textureName.c_str(), nullptr, &model_texture_);
		if (FAILED(hr))
		{
			throw GameException("CreateWICTextureFromFile() failed.", hr);
		}

		mKeyboard = (Keyboard*)mGame->Services().GetService(Keyboard::TypeIdClass());

	}

	GameObject::~GameObject()
	{
		DeleteObject(mMaterial);
		DeleteObject(mEffect);
		ReleaseObject(mVertexBuffer);
		ReleaseObject(mIndexBuffer);
	}

	void GameObject::Draw(const GameTime& gameTime, XMVECTOR* spec_colour, float* spec_power, XMVECTOR* ambient_colour,
		PointLight* light, ID3D11ShaderResourceView* shadow_map, XMVECTOR* shadow_size, XMMATRIX* projective_view)
	{
		if (!mVisible)
		{
			return;
		}

		ID3D11DeviceContext* direct3DDeviceContext = mGame->Direct3DDeviceContext();
		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Pass* pass = mMaterial->CurrentTechnique()->Passes().at(0);
		ID3D11InputLayout* inputLayout = mMaterial->InputLayouts().at(pass);
		direct3DDeviceContext->IASetInputLayout(inputLayout);

		UINT stride = mMaterial->VertexSize();
		UINT offset = 0;
		direct3DDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX wvp = XMLoadFloat4x4(&m_world_matrix) * mCamera->ViewMatrix() * mCamera->ProjectionMatrix();
		XMMATRIX model_matrix = XMLoadFloat4x4(&m_world_matrix);

		mMaterial->WorldViewProjection() << wvp;
		mMaterial->World() << model_matrix;
		mMaterial->SpecularColor() << *spec_colour;
		mMaterial->SpecularPower() << *spec_power;
		mMaterial->AmbientColor() << *ambient_colour;
		mMaterial->LightColor() << light->ColorVector();
		mMaterial->LightPosition() << light->PositionVector();
		mMaterial->LightRadius() << light->Radius();

		mMaterial->ColorTexture() << model_texture_;
		mMaterial->CameraPosition() << mCamera->PositionVector();
		mMaterial->ProjectiveTextureMatrix() << model_matrix * (*projective_view);
		mMaterial->ShadowMap() << shadow_map;
		mMaterial->ShadowMapSize() << *shadow_size;

		pass->Apply(0, direct3DDeviceContext);

		direct3DDeviceContext->DrawIndexed(m_model_index_count, 0, 0);
		mGame->UnbindPixelShaderResources(0, 3);

	}

	void GameObject::Update(const GameTime & gameTime)
	{

		XMMATRIX worldMatrix = XMMatrixIdentity();

		XMStoreFloat4x4(&m_world_matrix, XMMatrixRotationRollPitchYaw(orientation_.x, orientation_.y, orientation_.z) *
			XMMatrixScaling(scale_.x, scale_.y, scale_.z) * XMMatrixTranslation(position_.x, position_.y, position_.z));

	}

	ID3D11Buffer ** GameObject::GetPtrToDepthBuffer()
	{
		ID3D11Buffer** temp = &mDepthBuffer;
		return temp;
	}

	Mesh* GameObject::GetMesh()
	{
		return mesh_;
	}

	XMFLOAT4X4* GameObject::GetWorldMatrix()
	{
		return &m_world_matrix;
	}


	void GameObject::StringToWString(std::wstring &ws, const std::string &s)
	{
		std::wstring wsTmp(s.begin(), s.end());

		ws = wsTmp;
	}
//}