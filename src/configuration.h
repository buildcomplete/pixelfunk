#pragma once
#include <NeoPixelBus.h>

#define colorSaturation 128

// define type for the bus we are using
using MyBus = NeoPixelBus<NeoBrgFeature, NeoEspBitBangMethodBase<NeoEspBitBangSpeed400Kbps>>;

