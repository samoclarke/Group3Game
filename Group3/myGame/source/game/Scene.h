#pragma once

#include <Common.h>
#include <vector>
#include "RigidBody.h"

class Scene
{

public:

	Scene();
	~Scene();

	void SetGravity(float grav) { gravity_ = { 0.0f, grav, 0.0f }; }
	XMFLOAT3 GetGravity() { return gravity_; }
	float GetGravityF() { return gravity_.y; }

	void SetDT(float dt) { dt_ = dt; }

	void Step();

	float GetDT() { return dt_; }
	std::vector<RigidBody*> GetBodyList() { return body_list_; }

private:

	float dt_;
	float inv_dt_;
	std::vector<RigidBody*> body_list_;
	int body_count_;
	bool debug_draw_;
	XMFLOAT3 gravity_;

	void EulerIntegration(float d_time);

};