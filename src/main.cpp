#include <Arduino.h>
#include <NeoPixelBus.h>

const uint16_t PixelCount = 50; 

// make sure to set this to the correct pins
const uint8_t DotDataPin = 2;
#define colorSaturation 128

NeoPixelBus<NeoBrgFeature, NeoEspBitBangMethodBase<NeoEspBitBangSpeed400Kbps>> strip(PixelCount, DotDataPin);

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

// structure to 'walk' a color on the strip...
struct Walker
{
public:

	int _position;
	float _speed;
	float _accumulatedTime;
	RgbColor _color;

	Walker(int position, float speed, RgbColor color)
	{
		_position = position;
		_speed = speed;
		_accumulatedTime = 0;
		_color = color;
	}
	
	void Update(float dt, int lim)
	{
		_accumulatedTime += dt;
		int steps = _accumulatedTime / _speed;
		if (steps != 0)
		{
			_accumulatedTime -= (_speed * steps); 
			_position += steps;
			_position %= lim;
		}
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
Walker mrBlue(0, 0.166f, RgbColorF(0,0,1));
Walker mrGreen(1, 0.100f, RgbColorF(0,1,0));
Walker mrRed(2, 0.200f, RgbColorF(1,0,0));
Walker mrYellow(3, 0.400f, RgbColorF(1,1,0));

void loop()
{
	updateTimers();
	
	// update the walker position
	mrBlue.Update(delta, PixelCount);
	mrGreen.Update(delta, PixelCount);
	mrRed.Update(delta, PixelCount);
	mrYellow.Update(delta, PixelCount);
	
	// draw the walker
	strip.SetPixelColor(mrBlue._position, mrBlue._color);
	strip.SetPixelColor(mrGreen._position, mrGreen._color);
	strip.SetPixelColor(mrRed._position, mrRed._color);
	strip.SetPixelColor(mrYellow._position, mrYellow._color);
	
    strip.Show();
}	
