#include "AnimatedParticle.h"
#include <Arduino.h>

AnimatedParticle::AnimatedParticle(int id, float ox, float oy, float dx, float dy, float speed, RgbColor baseColor, MyMosaic& mosaic, int* map)
{
	_id = id;
	_ox = ox;
	_oy = oy;
	
	// normalize direction vector to have length off speed
	float c = sqrt(dx*dx+dy*dy);
	
	_dx = speed * dx/c;
	_dy = speed * dy/c;
	_speed = speed;
	_color = baseColor;
	_mosaic = &mosaic;
	_map = map;
	
}

void AnimatedParticle::Update(float dt, int lim, MyBus& strip)
{
	_ox += _dx * dt;
	_oy += _dy * dt;

	// wrap
	if (_ox >= _mosaic->getWidth())
	{
		_ox = 0;
	}
	if (_oy >= _mosaic->getHeight())
	{
		_oy = 0;
	}
	
	if (_ox < 0)
	{
		_ox = _mosaic->getWidth()-1;
	}
	if (_oy < 0)
	{
		_oy = _mosaic->getHeight()-1;
	}

	if (QPos() != _lastDrawPos)
	{
		// If we where the last guy comming here, set the color to black when leaving, otherwise do nothing.
		if (_map[_lastDrawPos] == _id)
		{
			strip.SetPixelColor(_lastDrawPos, RgbColor(0));
		}
		
		// sample color already in field to 
		strip.SetPixelColor(QPos(), _color);
		_lastDrawPos = QPos();
		_map[_lastDrawPos] = _id;
	}
}

int AnimatedParticle::QPos()
{
	return _mosaic->Map(_ox, _oy);
}
