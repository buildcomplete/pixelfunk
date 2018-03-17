#include <Arduino.h>
#include "configuration.h"

#include "AnimatedObject.h"
#include "Walker.h"
#include "PixelNoise.h"
#include "AnimatedSun.h"
#include "AnimatedParticle.h"
#include "colormaps.h"

#include <ESP8266WiFi.h>

#include "wifisecret.h"

#include <RunningAverage.h>


const uint8_t PanelWidth = 16;  // 16 pixel x 16 pixel matrix of leds
const uint8_t PanelHeight = 16;
const uint8_t TileWidth = 2;  // laid out in 2 panels x 2 panels mosaic
const uint8_t TileHeight = 2;
MyMosaic mosaic(PanelWidth, PanelHeight, TileWidth, TileHeight);

// make sure to set this to the correct pins
const uint8_t DotDataPin = 2;
const uint16_t PixelCount = PanelWidth * PanelHeight * TileWidth * TileHeight;
MyBus strip(PixelCount, DotDataPin);
bool runAnimation = true;

#define LISTEN_PORT 80


WiFiServer server(LISTEN_PORT);

int toggleLeds(String command)
{
	strip.ClearTo(RgbColor(255,255,255));
	return 1;
}

void setup()
{
    Serial.begin(115200);
    while (!Serial); // wait for serial attach
	
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
	
    // this resets all the neopixels to an off state
    strip.Begin();
    strip.ClearTo(RgbColor(0));
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

const int nObjects = 512;
AnimatedObject** animatedObjects = NULL;
uint16_t animatedObjectIndexMap[PixelCount];


uint16_t updateVarDelay = 50;
uint16_t varCount = 0;
NeoGamma<NeoGammaEquationMethod> colorGamma;


const uint16_t left = 0;
const uint16_t right = PanelWidth * TileWidth - 1;
const uint16_t top = 0;
const uint16_t bottom = PanelHeight * TileHeight - 1;

RgbColor red(128, 0, 0);
RgbColor green(0, 128, 0);
RgbColor blue(0, 0, 128);
RgbColor white(128);


void loop()
{
	if (animatedObjects == NULL)
	{
		animatedObjects = new AnimatedObject*[nObjects];

		for (int i=0;i<nObjects-2;++i)
		{
			animatedObjectIndexMap[i]=0;
			float v = ((float)i/(float)max(nObjects-1,1));
			animatedObjects[i] = new AnimatedParticle(i, 15, 15,cos(v*6.28), sin(v*6.28), 1.0 + ((float)random(0, 50000) / 50000.0f) ,
				colorGamma.Correct(GetJetColour(i, 0, nObjects-1, 100)), mosaic, animatedObjectIndexMap);
		}
		animatedObjects[nObjects-2] = new AnimatedParticle(nObjects, 15, 15,1, 0, 1.0 + ((float)random(0, 50000) / 50000.0f) ,
				colorGamma.Correct(RgbColor(255,255,255)), mosaic, animatedObjectIndexMap);
		
		animatedObjects[nObjects-1]= new Walker(0,5,0.2, 
			colorGamma.Correct(RgbColor(128,128,0)));
	}

	{
		delay(1);
	}

	updateTimers();
	if (runAnimation)
	{
		// update and draw animated objects
		for (int i=0;i<nObjects;++i)
		{
			animatedObjects[i]->Update(delta, PixelCount, strip);
		}
	}
    strip.Show();
}
