#pragma once
#include "Mesh.h"
#include <Common.h>
#include "DrawableGameComponent.h"
#include "Keyboard.h"
#include "Rigidbody.h"

namespace Library
{
	class Effect;
	class BasicMaterial;
	class Sprite : public DrawableGameComponent
	{
	public:
		Sprite(Game& game, Camera& camera);
		~Sprite();

		void Draw(const GameTime& gameTime);
		virtual void Update(const GameTime& gameTime);

		void SetPosition(XMFLOAT3 position) { position_ = position; }
		XMFLOAT3 GetPosition() { return position_; }

		void SetScale(XMFLOAT3 scale) { scale_ = scale; }
		XMFLOAT3 GetScale() { return scale_; }

		void SetOrientation(XMFLOAT3 orientation) { orientation_ = orientation; }
		XMFLOAT3 GetOrientation() { return orientation_; }

		UINT GetModelIndexCount() { return m_model_index_count; }

		Mesh* GetMesh();
		XMFLOAT4X4 GetWorldMatrix();

	private:
		int numberOfAnimations, numberOfFrames;
		int frameWidth, frameHeight;


		XMFLOAT3 position_;
		XMFLOAT3 scale_;
		XMFLOAT3 orientation_;

		XMFLOAT4 mColor;
		XMFLOAT4X4 mWorldMatrix;
		XMFLOAT4X4 mScaleMatrix;

		UINT m_model_index_count;

		Effect* mEffect;
		BasicMaterial* mMaterial;

		ID3D11Buffer* mVertexBuffer;
		ID3D11Buffer* mIndexBuffer;

		ID3D11ShaderResourceView* texture_;

		Mesh* mesh_;

		Keyboard* mKeyboard;

	};

}