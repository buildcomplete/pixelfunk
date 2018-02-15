#include <Arduino.h>
#include <NeoPixelBus.h>
#include "SerialCommands.h"

const uint16_t PixelCount = 50; 

// make sure to set this to the correct pins
const uint8_t DotDataPin = 2;
#define colorSaturation 128

// define type for the bus we are using
using MyBus = NeoPixelBus<NeoBrgFeature, NeoEspBitBangMethodBase<NeoEspBitBangSpeed400Kbps>>;

MyBus strip(PixelCount, DotDataPin);

char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");

//This is the default handler, and gets called when no other command matches. 
void cmd_unrecognized(SerialCommands* sender, const char* cmd)
{
	sender->GetSerial()->print("Unrecognized command [");
	sender->GetSerial()->print(cmd);
	sender->GetSerial()->println("]");
}

RgbColor RgbColorF(float r, float g, float b)
{
	return RgbColor(r*colorSaturation, g*colorSaturation, b*colorSaturation);
}

void cmdRgbFLed_cb(SerialCommands* sender)
{
	//Note: Every call to Next moves the pointer to next parameter
	char* idStr = sender->Next();
	char* rStr = sender->Next();
	char* gStr = sender->Next();
	char* bStr = sender->Next();
	if (   idStr == NULL 
		|| rStr == NULL
		|| gStr == NULL
		|| bStr == NULL )
	{
		sender->GetSerial()->println("no index: format [LEDF idx r g b], ex: LED 1 1.0 0.0 0.0 ");
		return;
	}
	int id = atoi(idStr);
	float r = atof(rStr);
	float g = atof(gStr);
	float b = atof(bStr);
	
	strip.SetPixelColor(id, RgbColorF(r,g,b));
}

void cmdRgbAllLed_cb(SerialCommands* sender)
{
	//Note: Every call to Next moves the pointer to next parameter
	char* rStr = sender->Next();
	char* gStr = sender->Next();
	char* bStr = sender->Next();
	if (   rStr == NULL
		|| gStr == NULL
		|| bStr == NULL )
	{
		sender->GetSerial()->println("format [ALL r g b], ex: LED 1 1.0 0.0 0.0 ");
		return;
	}
	int r = atoi(rStr);
	int g = atoi(gStr);
	int b = atoi(bStr);
	strip.ClearTo(RgbColor(r,g,b));
}

SerialCommand cmdRgbFLed("LEDF", cmdRgbFLed_cb);
SerialCommand cmdRgbAllLed("ALL", cmdRgbAllLed_cb);

void setup()
{
    Serial.begin(115200);
    while (!Serial); // wait for serial attach
	serial_commands_.SetDefaultHandler(&cmd_unrecognized);
    serial_commands_.AddCommand(&cmdRgbFLed);
	serial_commands_.AddCommand(&cmdRgbAllLed);
	

    // this resets all the neopixels to an off state
    strip.Begin();
    strip.ClearTo(RgbColorF(0,0,0));
    strip.Show();

}
class AnimatedObject
{
public:
	virtual void Update(float dt, int lim, MyBus& strip) = 0;
};

RgbColor black(0,0,0);

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
Walker mrGreen(1, 4, 0.100f, RgbColorF(0,1,0));
Walker mrRed(2, 6, 0.200f, RgbColorF(1,0,0));
Walker mrYellow(3, 10, 0.400f, RgbColorF(1,1,0));

AnimatedObject* animatedObjects[] 
{
	&mrBlue,
	&mrGreen,
	&mrRed,
	&mrYellow
};
void loop()
{
	//~ updateTimers();
	
	// update and draw animated objects
	//~ for (int i=0;i<4;++i)
	//~ {
		//~ animatedObjects[i]->Update(delta, PixelCount, strip);
	//~ }
	serial_commands_.ReadSerial();
    strip.Show();
}	
