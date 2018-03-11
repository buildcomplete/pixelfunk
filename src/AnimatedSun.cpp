#include "AnimatedSun.h"


AnimatedSun::AnimatedSun(int ox, int oy, float speed, RgbColor baseColor, MyMosaic& mosaic)
{
	_ox = ox;
	_oy = oy;
	_speed = speed;
	_accumulatedTime = 0;
	_color = baseColor;
	_mosaic = &mosaic;
}

void AnimatedSun::Update(float dt, int lim, MyBus& strip)
{
	_accumulatedTime += dt;
	int spawns = _accumulatedTime / _speed;
	if (spawns != 0)
	{
		
	}
	
}
