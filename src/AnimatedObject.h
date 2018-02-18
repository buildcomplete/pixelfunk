#pragma once

#include "configuration.h"

class AnimatedObject
{
public:
	virtual void Update(float dt, int lim, MyBus& strip) = 0;
};
