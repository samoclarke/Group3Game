#pragma once

#include "GameObject.h"
#include "ProxyModel.h"

namespace Library
{
	class Player : public GameObject
	{

	public:

		Player(Game& game, Camera& camera, Keyboard& keyboard);
		~Player();

		void Update(const GameTime& gameTime);
		void DrawDebug(const GameTime& gameTime);

		Rigidbody* GetRigidBody() { return rigid_; }

	private:

		Keyboard* keyboard_;
		float speed_;

		float jump_force_;
		bool touching_ground_;

		double jump_timer_max_;
		double jump_timer_;

		XMFLOAT3 collider_offset_;
		
		Rigidbody* rigid_;
		ProxyModel* debug_collider_;

	};
}