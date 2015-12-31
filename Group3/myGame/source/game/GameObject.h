#pragma once

#include <Common.h>
#include "DrawableGameComponent.h"

namespace Library
{
	class Effect;
	class BasicMaterial;

	class GameObject : public DrawableGameComponent
	{

	public:

		GameObject(const std::string& name, Game& game, Camera& camera);
		~GameObject();

		void Draw(const GameTime& gameTime);
		void Update(const GameTime& gameTime);

		void SetPosition(XMFLOAT3 position) { position_ = position; }
		XMFLOAT3 GetPosition() { return position_; }

		void SetScale(XMFLOAT3 scale) { scale_ = scale; }
		XMFLOAT3 GetScale() { return scale_; }

		void SetOrientation(XMFLOAT3 orientation) { orientation_ = orientation; }
		XMFLOAT3 GetOrientation() { return orientation_; }

		UINT GetModelIndexCount() { return m_model_index_count; }

	private:

		XMFLOAT3 position_;
		XMFLOAT3 scale_;
		XMFLOAT3 orientation_;

		XMFLOAT4X4 mWorldMatrix;
		XMFLOAT4X4 mScaleMatrix;

		UINT m_model_index_count;
		Effect* mEffect;
		BasicMaterial* mMaterial;
		ID3D11Buffer* mVertexBuffer;
		ID3D11Buffer* mIndexBuffer;

	};
}