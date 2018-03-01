#pragma once

#include "AnimatedObject.h"

// structure to 'walk' a color on the strip...
class PixelNoise : public AnimatedObject
{
public:
	PixelNoise(int position, float speed, RgbColor color);
	void Update(float dt, int lim, MyBus& strip);
private:
	int _position;
	float _speed;
	float _accumulatedTime;
	RgbColor _color;
};
