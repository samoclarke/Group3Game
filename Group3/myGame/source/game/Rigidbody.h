#pragma once
#include "VectorHelper.h"

using namespace Library;

struct AABB
{
	XMFLOAT3 center;
	XMFLOAT3 halfwidths;

	XMFLOAT3 GetMin()
	{
		float x = center.x - halfwidths.x;
		float y = center.y - halfwidths.y;
		float z = center.z - halfwidths.z;
		return { x, y, z };
	}

	XMFLOAT3 GetMax()
	{
		float x = center.x + halfwidths.x;
		float y = center.y + halfwidths.y;
		float z = center.z + halfwidths.z;
		return{ x, y, z };
	}
};

struct SphereCollider
{
	XMFLOAT3 center;
	float r;
};

struct Contact
{
	void Init()
	{
		normal = { 0,0,0 };
		penetration = 0;
		point = { 0,0,0 };
	}

	XMFLOAT3 normal;
	float penetration;
	XMFLOAT3 point;
};

class Rigidbody
{

public:

	Rigidbody(float mass, SphereCollider* sphere, XMFLOAT3 position);
	Rigidbody(float mass, AABB* box, XMFLOAT3 position);

	void Step(double dt, float gravity);

	void AddForce(XMFLOAT3 force);

	XMFLOAT3 GetVelocity() { return velocity_; }
	void SetVelocity(XMFLOAT3 vel) { velocity_ = vel; }
	void AddVelocity(XMFLOAT3 add);

	XMFLOAT3 GetPosition() { return position_; }
	void SetPosition(XMFLOAT3 pos) { position_ = pos; }
	void AddPosition(XMFLOAT3 add);

	float GetMass() { return mass_; }

	SphereCollider* GetSphereCollider() { return sphere_collider_; }
	AABB* GetBoxCollider() { return box_collider_; }

	bool IsColliding() { return is_colliding_; }
	void SetColliding(bool colliding) { is_colliding_ = colliding; }

	Contact* GetLastCollisionData() { return &last_collision_data_; }
	void SetCollisionData(Contact contact) { last_collision_data_ = contact; }

private:

	float mass_;
	XMFLOAT3 position_;
	XMFLOAT3 velocity_;
	XMFLOAT3 force_;
	float friction_value_;

	SphereCollider* sphere_collider_;
	AABB* box_collider_;
	bool is_colliding_;
	Contact last_collision_data_;

};