#pragma once

#include "GameObject.h"
#include "ProxyModel.h"
#include "GameTime.h"
#include <math.h>

#define PI 3.14159265359

class Player : public GameObject
{

public:

	Player(Game& game, Camera& camera, Keyboard& keyboard);
	~Player();

	void Update(const GameTime& gameTime);
	void DrawDebug(const GameTime& gameTime);

	Rigidbody* GetRigidBody() { return rigid_; }

	void SetMoveable(bool move) { moveable_ = move; }

private:

	Keyboard* keyboard_;
	float speed_;

	float jump_force_;
	bool touching_ground_;

	double jump_timer_max_;
	double jump_timer_;

	XMFLOAT3 collider_offset_;
		
	Rigidbody* rigid_;
	//ProxyModel* debug_collider_;
	bool moveable_;

	float target_y_rotation_;

};