#include "Player.h"
#include "GameTime.h"
#include <math.h>

Player::Player(Game& game, Camera& camera, Keyboard& keyboard) : GameObject("Miguel.3ds", game, camera)
{
	jump_timer_max_ = 0.1;
	jump_timer_ = jump_timer_max_;

	touching_ground_ = false;

	speed_ = 20.0f;
	jump_force_ = 8000.0f;
	keyboard_ = &keyboard;

	position_ = { 0,1,0 };

	SphereCollider* collider = new SphereCollider();
	//collider_offset_ = { 0.0f, 2.5f, 0.0f };
	collider->center = { position_.x, position_.y, position_.z };
	collider->r = 2.5f;

	rigid_ = new Rigidbody(1, collider, position_);

	debug_collider_ = new ProxyModel(*mGame, *mCamera, "Content\\Models\\PointLightProxy.obj", XMFLOAT3{ 2.5,2.5,2.5 });
	debug_collider_->Initialize();
}

Player::~Player()
{
	DeleteObject(debug_collider_);
}

void Player::Update(const GameTime& gameTime)
{
	touching_ground_ = false;

	if (rigid_->IsColliding())
	{
		XMVECTOR pos = XMLoadFloat3(&GetPosition());
		XMVECTOR point = XMLoadFloat3(&rigid_->GetLastCollisionData()->point);

		XMVECTOR pos_to_point = point - pos;
		pos_to_point = XMVector3Normalize(pos_to_point);
		XMVECTOR up = { 0,1,0 };

		float t = XMVectorGetX(XMVector3Dot(up, pos_to_point));
		if ( t < 0.0f)
		{
			touching_ground_ = true;
		}

	}

	if (touching_ground_ && jump_timer_ < ( jump_timer_max_ + 0.1 ))
	{
		jump_timer_ += gameTime.ElapsedGameTime();
	}

	XMFLOAT3 force = { 0.0f, 0.0f, 0.0f };

	if (keyboard_ != nullptr && !keyboard_->IsKeyDown(DIK_LCONTROL))
	{
		if (keyboard_->IsKeyDown(DIK_W))
		{
			force.z -= speed_;
		}

		if (keyboard_->IsKeyDown(DIK_S))
		{
			force.z += speed_;
		}

		if (keyboard_->IsKeyDown(DIK_D))
		{
			force.x += speed_;
		}

		if (keyboard_->IsKeyDown(DIK_A))
		{
			force.x -= speed_;
		}

		if (keyboard_->IsKeyDown(DIK_SPACE))
		{
			if (touching_ground_ && jump_timer_ > jump_timer_max_)
			{
				force.y += jump_force_;
				jump_timer_ = 0.0;
			}
		}

		if (keyboard_->IsKeyDown(DIK_R))
		{
			rigid_->SetPosition({ 0,10,0 });
			rigid_->SetVelocity({ 0,0,0 });
		}

	}
	
	rigid_->AddForce(force);

	XMMATRIX worldMatrix = XMMatrixIdentity();

	XMStoreFloat4x4(&mWorldMatrix, XMMatrixRotationRollPitchYaw(orientation_.x, orientation_.y, orientation_.z) *
		XMMatrixScaling(scale_.x, scale_.y, scale_.z) * XMMatrixTranslation(position_.x, position_.y, position_.z));

	position_ = rigid_->GetPosition();
	debug_collider_->SetPosition(rigid_->GetPosition());
	debug_collider_->Update(gameTime);

}

void Player::DrawDebug(const GameTime& gameTime)
{
	debug_collider_->Draw(gameTime);
}
