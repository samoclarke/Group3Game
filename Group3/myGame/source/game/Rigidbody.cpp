#include "Rigidbody.h"
#include "VectorHelper.h"

Rigidbody::Rigidbody(float mass, SphereCollider* sphere, XMFLOAT3 position)
{

	is_colliding_ = false;
	last_collision_data_.Init();

	position_ = position;
	mass_ = mass;
	velocity_ = { 0.0f ,0.0f ,0.0f };
	force_ = { 0.0f, 0.0f, 0.0f };

	sphere_collider_ = sphere;
	box_collider_ = NULL;

}

Rigidbody::Rigidbody(float mass, AABB* box, XMFLOAT3 position)
{

	is_colliding_ = false;
	last_collision_data_.Init();

	position_ = position;
	mass_ = mass;
	velocity_ = { 0.0f ,0.0f ,0.0f };
	force_ = { 0.0f, 0.0f, 0.0f };

	sphere_collider_ = NULL;
	box_collider_ = box;

}

void Rigidbody::Step(double dt, float gravity)
{

	if (mass_ > 0)
	{

		XMFLOAT3 acceleration = { force_.x / mass_, ( force_.y -= gravity ) / mass_, force_.z / mass_ };

		velocity_.x += acceleration.x * dt;
		velocity_.y += acceleration.y * dt;
		velocity_.z += acceleration.z * dt;

		position_.x += velocity_.x * dt;
		position_.y += velocity_.y * dt;
		position_.z += velocity_.z * dt;

		if (sphere_collider_ != NULL)
		{
			sphere_collider_->center = position_;
		}

		if (box_collider_ != NULL)
		{
			box_collider_->center = position_;
		}

		force_ = {0, 0, 0};

	}

}

void Rigidbody::AddForce(XMFLOAT3 force)
{
	force_.x += force.x;
	force_.y += force.y;
	force_.z += force.z;
}

void Rigidbody::AddVelocity(XMFLOAT3 add)
{
	velocity_.x += add.x;
	velocity_.y += add.y;
	velocity_.z += add.z;
}

void Rigidbody::AddPosition(XMFLOAT3 add)
{
	position_.x += add.x;
	position_.y += add.y;
	position_.z += add.z;
}
