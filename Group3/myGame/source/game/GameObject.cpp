#include "GameObject.h"
#include "Game.h"
#include "MatrixHelper.h"
#include "Model.h"
#include "Effect.h"
#include "BasicMaterial.h"
#include "Camera.h"
#include "Utility.h"
#include "GameException.h"
#include <WICTextureLoader.h>

namespace Library
{

	GameObject::GameObject(const std::string& name, Game& game, Camera& camera)
		: DrawableGameComponent(game, camera), mEffect(nullptr), mMaterial(nullptr),
		mVertexBuffer(nullptr), mIndexBuffer(nullptr), m_model_index_count(0),
		mWorldMatrix(MatrixHelper::Identity), mScaleMatrix(MatrixHelper::Identity)
	{

		std::string s1 = "Content\\Models\\";
		s1 = s1 + name;

		std::unique_ptr<Model> model(new Model(*mGame, s1, true));
		Mesh* mesh = model->Meshes().at(0);
		mesh_ = mesh;
		m_model_index_count = mesh_->Indices().size();

		position_ = { 0.0f, 0.0f, 0.0f };
		scale_ = { 0.02f, 0.02f, 0.02f };
		orientation_ = { -1.5f, 0.0f, 0.0f };

		XMStoreFloat4x4(&mScaleMatrix, XMMatrixScaling(scale_.x, scale_.y, scale_.z));

		mEffect = new Effect(*mGame);
		mEffect->LoadCompiledEffect(L"Content\\Effects\\BasicEffect.cso");

		mMaterial = new BasicMaterial();
		mMaterial->Initialize(*mEffect);

		
		mMaterial->CreateVertexBuffer(mGame->Direct3DDevice(), *mesh_, &mVertexBuffer);
		mesh_->CreateIndexBuffer(&mIndexBuffer);

		std::wstring textureName = L"Content\\Textures\\house.bmp";
		HRESULT hr = DirectX::CreateWICTextureFromFile(mGame->Direct3DDevice(), mGame->Direct3DDeviceContext(), textureName.c_str(), nullptr, &texture_);
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

	void GameObject::Draw(const GameTime& gameTime)
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

		XMMATRIX wvp = XMLoadFloat4x4(&mWorldMatrix) * mCamera->ViewMatrix() * mCamera->ProjectionMatrix();
		mMaterial->WorldViewProjection() << wvp;

		pass->Apply(0, direct3DDeviceContext);

		direct3DDeviceContext->DrawIndexed(m_model_index_count, 0, 0);

	}

	void GameObject::Update(const GameTime & gameTime)
	{

		XMMATRIX worldMatrix = XMMatrixIdentity();
		
		XMStoreFloat4x4(&mWorldMatrix, XMMatrixRotationRollPitchYaw(orientation_.x, orientation_.y, orientation_.z) * 
			XMMatrixScaling(scale_.x, scale_.y, scale_.z) * XMMatrixTranslation(position_.x, position_.y, position_.z));
		
	}

	Mesh* GameObject::GetMesh()
	{
		return mesh_;
	}
	XMFLOAT4X4 GameObject::GetWorldMatrix()
	{
		return mWorldMatrix;
	}
}