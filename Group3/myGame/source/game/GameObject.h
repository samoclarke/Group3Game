#pragma once

#include <Common.h>
#include "Game.h"

class GameObject
{

public:

	GameObject();
	void Update(double dt);
	void Render();

	void SetPosition(XMFLOAT3 position) { position_ = position; }
	XMFLOAT3 GetPosition() { return position_; }

	void SetScale(XMFLOAT3 scale) { scale_ = scale; }
	XMFLOAT3 GetScale() { return scale_; }

	void SetOrientation(XMFLOAT3 orientation) { orientation_ = orientation; }
	XMFLOAT3 GetOrientation() { return orientation_; }

private:

	XMFLOAT3 position_;
	XMFLOAT3 scale_;
	XMFLOAT3 orientation_;

};