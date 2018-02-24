#include "PixelNoise.h"

PixelNoise::PixelNoise(int position, float speed, RgbColor color)
{
	_position = position;
	_speed = speed;
	_accumulatedTime = 0;
	_color = color;
}

void PixelNoise::Update(float dt, int lim, MyBus& strip)
{
	_accumulatedTime += dt;
	if (_accumulatedTime > _speed)
	{
		_color = RgbColor::LinearBlend(
			_color,
			RgbColor(
				random(0, 128), 
				random(0, 128), 
				random(0, 128)),
			0.25);
		
		strip.SetPixelColor(_position, _color);
		_accumulatedTime = 0;
	}
}
