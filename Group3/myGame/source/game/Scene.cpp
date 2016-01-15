#include "Scene.h"

Scene::Scene()
{

}

void Scene::Step(double dt)
{

	for
	(
		std::list<Rigidbody*>::iterator iter = body_list_.begin();
		iter != body_list_.end();
		iter++
	)
	{
		(*iter)->Step(dt, GRAV);
		(*iter)->SetColliding(false);
	}


	for
		(
			std::list<Rigidbody*>::iterator iter = body_list_.begin();
			iter != body_list_.end();
			iter++
			)
	{
		for
			(
				std::list<Rigidbody*>::iterator iter_j = body_list_.begin();
				iter_j != body_list_.end();
				iter_j++
				)
		{
			if (iter != iter_j)
			{

				if ((*iter)->GetBoxCollider() != NULL)
				{
					//a is box, b is sphere
					if ((*iter_j)->GetSphereCollider() != NULL)
					{
						p_ = { 0,0,0 };
						Contact c;
						if (TestSphereAABB((*iter_j)->GetSphereCollider(), (*iter)->GetBoxCollider(), c))
						{
							(*iter_j)->SetColliding(true);
							(*iter_j)->SetCollisionData(c);

							(*iter)->SetColliding(true);
							(*iter)->SetCollisionData(c);

							ResolveSphereAABBCollision((*iter_j), (*iter), c);
						}
					}
				}

				/*
				if ((*iter_j)->GetBoxCollider() != NULL)
				{
					//a is box, b is sphere
					if ((*iter)->GetSphereCollider() != NULL)
					{
						p_ = { 0,0,0 };
						Contact c;
						if (TestSphereAABB((*iter)->GetSphereCollider(), (*iter_j)->GetBoxCollider(), c))
						{
							ResolveSphereAABBCollision((*iter), (*iter_j), c);
						}
					}
				}
				*/

			}

		}
	}

}

void Scene::ResolveSphereAABBCollision(Rigidbody* sphere, Rigidbody* aabb, Contact& c)
{

	XMVECTOR normal_vector = XMLoadFloat3(&c.normal);
	normal_vector = XMVector3Normalize(normal_vector);

	XMVECTOR rel_vel = XMLoadFloat3(&sphere->GetVelocity()) - XMLoadFloat3(&aabb->GetVelocity());
	float contact_velocity = XMVectorGetX(XMVector3Dot(rel_vel, normal_vector));

	if (contact_velocity < 0)
	{
		return;
	}

	float im1;
	if (sphere->GetMass() != 0)
	{
		im1 = 1 / sphere->GetMass();
	}
	else
	{
		im1 = 0;
	}

	float im2;
	if (aabb->GetMass() != 0)
	{
		im2 = 1 / aabb->GetMass();
	}
	else
	{
		im2 = 0;
	}

	float j = -contact_velocity;
	j /= im1 + im2;

	XMVECTOR impulse = j * normal_vector;

	XMVECTOR sphere_vel_impulse = im1 * impulse;
	XMVECTOR box_vel_impulse = im2 * impulse;

	XMFLOAT3 impulse_arg;
	XMStoreFloat3(&impulse_arg, sphere_vel_impulse);
	
	sphere->AddVelocity(impulse_arg);

	XMStoreFloat3(&impulse_arg, box_vel_impulse);
	aabb->AddVelocity(impulse_arg);

	//float k_slop = 0.0f; // Penetration allowance
	float percent = 0.01f; // Penetration percentage to correct
	float penetration_mult = c.penetration;

	if (penetration_mult < 0)
	{
		penetration_mult = 0;
	}

	XMVECTOR correction = percent * normal_vector;

	XMFLOAT3 sphere_correction;
	XMStoreFloat3(&sphere_correction, (im1 * correction) * -1);
	sphere->AddPosition(sphere_correction);

	XMFLOAT3 box_correction;
	XMStoreFloat3(&box_correction, (im2 * correction) );
	sphere->AddPosition(box_correction);

}

void Scene::ResolveSphereSphere(Rigidbody * a, Rigidbody * b)
{

	XMFLOAT3 a_pos = a->GetPosition();
	XMFLOAT3 b_pos = b->GetPosition();

	XMFLOAT3 delta = a_pos;
	delta.x -= b_pos.x;
	delta.y -= b_pos.y;
	delta.z -= b_pos.z;

	XMVECTOR delta_vector = XMLoadFloat3(&delta);
	float length = XMVectorGetX(XMVector3Length(delta_vector));
	
	float mtd_a = ((a->GetSphereCollider()->r + b->GetSphereCollider()->r) - length) / length;
	XMVECTOR mtd = delta_vector * mtd_a;

	float im1 = 1 / a->GetMass();
	float im2 = 1 / b->GetMass();

	XMVECTOR mtd_mult = mtd * (im1 / (im1 + im2));
	XMVECTOR mtd_mult_2 = mtd * (im2 / (im1 + im2));

	XMFLOAT3 arg;
	
	XMStoreFloat3(&arg, XMLoadFloat3(&a_pos) + mtd_mult);
	a->SetPosition(arg);

	XMStoreFloat3(&arg, XMLoadFloat3(&b_pos) - mtd_mult_2);
	b->SetPosition(arg);

}

bool Scene::TestSphereAABB(SphereCollider* s, AABB* b, Contact& contact)
{
	ClosestPointAABB(s->center, b);

	XMVECTOR v = XMLoadFloat3(&p_) - XMLoadFloat3(&s->center);
	if (XMVectorGetX(XMVector3Dot(v, v)) <= s->r * s->r)
	{
		XMFLOAT3 norm;
		XMStoreFloat3(&norm, v);
		contact.normal = norm;
		contact.penetration = fabsf(s->r - SqDistPointAABB(s->center, b));
		contact.a_pos = s->center;
		contact.b_pos = b->center;
		return true;
	}

	return false;
}

bool Scene::TestSphereSphere(SphereCollider* a, SphereCollider* b)
{
	XMFLOAT3 delta = { a->center.x - b->center.x, a->center.y - b->center.y, a->center.z - b->center.z };
	float sum_of_radi = a->r + b->r;
	float sqr_of_radi = sum_of_radi * sum_of_radi;

	float dist_sqr = (delta.x * delta.x) + (delta.y * delta.y) + (delta.z * delta.z);

	if (dist_sqr <= sqr_of_radi)
	{
		return true;
	}

	return false;
}

void Scene::AddRigidBody(Rigidbody* body)
{
	body_list_.push_back(body);
}

void Scene::RemoveRigidBody(Rigidbody* body)
{
	for
	(
		std::list<Rigidbody*>::iterator iter = body_list_.begin();
		iter != body_list_.end();
		iter++
	)
	{
		if ((*iter) == body)
		{
			iter = body_list_.erase(iter);
		}
	}
}

void Scene::ClosestPointAABB(XMFLOAT3 p, AABB * b)
{
	float point_array	[3] = { p.x, p.y, p.z };
	float min_array		[3] = { b->GetMin().x, b->GetMin().y, b->GetMin().z };
	float max_array		[3] = { b->GetMax().x, b->GetMax().y, b->GetMax().z };
	float q_array		[3] = { 0,0,0 };

	for (int i = 0; i < 3; ++i)
	{
		float v = point_array[i];
		if (v < min_array[i]) v = min_array[i];
		if (v > max_array[i]) v = max_array[i];
		q_array[i] = v;
	}

	p_.x = q_array[0];
	p_.y = q_array[1];
	p_.z = q_array[2];
}

float Scene::SqDistPointAABB(XMFLOAT3 p, AABB * b)
{
	float sq_dist = 0;

	float p_array[3] = { p.x, p.y, p.z };
	float min_array[3] = { b->GetMin().x, b->GetMin().y, b->GetMin().z };
	float max_array[3] = { b->GetMax().x, b->GetMax().y, b->GetMax().z };

	for (int i = 0; i < 3; i++)
	{
		float v = p_array[i];
		if (v < min_array[i]) sq_dist += (min_array[i] - v) * (min_array[i] - v);
		if (v > min_array[i]) sq_dist += (v - max_array[i]) * (v - max_array[i]);
	}

	return sq_dist;
}
