#pragma once
#include <NeoPixelBus.h>

#define colorSaturation 128

// define type for the bus we are using
using MyBus = NeoPixelBus<NeoBrgFeature, NeoEsp8266DmaWs2813Method>;
using MyMosaic = NeoMosaic <ColumnMajorAlternatingLayout>;
