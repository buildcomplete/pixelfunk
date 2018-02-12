#include <Arduino.h>
#include <NeoPixelBus.h>

const uint16_t PixelCount = 50; 

// make sure to set this to the correct pins
const uint8_t DotDataPin = 2;
#define colorSaturation 128

// define type for the bus we are using
using MyBus = NeoPixelBus<NeoBrgFeature, NeoEspBitBangMethodBase<NeoEspBitBangSpeed400Kbps>>;

MyBus strip(PixelCount, DotDataPin);

RgbColor RgbColorF(float r, float g, float b)
{
	return RgbColor(r*colorSaturation, g*colorSaturation, b*colorSaturation);
}

void setup()
{
    Serial.begin(115200);
    while (!Serial); // wait for serial attach

    Serial.println();
    Serial.println("Initializing...");
    Serial.flush();

    // this resets all the neopixels to an off state
    strip.Begin();
    strip.ClearTo(RgbColorF(0,0,0));
    strip.Show();

    Serial.println();
    Serial.println("Running...");
}
class AnimatedObject
{
public:
	virtual void Update(float dt, int lim, MyBus& strip) = 0;
};

// structure to 'walk' a color on the strip...
class Walker : public AnimatedObject
{
	int _position;
	int _length;
	float _speed;
	float _accumulatedTime;
	RgbColor _color;

public:
	Walker(int position, int length, float speed, RgbColor color)
	{
		_position = position;
		_length = length;
		_speed = speed;
		_accumulatedTime = 0;
		_color = color;
	}
	
	void Update(float dt, int lim, MyBus& strip)
	{
		_accumulatedTime += dt;
		int steps = _accumulatedTime / _speed;
		if (steps != 0)
		{
			_accumulatedTime -= (_speed * steps); 
			_position += steps;
			_position %= lim;
		}
		strip.SetPixelColor(_position, _color);
	}
};

float currentTime = 0;
float delta;
void updateTimers()
{
	float time = (float)millis() / 1000.0f;
    delta = time - currentTime;
    currentTime = time;
}

// create 4 walkers...
Walker mrBlue(0, 5, 0.166f, RgbColorF(0,0,1));
Walker mrGreen(1, 5, 0.100f, RgbColorF(0,1,0));
Walker mrRed(2, 5, 0.200f, RgbColorF(1,0,0));
Walker mrYellow(3, 5, 0.400f, RgbColorF(1,1,0));

AnimatedObject* animatedObjects[] 
{
	&mrBlue,
	&mrGreen,
	&mrRed,
	&mrYellow
};
void loop()
{
	updateTimers();
	
	// update and draw animated objects
	for (int i=0;i<4;++i)
	{
		animatedObjects[i]->Update(delta, PixelCount, strip);
	}
	
    strip.Show();
}	
