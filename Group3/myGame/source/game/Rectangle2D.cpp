#include "Rectangle2D.h"


Rectangle2D::Rectangle2D() : x(0), y(0), w(0), h(0)
{

}

Rectangle2D::Rectangle2D(float _x, float _y, float _w, float _h)
{
	//Sorts the x and y so that they are less than the w and h
	float temp;
	if (x < w)
	{
		temp = w;
		w = x;
		x = temp;
	}
	if (y < h)
	{
		temp = h;
		h = y;
		y = temp;
	}
}

Rectangle2D::~Rectangle2D()
{
}

bool Rectangle2D::ContainsPoint(float otherX, float otherY)
{
	if (otherX > x)
	{
		if (otherX < w)
		{
			if (otherY > y)
			{
				return (otherY < h);
			}
		}
	}
	return false;
}

bool Rectangle2D::Intersects(Rectangle2D* otherRect)
{
	if (otherRect->x >= w)
	{
		if (x >= otherRect->w)
		{
			if (otherRect->y >= h)
			{
				return y >= otherRect->h;
			}
		}
	}
	return false;
}