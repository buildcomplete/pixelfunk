#include "AnimatedParticle.h"
RgbColor pblack(0,0,0);

#include <Arduino.h>

AnimatedParticle::AnimatedParticle(float ox, float oy, float dx, float dy, float speed, RgbColor baseColor, MyMosaic& mosaic)
{
	_ox = ox;
	_oy = oy;
	
	// normalize direction vector to have length one
	float c = sqrt(dx*dx+dy*dy);
	
	_dx = dx/c;
	_dy = dy/c;
	_speed = speed;
	_accumulatedTime = 0;
	_color = baseColor;
	_mosaic = &mosaic;
}

void AnimatedParticle::Update(float dt, int lim, MyBus& strip)
{
	_accumulatedTime += dt;
	int steps = _accumulatedTime / _speed;
	if (steps > 0)
	{
		strip.SetPixelColor(QPos(), pblack);
		_ox += _dx * steps;
		_oy += _dy;
		
		// bounce
		if (_ox >= _mosaic->getWidth())
		{
			_dx = -_dx;
			_ox = _mosaic->getWidth() - 2;
		}
		if (_oy >= _mosaic->getHeight())
		{
			_dy = -_dy;
			_oy = _mosaic->getHeight() - 2;
		}
		
		if (_ox < 0)
		{
			_dx = -_dx;
			_ox = 0;
		}
		if (_oy < 0)
		{
			_dy = -_dy;
			_oy = 0;
		}
		
		
		_accumulatedTime -= _speed * ((float)steps);
		strip.SetPixelColor(QPos(), _color);
	}
}

int AnimatedParticle::QPos()
{
	return _mosaic->Map(_ox, _oy);
}
