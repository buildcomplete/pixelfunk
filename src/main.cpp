// DotStarTest
// This example will cycle between showing four pixels as Red, Green, Blue, White
// and then showing those pixels as Black.
//
// There is serial output of the current state so you can confirm and follow along
//

#include <Arduino.h>
#include <NeoPixelBus.h>


const uint16_t PixelCount = 50; // this example assumes 4 pixels, making it smaller will cause a failure

// make sure to set this to the correct pins
//const uint8_t DotClockPin = 2;
const uint8_t DotDataPin = 2;  

#define colorSaturation 128

// for software bit bang
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

void loop()
{

    //delay(100);
    
	float time = (float)millis() / 1000.0f;
    float power = sin(time);
	if (power < 0 )
	{
		power = -power;
    }

	for (int i=0;i<PixelCount;++i)
	{
		// set the colors, 
		strip.SetPixelColor(i, RgbColorF(power, 1-power, power));
    }
        
    strip.Show();
}	
