#pragma once
#include "configuration.h"
#include <LinkedList.h>
#include "AnimatedParticle.h"

class AnimatedSun
{
public:
	AnimatedSun(int ox, int oy, float speed, RgbColor baseColor, MyMosaic& mosaic);
	void Update(float dt, int lim, MyBus& strip);
private:
	int _ox;
	int _oy;
	float _speed;
	float _accumulatedTime;
	RgbColor _color;
	MyMosaic* _mosaic;
	LinkedList<AnimatedParticle*> sunParticles;
};
