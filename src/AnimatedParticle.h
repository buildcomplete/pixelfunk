#pragma once
#include "configuration.h"
#include "AnimatedObject.h"

class AnimatedParticle : public AnimatedObject
{
public:
	AnimatedParticle(int id, float ox, float oy, float dx, float dy, float speed, RgbColor baseColor, MyMosaic& mosaic, int* particleRegisterMap);
	void Update(float dt, int lim, MyBus& strip);
private:
	int _id;
	float _ox;
	float _oy;
	float _dx;
	float _dy;
	float _speed;
	RgbColor _color;
	MyMosaic* _mosaic;
	int QPos();
	int _lastDrawPos = -1;
	int* _map;
};
