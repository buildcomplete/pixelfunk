#include "Walker.h"
RgbColor black(0,0,0);

Walker::Walker(int position, int length, float speed, RgbColor color)
{
	_position = position;
	_length = length;
	_speed = speed;
	_accumulatedTime = 0;
	_color = color;
}
	
void Walker::Update(float dt, int lim, MyBus& strip )
{
	_accumulatedTime += dt;
	int steps = _accumulatedTime / _speed;
	if (steps != 0)
	{
		_accumulatedTime -= (_speed * steps); 
		_position += steps;
		_position %= lim;
		
		for (int i = 0; i < _length; ++i)
		{
			int idx = (_position - i + lim) % lim;
			RgbColor curColor = strip.GetPixelColor(idx);
			RgbColor newColor = RgbColor::BilinearBlend(
				curColor, 
				curColor, 
				_color, 
				black, 0.5f, (float)i/(float)(_length-1));
			strip.SetPixelColor(idx, newColor);
		}
	}
}
