#include <Arduino.h>
#include <NeoPixelBus.h>
#include "SerialCommands.h"
#include "AnimatedObject.h"
#include "Walker.h"
#include "configuration.h"
#include <ESP8266WiFi.h>
#include <aREST.h>
#include "wifisecret.h"

const uint16_t PixelCount = 50; 

// make sure to set this to the correct pins
const uint8_t DotDataPin = 2;

MyBus strip(PixelCount, DotDataPin);

char serial_command_buffer_[32];
SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\r\n", " ");
bool runAnimation = false;

aREST rest = aREST();
#define LISTEN_PORT 80

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

SerialCommand cmdRgbFLed("LEDF", cmdRgbFLed_cb);
SerialCommand cmdRgbAllLed("ALL", cmdRgbAllLed_cb);
SerialCommand cmdSetAnimstate("ANIM", cmdSetAnimationstate_cb);

void setup()
{
    Serial.begin(115200);
    while (!Serial); // wait for serial attach
	
	rest.function("led", toggleLeds);
	
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
    serial_commands_.AddCommand(&cmdRgbFLed);
	serial_commands_.AddCommand(&cmdRgbAllLed);
	serial_commands_.AddCommand(&cmdSetAnimstate);
	

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
	if (runAnimation)
	{
		// update and draw animated objects
		for (int i=0;i<4;++i)
		{
			animatedObjects[i]->Update(delta, PixelCount, strip);
		}
	}
	serial_commands_.ReadSerial();
    strip.Show();
}	
