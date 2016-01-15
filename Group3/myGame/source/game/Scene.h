#pragma once

#include <list>
#include "Rigidbody.h"
#include <iostream>
#include <algorithm>

#define GRAV 50

class Scene
{

public:

	Scene();

	void Step(double dt);
	std::list<Rigidbody*> GetBodyList() { return body_list_; }

	void ResolveSphereAABBCollision(Rigidbody* sphere, Rigidbody* aabb, Contact& c);
	void ResolveSphereSphere(Rigidbody* a, Rigidbody* b);

	bool TestSphereAABB(SphereCollider* s, AABB* b, Contact& contact);
	bool TestSphereSphere(SphereCollider* a, SphereCollider* b);

	void AddRigidBody( Rigidbody* body );
	void RemoveRigidBody( Rigidbody* body );

	void ClosestPointAABB(XMFLOAT3 p, AABB* b);
	float SqDistPointAABB(XMFLOAT3 p, AABB* b);


private:

	std::list<Rigidbody*> body_list_;
	int body_count_;

	XMFLOAT3 p_;

};