#include <Arduino.h>
#include "configuration.h"

#include "SerialCommands.h"

#include <NeoPixelBus.h>
#include "AnimatedObject.h"
#include "Walker.h"
#include "PixelNoise.h"

#include <ESP8266WiFi.h>
#include <aREST.h>

#include "wifisecret.h"

#include <RunningAverage.h>

const uint16_t PixelCount = 512; 

// make sure to set this to the correct pins
const uint8_t DotDataPin = 2;

MyBus strip(PixelCount, DotDataPin);

char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
bool runAnimation = true;

aREST rest = aREST();
#define LISTEN_PORT 80



RgbColor GetJetColour(float v, float vmin, float vmax, float maxColor)
{
	if (v < vmin)
		v = vmin;
	if (v > vmax)
		v = vmax;

	double dv = vmax - vmin;

	if (v < (vmin + 0.25 * dv))
		return RgbColor(
			0, 
			(int)(maxColor * (4 * (v - vmin) / dv)),
			(int)maxColor);

	if (v < (vmin + 0.5 * dv))
		return RgbColor(
			0,
			(int)maxColor,
			(int)(maxColor * (1 + 4 * (vmin + 0.25 * dv - v) / dv)));

	if (v < (vmin + 0.75 * dv))
		return RgbColor(
			(int)(maxColor * (4 * (v - vmin - 0.5 * dv) / dv)),
			(int)maxColor,
			0);
	
	return RgbColor(
		(int)maxColor,
		(int)(maxColor * (1 + 4 * (vmin + 0.75 * dv - v) / dv)),
		0);
}

WiFiServer server(LISTEN_PORT);

int toggleLeds(String command)
{
	strip.ClearTo(RgbColor(255,255,255));
	return 1;
}

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
void cmdRgbLed_cb(SerialCommands* sender)
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
	int r = atoi(rStr);
	int g = atoi(gStr);
	int b = atoi(bStr);
	
	strip.SetPixelColor(id, RgbColor(r,g,b));
}

void cmdSetAnimationstate_cb(SerialCommands* sender)
{
	char* onOff = sender->Next();
	if (onOff == NULL)
	{
		sender->GetSerial()->println("format: ANIM On/Off ");
		return;
	}
	
	runAnimation = (strcmp(onOff, "On") == 0);
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

void cmdGetIP_cb(SerialCommands* sender)
{
	sender->GetSerial()->println(WiFi.localIP());
}

SerialCommand* _commands[] = 
{
	new SerialCommand("LEDF", cmdRgbFLed_cb),
	new SerialCommand("LED", cmdRgbLed_cb),
	new SerialCommand("ALL", cmdRgbAllLed_cb),
	new SerialCommand("ANIM", cmdSetAnimationstate_cb),
	new SerialCommand("IP", cmdGetIP_cb),
};
const int nCommands  = 5;

RunningAverage myRA(10);
float averageCycleDelay = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial); // wait for serial attach
	
	rest.function("led", toggleLeds);
	rest.variable("Average cycle delay", &averageCycleDelay);
	rest.variable("Animations running", &runAnimation);
	
	rest.set_id("1");
	rest.set_name("lightserver");
	
	WiFi.begin(WIFI_SSID,WIFI_PASS);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");
	
	server.begin();
	Serial.println("Server started");
	
	Serial.println(WiFi.localIP());
	
	serial_commands_.SetDefaultHandler(&cmd_unrecognized);
    for (int i=0;i<nCommands;++i)
	{
		serial_commands_.AddCommand(_commands[i]);
	}

    // this resets all the neopixels to an off state
    strip.Begin();
    strip.ClearTo(RgbColorF(0,0,0));
    strip.Show();
}

float currentTime = 0;
float delta;
void updateTimers()
{
	float time = (float)millis() / 1000.0f;
    delta = time - currentTime;
    currentTime = time;
}

AnimatedObject** animatedObjects = NULL;
const int nObjects = 256;

int updateVarDelay = 50;
int varCount = 0;
NeoGamma<NeoGammaEquationMethod> colorGamma;

void loop()
{
	if (animatedObjects == NULL)
	{
		
		animatedObjects = new AnimatedObject*[nObjects];
		//~ for (int i=0;i<nObjects; ++i)
			//~ animatedObjects[i] = new PixelNoise(i, 0.2, RgbColor(50,50,50));
		for (int i=0;i<nObjects;++i)
		{
			float v = ((float)i/(float)nObjects);
			animatedObjects[i] = new Walker(v * PixelCount, 5, 0.02, 
				colorGamma.Correct(GetJetColour(i, 0, nObjects-1, 100)));
		}
	}
	
	WiFiClient client = server.available();
	if (client )
	{
		while (!client.available())
		{
			delay(1);
		}
		rest.handle(client);
	}
	
	updateTimers();
	myRA.addValue(delta);
	if (varCount == updateVarDelay)
	{
		varCount=0;
		averageCycleDelay = myRA.getFastAverage();
	}
	else
	{
		++varCount;
	}
	
	
	if (runAnimation)
	{
		// update and draw animated objects
		for (int i=0;i<nObjects;++i)
		{
			animatedObjects[i]->Update(delta, PixelCount, strip);
		}
	}
	serial_commands_.ReadSerial();
    strip.Show();
}	
