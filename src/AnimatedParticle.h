#pragma once
#include "configuration.h"
#include "AnimatedObject.h"

class AnimatedParticle : public AnimatedObject
{
public:
	AnimatedParticle(float ox, float oy, float dx, float dy, float speed, RgbColor baseColor, MyMosaic& mosaic);
	void Update(float dt, int lim, MyBus& strip);
private:
	float _ox;
	float _oy;
	float _dx;
	float _dy;
	float _speed;
	float _accumulatedTime;
	RgbColor _color;
	MyMosaic* _mosaic;
	
	int QPos();
};
