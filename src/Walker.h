#pragma once

#include "AnimatedObject.h"

// structure to 'walk' a color on the strip...
class Walker : public AnimatedObject
{
public:
	Walker(int position, int length, float speed, RgbColor color);
	void Update(float dt, int lim, MyBus& strip);
private:
	int _position;
	int _length;
	float _speed;
	float _accumulatedTime;
	RgbColor _color;
};
