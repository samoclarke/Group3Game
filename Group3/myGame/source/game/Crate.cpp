#include "Crate.h"

Crate::Crate(Game& game, Camera& camera, XMFLOAT3 pos) : GameObject("Box", game, camera)
{
	position_ = pos;
	scale_ = { 0.15f, 0.15f, 0.15f };

	AABB* collider = new AABB();
	collider->center = { position_.x, position_.y, position_.z };
	collider->halfwidths = { scale_.x * 10, scale_.y * 10, scale_.z * 10 };

	rigid_ = new Rigidbody(0, collider, position_);
}

void Crate::Update(const GameTime & gameTime)
{

	XMMATRIX worldMatrix = XMMatrixIdentity();

	XMStoreFloat4x4(&m_world_matrix, XMMatrixRotationRollPitchYaw(orientation_.x, orientation_.y, orientation_.z) *
		XMMatrixScaling(scale_.x, scale_.y, scale_.z) * XMMatrixTranslation(position_.x, position_.y, position_.z));

}
