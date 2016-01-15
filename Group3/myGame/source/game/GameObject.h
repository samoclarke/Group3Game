#pragma once

#include "Mesh.h"
#include <Common.h>
#include "DrawableGameComponent.h"
#include "Keyboard.h"
#include "Rigidbody.h"
#include "ShadowMappingMaterial.h"
#include "PointLight.h"

class GameObject : public DrawableGameComponent
{

public:

	GameObject(const std::string& name, Game& game, Camera& camera);
	~GameObject();

	void Draw(const GameTime& gameTime, XMVECTOR* spec_colour, float* spec_power, XMVECTOR* ambient_colour,
		PointLight* light, ID3D11ShaderResourceView* shadow_map, XMVECTOR* shadow_size, XMMATRIX* projective_view);

	virtual void Update(const GameTime& gameTime);

	void SetPosition(XMFLOAT3 position) { position_ = position; }
	XMFLOAT3 GetPosition() { return position_; }

	void SetScale(XMFLOAT3 scale) { scale_ = scale; }
	XMFLOAT3 GetScale() { return scale_; }

	void SetOrientation(XMFLOAT3 orientation) { orientation_ = orientation; }
	XMFLOAT3 GetOrientation() { return orientation_; }

	UINT GetModelIndexCount() { return m_model_index_count; }

	ID3D11Buffer* GetDepthBuffer() { return mDepthBuffer; }

	ID3D11Buffer* GetIndexBuffer() { return mIndexBuffer; }
	ID3D11Buffer* GetVertexBuffer() { return mVertexBuffer; }
	
	ID3D11Buffer** GetPtrToDepthBuffer();

	Mesh* GetMesh();
	XMFLOAT4X4* GetWorldMatrix();

protected:

	XMFLOAT3 position_;
	XMFLOAT3 scale_;
	XMFLOAT3 orientation_;

	XMFLOAT4X4 m_world_matrix;
	XMFLOAT4X4 mScaleMatrix;

	UINT m_model_index_count;

	Effect* mEffect;
	ShadowMappingMaterial* mMaterial;
	ID3D11ShaderResourceView* texture_;

	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mDepthBuffer;
	ID3D11Buffer* mIndexBuffer;

	ID3D11ShaderResourceView* model_texture_;

	Mesh* mesh_;
	Model* model_;

	Keyboard* mKeyboard;

	void StringToWString(std::wstring &ws, const std::string &s);

};