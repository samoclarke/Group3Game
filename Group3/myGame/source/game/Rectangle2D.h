#pragma once
#include "Game.h"
class Rectangle2D
{
public:
	Rectangle2D();
	Rectangle2D(float _x, float _y, float _w, float _h);
	~Rectangle2D();
	bool ContainsPoint(float x, float y);
	bool Intersects(Rectangle2D* otherRect);
	float x,y,w, h;
};

