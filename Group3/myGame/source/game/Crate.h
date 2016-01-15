#pragma once

#include "GameObject.h"


class Crate : public GameObject
{

public:

	Crate(Game& game, Camera& camera, XMFLOAT3 pos);
	Rigidbody* GetRigidBody() { return rigid_; }
	void Update(const GameTime& gameTime);

private:

	Rigidbody* rigid_;

};

